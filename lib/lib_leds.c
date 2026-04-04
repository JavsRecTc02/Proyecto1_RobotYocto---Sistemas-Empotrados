#include "lib_leds.h"
#include "../src/robot_state.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Escribe una cadena en un archivo sysfs
static int sysfs_write(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    ssize_t n = write(fd, value, strlen(value));
    close(fd);
    return (n < 0) ? -1 : 0;
}

// Exporta un pin GPIO y lo configura como salida 
static int gpio_export(int pin)
{
    char path[64];

    // Verificar si ya esta exportado el gpio
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
    struct stat st;
    if (stat(path, &st) == 0) {
        // Si lo esta, entonces solo asegurar direccion output
        char dir_path[80];
        snprintf(dir_path, sizeof(dir_path),
                 "/sys/class/gpio/gpio%d/direction", pin);
        sysfs_write(dir_path, "out");
        return 0;
    }

    // Exportar el pin
    char pin_str[8];
    snprintf(pin_str, sizeof(pin_str), "%d", pin);
    if (sysfs_write("/sys/class/gpio/export", pin_str) < 0) {
        fprintf(stderr, "[leds] ERROR: no se pudo exportar GPIO %d\n", pin);
        return -1;
    }

    // Esperar a que el kernel cree la entrada 
    usleep(100 * 1000); 

    // Configurar el pin como salida 
    char dir_path[80];
    snprintf(dir_path, sizeof(dir_path),
             "/sys/class/gpio/gpio%d/direction", pin);
    if (sysfs_write(dir_path, "out") < 0) {
        fprintf(stderr, "[leds] ERROR: no se pudo configurar GPIO %d como salida\n", pin);
        return -1;
    }

    return 0;
}

/* ══════════════════════════════════════════════════════════
   Estado de los LEDS
══════════════════════════════════════════════════════════ */

// Escribe el valor (0/1) en el pin GPIO 
static void gpio_write(int pin, int value)
{
    char path[80];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    sysfs_write(path, value ? "1" : "0");
}

// Libera (unexport) un pin GPIO 
static void gpio_unexport(int pin)
{
    char pin_str[8];
    snprintf(pin_str, sizeof(pin_str), "%d", pin);
    sysfs_write("/sys/class/gpio/unexport", pin_str);
}

/* ══════════════════════════════════════════════════════════
   Estado interno de la biblioteca
══════════════════════════════════════════════════════════ */

static const char *LED_NAMES[LED_COUNT] = {
    "POWER", "AUTONOMOUS", "MANUAL", "OBSTACLE"
};

static const int LED_PINS[LED_COUNT] = {
    LED_GPIO_POWER,
    LED_GPIO_AUTONOMOUS,
    LED_GPIO_MANUAL,
    LED_GPIO_OBSTACLE
};

// Estado 1 | 0
typedef struct {
    int            state;     
} LedEntry;

static LedEntry    g_leds[LED_COUNT];
static pthread_mutex_t g_lock;
static int         g_initialized = 0;


/* ══════════════════════════════════════════════════════════
   API publica para los LEDS
══════════════════════════════════════════════════════════ */

int lib_leds_init(void)
{
    if (g_initialized) return 0;

    memset(g_leds, 0, sizeof(g_leds));

    if (pthread_mutex_init(&g_lock, NULL) != 0) {
        fprintf(stderr, "[leds] ERROR: mutex init failed\n");
        return -1;
    }

    printf("[leds] Inicializando pines GPIOs\n");

    int any_error = 0;
    for (int i = 0; i < LED_COUNT; i++) {
        if (gpio_export(LED_PINS[i]) < 0) {
            fprintf(stderr, "[leds] WARN: GPIO %d (%s) no disponible\n",
                    LED_PINS[i], LED_NAMES[i]);
            any_error = 1;
        } else {
            // Apagar LED al iniciar
            gpio_write(LED_PINS[i], 0);
            printf("[leds] GPIO %d configurado para LED_%s\n",
                   LED_PINS[i], LED_NAMES[i]);
        }
    }

    g_initialized = 1;

    // Encender LED de power automaticamente
    lib_leds_set(LED_POWER, 1);

    if (any_error)
        printf("[leds] Inicializado con fallos en GPIOs\n");
    else
        printf("[leds] Inicializado GPIOs OK\n");

    return 0;
}

/* ══════════════════════════════════════════════════════════
   Funciones Principales
══════════════════════════════════════════════════════════ */

// Encender o Apagar el LED

void lib_leds_set(LedId led, int state)
{
    if (!g_initialized) return;
    if (led < 0 || led >= LED_COUNT) return;

    state = state ? 1 : 0;

    pthread_mutex_lock(&g_lock);

    int prev = g_leds[led].state;
    g_leds[led].state = state;

    pthread_mutex_unlock(&g_lock);

    // Escribir al GPIO fisico
    gpio_write(LED_PINS[led], state);

    // Print para ver estado en el Server
    if (prev != state) {
        printf("[leds] LED_%-10s GPIO %2d → %s\n",
               LED_NAMES[led], LED_PINS[led],
               state ? "ON" : "OFF");
    }
}

// Obtener el estado actual del LED

int lib_leds_get(LedId led)
{
    if (!g_initialized || led < 0 || led >= LED_COUNT) return 0;

    pthread_mutex_lock(&g_lock);
    int state = g_leds[led].state;
    pthread_mutex_unlock(&g_lock);

    return state;
}

// Apagar LEDS y liberar los GPIO

void lib_leds_destroy(void)
{
    if (!g_initialized) return;

    printf("[leds] Apagando LEDs y liberando GPIOs\n");

    for (int i = 0; i < LED_COUNT; i++) {
        gpio_write(LED_PINS[i], 0);
        gpio_unexport(LED_PINS[i]);
        printf("[leds] LED_%-10s GPIO %2d → OFF\n",
               LED_NAMES[i], LED_PINS[i]);
    }

    pthread_mutex_destroy(&g_lock);
    g_initialized = 0;
}

// Sincronizar los LEDS

void lib_leds_sync_from_state(void)
{
    RobotState *rs = robot_state_get();
    if (!rs) return;

    pthread_mutex_lock(&rs->lock);
    int power     = rs->leds.power;
    int autonomous= rs->leds.autonomous;
    int manual    = rs->leds.manual;
    int obstacle  = rs->leds.obstacle;
    pthread_mutex_unlock(&rs->lock);

    printf("[leds] LEDS sincronizados\n");
    lib_leds_set(LED_POWER,      power);
    lib_leds_set(LED_AUTONOMOUS, autonomous);
    lib_leds_set(LED_MANUAL,     manual);
    lib_leds_set(LED_OBSTACLE,   obstacle);
}