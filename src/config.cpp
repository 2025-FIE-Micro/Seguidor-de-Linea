/*
    programa implementacion de configuracion de seguidor
    * tipo de linea
    * constantes Ziglers-Nichols
    * calculo Ziglers-Nichols
*/

#include "config.hpp"


// ============================
// PINES - LEDS, BOTONES, BOCINA y SENSOR IR
// ============================
const uint8_t pinBuzzer = 17;
const uint8_t ledMotores = 13;          // LED para encender/apagar motores
const uint8_t ledCalibracion = 2;       // LED azul que indica calibración finalizada
const uint8_t BTN_RUN = 19;
const uint8_t BTN_STOP = 22;
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


// ===================================
// LINEA NEGRA o BLANCA - CAMBIAR EN PLATFORMIO.INI
// ===================================
#ifdef LINEA_NEGRA
    Linea linea_competencia = NEGRA;
#else
    Linea linea_competencia = BLANCA;
#endif 

// ===================================
// SELECCION DE CORREDOR - CAMBIAR EN PLATFORMIO.INI
// ===================================
#if (CORREDOR == NIGHTFALL)
    //Motores de 500 RPM
    #define USAR_MICROS   0         // Cambiar a 0 para millis()
    uint8_t baseSpeed = 90;         // Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.045;
    const float Tu = 0.5;
#elif (CORREDOR == ARGENTUM)
    //Motores de 800 RPM
    #define USAR_MICROS   0         // Cambiar a 0 para millis()
    uint8_t baseSpeed = 78;         // Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.05;
    const float Tu = 0.31;
#elif (CORREDOR == DIEGO)
    //Motores de 800 RPM
    #define USAR_MICROS   0         // Cambiar a 0 para millis()
    uint8_t baseSpeed = 70;         // Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.05;
    const float Tu = 0.38;
#else
  #error "Valor de CORREDOR inválido. Use NIGHTFALL, ARGENTUM o DIEGO."
#endif


// ============================
// CONTROL PID - METODO Ziegler-Nichols
// ============================
#ifdef TEST_PID
    const float Kp = Ku;
    const float Ki = 0;
    const float Kd = 0;
    #define MUTEAR
#else
    /* Valores de planta estabilizada de ejemplo:
            *P = 1.9    
            *I = 1.5    casi como p 
            *D = 0.2    valor chico
    */
    const float Kp = 0.6 * Ku;
    const float Ki = 2 * Kp / Tu;
    const float Kd = Kp * Tu / 8;
#endif