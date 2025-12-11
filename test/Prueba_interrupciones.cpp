/**
 @file prueba_interrupciones.cpp
 @brief Test de funcionamiento de las Rutinas de Servicio de Interrupción (ISR) y botones. Este test verifica que los pines configurados como BTN_ON y BTN_OFF activan las interrupciones físicas correctamente, sin depender del loop principal.
 @note La funcionalidad se valida observando el cambio de estado del LED_EXTERNAL.
 @author Legion de Ohm
*/

#include <Arduino.h>

// ============================
// DEFINICIÓN DE PINES (OUTPUTS)
// ============================
// LEDs (para visualización del estado)
const int LED_INTERNAL = 2;   ///< LED interno del módulo (Pin 2 en ESP32, si aplica).
const int LED_EXTERNAL = 13;  ///< LED externo de prueba (ej. en el kit).

// ============================
// DEFINICIÓN DE PINES (INPUTS)
// ============================
// Botones (configurados como entradas para generar interrupciones)
const int BTN_ON = 19;      ///< Pin del botón para Encender el LED_EXTERNAL.
const int BTN_OFF = 22;     ///< Pin del botón para Apagar el LED_EXTERNAL.

// ============================
// RUTINAS DE SERVICIO DE INTERRUPCIÓN (ISR)
// ============================

/**
 @brief ISR para el botón de Encendido (BTN_ON). Establece el pin LED_EXTERNAL en estado ALTO (HIGH) al detectar un flanco ascendente (RISING). Usa IRAM_ATTR para asegurar su ubicación en la memoria RAM de instrucción para ejecución rápida.
*/
void IRAM_ATTR handleOn() {
  digitalWrite(LED_EXTERNAL, HIGH);
}

/**
 @brief ISR para el botón de Apagado (BTN_OFF). Establece el pin LED_EXTERNAL en estado BAJO (LOW) al detectar un flanco ascendente (RISING).
*/
void IRAM_ATTR handleOff() {
  digitalWrite(LED_EXTERNAL, LOW);
}


/**
 @brief Función de inicialización: configura pines e interrupciones.
*/
void setup() {
  // Configuración de pines de salida y entrada
  pinMode(LED_EXTERNAL, OUTPUT);
  pinMode(BTN_ON, INPUT);
  pinMode(BTN_OFF, INPUT);

  // Adjuntar las rutinas de interrupción (handleOn/handleOff) a los pines (BTN_ON/BTN_OFF)
  // La interrupción se dispara en el flanco ascendente (RISING) al presionar el botón.
  attachInterrupt(digitalPinToInterrupt(BTN_ON), handleOn, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_OFF), handleOff, RISING);

  // Inicializar la comunicación serial
  Serial.begin(115200);
  Serial.println("Sistema iniciado. Usa BTN_ON para encender y BTN_OFF para apagar.");
}

/**
 @brief Bucle principal de ejecución. En este test, el loop principal no realiza ninguna tarea, demostrando que el control del LED es completamente manejado por las interrupciones asíncronas.
*/
void loop() {
  // No hace nada en el loop, todo es por interrupciones
}
