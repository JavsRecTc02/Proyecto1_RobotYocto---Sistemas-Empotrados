#ifndef LIB_LEDS_H
#define LIB_LEDS_H

// Pines GPIO BCM 
#define LED_GPIO_POWER       17   /* GPIO 17 — Pin fisico 11 */
#define LED_GPIO_AUTONOMOUS  27   /* GPIO 27 — Pin fisico 13 */
#define LED_GPIO_MANUAL      22   /* GPIO 22 — Pin fisico 15 */
#define LED_GPIO_OBSTACLE    23   /* GPIO 23 — Pin fisico 16 */

// ID del LED
typedef enum {
    LED_POWER      = 0,
    LED_AUTONOMOUS = 1,
    LED_MANUAL     = 2,
    LED_OBSTACLE   = 3,
    LED_COUNT      = 4
} LedId;

// Inicializar la biblioteca y pines GPIO
int  lib_leds_init(void);

// Enciende (state=1) o apaga (state=0) un LED por su ID
void lib_leds_set(LedId led, int state);

// Obtener estado actual 1 | 0
int  lib_leds_get(LedId led);

// Apaga los LED y libera los GPIO 
void lib_leds_destroy(void);

// Sincronizar los LEDS
void lib_leds_sync_from_state(void);

#endif