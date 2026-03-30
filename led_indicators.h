#ifndef LED_INDICATORS_H
#define LED_INDICATORS_H

void leds_init();
void leds_apagar_todos();

void led_set_sistema(int estado);
void led_set_autonomo(int estado);
void led_set_manual(int estado);
void led_set_obstaculo(int estado);

#endif // LED_INDICATORS_H