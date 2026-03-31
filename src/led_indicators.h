#ifndef LED_INDICATORS_H
#define LED_INDICATORS_H

// Definición de pines para los LEDs (BCM)
#define LED_SISTEMA    16
#define LED_AUTONOMO   20
#define LED_MANUAL     21
#define LED_OBSTACULO  26

void leds_init();
void leds_apagar_todos();
void led_set(int pin, int estado);

void led_set_sistema(int estado);
void led_set_autonomo(int estado);
void led_set_manual(int estado);
void led_set_obstaculo(int estado);

#endif // LED_INDICATORS_H