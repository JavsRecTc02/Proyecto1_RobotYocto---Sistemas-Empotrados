#include "led_indicators.h"
#include <pigpio.h>


void leds_init() {
    gpioSetMode(LED_SISTEMA, PI_OUTPUT);
    gpioSetMode(LED_AUTONOMO, PI_OUTPUT);
    gpioSetMode(LED_MANUAL, PI_OUTPUT);
    gpioSetMode(LED_OBSTACULO, PI_OUTPUT);
    leds_apagar_todos();
}

void leds_apagar_todos() {
    gpioWrite(LED_SISTEMA, 0);
    gpioWrite(LED_AUTONOMO, 0);
    gpioWrite(LED_MANUAL, 0);
    gpioWrite(LED_OBSTACULO, 0);
}

void led_set(int pin, int estado) {
    gpioWrite(pin, estado ? 1 : 0);
}

void led_set_sistema(int estado) { led_set(LED_SISTEMA, estado); }
void led_set_autonomo(int estado) { led_set(LED_AUTONOMO, estado); }
void led_set_manual(int estado) { led_set(LED_MANUAL, estado); }
void led_set_obstaculo(int estado) { led_set(LED_OBSTACULO, estado); }