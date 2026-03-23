#include "SensorUltrasonico.hpp"
#include <pigpio.h>

SensorUltrasonico::SensorUltrasonico(int trigger, int echo) : pinTrigger(trigger), pinEcho(echo) {
    gpioSetMode(pinTrigger, PI_OUTPUT);
    gpioSetMode(pinEcho, PI_INPUT);
    gpioWrite(pinTrigger, 0);
}

SensorUltrasonico::~SensorUltrasonico() {
    gpioWrite(pinTrigger, 0);
}

double SensorUltrasonico::leerDistancia() {
    gpioWrite(pinTrigger, 1);
    gpioDelay(10); // Pulso de 10us
    gpioWrite(pinTrigger, 0);

    uint32_t inicio = gpioTick();
    uint32_t timeout = inicio;

    // Esperar a que el Echo suba a HIGH (con timeout para evitar bloqueos)
    while (gpioRead(pinEcho) == 0) {
        inicio = gpioTick();
        if (inicio - timeout > 50000) return -1.0; // Timeout
    }

    uint32_t fin = gpioTick();
    // Esperar a que el Echo baje a LOW
    while (gpioRead(pinEcho) == 1) {
        fin = gpioTick();
        if (fin - inicio > 50000) return -1.0; // Timeout de fuera de rango
    }

    uint32_t duracion = fin - inicio;
    return (duracion * 34300.0) / 2000000.0; 
}