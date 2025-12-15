#pragma once
#include <Arduino.h>

enum Linea { BLANCA, NEGRA };
extern Linea linea_competencia;

// Variables públicas del módulo
extern uint16_t position;

// API del módulo
void setupSensores();
void calibrarSensores();
uint16_t leerLinea();
