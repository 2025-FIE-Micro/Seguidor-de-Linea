/**
 @file pid.hpp
 @brief Cabecera para las variables y funciones del controlador Proporcional-Integral-Derivativo (PID). Contiene las declaraciones de las variables de estado (error, integral, tiempo) la función principal para calcular la corrección necesaria para seguir la línea.
 @author Legion de Ohm
 */
#pragma once

// ===================================
// VARIABLES DE REFERENCIA Y ZONA SEGURA
// ===================================

/**
 @var setpoint
 @brief Valor deseado del error o posición (referencia). Generalmente se establece en el centro de la pista (ej. 0 o 2500), donde el error debe ser cero.
 */
extern uint16_t setpoint;

/**
 @var zonaMuerta
 @brief Rango de error permitido alrededor del setpoint donde no se aplica corrección (deadband). Usado en `controlMotores` para evitar oscilaciones innecesarias del robot.
 */
extern uint16_t zonaMuerta;

// ===================================
// VARIABLES DE ESTADO PID
// ===================================

/**
 @var lastError
 @brief Último valor de error calculado en la iteración anterior. Es esencial para el término Derivativo (D) del PID, ya que calcula el cambio en el error.
 */
extern float lastError;

/**
 @var integral
 @brief Acumulador del error a lo largo del tiempo. Es esencial para el término Integral (I) del PID, ayudando a corregir errores persistentes.
 */
extern float integral;

/**
 @var lastTime
 @brief Marca de tiempo (millis o micros) del último cálculo de PID. Se utiliza para calcular el tiempo transcurrido ($\Delta T$), necesario para los términos Integral y Derivativo.
 */
extern uint32_t lastTime;

// ===================================
// FUNCIÓN PRINCIPAL
// ===================================

/**
 @brief Calcula la corrección necesaria para el sistema usando la fórmula PID. La salida es la corrección que se suma o resta de la velocidad base de los motores. La fórmula general es: $ \text{Salida} = K_p \cdot \text{Error} + K_i \cdot \text{Integral} + K_d \cdot \text{Derivativo} $
 @param pos Posición actual leída por los sensores (el valor del error de entrada).
 @param deltaTime Tiempo transcurrido ($\Delta T$) desde la última vez que se llamó a la función (en segundos).
 @return float Valor de corrección PID a aplicar a las velocidades de los motores.
 */
float calculo_pid(uint16_t pos, float deltaTime);
