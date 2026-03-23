#ifndef ROBOT_HARDWARE_HPP
#define ROBOT_HARDWARE_HPP

#include "ControlMotores.hpp"
#include "SensorUltrasonico.hpp"
#include "IndicadoresLED.hpp"

/**
 * @class RobotHardware
 */
class RobotHardware {
private:
    ControlMotores* motores;
    SensorUltrasonico* sensorFrontal;
    SensorUltrasonico* sensorLateralIzq;
    SensorUltrasonico* sensorLateralDer;
    IndicadoresLED* leds;
    bool inicializado;

public:
    RobotHardware();
    ~RobotHardware();

    // Exponer subsistemas de forma segura
    ControlMotores* getMotores() const;
    SensorUltrasonico* getSensorFrontal() const;
    SensorUltrasonico* getSensorLateralIzq() const;
    SensorUltrasonico* getSensorLateralDer() const;
    IndicadoresLED* getLeds() const;
};

#endif // ROBOT_HARDWARE_HPP