#include "led_indicators.h"
#include <pigpio.h>

#define PIN_SISTEMA 16
#define PIN_AUTONOMO 20
#define PIN_MANUAL 21
#define PIN_OBSTACULO 26

void leds_init() {
    gpioSetMode(PIN_SISTEMA, PI_OUTPUT);
    gpioSetMode(PIN_AUTONOMO, PI_OUTPUT);
    gpioSetMode(PIN_MANUAL, PI_OUTPUT);
    gpioSetMode(PIN_OBSTACULO, PI_OUTPUT);
    leds_apagar_todos();
}

void leds_apagar_todos() {
    gpioWrite(PIN_SISTEMA, 0);
    gpioWrite(PIN_AUTONOMO, 0);
    gpioWrite(PIN_MANUAL, 0);
    gpioWrite(PIN_OBSTACULO, 0);
}

void led_set_sistema(int estado) { gpioWrite(PIN_SISTEMA, estado ? 1 : 0); }
void led_set_autonomo(int estado) { gpioWrite(PIN_AUTONOMO, estado ? 1 : 0); }
void led_set_manual(int estado) { gpioWrite(PIN_MANUAL, estado ? 1 : 0); }
void led_set_obstaculo(int estado) { gpioWrite(PIN_OBSTACULO, estado ? 1 : 0); }