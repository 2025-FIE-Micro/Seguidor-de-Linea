/*
    programa implementacion de pines de seguidor
    * botones
    * leds
    * sensores
*/

#include "config.hpp"


// ============================
// PINES - LEDS, BOTONES, BOCINA y SENSOR IR
// ============================
const uint8_t pinBuzzer = 17;
const uint8_t ledMotores = 13;          // LED para encender/apagar motores
const uint8_t ledCalibracion = 2;       // LED azul que indica Calibración finalizada y Setpoint
const uint8_t BTN_RUN = 19;             // Boton SW nro PAR
const uint8_t BTN_STOP = 22;            // Boton SW nro IMPAR
//const uint8_t IR_PIN = 4;

// ============================
// CONFIGURACIÓN DE MOTORES
// ============================ 
const uint8_t motorPinIN1_Izq = 21;   const uint8_t motorPinIN2_Izq = 18;
const uint8_t motorPinSleep_Izq = 23;

// MOTOR DERECHO
const uint8_t motorPinIN1_Der = 26;   const uint8_t motorPinIN2_Der = 25;
const uint8_t motorPinSleep_Der = 16;

// ============================
// CONFIGURACIÓN SENSORES QTR & LINEA
// ============================
const uint8_t S8 = 14;  const uint8_t S7 = 27;  const uint8_t S6 = 33;  const uint8_t S5 = 32;
const uint8_t S4 = 35;  const uint8_t S3 = 34;  const uint8_t S2 = 39;  const uint8_t S1 = 36;