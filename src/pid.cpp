/*
    programa de calculo de controlador
    * contralor PID
    * selecciona el seguidor
    * constantes PID
*/

#include <Arduino.h>
#include "config.hpp"
//#include "drv8833.hpp"
#include "pid.hpp"

// Variables auxiliares para el calculo PID 
float  lastError = 0;   // Error previo         -   control D
float  integral = 0;    // Acumulador           -   control I
uint32_t lastTime = 0;  // Millis previo        -   delta Tiempo


// ===================================
// SELECCION DE CORREDOR - CAMBIAR EN PLATFORMIO.INI
// ===================================
#if (CORREDOR == NIGHTFALL)
    uint8_t baseSpeed = 70;         // Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.065;         //buena combinacion 75 0.065 0.350 (max 92)
    const float Tu = 0.350;
#elif (CORREDOR == ARGENTUM)
    uint8_t baseSpeed = 78;         // Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.05;
    const float Tu = 0.31;
#elif (CORREDOR == DIEGO)
    uint8_t baseSpeed = 70;         // Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.05;
    const float Tu = 0.38;
#else
  #error "Valor de CORREDOR inválido. Use NIGHTFALL, ARGENTUM o DIEGO."
#endif


// ============================
// CONTROL PID - METODO Ziegler-Nichols
// ============================
#ifdef TEST_PID
    const float Kp = Ku;
    const float Ki = 0;
    const float Kd = 0;
#else
    const float Kp = 0.6 * Ku;
    const float Ki = 2 * Kp / Tu;
    const float Kd = Kp * Tu / 8;
#endif


// ============================
// FUNCION CALCULO DE PID
// ============================
float calculo_pid(uint16_t pos, float deltaTime) {
    // Calcular deltaTime
    //float  deltaTime = (now - lastTime) / TIME_DIVISOR; // Convertir a segundos
    deb(Serial.printf("deltaTime=%.3f\n", deltaTime);)

    // Calcular el error
    float  error = pos - setpoint;               // valores entre -3500 y 3500
    
    // Calcular derivativo (tasa de cambio del error)
    float  derivativo = (error - lastError) / deltaTime;

    // Actualizar el último error proporcional
    lastError = error;

    // Calcular integral (acumulación del error)
    integral += error * deltaTime;

    // Calcular la salida del PID
    float  output = (error * Kp) + (derivativo * Kd) + (integral * Ki);

    deb(Serial.printf("PID=%.6f\n", output);)
    return output;
}


// reinicio la constante intergrativa y derivativa.
void reiniciar_pid() {
    lastError = 0; integral =0;
}