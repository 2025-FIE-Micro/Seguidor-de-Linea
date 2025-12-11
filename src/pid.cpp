/**
 @file pid.cpp
 @brief Implementación de la función de cálculo del controlador PID. Contiene la lógica para calcular los términos Proporcional, Integral y Derivativo y obtener la corrección final aplicada a los motores.
 @author Legion de Ohm
 */

#include <Arduino.h>
#include "config.hpp"
#include "drv8833.hpp"
#include "pid.hpp"

// ===================================
// VARIABLES GLOBALES PID (Definidas aquí)
// ===================================

uint16_t setpoint = 0;
uint16_t zonaMuerta = 0;

float lastError = 0;   // Error previo         -   control D
float integral = 0;    // Acumulador           -   control I
uint32_t lastTime = 0;  // Millis previo        -   delta Tiempo


// ============================
// FUNCION CALCULO DE PID
// ============================
/**
 @brief Calcula la corrección necesaria para el sistema usando la fórmula PID. Este es el núcleo del algoritmo de control. Utiliza las constantes Kp, Ki, Kd definidas en `config.cpp` para generar un valor de corrección basado en el error actual, la acumulación del error y la tasa de cambio del error.
 @param pos Posición actual leída por los sensores. Este es el valor de entrada del proceso.
 @param deltaTime Tiempo transcurrido ($\Delta T$) desde el último cálculo de PID (en segundos).
 @return float Valor de corrección PID (output) a aplicar a las velocidades de los motores.
 */
float calculo_pid(uint16_t pos, float deltaTime) {
    // La macro deb() permite la impresión de depuración solo si está activa la bandera.
    deb(Serial.printf("deltaTime=%.6f\n", deltaTime);)

    // 1. Cálculo del Error Proporcional (P)
    // Error = Posición actual - Posición deseada (Setpoint)
    float error = pos - setpoint;
deb(Serial.printf("error=%.6f\n", error);)

    // 2. Cálculo del Término Derivativo (D)
    // Derivativo = (Cambio en el error) / (Tiempo transcurrido)
    float derivativo = (error - lastError) / deltaTime;

    // Actualizar el último error para la próxima iteración.
    lastError = error;

    // 3. Cálculo del Término Integral (I)
    // Integral se acumula: Integral = Integral previa + (Error actual * Tiempo transcurrido)
    integral += error * deltaTime;
    // *Nota: Generalmente se aplica un 'constrain' o 'anti-windup' al acumulador 'integral'
    // para evitar que crezca demasiado y cause inestabilidad.

    // 4. Cálculo de la Salida Final del PID
    // Output = (Error * Kp) + (Derivativo * Kd) + (Integral * Ki)
    float output = (error * Kp) + (derivativo * Kd) + (integral * Ki);

    deb(Serial.printf("PID=%.6f\n", output);)
    return output;
}
