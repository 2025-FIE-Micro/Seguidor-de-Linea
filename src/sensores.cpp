/*
    programa de lectura y setup de sensores QTR8A
    * calibracion de sensores
    * tipo de linea
    * lectura de linea (B o N)
    * setup - tipo de sensor analogico, pines y nro de sensores
*/

#include "sensores.hpp"
#include "QTRSensors.h"
#include "config.hpp"
#include "motores.hpp"
#include "buzzer.hpp"

// ============================
// CONFIGURACIÓN QTR
// ============================
// Objeto estatico para manejar los sensores QTR - inicializa una sola vez y mantiene su valor
static QTRSensors qtr;

// Numero de sensores QTR (8 sensores en total, desde S1 a S8)
static const uint8_t SensorCount = 8;

// Definicion de los pines del sensor QTR (cada pin esta asignado a un sensor de la lista S1 a S8).
static const uint8_t sensorPins[SensorCount] = {S8, S7, S6, S5, S4, S3, S2, S1};

// Array para almacenar los valores de lectura de cada sensor QTR - 8 elementos c/u con valor de sensor individual.
static uint16_t sensorValues[SensorCount];

// Variable para almacenar la posición de la línea detectada por los sensores QTR
uint16_t position;

// ===================================
// LINEA NEGRA o BLANCA - CAMBIAR EN PLATFORMIO.INI
// ===================================
/**
 @var linea_competencia
 @brief Almacena el modo de lectura actual (BLANCA o NEGRA) para el procesamiento de las señales.
 */
#ifdef LINEA_NEGRA
    Linea linea_competencia = NEGRA;
#else
    Linea linea_competencia = BLANCA;
#endif 

// ============================
// SETUP DE SENSORES
// ============================
void setupSensores() {
    // Usaremos lectura analógica (ADC)
    qtr.setTypeAnalog();
    
    // Cargamos los pines de sensores QTR
    qtr.setSensorPins(sensorPins, SensorCount);

    // Calibracion inicial
    calibrarSensores();
}

// ============================
// FUNCION CALIBRAR
// ============================
void calibrarSensores() {
    // Aviso sonoro de inicio de calibración (solo si mute está habilitado)
    mute(
        buzzer.play(NOTE_A4);
        delay(150);
        buzzer.stop();
    );

    // Encendemos el LED de estado para indicar proceso de calibración
    digitalWrite(ledCalibracion, HIGH);
    deb(Serial.println("Calibrando sensores..."); )

    // Se realizan múltiples lecturas para que la librería QTR tome
    // valores mínimo y máximo de cada sensor y ajuste su calibración
    for (uint16_t i = 0; i < 300; i++) { qtr.calibrate(); }    

    // Apagamos indicador de calibración
    digitalWrite(ledCalibracion, LOW);
    deb(Serial.println("Calibracion lista!");)

    // Aviso de final de calibración (solo si mute está habilitado)
    mute(
        buzzer.play(NOTE_C5);
        delay(200);
        buzzer.stop();
    )
}


// ============================
// LECTURA DE POSICIÓN
// ============================
uint16_t leerLinea() {
    // Dependiendo del color de la pista, se usa lectura inversa:
    if (linea_competencia == BLANCA)    position = qtr.readLineWhite(sensorValues);
    else                                position = qtr.readLineBlack(sensorValues);

    // Devuelve un valor entre ~0 (izquierda) y ~7000 (derecha)
    return position;
}