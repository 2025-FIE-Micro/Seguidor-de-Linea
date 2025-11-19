#include "config.hpp"


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