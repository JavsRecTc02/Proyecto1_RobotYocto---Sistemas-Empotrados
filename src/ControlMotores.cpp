#include "ControlMotores.hpp"
#include <pigpio.h>
#include <unistd.h>

ControlMotores::ControlMotores() {
    // Configuración de pines como salida
    gpioSetMode(ENA, PI_OUTPUT);
    gpioSetMode(IN1, PI_OUTPUT);
    gpioSetMode(IN2, PI_OUTPUT);
    gpioSetMode(ENB, PI_OUTPUT);
    gpioSetMode(IN3, PI_OUTPUT);
    gpioSetMode(IN4, PI_OUTPUT);
    
    detener();
}

ControlMotores::~ControlMotores() {
    detener();
}

void ControlMotores::avanzar(int velocidad) {
    gpioWrite(IN1, 1); gpioWrite(IN2, 0);
    gpioWrite(IN3, 1); gpioWrite(IN4, 0);

    // "Kickstart" para romper inercia estática (sino se traban)
    gpioPWM(ENA, 255); gpioPWM(ENB, 255);
    usleep(50000); 

    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void ControlMotores::retroceder(int velocidad) {
    gpioWrite(IN1, 0); gpioWrite(IN2, 1);
    gpioWrite(IN3, 0); gpioWrite(IN4, 1);
    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void ControlMotores::girarIzquierda(int velocidad) {
    gpioWrite(IN1, 1); gpioWrite(IN2, 0);
    gpioWrite(IN3, 0); gpioWrite(IN4, 1);
    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void ControlMotores::girarDerecha(int velocidad) {
    gpioWrite(IN1, 0); gpioWrite(IN2, 1);
    gpioWrite(IN3, 1); gpioWrite(IN4, 0);
    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void ControlMotores::detener() {
    gpioWrite(IN1, 0); gpioWrite(IN2, 0); gpioPWM(ENA, 0);
    gpioWrite(IN3, 0); gpioWrite(IN4, 0); gpioPWM(ENB, 0);
}