/**
 @file drv8833.hpp
 @brief Cabecera para la clase de control de bajo nivel del driver de motores DRV8833.
*Nota:* Esta clase maneja un solo motor. Para controlar el robot completo, se deben instanciar dos objetos Drv8833 (uno para el motor izquierdo y uno para el derecho).
 @author Legion de Ohm
 */

#pragma once

/**
 @class Drv8833
 @brief Clase que encapsula la lógica y configuración para controlar un motor DC a través del driver DRV8833. Incluye funciones para configurar sus pines, ajustar el PWM y manejar la dirección del motor (adelante/atrás).
 */
class Drv8833
{
public:
/**
 @brief Constructor por defecto. Inicializa la instancia del driver. Se asume que el pin Sleep está habilitado (HIGH) o controlado externamente para activar el driver.
 */
    Drv8833();

    /**
     @brief Configura los pines de control del motor y el canal PWM del ESP32. Este método es crucial para mapear los pines GPIO y el canal PWM (`ledc`) del ESP32 a las entradas del driver.
     @param pinIN1 Pin GPIO conectado a la entrada IN1 del driver.
     @param pinIN2 Pin GPIO conectado a la entrada IN2 del driver.
     @param pinSleep Pin GPIO conectado al pin SLEEP del driver (para habilitar/deshabilitar).
     @param chPWM Canal PWM (LEDC Channel) a utilizar (0-15 en ESP32).
     @param freqPWM Frecuencia del PWM. Recomendado: 20 kHz (audible) o 30 kHz (silencioso).
     @param resPWM Resolución del PWM (ej. 8 bits [0-255] o 10 bits [0-1023]).
     @return void
     */
    void setup(uint8_t pinIN1, uint8_t pinIN2, uint8_t pinSleep, uint8_t chPWM, uint32_t freqPWM, uint8_t resPWM);

    /**
     @brief Mueve el motor en dirección de avance. Configura los pines de dirección (IN1/IN2) para el movimiento en sentido de avance y aplica la señal PWM para controlar la velocidad.
     @param pPWM Porcentaje de la velocidad (PWM) a aplicar al motor [0-100] %.
     @return void
     */
    void forward(uint8_t pPWM);

    /**
     @brief Mueve el motor en dirección de reversa. Configura IN1/IN2 para reversa y aplica la señal PWM para controlar la velocidad.
     @param pPWM Porcentaje de la velocidad (PWM) a aplicar al motor [0-100] %.
     @return void
     */
    void reverse(uint8_t pPWM);

    /**
     @brief Detiene el motor mediante el frenado rápido (brake). Generalmente, establece ambas entradas (IN1 y IN2) en HIGH o LOW.
     @return void
     */
    void stop();

private:
    uint8_t _pinIN1;    ///< GPIO del pin IN1 (Control de dirección/PWM).
    uint8_t _pinIN2;    ///< GPIO del pin IN2 (Control de dirección/PWM).
    uint8_t _pinSleep;  ///< GPIO del pin SLEEP (Habilitación del driver).
    uint8_t _chPWM;     ///< Canal PWM utilizado (0-15).
    uint32_t _freqPWM;  ///< Frecuencia del PWM configurada (Hz).
    uint8_t _resPWM;    ///< Resolución del PWM configurada (bits).
};