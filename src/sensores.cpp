/**
 @file sensores.cpp
 @brief Implementación de las funciones de configuración, calibración y lectura de línea. Contiene la lógica para la inicialización de la librería QTRSensors, la rutina de calibración (que incluye feedback visual y sonoro) y la función principal de lectura de la línea.
 @author Legion de Ohm
 */

#include <Arduino.h>
#include "sensores.hpp"
#include "motores.hpp"
#include "config.hpp"

// ===================================
// VARIABLES GLOBALES (Definidas aquí)
// ===================================

// El objeto QTR debe ser inicializado con el número máximo de pines a usar
// (asumiendo que en otro lado se define un número máximo, aquí se usa una instancia vacía).
QTRSensors qtr; 
const uint8_t SensorCount = 8;        // Asumimos 8 sensores
uint16_t sensorValues[8];             // Array para almacenar las 8 lecturas
uint16_t position = 0;

const uint8_t ledCalibracion = 13;    // Pin LED de feedback
// La instancia del Buzzer y Linea se asume que están definidas en otro lado o se inicializan aquí
// extern Buzzer buzzer; (se inicializa en buzzer.cpp o se asume un constructor por defecto)


// ============================
// FUNCION CALIBRAR
// ============================

/**
 @brief Ejecuta la rutina de calibración automática para los sensores QTR. Durante la calibración, el robot debe moverse lentamente sobre la línea y el fondo. El proceso parpadea el LED de calibración y emite tonos para indicar el inicio y fin.
 @return void
 */
void calibrarSensores() {
    // Tono de inicio de calibración (Envuelto en macro 'mute' por si está desactivado)
    mute(
        buzzer.play(NOTE_A4);
        delay(150);
        buzzer.stop();
    );

    // Asegura que los motores estén detenidos durante la calibración
    motorIzq.stop();
    motorDer.stop();

    // Feedback visual
    digitalWrite(ledCalibracion, HIGH);
    deb(Serial.println("Calibrando sensores..."); )

    // Ciclo de calibración. Se llama a la función de la librería 300 veces para cubrir
    // los rangos mínimo y máximo del fondo y la línea.
    for (uint16_t i = 0; i < 300; i++) { 
        qtr.calibrate(); 
    } 
    
    // Feedback visual y finalización
    digitalWrite(ledCalibracion, LOW);
    deb(Serial.println("Calibracion lista!");)

    // Tono de fin de calibración (más agudo)
    mute(
        buzzer.play(NOTE_C5);
        delay(200);
        buzzer.stop();
    );
}


// ============================
// LECTURA DE POSICIÓN
// ============================

/**
 @brief Lee los sensores y calcula la posición ponderada de la línea. Utiliza la constante global `linea_competencia` (NEGRA o BLANCA) para seleccionar el modo de lectura de la librería QTR (`readLineBlack` o `readLineWhite`).
 @return uint16_t La posición ponderada de la línea. (Ej. 0 a 4000).
 */
uint16_t leerLinea() {
    if (linea_competencia == BLANCA)
        // El robot busca el blanco (los valores altos en el array de sensores)
        position = qtr.readLineWhite(sensorValues);
    else
        // El robot busca el negro (los valores bajos en el array de sensores)
        position = qtr.readLineBlack(sensorValues);
    
    return position;
}


// ============================
// SETUP DE SENSORES
// ============================

/**
 @brief Inicializa el objeto QTR y configura los pines del sensor. Este método debe llamarse una sola vez en el `setup()` de Arduino.
 @param sensorPins Array que contiene los pines GPIO a los que están conectados los sensores.
 @param SensorCount Número total de sensores a inicializar.
 @return void
 */
void setupSensores(const uint8_t* sensorPins, uint8_t SensorCount) {
    // Establece el modo de lectura a analógico (si el hardware lo soporta).
    qtr.setTypeAnalog(); 
    
    // Asigna los pines y el conteo de sensores a la librería QTR.
    qtr.setSensorPins(sensorPins, SensorCount);
}