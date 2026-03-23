#ifndef CONTROL_MOTORES_HPP
#define CONTROL_MOTORES_HPP

/**
 * @class ControlMotores
 * @brief Maneja la tracción diferencial del robot usando un L298N aislado ópticamente.
 */
class ControlMotores {
private:
    // Pines para Motor Izquierdo (A)
    const int ENA = 12; // PWM Hardware
    const int IN1 = 5;
    const int IN2 = 6;

    // Pines para Motor Derecho (B)
    const int ENB = 13; // PWM Hardware
    const int IN3 = 23;
    const int IN4 = 24;

public:
    ControlMotores();
    ~ControlMotores();

    void avanzar(int velocidad);
    void retroceder(int velocidad);
    void girarIzquierda(int velocidad);
    void girarDerecha(int velocidad);
    void detener();
};

#endif // CONTROL_MOTORES_HPP