#include "lib_sensors.h"
#include <pigpio.h>

void sensor_init(SensorUltrasonico* sensor, int trigger, int echo) {
    sensor->pinTrigger = trigger;
    sensor->pinEcho = echo;
    
    gpioSetMode(sensor->pinTrigger, PI_OUTPUT);
    gpioSetMode(sensor->pinEcho, PI_INPUT);
    gpioWrite(sensor->pinTrigger, 0);
}

double sensor_leer_distancia(SensorUltrasonico* sensor) {
    gpioWrite(sensor->pinTrigger, 1);
    gpioDelay(10); // Pulso de 10us
    gpioWrite(sensor->pinTrigger, 0);

    uint32_t inicio = gpioTick();
    uint32_t timeout = inicio;

    // Esperar a que el Echo suba a HIGH (con timeout para evitar bloqueos)
    while (gpioRead(sensor->pinEcho) == 0) {
        inicio = gpioTick();
        if (inicio - timeout > 50000) return -1.0; // Timeout
    }

    uint32_t fin = gpioTick();
    // Esperar a que el Echo baje a LOW
    while (gpioRead(sensor->pinEcho) == 1) {
        fin = gpioTick();
        if (fin - inicio > 50000) return -1.0; // Timeout de fuera de rango
    }

    uint32_t duracion = fin - inicio;
    return (duracion * 34300.0) / 2000000.0; 
}