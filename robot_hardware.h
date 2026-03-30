#ifndef ROBOT_HARDWARE_H
#define ROBOT_HARDWARE_H

#include "motor_control.h"
#include "ultrasonic_sensor.h"
#include "led_indicators.h"

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

#endif // ROBOT_HARDWARE_H