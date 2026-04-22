#include "lib_motors.h"
#include <pigpiod_if2.h>
#include <unistd.h> // Para usleep

// Pines para Motor Izquierdo (A)
#define ENA 12 // PWM Hardware
#define IN1 5
#define IN2 6

// Pines para Motor Derecho (B)
#define ENB 13 // PWM Hardware
#define IN3 23
#define IN4 24

static int g_pi = -1;

void motores_init(int pi) {  
    g_pi = pi;
    set_mode(g_pi, ENA, PI_OUTPUT);
    set_mode(g_pi, IN1, PI_OUTPUT);
    set_mode(g_pi, IN2, PI_OUTPUT);
    set_mode(g_pi, ENB, PI_OUTPUT);
    set_mode(g_pi, IN3, PI_OUTPUT);
    set_mode(g_pi, IN4, PI_OUTPUT);
    motores_detener();
}

void motores_detener() {
    gpio_write(g_pi, IN1, 0); gpio_write(g_pi, IN2, 0); set_PWM_dutycycle(g_pi, ENA, 0);
    gpio_write(g_pi, IN3, 0); gpio_write(g_pi, IN4, 0); set_PWM_dutycycle(g_pi, ENB, 0);
}

void motores_avanzar(int velocidad) {
    gpio_write(g_pi, IN1, 1); gpio_write(g_pi, IN2, 0);
    gpio_write(g_pi, IN3, 1); gpio_write(g_pi, IN4, 0);
    set_PWM_dutycycle(g_pi, ENA, 255); set_PWM_dutycycle(g_pi, ENB, 255);
    usleep(50000);
    set_PWM_dutycycle(g_pi, ENA, velocidad); set_PWM_dutycycle(g_pi, ENB, velocidad);
}

void motores_retroceder(int velocidad) {
    gpio_write(g_pi, IN1, 0); gpio_write(g_pi, IN2, 1);
    gpio_write(g_pi, IN3, 0); gpio_write(g_pi, IN4, 1);
    set_PWM_dutycycle(g_pi, ENA, velocidad); set_PWM_dutycycle(g_pi, ENB, velocidad);
}

void motores_girar_izquierda(int velocidad) {
    gpio_write(g_pi, IN1, 1); gpio_write(g_pi, IN2, 0);
    gpio_write(g_pi, IN3, 0); gpio_write(g_pi, IN4, 1);
    set_PWM_dutycycle(g_pi, ENA, velocidad); set_PWM_dutycycle(g_pi, ENB, velocidad);
}

void motores_girar_derecha(int velocidad) {
    gpio_write(g_pi, IN1, 0); gpio_write(g_pi, IN2, 1);
    gpio_write(g_pi, IN3, 1); gpio_write(g_pi, IN4, 0);
    set_PWM_dutycycle(g_pi, ENA, velocidad); set_PWM_dutycycle(g_pi, ENB, velocidad);
}
