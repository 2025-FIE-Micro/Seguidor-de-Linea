#include "interrupciones.hpp"

/*
    Interrupciones físicas y por timer:
    * Botón RUN: inicia ciclo y habilita control
    * Botón STOP: detiene ciclo
    * Timer: marca paso de tiempo fijo (deltaT)
*/

// Flags compartidos con la máquina de estados
volatile bool RUN = false;      // Flag indicador de modo corredor
volatile bool SETPOINT = true;  // Flag indicador de control

const int32_t TIEMPO_TIMER = 5000;  // Interrupción cada 5 ms - unidad en microsegundos (700m por sensor)

// Timer del ESP32
hw_timer_t *timer = NULL;
volatile bool has_expired = false;  // Marca que ocurrió un tick de tiempo 

// ISR del Timer: solo señaliza el evento, no ejecutar lógica aquí
void IRAM_ATTR timerInterrupcion() {
    has_expired = true;
}

// ISR Botón RUN: pasa a modo corredor (Acelerar o Control)
void IRAM_ATTR handleRun() {
    RUN = true;
    SETPOINT = true;
}

// ISR Botón STOP: deshabilita ejecución
void IRAM_ATTR handleStop(){
    RUN = false;
}