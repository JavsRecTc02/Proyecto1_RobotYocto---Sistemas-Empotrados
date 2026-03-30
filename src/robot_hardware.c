#include "robot_hardware.h"
#include <pigpio.h>
#include <stdio.h>

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

    // Inicializar subsistemas
    motores_init();
    leds_init();

    // Asignar pines a los sensores
    sensor_init(&sensorFrontal, 17, 27);
    sensor_init(&sensorLateralIzq, 22, 10);
    sensor_init(&sensorLateralDer, 9, 11);

    // Indicar hardware listo
    led_set_sistema(1); 

    return 0;
}

void robot_hw_cleanup() {
    if (inicializado) {
        motores_detener();
        leds_apagar_todos();
        
        // Apagar los triggers de los sensores
        gpioWrite(sensorFrontal.pinTrigger, 0);
        gpioWrite(sensorLateralIzq.pinTrigger, 0);
        gpioWrite(sensorLateralDer.pinTrigger, 0);

        gpioTerminate();
        inicializado = 0;
    }
}