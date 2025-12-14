/*
    programa declaracion 
*/

#pragma once
#include <Arduino.h>

// Variable global que indica si el robot debe correr
extern volatile bool RUN;
extern volatile bool SETPOINT;
extern volatile bool has_expired;

extern hw_timer_t *timer;

// Declaración de funciones de interrupción
void IRAM_ATTR handleRun();
void IRAM_ATTR handleStop();
void IRAM_ATTR timerInterrupcion();
