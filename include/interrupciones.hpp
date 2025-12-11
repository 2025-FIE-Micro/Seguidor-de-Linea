/**
 @file interrupciones.hpp
 @brief Cabecera para la gestión de las interrupciones externas (ISR). Este módulo maneja la lógica de inicio y detención de la rutina principal del robot a través de un pin de interrupción (ej. un botón o jumper de seguridad).
 @author Legion de Ohm
 */

#pragma once
#include <Arduino.h>

// ===================================
// VARIABLES GLOBALES DE ESTADO
// ===================================

/**
 @var RUN
 @brief Bandera de estado que controla si el ciclo principal del robot debe ejecutarse. El calificador `volatile` es crucial para indicar que esta variable puede ser modificada por una función de interrupción (ISR) en cualquier momento y debe ser leída directamente de la memoria en el ciclo principal (`loop`).
 */
extern volatile bool RUN;

// ===================================
// FUNCIONES DE SERVICIO DE INTERRUPCIÓN (ISRs)
// ===================================

/**
 @brief Función de servicio de interrupción (ISR) para iniciar la rutina principal. El atributo `IRAM_ATTR` se utiliza en el ESP32 para colocar la función en la RAM interna, asegurando que se pueda ejecutar rápidamente sin problemas de caché o latencia, lo cual es vital para las interrupciones. Esta función probablemente establece `RUN = true;`.
 */
void IRAM_ATTR handleRun();

/**
 @brief Función de servicio de interrupción (ISR) para detener la rutina principal. Se ejecuta cuando se detecta el evento de detención en el pin de interrupción. Esta función probablemente establece `RUN = false;`.
 */
void IRAM_ATTR handleStop();