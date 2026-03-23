#include "RobotHardware.hpp"
#include <pigpio.h>
#include <iostream>

RobotHardware::RobotHardware() : inicializado(false) {
    if (gpioInitialise() < 0) {
        std::cerr << "Error: No se pudo inicializar pigpio. Requiere sudo." << std::endl;
        return;
    }
    inicializado = true;

    // Instanciar subsistemas con pines específicos
    motores = new ControlMotores();
    sensorFrontal = new SensorUltrasonico(17, 27); // Asignar pines
    sensorLateralIzq = new SensorUltrasonico(22, 10);
    sensorLateralDer = new SensorUltrasonico(9, 11);

    leds = new IndicadoresLED();

    leds->setSistemaEncendido(true); // Indicar hardware listo
}

RobotHardware::~RobotHardware() {
    if (inicializado) {
        delete motores;
        delete sensorFrontal;
        delete sensorLateralIzq;
        delete sensorLateralDer;
        delete leds;
        gpioTerminate();
    }
}

ControlMotores* RobotHardware::getMotores() const { return motores; }
SensorUltrasonico* RobotHardware::getSensorFrontal() const { return sensorFrontal; }
SensorUltrasonico* RobotHardware::getSensorLateralIzq() const { return sensorLateralIzq; }
SensorUltrasonico* RobotHardware::getSensorLateralDer() const { return sensorLateralDer; }
IndicadoresLED* RobotHardware::getLeds() const { return leds; }