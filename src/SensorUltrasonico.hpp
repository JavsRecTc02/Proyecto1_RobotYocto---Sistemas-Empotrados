#ifndef SENSOR_ULTRASONICO_HPP
#define SENSOR_ULTRASONICO_HPP

#include <stdint.h>

/**
 * @class SensorUltrasonico
 * @brief Gestiona la lectura de un sensor HC-SR04.
 * @warning El pin ECHO requiere un divisor de voltaje (5V -> 3.3V) antes de conectar a la Pi.*/
class SensorUltrasonico {
private:
    int pinTrigger;
    int pinEcho;

public:
    SensorUltrasonico(int trigger, int echo);
    ~SensorUltrasonico();

    /**
     * @brief Calcula la distancia al obstáculo más cercano.
     * @return Distancia en centímetros.
     */
    double leerDistancia();
};

#endif // SENSOR_ULTRASONICO_HPP