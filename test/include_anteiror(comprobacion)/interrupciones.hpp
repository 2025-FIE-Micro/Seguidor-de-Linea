/*
    programa declaracion 
*/

#pragma once
#include <Arduino.h>

// ============================
// FLAGS COMPARTIDOS
// ============================
extern volatile bool RUN;
extern volatile bool SETPOINT;
extern volatile bool has_expired;

// ============================
// TIEMPO DE CONTROL
// ============================
extern const int32_t TIEMPO_TIMER;   // us
extern const float FIXED_DT_S;       // s

// ============================
// INICIALIZACIÓN
// ============================
void setupInterrupciones();

// ============================
// ISRs
// ============================
void IRAM_ATTR handleRun();
void IRAM_ATTR handleStop();
void IRAM_ATTR timerInterrupcion();
