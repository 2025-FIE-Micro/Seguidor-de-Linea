/**
 @file motores.cpp
 @brief Implementación de las funciones de control de motores de alto nivel. Contiene la lógica para la asignación de velocidad (PWM) y dirección de los motores a partir de la corrección PID o movimientos sin corrección.
 @author Legion de Ohm
 */
#include <Arduino.h>
#include "sensores.hpp"
#include "config.hpp"
#include "drv8833.hpp"
#include "motores.hpp"
#include "pid.hpp"

// ===================================
// VARIABLES GLOBALES DE MOVIMIENTO (Definidas aquí)
// ===================================

int32_t motorSpeedIzq = 0;
int32_t motorSpeedDer = 0;
Drv8833 motorIzq;
Drv8833 motorDer;
const int32_t maxSpeed = 100; // Velocidad definida entre 0 y 100

// ============================
// FUNCIONES PARA MOVER MOTORES
// ============================

/**
 @brief Función de interfaz que traduce las velocidades con signo a comandos del driver. Determina la dirección de cada motor (forward, reverse, stop) basándose en el signo del valor de velocidad y llama a los métodos de la clase Drv8833.
 @param motorSpeedIzq Velocidad del motor izquierdo. Positivo = avance, Negativo = reversa.
 @param motorSpeedDer Velocidad del motor derecho. Positivo = avance, Negativo = reversa.
 @return void
 */
void moverMotores(int32_t motorSpeedIzq, int32_t motorSpeedDer) {
    // Macro de depuración para imprimir velocidades si DEBUG está activo.
    deb(Serial.printf("MotorIzq=%d\n", motorSpeedIzq);)
    deb(Serial.printf("MotorDer=%d\n", motorSpeedDer);)

    // Control del Motor Izquierdo
    if(motorSpeedIzq > 0) {
        motorIzq.forward(motorSpeedIzq);
        }
    else if (motorSpeedIzq < 0) { 
        motorIzq.reverse(abs(motorSpeedIzq)); 
    }
    else {
        motorIzq.stop();
    }

    // Control del Motor Derecho
    if(motorSpeedDer > 0) {
        motorDer.forward(motorSpeedDer);
    }
    else if (motorSpeedDer < 0) {
        motorDer.reverse(abs(motorSpeedDer));
    }
    else {
        motorDer.stop();
    }
}

/**
 @brief Mueve los motores sin considerar el signo, forzando solo el avance. Esta función se utiliza para movimientos donde la reversa no es necesaria (ej. calibración) y asegura que las velocidades estén limitadas entre 0 y 100.
 @param motorSpeedIzq Velocidad (PWM) para el motor izquierdo (se limita a 0-100).
 @param motorSpeedDer Velocidad (PWM) para el motor derecho (se limita a 0-100).
 @return void
 */
void moverMotoresSinCorrecion(int32_t motorSpeedIzq, int32_t motorSpeedDer) {
    // Limita las velocidades a un rango positivo de 0 a 100%.
    motorSpeedIzq = constrain(motorSpeedIzq, 0, 100);
    motorSpeedDer = constrain(motorSpeedDer, 0, 100);

    // Macro de depuración
    deb(Serial.printf("MotorIzq=%d\n", motorSpeedIzq);)
    deb(Serial.printf("MotorDer=%d\n", motorSpeedDer);)

    // Aplica las velocidades de avance o detiene el motor.
    if(motorSpeedIzq > 0) { 
        motorIzq.forward(motorSpeedIzq);
    }
    else { 
        motorIzq.stop();
    }
    if(motorSpeedDer > 0) {
        motorDer.forward(motorSpeedDer);
    }
    else { 
        motorDer.stop();
    }
}

// ============================
// FUNCION CONTROL MOTORES - Zona muerta o Pid
// ============================
/**
 @brief Función principal que aplica la corrección PID a las velocidades base. Implementa la lógica de la "zona muerta" (deadband) para permitir un movimiento recto sin acumular error integral cuando el error es insignificante.
 @param correcion Valor flotante de la salida del PID (error corregido).
 @return void
 */
void controlMotores(float correcion) {
    // 1. Lógica de Zona Muerta (Deadband)
    // Si la posición leída está dentro de la zona segura (cerca del setpoint).
    if (abs(position - setpoint) < zonaMuerta) {
        // Acelera gradualmente
        motorSpeedIzq++;
        motorSpeedDer++;
        correcion = 0;      // Se anula la corrección.
        integral = 0;       // Se resetea el acumulador integral (anti-windup en zona muerta).
    }
    else{
        // 2. Cálculo de velocidad con Corrección PID
        // La corrección se resta de un lado y se suma al otro para girar.
        motorSpeedIzq = baseSpeed - correcion;
        motorSpeedDer = baseSpeed + correcion;
    }

    // 3. Limitación de Velocidades
    // Limitar a rango válido [ -maxSpeed, maxSpeed ]
    motorSpeedIzq = constrain(motorSpeedIzq, -maxSpeed, maxSpeed);
    motorSpeedDer = constrain(motorSpeedDer, -maxSpeed, maxSpeed);
}
