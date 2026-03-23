#include "IndicadoresLED.hpp"
#include <pigpio.h>

IndicadoresLED::IndicadoresLED() {
    gpioSetMode(PIN_SISTEMA, PI_OUTPUT);
    gpioSetMode(PIN_AUTONOMO, PI_OUTPUT);
    gpioSetMode(PIN_MANUAL, PI_OUTPUT);
    gpioSetMode(PIN_OBSTACULO, PI_OUTPUT);
    apagarTodos();
}

IndicadoresLED::~IndicadoresLED() {
    apagarTodos();
}

void IndicadoresLED::setSistemaEncendido(bool estado) { gpioWrite(PIN_SISTEMA, estado ? 1 : 0); }
void IndicadoresLED::setModoAutonomo(bool estado) { gpioWrite(PIN_AUTONOMO, estado ? 1 : 0); }
void IndicadoresLED::setModoManual(bool estado) { gpioWrite(PIN_MANUAL, estado ? 1 : 0); }
void IndicadoresLED::setAlertaObstaculo(bool estado) { gpioWrite(PIN_OBSTACULO, estado ? 1 : 0); }

void IndicadoresLED::apagarTodos() {
    gpioWrite(PIN_SISTEMA, 0);
    gpioWrite(PIN_AUTONOMO, 0);
    gpioWrite(PIN_MANUAL, 0);
    gpioWrite(PIN_OBSTACULO, 0);
}