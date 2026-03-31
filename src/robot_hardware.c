#include "robot_hardware.h"
#include <stdio.h>


#ifdef SIMULACION
    #include "pigpio_mock.h"
#else
    #include <pigpio.h>
#endif

// Instancias globales de los sensores
SensorUltrasonico sensorFrontal;
SensorUltrasonico sensorLateralIzq;
SensorUltrasonico sensorLateralDer;

static int inicializado = 0;

int robot_hw_init() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Error: No se pudo inicializar pigpio. Requiere sudo.\n");
        return -1;
    }
    
    inicializado = 1;

    // Inicializar los pines de los motores y los LEDs
    motores_init();
    leds_init();

    // Asignar pines BCM a los sensores (Trigger, Echo)
    // NOTA: Los pines Echo (27, 10, 11) deben tener el divisor de voltaje a 3.3V
    sensor_init(&sensorFrontal, 17, 27);
    sensor_init(&sensorLateralIzq, 22, 10);
    sensor_init(&sensorLateralDer, 9, 11);

    // Indicar hardware listo encendiendo el LED de sistema
    led_set_sistema(1); 

    return 0;
}

void robot_hw_cleanup() {
    if (inicializado) {
        motores_detener();
        
        // Apagar todos los LEDs
        leds_apagar_todos();
        
        // Bajar a LOW los pines Trigger para no dejarlos emitiendo
        gpioWrite(sensorFrontal.pinTrigger, 0);
        gpioWrite(sensorLateralIzq.pinTrigger, 0);
        gpioWrite(sensorLateralDer.pinTrigger, 0);

        // Liberar recursos de hardware
        gpioTerminate();
        inicializado = 0;
    }
}

/* ══════════════════════════════════════════════════════════
   LECTURA DE SENSORES (REAL vs SIMULACIÓN)
══════════════════════════════════════════════════════════ */

#ifdef SIMULACION
// Función interna para leer el archivo de texto generado por Python
static double leer_simulador(int index) {
    FILE *f = fopen("sim_data.txt", "r");
    if (!f) return 50.0; // Distancia segura por defecto si no existe el archivo
    
    double d[3] = {50.0, 50.0, 50.0};
    fscanf(f, "%lf %lf %lf", &d[0], &d[1], &d[2]);
    fclose(f);
    
    return d[index];
}
#endif

double robot_get_distancia_frontal() { 
#ifdef SIMULACION
    return leer_simulador(0);
#else
    return sensor_leer_distancia(&sensorFrontal); 
#endif
}

double robot_get_distancia_izq() { 
#ifdef SIMULACION
    return leer_simulador(1);
#else
    return sensor_leer_distancia(&sensorLateralIzq); 
#endif
}

double robot_get_distancia_der() { 
#ifdef SIMULACION
    return leer_simulador(2);
#else
    return sensor_leer_distancia(&sensorLateralDer); 
#endif
}