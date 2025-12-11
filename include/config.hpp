/**
 @file config.hpp
 @brief Archivo de configuración global que define las constantes, banderas de compilación y la selección de perfil de corredor para el sistema PID.
 @author Legion de Ohm
 */
#pragma once
#include <Arduino.h>
#include "sensores.hpp"

/**
 @var linea_competencia
 @brief Objeto global de la clase Linea que gestiona la lectura y calibración de los sensores QTR.
 */
extern Linea linea_competencia;

/**
 @var baseSpeed
 @brief Velocidad base de los motores que se usa como punto de partida para los cálculos PID.Este valor es ajustado por la corrección PID.
 */
extern uint8_t baseSpeed;


/**
 @name Constantes Ziegler-Nichols
 @brief Constantes experimentales obtenidas del método de Ziegler-Nichols para el PID de los seguidores de línea.
 */
///@{
/**
 @var Ku
 @brief Ganancia crítica obtenida experimentalmente para cada seguidor de línea.
 */
extern const float Ku;
/**
 @var Tu
 @brief Periodo o tiempo de oscilación crítica, obtenida de la grabación de oscilaciones sostenidas de cada seguidor de línea.
 */
extern const float Tu;
///@}

/**
 @name Constantes PID
 @brief Constantes Proporcional, Integral y Derivativa utilizadas en el algoritmo final.
 */
///@{
/**
 @var Kp
 @brief Constante Proporcional.
 */
extern const float Kp;
/**
 @var Ki
 @brief Constante Integral.
 */
extern const float Ki;
/**
 @var Kd
 @brief Constante Derivativa.
 */
extern const float Kd;
///@}


/**
 @def DEBUG
 @brief Habilita los mensajes de depuración en el puerto serial. Macro que ejecuta el código 'x' si DEBUG está definido. Macro vacía (no hace nada) si DEBUG no está definido. Se activa añadiendo `-D DEBUG` en `platformio.ini`.
 */
#ifdef DEBUG
  #define deb(x) x
#else
  #define deb(x)
#endif


/**
 @def USAR_MICROS
 @brief Bandera de compilación para seleccionar la precisión de tiempo. Si está definida se ejecuta la función que retorna el tiempo transcurrido en microsegundos. Si no está definida, se ejecuta la función que convierte el tiempo de microsegundos a segundos.
 */
#if USAR_MICROS
  #define tiempoActual() micros()
  const float TIME_DIVISOR = 1000000.0;
#else
  #define tiempoActual() millis()
  const float TIME_DIVISOR = 1000.0;
#endif


/**
 @name Perfiles de Corredor
 @brief Identificadores numéricos para cada perfil de constantes PID y calibración.
 */
///@{
/** 
 @def NIGHTFALL
 @brief Perfil de constantes NIGHTFALL. Valor: 1.
*/
#define NIGHTFALL 1
/** 
 @def ARGENTUM
 @brief Perfil de constantes ARGENTUM. Valor: 2.
*/
#define ARGENTUM  2
/** 
 @def DIEGO
 @brief Perfil de constantes DIEGO. Valor: 3.
*/
#define DIEGO     3
/**
 @def CORREDOR
 @brief La macro definida externamente (-D CORREDOR=...) que selecciona el perfil activo. Su valor debe ser uno de los identificadores numéricos definidos arriba (1, 2 o 3).
 @attention La compilación fallará si no se define en `platformio.ini` un valor válido para CORREDOR.
 */
#ifndef CORREDOR
  #error "Debe definir CORREDOR en platformio.ini (ej: -D CORREDOR=NIGHTFALL)"
#endif
///@}


/**
 @def MUTEAR
 @brief Bandera de compilación para silenciar (mutear) todas las salidas de audio (buzzer). Se activa añadiendo `-D MUTEAR` en `platformio.ini`.
 */
#ifdef MUTEAR
    /** 
     @def mute(bz)
     @brief Macro que no ejecuta código si MUTEAR está definido.
    */
    #define mute(bz)
#else
    /** 
     @def mute(bz)
     @brief Macro que ejecuta la función de sonido `bz` si MUTEAR está definido.
    */    
   #define mute(bz) bz
#endif


/**
 @def USAR_CONTROL_IR
 @brief Bandera de compilación para habilitar la lógica de control remoto IR. Si no está definido, todo el código relacionado con la recepción IR se ignora.
*/
#ifdef USAR_CONTROL_IR
    /** 
     @def control_ir(IR)
     @brief Macro que ejecuta el código 'IR' si USAR_CONTROL_IR está definido.
    */
    #define control_ir(IR) IR
#else
    /**
     @def control_ir(IR)
     @brief Macro que no ejecuta código si USAR_CONTROL_IR no está definido.
    */
    #define control_ir(IR)
#endif