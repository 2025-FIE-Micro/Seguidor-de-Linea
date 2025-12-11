/**
 @brief Implementación para el driver de motores DRV8833. Contiene la lógica para la inicialización, configuración de PWM y el control de movimiento de un solo motor DC.
 @author Legion de Ohm
 */

#include <Arduino.h>
#include "drv8833.hpp"

/**
 @brief Constructor de la clase Drv8833. Inicializa el pin SLEEP como salida y lo establece en HIGH para asegurar que el driver esté activado y listo para recibir comandos de movimiento.
 */
Drv8833::Drv8833() 
{
    // Configuración de pinSleep como salida.
    pinMode(_pinSleep, OUTPUT);
    // LOW lo desactiva. HIGH lo activa
    digitalWrite(_pinSleep, HIGH);
}

/**
 @brief Configura el mapeo de pines y el canal de PWM del ESP32 para el control del motor.
 @param pinIN1 Pin GPIO conectado a la entrada IN1 del driver.
 @param pinIN2 Pin GPIO conectado a la entrada IN2 del driver.
 @param pinSleep Pin GPIO conectado al pin SLEEP del driver.
 @param chPWM Canal PWM (LEDC Channel) a utilizar.
 @param freqPWM Frecuencia del PWM.
 @param resPWM Resolución del PWM (bits).
 @return void
 */
void Drv8833::setup(uint8_t pinIN1, uint8_t pinIN2, uint8_t pinSleep, uint8_t chPWM, uint32_t freqPWM, uint8_t resPWM)
{
    // Almacena los parámetros de configuración internamente.
    _pinIN2 = pinIN2;
    _pinSleep = pinSleep;
    _chPWM = chPWM;
    _freqPWM = freqPWM;
    _resPWM = resPWM;

    // Configura los pines IN1/IN2 como salidas digitales.
    pinMode(_pinIN1, OUTPUT);
    pinMode(_pinIN2, OUTPUT);

    // Configura la frecuencia y la resolución del canal PWM (ledcSetup).
    ledcSetup(_chPWM, _freqPWM, _resPWM);
}

/**
 @brief Mueve el motor en dirección de avance con un porcentaje de velocidad (PWM). Implementa el control de dirección: establece un pin en estado digital bajo (LOW) y el otro pin en el canal PWM para controlar la velocidad.
 @param pPWM Porcentaje de PWM a aplicar al motor [0, 100] %.
 @return void
 */
void Drv8833::forward(uint8_t pPWM)
{
    // 1. Define la dirección (IN2 en bajo, IN1 en PWM).
    // Detach es crucial para mover el pin de control digital a control PWM.
    ledcDetachPin(_pinIN2);     // Separa IN2 del PWM para control digital.
    digitalWrite(_pinIN2, 0);   // Aplica un bajo (LOW) en IN2.

    // 2. Aplica la señal de velocidad al pin opuesto (IN1).
    ledcAttachPin(_pinIN1, _chPWM); // Conecta IN1 al canal de PWM configurado.

    // 3. Calcula y aplica el ciclo de trabajo.
    /**
     @brief Cálculo del ciclo de trabajo (Duty Cycle). Transforma el porcentaje de PWM (0-100%) en un valor de ciclo de trabajo basado en la resolución configurada ($\text{Duty} = 2^{\text{resPWM}} \times (\text{pPWM}/100)$).
     */
    uint16_t pwmDutyCycle;
    // Cálculo: se multiplica el rango máximo (2^res) por el porcentaje (pPWM/100.0)
    pwmDutyCycle = round(pow(2, _resPWM) * (pPWM / 100.0));

    // Escribe el valor PWM al canal.
    ledcWrite(_chPWM, pwmDutyCycle);
}

/**
 @brief Mueve el motor en dirección de reversa con un porcentaje de velocidad (PWM). Implementa el control de dirección invirtiendo la señal: establece IN1 en LOW y aplica la señal PWM en IN2.
 @param pPWM Porcentaje de PWM a aplicar al motor [0, 100] %.
 @return void
 */
void Drv8833::reverse(uint8_t pPWM)
{
    // 1. Define la dirección (IN1 en bajo, IN2 en PWM).
    ledcDetachPin(_pinIN1);     // Separa IN1 del PWM para control digital.
    digitalWrite(_pinIN1, 0);// Aplica un bajo (LOW) en IN1.

    // 2. Aplica la señal de velocidad al pin opuesto (IN2).
    ledcAttachPin(_pinIN2, _chPWM);// Conecta IN2 al canal de PWM configurado.

    // 3. Calcula y aplica el ciclo de trabajo.
    uint16_t pwmDutyCycle;
    pwmDutyCycle = round(pow(2, _resPWM) * (pPWM / 100.0));

    // Escribe el valor PWM al canal.
    ledcWrite(_chPWM, pwmDutyCycle);
}

/**
 @brief Detiene el motor mediante el frenado rápido (brake). El frenado rápido se logra al detener la señal PWM en ambos pines y establecerlos en un estado digital bajo (LOW), cortando la corriente.
 @return void
 */
void Drv8833::stop()
{
    // 1. Asegura que los pines ya no están controlados por el PWM.
    ledcDetachPin(_pinIN1);
    ledcDetachPin(_pinIN2);

    // 2. Asigna un bajo (LOW) a ambos pines para frenado.
    digitalWrite(_pinIN1, 0);
    digitalWrite(_pinIN2, 0);
}
