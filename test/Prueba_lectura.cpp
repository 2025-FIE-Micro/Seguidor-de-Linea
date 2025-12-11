/**
 @file prueba_lectura.cpp
 @brief Test para la inicialización, calibración y lectura periódica de los sensores QTR.Este test verifica que la librería QTRSensors está configurada correctamente para los pines asignados, y que la calibración ajusta los valores de umbral. La salida serial muestra los valores crudos de los 8 sensores y la posición calculada.
 @note La calibración debe realizarse moviendo el sensor sobre la línea y el fondo.
 @see QTRSensors.h
 @author Legion de Ohm
*/

#include <Arduino.h>
#include "QTRSensors.h"

// ============================
// CONFIGURACIÓN GLOBAL
// ============================
#define USE_RC  // Descomentar para usar el modo de lectura RC (medición de tiempo de carga/descarga). Comentar para usar ADC (lectura analógica).
#define LED_BUILTIN 2 // Pin del LED para indicar el estado (ej. calibración)

const uint8_t SensorCount = 8;                                              ///< Número total de sensores de línea utilizados.
const uint8_t sensorPins[SensorCount] = {14, 27, 33, 32, 35, 34, 39, 36};   ///< Pines del ESP32 conectados a las salidas de los sensores.
uint16_t sensorValues[SensorCount];                                         ///< Array para almacenar los valores brutos leídos de cada sensor.
uint16_t position;                                                          ///< Variable que almacena la posición de la línea calculada (0-7000).

QTRSensors qtr;                                                             ///< Instancia global de la clase QTRSensors.

unsigned long previousMillis = 0; // Almacena el tiempo de la última lectura
const long interval = 10;         ///< Intervalo de tiempo (en ms) entre lecturas del sensor.

// ============================
// FUNCIÓN SETUP
// ============================
/**
 @brief Inicializa la comunicación serial, configura el tipo de sensor (RC o Analógico) y realiza la calibración.
*/
void setup() {
    Serial.begin(115200);

    // Configurar tipo de sensor según la macro USE_RC
    #ifdef USE_RC
        qtr.setTypeRC(); // Usa el método de tiempo de descarga RC
    #else
        qtr.setTypeAnalog(); // Usa el conversor Analógico/Digital (ADC)
        qtr.setEmitterPin(2); // Opcional: Pin para controlar los LEDs emisores (solo para ADC si se requiere control)
    #endif

    // Asignar los pines configurados a la instancia de la librería
    qtr.setSensorPins(sensorPins, SensorCount);

    // Calibración inicial: 400 lecturas para establecer máximos y mínimos
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // Enciende el LED para indicar Calibración
    Serial.println("Calibrando sensores... mueve el sensor sobre la linea negra y fondo");

    // Proceso de calibración (400 ciclos)
    for (uint16_t i = 0; i < 400; i++) {
        qtr.calibrate();
        delay(25); 
    }

    digitalWrite(LED_BUILTIN, LOW); // Apaga el LED: Calibración finalizada
    Serial.println("Calibración finalizada.");
    // 
}

// ============================
// FUNCIÓN LOOP
// ============================
/**
 @brief Bucle principal de lectura periódica. Realiza una lectura de los sensores a intervalos definidos por 'interval' y muestra la posición calculada y los valores brutos de los sensores por serial.
*/
void loop() {
    unsigned long currentMillis = millis();

    // Control de tiempo: solo lee cada 10 ms
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Lectura de sensores y cálculo de posición
        #ifdef USE_RC
            // En el modo RC, la lectura se realiza antes de calcular la posición
            qtr.read(sensorValues); 
            position = qtr.readLineBlack(sensorValues); // Asume línea negra
        #else
            // En el modo Analógico (ADC), se llama directamente a la función de posición
            position = qtr.readLineWhite(sensorValues); // Asume línea blanca
        #endif

        // Mostrar valores y posición
        for (uint8_t i = 0; i < SensorCount; i++) {
            Serial.print(sensorValues[i]);
            Serial.print('\t'); // Separador para fácil lectura en Plotter Serial
        }
        Serial.print(" | Posicion: ");
        Serial.println(position);
    }
}
