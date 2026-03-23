#ifndef INDICADORES_LED_HPP
#define INDICADORES_LED_HPP

/**
 * @class IndicadoresLED
 * @brief Gestiona los 4 LEDs de estado del sistema.
 */
class IndicadoresLED {
private:
    const int PIN_SISTEMA = 16;
    const int PIN_AUTONOMO = 20;
    const int PIN_MANUAL = 21;
    const int PIN_OBSTACULO = 26;

public:
    IndicadoresLED();
    ~IndicadoresLED();

    void setSistemaEncendido(bool estado);
    void setModoAutonomo(bool estado);
    void setModoManual(bool estado);
    void setAlertaObstaculo(bool estado);
    void apagarTodos();
};

#endif // INDICADORES_LED_HPP