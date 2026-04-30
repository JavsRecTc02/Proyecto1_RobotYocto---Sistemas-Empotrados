#ifndef ROBOT_HARDWARE_H
#define ROBOT_HARDWARE_H

#include "lib_motors.h"
#include "lib_sensors.h"

extern SensorUltrasonico sensorFrontal;
extern SensorUltrasonico sensorLateralIzq;
extern SensorUltrasonico sensorLateralDer;

int robot_hw_init();
int robot_hw_get_pi();    // ← nuevo — expone el handle
void robot_hw_cleanup();
double robot_get_distancia_frontal();
double robot_get_distancia_izq();
double robot_get_distancia_der();

#endif // ROBOT_HARDWARE_H
