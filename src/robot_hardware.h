#ifndef ROBOT_HARDWARE_H
#define ROBOT_HARDWARE_H

#include "lib_motors.h"
#include "lib_sensors.h"

// Variables globales para acceder a los sensores desde el servidor si es necesario
extern SensorUltrasonico sensorFrontal;
extern SensorUltrasonico sensorLateralIzq;
extern SensorUltrasonico sensorLateralDer;

/**
 * @brief Inicializa pigpio y todos los subsistemas (motores, leds, sensores).
 * @return 0 si es exitoso, -1 si falla pigpio.
 */
int robot_hw_init();

/**
 * @brief Apaga todos los componentes y termina la conexión con pigpio.
 */
void robot_hw_cleanup();

double robot_get_distancia_frontal();
double robot_get_distancia_izq();
double robot_get_distancia_der();

#endif // ROBOT_HARDWARE_H