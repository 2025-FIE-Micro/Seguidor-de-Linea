/**
 @file motores.hpp
 @brief Cabecera de la capa de control de motores de alto nivel. Contiene las declaraciones de las variables de velocidad globales, las instancias de los drivers DRV8833, y las funciones que aplican la corrección PID para mover el robot.
 @author Legion de Ohm
 */

#pragma once
#include <Arduino.h>
#include "config.hpp"
#include "drv8833.hpp"
#include "sensores.hpp"
#include "pid.hpp"

// ===================================
// VARIABLES GLOBALES DE MOVIMIENTO
// ===================================

/**
 @var motorSpeedIzq
 @brief Velocidad final del motor izquierdo, calculada después de aplicar la corrección PID. Este valor puede ser positivo (avance) o negativo (reversa) y se traduce a PWM.
 */
extern int32_t motorSpeedIzq;

/**
 @var motorSpeedDer
 @brief Velocidad final del motor derecho, calculada después de aplicar la corrección PID. Este valor puede ser positivo (avance) o negativo (reversa) y se traduce a PWM.
 */
extern int32_t motorSpeedDer;

/**
 @var motorIzq
 @brief Instancia global del driver DRV8833 para el motor del lado izquierdo.
 */
extern Drv8833 motorIzq;

/**
 @var motorDer
 @brief Instancia global del driver DRV8833 para el motor del lado derecho.
 */
extern Drv8833 motorDer;

/**
 @var maxSpeed
 @brief Límite máximo de velocidad (PWM) que puede alcanzar cualquier motor. Usado para asegurar que la velocidad calculada por el PID no exceda la capacidad del sistema.
 */
extern const int32_t maxSpeed;

// ===================================
// FUNCIONES DE CONTROL DE MOVIMIENTO
// ===================================

/**
 @brief Función principal de control que calcula la velocidad final de cada motor. Utiliza la velocidad base (`baseSpeed` de config.hpp) y aplica la corrección diferencial para determinar `motorSpeedIzq` y `motorSpeedDer`.
 @param correcion Valor flotante de la salida del PID (error corregido).
 @return void
 */
void controlMotores(float correcion);

/**
 @brief Mueve los motores aplicando directamente los valores de velocidad. Esta función es la interfaz de bajo nivel que llama a las funciones `forward` / `reverse`de las instancias de los drivers (`motorIzq`, `motorDer`).
 @param motorSpeedIzq Velocidad (PWM) final para el motor izquierdo. Positivo = avance.
 @param motorSpeedDer Velocidad (PWM) final para el motor derecho. Positivo = avance.
 @return void
 */
void moverMotores(int32_t motorSpeedIzq, int32_t motorSpeedDer);

/**
 @brief Mueve los motores sin aplicar la corrección. Útil para movimientos rectos o debugging.
 @param motorSpeedIzq Velocidad (PWM) para el motor izquierdo.
 @param motorSpeedDer Velocidad (PWM) para el motor derecho.
 @return void
 */
void moverMotoresSinCorrecion(int32_t motorSpeedIzq, int32_t motorSpeedDer);