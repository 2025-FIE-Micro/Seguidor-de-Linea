/**
 @file prueba_interrupciones.cpp
 @brief Programa de prueba para la gestión de interrupciones externas mediante botones.
 @details Este test verifica la respuesta inmediata del hardware ante señales de entrada 
 externas (botones) utilizando las funciones de interrupción de Arduino/ESP32 para 
 conmutar un LED sin intervención del lazo principal.
 @author Legion de Ohm
 */

#include <Arduino.h>

// LEDs
/** @brief Pin del LED interno de la placa. */
const int LED_INTERNAL = 2;
/** @brief Pin del LED externo utilizado para la prueba. */
const int LED_EXTERNAL = 13;

// Botones
/** @brief Pin del botón asignado para la acción de encendido. */
const int BTN_ON  = 19;   // enciende LED
/** @brief Pin del botón asignado para la acción de apagado. */
const int BTN_OFF = 22;  // apaga LED

// ISR: botón ON
/**
 @brief ISR para el encendido del LED.
 @details Se ejecuta inmediatamente al detectar un flanco ascendente en BTN_ON.
 */
void IRAM_ATTR handleOn() {
  digitalWrite(LED_EXTERNAL, HIGH);
}

// ISR: botón OFF
/**
 @brief ISR para el apagado del LED.
 @details Se ejecuta inmediatamente al detectar un flanco ascendente en BTN_OFF.
 */
void IRAM_ATTR handleOff() {
  digitalWrite(LED_EXTERNAL, LOW);
}

/**
 @brief Configuración inicial del hardware de interrupciones.
 @details Define los modos de los pines y vincula los disparadores de interrupción 
 por flanco de subida (RISING) a sus respectivas funciones controladoras.
 */
void setup() {
  pinMode(LED_EXTERNAL, OUTPUT);
  pinMode(BTN_ON, INPUT);
  pinMode(BTN_OFF, INPUT);

  attachInterrupt(digitalPinToInterrupt(BTN_ON), handleOn, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_OFF), handleOff, RISING);

  Serial.begin(115200);
  Serial.println("Sistema iniciado. Usa BTN_ON para encender y BTN_OFF para apagar.");
}

/**
 @brief Lazo principal.
 @details En este programa de prueba, el lazo se mantiene vacío ya que el control 
 es gestionado íntegramente por hardware a través de las interrupciones.
 */
void loop() {
  // No hace nada en el loop, todo es por interrupciones
}
