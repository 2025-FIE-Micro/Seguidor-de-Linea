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

// MODO CORREDOR
void IRAM_ATTR handleRun() {
    RUN = true;
}

// MODO PARADO
void IRAM_ATTR handleStop(){
    RUN = false;
}

// todo: SALIDA  Y ENTRADA A SETPOINT
