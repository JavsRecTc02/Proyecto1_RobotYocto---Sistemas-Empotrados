#include "motor_control.h"
#include <pigpio.h>
#include <unistd.h> // Para usleep

// Pines para Motor Izquierdo (A)
#define ENA 12 // PWM Hardware
#define IN1 5
#define IN2 6

// Pines para Motor Derecho (B)
#define ENB 13 // PWM Hardware
#define IN3 23
#define IN4 24

void motores_init() {
    gpioSetMode(ENA, PI_OUTPUT);
    gpioSetMode(IN1, PI_OUTPUT);
    gpioSetMode(IN2, PI_OUTPUT);
    gpioSetMode(ENB, PI_OUTPUT);
    gpioSetMode(IN3, PI_OUTPUT);
    gpioSetMode(IN4, PI_OUTPUT);
    
    motores_detener();
}

void motores_detener() {
    gpioWrite(IN1, 0); gpioWrite(IN2, 0); gpioPWM(ENA, 0);
    gpioWrite(IN3, 0); gpioWrite(IN4, 0); gpioPWM(ENB, 0);
}

void motores_avanzar(int velocidad) {
    gpioWrite(IN1, 1); gpioWrite(IN2, 0);
    gpioWrite(IN3, 1); gpioWrite(IN4, 0);

    // "Kickstart" para romper inercia estática (sino se traban)
    gpioPWM(ENA, 255); gpioPWM(ENB, 255);
    usleep(50000); 

    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void motores_retroceder(int velocidad) {
    gpioWrite(IN1, 0); gpioWrite(IN2, 1);
    gpioWrite(IN3, 0); gpioWrite(IN4, 1);
    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void motores_girar_izquierda(int velocidad) {
    gpioWrite(IN1, 1); gpioWrite(IN2, 0);
    gpioWrite(IN3, 0); gpioWrite(IN4, 1);
    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}

void motores_girar_derecha(int velocidad) {
    gpioWrite(IN1, 0); gpioWrite(IN2, 1);
    gpioWrite(IN3, 1); gpioWrite(IN4, 0);
    gpioPWM(ENA, velocidad); gpioPWM(ENB, velocidad);
}