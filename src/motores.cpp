/*
    programa de direccion y control de motores
    * movimiento con correcion para atras
    * movimiento con correcion solamente recta
    * control de motores - setpoint o pid
    * TODO: separar correccion y setpoint  
*/

#include <Arduino.h>
#include "sensores.hpp"
#include "config.hpp"
//#include "drv8833.hpp"
#include "motores.hpp"
#include "pid.hpp"
#include "interrupciones.hpp"

Drv8833::Drv8833() {}

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


// ============================
// CONFIGURACIÓN DE MOTORES
// ============================

// Objetos de motores (visibles solo en este modulo)
static Drv8833 motorIzq;
static Drv8833 motorDer;

// Canales PWM usados por cada motor
static const uint8_t motorPWM_Izq = 0;
static const uint8_t motorPWM_Der = 1;

// Parámetros de PWM
static const uint32_t freqPWM = 20000; // 20 kHz
static const uint8_t  resPWM  = 8;     // 8 bits (0..255)

// Velocidades actuales
int32_t motorSpeedIzq = 0;
int32_t motorSpeedDer = 0;


// INICIACION DE MOTORES
void setupMotores() {
    motorDer.setup(motorPinIN1_Der, motorPinIN2_Der,
                   motorPinSleep_Der, motorPWM_Der,
                   freqPWM, resPWM);

    motorIzq.setup(motorPinIN1_Izq, motorPinIN2_Izq,
                   motorPinSleep_Izq, motorPWM_Izq,
                   freqPWM, resPWM);

    detenerMotores();
}


// ============================
// FUNCION PARA MOVER MOTORES
// ============================
// Aplica las velocidades calculadas a cada motor.
// Valores positivos hacen avanzar el motor,
// valores negativos lo hacen girar en reversa.
void moverMotores(int32_t motorSpeedIzq, int32_t motorSpeedDer) {
    deb(Serial.printf("MotorIzq=%d\n", motorSpeedIzq);)
    deb(Serial.printf("MotorDer=%d\n", motorSpeedDer);)

    if      (motorSpeedIzq > 0) {   motorIzq.forward(motorSpeedIzq);        }
    else if (motorSpeedIzq < 0) {   motorIzq.reverse(abs(motorSpeedIzq));   }
    else                        {   motorIzq.stop();    }

    if      (motorSpeedDer > 0) {   motorDer.forward(motorSpeedDer);        }
    else if (motorSpeedDer < 0) {   motorDer.reverse(abs(motorSpeedDer));   }
    else                        {   motorDer.stop();    }   
}

// Detener ambos motores
void detenerMotores(){
    motorIzq.stop();
    motorDer.stop();
}


// Determina si el robot se encuentra dentro de la zona muerta del setpoint.
// Actualiza la bandera SETPOINT usada por la FSM.
void actualizarSP(uint16_t pos) {
    SETPOINT = (abs(pos - setpoint) < zonaMuerta);
}


// ============================
// FUNCION CONTROL MOTORES
// ============================
// Calcula las velocidades de los motores a partir de la corrección del PID.
// Si el robot no está en el setpoint, aplica corrección diferencial.
// En el setpoint, mantiene el estado sin corrección.
void controlMotores(float correcion) {
    // Si NO estoy en Zona muerta solo controlo
    if ( !SETPOINT ) {
        // Calcular velocidad motores
        motorSpeedIzq = baseSpeed - correcion;
        motorSpeedDer = baseSpeed + correcion;

        // Limitar a rango válido incluyendo negativos
        motorSpeedIzq = constrain(motorSpeedIzq, -maxSpeed, maxSpeed);
        motorSpeedDer = constrain(motorSpeedDer, -maxSpeed, maxSpeed);
        return;
    }

    // se encuentra en el setpoint
    SETPOINT = true;
}