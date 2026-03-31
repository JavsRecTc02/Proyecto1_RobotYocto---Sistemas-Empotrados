#ifndef PIGPIO_MOCK_H
#define PIGPIO_MOCK_H

#include <stdio.h>
#include <stdint.h>

#define PI_OUTPUT 1
#define PI_INPUT 0

static inline int gpioInitialise(void) { return 0; }
static inline void gpioTerminate(void) { }
static inline int gpioSetMode(unsigned g, unsigned m) { (void)g; (void)m; return 0; }
static inline int gpioWrite(unsigned g, unsigned l) { (void)g; (void)l; return 0; }
static inline int gpioPWM(unsigned g, unsigned d) { (void)g; (void)d; return 0; }
static inline uint32_t gpioTick(void) { return 0; }
static inline void gpioDelay(uint32_t m) { (void)m; }
static inline int gpioRead(unsigned g) { (void)g; return 0; }

#endif