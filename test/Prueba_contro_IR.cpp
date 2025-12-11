/** 
 @file Prueba_contro_IR.cpp
 @brief Test de control híbrido que combina la lectura del receptor Infrarrojo (IR) y las Interrupciones Físicas (botones). El objetivo es encender y apagar un LED mediante:
1. La pulsación de botones físicos (BTN_ON/BTN_OFF) que disparan ISR.
2. La recepción de comandos específicos (RAW_ON/RAW_OFF) desde un control remoto IR.
 @author Legion de Ohm
*/

#include <Arduino.h>
#include <IRremote.hpp>

// ============================
// DEFINICIÓN DE PINES
// ============================
#define LED_EXTERNAL 13 ///< Pin del LED que indica el estado del sistema.
#define BTN_ON  19      ///< Pin de entrada para el botón de Encendido (ISR).
#define BTN_OFF 22      ///< Pin de entrada para el botón de Apagado (ISR).
#define IR_PIN  4       ///< Pin conectado al receptor IR.

// ============================
// CÓDIGOS DE CONTROL IR ESPERADOS
// ============================
#define CMD_ON  0x18       ///< Comando IR para Encender (8 bits).
#define RAW_ON  0xE718FF00  ///< Código RAW completo para Encender.

#define CMD_OFF 0x1C       ///< Comando IR para Apagar (8 bits).
#define RAW_OFF 0xE31CFF00  ///< Código RAW completo para Apagar.


// ============================
// MACRO DE COMPILACIÓN CONDICIONAL
// ============================
#define USAR_CONTROL_IR
#ifdef USAR_CONTROL_IR
    /** @brief Macro que habilita la sección de lectura del control IR. */
    #define control_ir(IR) IR
#else
    /** @brief Macro que deshabilita la sección de lectura del control IR (compila a vacío). */
    #define control_ir()
#endif

/** @brief Estado global del LED, modificada por las ISR y el control IR. */
volatile bool ledState = false;

// ============================
// RUTINAS DE SERVICIO DE INTERRUPCIÓN (ISR)
// ============================

/**
 @brief ISR para encender el LED. Se llama al detectar el flanco ascendente en BTN_ON.
 @note Usa IRAM_ATTR para optimizar la velocidad de ejecución en el ESP32.
*/
void IRAM_ATTR handleOn() { ledState = true; }

/**
 @brief ISR para apagar el LED. Se llama al detectar el flanco ascendente en BTN_OFF.
*/
void IRAM_ATTR handleOff() { ledState = false; }


/**
 @brief Inicializa pines, comunicación serial, receptor IR y adjunta las interrupciones.
*/
void setup() {
  Serial.begin(115200);

  pinMode(LED_EXTERNAL, OUTPUT);
  pinMode(BTN_ON, INPUT);
  pinMode(BTN_OFF, INPUT);

  // Adjuntar ISR a los pines BTN_ON y BTN_OFF, disparadas por flanco ascendente (RISING)
  attachInterrupt(digitalPinToInterrupt(BTN_ON), handleOn, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_OFF), handleOff, RISING);

  // Inicializar receptor IR y habilitar el feedback LED (si está disponible)
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Sistema iniciado. Usa botones o control IR para encender/apagar el LED.");
}


/**
 @brief Bucle principal: gestiona la lectura asíncrona del control IR y actualiza el estado del LED.
*/
void loop() {
  // --- Lectura IR (Solo se ejecuta si USAR_CONTROL_IR está definido) ---
  control_ir( 
    if (IrReceiver.decode()) {
      auto data = IrReceiver.decodedIRData;
      uint32_t raw = data.decodedRawData;
      uint8_t cmd = data.command;

      const char* proto = IrReceiver.getProtocolString();

      Serial.println("\n--- Señal IR recibida ---");
      Serial.printf("Protocolo: %s\n", proto);
      Serial.printf("Comando : 0x%02X\n", data.command);
      Serial.printf("RawData : 0x%08lX\n", data.decodedRawData);

      // Verifica el código RAW completo para mayor robustez
      if (raw == RAW_ON && cmd == CMD_ON) {
        ledState = true;
      } else
      if (raw == RAW_OFF && cmd == CMD_OFF) {
      ledState = false;
      }
      IrReceiver.resume(); // Reanudar la escucha del receptor
    }
  );

  // --- Actualización del LED ---
  // El estado se actualiza en cada ciclo del loop, reflejando el estado de ledState (modificado por ISR o IR)
  digitalWrite(LED_EXTERNAL, ledState ? HIGH : LOW);
}