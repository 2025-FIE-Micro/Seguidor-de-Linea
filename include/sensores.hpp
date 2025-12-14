#pragma once
#include <Arduino.h>
#include "QTRSensors.h"
// #include "buzzer.hpp"

enum Linea { BLANCA, NEGRA };

// Variables públicas del módulo
extern uint16_t position;

// API del módulo
void setupSensores();
void calibrarSensores();
uint16_t leerLinea();
