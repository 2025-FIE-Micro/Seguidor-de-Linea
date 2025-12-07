#include "interrupciones.hpp"

/*
    Programa de interrupciones fisicas y por timer
    * BOTON DE CORRER
    * BOTON DE PARADA
    * TODO: ISR TIMER - DELTA TIME
    * TODO: SETPOINT
*/

volatile bool RUN = false;          // Flag indicador de modo corredor
volatile bool SETPOINT = true;     // Flag indicador de control

const int32_t TIEMPO_TIMER = 5000;  // Interrupción cada 5 ms - unidad en microsegundos (700 por sensor)

hw_timer_t *timer = NULL;
volatile bool has_expired = false;

void IRAM_ATTR timerInterrupcion() {
    has_expired = true;
}

// MODO CORREDOR
void IRAM_ATTR handleRun() {
    RUN = true;
    SETPOINT = true;
}

// MODO PARADO
void IRAM_ATTR handleStop(){
    RUN = false;
}