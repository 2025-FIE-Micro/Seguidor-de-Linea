/*
    Interrupciones físicas y por timer:
    * Botón RUN: inicia ciclo y habilita control
    * Botón STOP: detiene ciclo
    * Timer: marca paso de tiempo fijo (deltaT)
*/

#include "interrupciones.hpp"
#include "config.hpp"

// ============================
// FLAGS
// ============================
volatile bool RUN = false;
volatile bool SETPOINT = true;
volatile bool has_expired = false;

// ============================
// TIMER
// ============================
// Timer del ESP32
static hw_timer_t *timer = NULL;

// Tiempo base del control
const int32_t TIEMPO_TIMER = 6000;              // unidad en us
const float FIXED_DT_S = TIEMPO_TIMER * 1e-6f;  // unidad en s

// ============================
// ISR TIMER
// ============================
// ISR del Timer: solo señaliza el evento
void IRAM_ATTR timerInterrupcion() {
    has_expired = true;
}

// ============================
// ISR BOTONES
// ============================
// ISR Botón RUN: pasa a modo corredor (Acelerar o Control)
void IRAM_ATTR handleRun() {
    RUN = true;
    SETPOINT = true;
}

// ISR Botón STOP: deshabilita ejecución
void IRAM_ATTR handleStop() {
    RUN = false;
}

// ============================
// SETUP INTERRUPCIONES
// ============================
// Funcion para iniciar interrupciones y el timer
void setupInterrupciones() {
    // Interrupciones FISICAS de arranque y parada
    attachInterrupt(digitalPinToInterrupt(BTN_RUN), handleRun, RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_STOP), handleStop, RISING);

    // Interrupciones por TIMER - Creamos el Timer 0, dividimos los 80MHz en 80 - cada 1us
    timer = timerBegin(0, 80, true);

    // Asocia la funcion de interrupcion (ISR)
    timerAttachInterrupt(timer, &timerInterrupcion, true);
    
    // Configura cada cuánto se activa la interrupcion
    timerAlarmWrite(timer, TIEMPO_TIMER, true);
   
    // Enciende el timer para que empiece a generar las interrupciones
    timerAlarmEnable(timer);
}