/*
    programa declaracion 
*/

#pragma once
#include <Arduino.h>

// Variable global que indica si el robot debe correr
extern volatile bool RUN;
extern volatile bool SAFE_START;

// Declaración de funciones de interrupción
void IRAM_ATTR handleRun();
void IRAM_ATTR handleStop();
