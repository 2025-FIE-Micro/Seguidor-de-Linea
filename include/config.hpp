/*
  programa declaracion de variables seguidor:
  * define debug, muteo, control IR
  * pines de seguidor
  * nro de seguidor (Argentum, Diego y Nightfall)
*/

#pragma once
#include <Arduino.h>

// ============================
// PINES - LEDS, BOTONES, BOCINA y SENSOR IR
// ============================
extern const uint8_t pinBuzzer;
extern const uint8_t ledMotores;
extern const uint8_t ledCalibracion;
extern const uint8_t BTN_RUN;
extern const uint8_t BTN_STOP;

// ============================
// CONFIGURACIÓN DE MOTORES
// ============================
extern const uint8_t motorPinIN1_Izq;
extern const uint8_t motorPinIN2_Izq;
extern const uint8_t motorPinSleep_Izq;

extern const uint8_t motorPinIN1_Der;
extern const uint8_t motorPinIN2_Der;
extern const uint8_t motorPinSleep_Der;

// ============================
// CONFIGURACIÓN SENSORES QTR & LINEA
// ============================
extern const uint8_t S8;
extern const uint8_t S7;
extern const uint8_t S6;
extern const uint8_t S5;
extern const uint8_t S4;
extern const uint8_t S3;
extern const uint8_t S2;
extern const uint8_t S1;


// ===================================
// MODO DEBUG - CAMBIAR EN PLATFORMIO.INI
// ===================================
#ifdef DEBUG
    // Hacemos lo que esta dentro
    #define deb(x) x
#else
    // No hacemos lo que esta dentro
    #define deb(x)
#endif


// ===================================
// SELECCION DE CORREDOR - CAMBIAR EN PLATFORMIO.INI
// ===================================
#define NIGHTFALL 1
#define ARGENTUM  2
#define DIEGO     3
#ifndef CORREDOR
  #error "Debe definir CORREDOR en platformio.ini (ej: -D CORREDOR=NIGHTFALL)"
#endif


// ===================================
// MODO MUTEADO - CAMBIA CUANDO SE HACE O NO PRUEBA
// ===================================
#ifdef MUTEAR
    // No sonamos la bocina
    #define mute(bz)
#else
    // Sonamos la bocina
    #define mute(bz) bz
#endif


// ===================================
// USAR CONTROL IR - CAMBIA CUANDO LO USAMOS Y CUANDO NO
// ===================================
#ifdef USAR_CONTROL_IR
    // Usamos el control
    #define control_ir(IR) IR
#else
    // No usamos el control
    #define control_ir(IR)
#endif