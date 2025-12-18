/**
 @file prueba_lectura.cpp
 @brief Programa de prueba para la lectura y calibración de sensores QTR.
 @details Este test permite verificar la correcta recepción de datos de los 8 sensores, 
 comparando el modo de lectura (RC o Analógico) y visualizando la posición calculada 
 de la línea a través del monitor serial.
 @author Legion de Ohm
 */

#include <Arduino.h>
#include "QTRSensors.h"

// ----------------------------
// CONFIGURACIÓN
// ----------------------------
/** @brief Define el tipo de sensor a utilizar. Si está definido usa RC, de lo contrario usa ADC. */
#define USE_RC  // Comentar para usar ADC

/** @brief Pin del LED integrado para feedback visual de calibración. */
#define LED_BUILTIN 2

/** @brief Cantidad total de sensores en el array. */
const uint8_t SensorCount = 8;

/** @brief Mapeo de pines GPIO para los 8 sensores reflectivos. */
const uint8_t sensorPins[SensorCount] = {14, 27, 33, 32, 35, 34, 39, 36};

/** @brief Almacena los valores normalizados leídos de cada sensor individual. */
uint16_t sensorValues[SensorCount];

/** @brief Variable para almacenar la posición calculada de la línea (0-7000). */
uint16_t position;

/** @brief Instancia del objeto QTRSensors para gestionar el hardware. */
QTRSensors qtr;

/** @brief Variable para el control de tiempo no bloqueante. */
unsigned long previousMillis = 0;

/** @brief Intervalo de muestreo entre lecturas en milisegundos. */
const long interval = 10; // Leer cada 10 ms

// ----------------------------
// SETUP
// ----------------------------
/**
 @brief Inicializa el puerto serial y configura el tipo de sensores y pines.
 @details Ejecuta una rutina de calibración de 400 ciclos (aprox. 10 segundos) 
 indicando el inicio y fin mediante el LED interno.
 */
void setup() {
    Serial.begin(115200);

    // Configurar tipo de sensor
    #ifdef USE_RC
        qtr.setTypeRC();
    #else
        qtr.setTypeAnalog();
        qtr.setEmitterPin(2); // Opcional solo para ADC
    #endif

    // Configurar pines
    qtr.setSensorPins(sensorPins, SensorCount);

    // Calibración inicial
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Calibrando sensores... mueve el sensor sobre la linea negra y fondo");

    for (uint16_t i = 0; i < 400; i++) {
        qtr.calibrate();
        delay(25); // opcional para no saturar el ADC/RC
    }

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Calibración finalizada.");
}

// ----------------------------
// LOOP
// ----------------------------
/**
 @brief Lazo principal de muestreo.
 @details Realiza la lectura de la línea cada 10ms. Si se usa RC, lee la línea negra; 
 si se usa Analógico, lee la línea blanca. Imprime los valores brutos y la posición por serial.
 */
void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Lectura de sensores
        #ifdef USE_RC
            qtr.read(sensorValues);                     // Leer valores RC
            position = qtr.readLineBlack(sensorValues);
        #else
            position = qtr.readLineWhite(sensorValues);
        #endif

        // Mostrar valores y posición
        for (uint8_t i = 0; i < SensorCount; i++) {
            Serial.print(sensorValues[i]);
            Serial.print('\t');
        }
        Serial.print(" | Posicion: ");
        Serial.println(position);
    }
}
