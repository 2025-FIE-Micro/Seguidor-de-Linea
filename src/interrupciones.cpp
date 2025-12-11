/**
 @file interrupciones.cpp
 @brief Implementación de las funciones de servicio de interrupción (ISR) para el control de estado RUN. Contiene la definición de la variable de estado RUN y la lógica de las ISRs para cambiar dicho estado rápidamente.
 @author Legion de Ohm
 */


#include "interrupciones.hpp"

// ===================================
// DEFINICIÓN DE VARIABLE DE ESTADO
// ===================================

/**
 @var RUN
 @brief Bandera de estado que controla si el ciclo principal del robot debe ejecutarse. Se inicializa en 'false', requiriendo una interrupción externa para iniciar la operación.
 */
volatile bool RUN = false;

// ===================================
// FUNCIONES DE SERVICIO DE INTERRUPCIÓN (ISRs)
// ===================================

/**
 @brief Función de servicio de interrupción (ISR) para iniciar la rutina principal. Establece la bandera global `RUN` a `true`, permitiendo que el bucle principal (`loop`) comience a ejecutar la lógica de seguimiento de línea.
 */
void IRAM_ATTR handleRun() { 
    RUN = true; 
}

/**
 @brief Función de servicio de interrupción (ISR) para detener la rutina principal. Establece la bandera global `RUN` a `false`, lo que debería detener inmediatamente el movimiento del robot en el bucle principal.
 */
void IRAM_ATTR handleStop() { 
    RUN = false; 
}