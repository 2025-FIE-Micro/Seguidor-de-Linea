#include <Arduino.h>
#include "drv8833.hpp"

Drv8833::Drv8833() {
}

void Drv8833::setup(uint8_t pinIN1, uint8_t pinIN2, uint8_t pinSleep, uint8_t chPWM, uint32_t freqPWM, uint8_t resPWM) {
    // Ajuste de variables
    _pinIN1 = pinIN1;
    _pinIN2 = pinIN2;
    _pinSleep = pinSleep;
    _chPWM = chPWM;
    _freqPWM = freqPWM;
    _resPWM = resPWM;

    // Configuración de pines.
    pinMode(_pinIN1, OUTPUT);
    pinMode(_pinIN2, OUTPUT);
    pinMode(_pinSleep, OUTPUT);

    digitalWrite(_pinSleep, HIGH); // Habilitar driver

    // Ajuste de PWM del canal
    ledcSetup(_chPWM, _freqPWM, _resPWM);
}

void Drv8833::forward(uint8_t pPWM) {
    ledcDetachPin(_pinIN2); 
    digitalWrite(_pinIN2, LOW);      

    ledcAttachPin(_pinIN1, _chPWM);

    // Cálculo correcto del duty
    uint16_t maxDuty = (1 << _resPWM) - 1;
    uint16_t pwmDutyCycle = (maxDuty * pPWM) / 100;

    ledcWrite(_chPWM, pwmDutyCycle);  
}

void Drv8833::reverse(uint8_t pPWM) {
    ledcDetachPin(_pinIN1);
    digitalWrite(_pinIN1, LOW);

    ledcAttachPin(_pinIN2, _chPWM);

    uint16_t maxDuty = (1 << _resPWM) - 1;
    uint16_t pwmDutyCycle = (maxDuty * pPWM) / 100;

    ledcWrite(_chPWM, pwmDutyCycle);
}

void Drv8833::stop() {
    ledcDetachPin(_pinIN1);
    ledcDetachPin(_pinIN2);

    digitalWrite(_pinIN1, LOW);
    digitalWrite(_pinIN2, LOW);
}
