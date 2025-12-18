/**
 @file prueba_contro_IR.cpp
 @brief Programa de prueba para el control dual (Botones e Infrarrojo) de un LED.
 @details Este test verifica la integración de la librería IRremote con interrupciones 
 de hardware para conmutar el estado de un LED externo.
 @author Legion de Ohm
 */

#include <Arduino.h>
#include <IRremote.hpp>

// Pines
/** @brief Pin del LED externo de prueba. */
#define LED_EXTERNAL 13
/** @brief Pin del botón físico para encendido. */
#define BTN_ON  19
/** @brief Pin del botón físico para apagado. */
#define BTN_OFF 22
/** @brief Pin de datos del receptor IR. */
#define IR_PIN  4

// Control
/** @brief Comando hexadecimal del botón ON en el control remoto. */
#define CMD_ON   0x18
/** @brief Dato crudo (Raw) esperado para el comando ON. */
#define RAW_ON   0xE718FF00

/** @brief Comando hexadecimal del botón OFF en el control remoto. */
#define CMD_OFF  0x1C
/** @brief Dato crudo (Raw) esperado para el comando OFF. */
#define RAW_OFF  0xE31CFF00

/** @brief Macro de configuración para habilitar o deshabilitar el bloque de control IR. */
#define USAR_CONTROL_IR
#ifdef USAR_CONTROL_IR
    #define control_ir(IR) IR
#else
    #define control_ir()
#endif

/** @brief Estado lógico global del LED, modificado por ISR y decodificación IR. */
volatile bool ledState = false;

// ISR físicas
/** @brief ISR para el botón de encendido. Cambia el estado a true. */
void IRAM_ATTR handleOn()  { ledState = true;  }
/** @brief ISR para el botón de apagado. Cambia el estado a false. */
void IRAM_ATTR handleOff() { ledState = false; }

/**
 @brief Configuración inicial del test de control.
 @details Inicializa los periféricos, configura las interrupciones externas por flanco 
 ascendente e inicializa el receptor IR con feedback visual.
 */
void setup() {
  Serial.begin(115200);

  pinMode(LED_EXTERNAL, OUTPUT);
  pinMode(BTN_ON, INPUT);
  pinMode(BTN_OFF, INPUT);

  attachInterrupt(digitalPinToInterrupt(BTN_ON), handleOn, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_OFF), handleOff, RISING);

  // Inicializar receptor IR
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Sistema iniciado. Usa botones o control IR para encender/apagar el LED.");
}

/**
 @brief Lazo principal de la prueba.
 @details Procesa las señales infrarrojas recibidas, decodifica el protocolo y compara 
 los comandos con los valores autorizados (RAW_ON/OFF). Actualiza el pin del LED según ledState.
 */
void loop() {
  // --- Lectura IR ---
  control_ir(  
    if (IrReceiver.decode()) {
      auto data = IrReceiver.decodedIRData;
      uint32_t raw = data.decodedRawData;
      uint8_t cmd  = data.command;

      const char* proto = IrReceiver.getProtocolString();

      Serial.println("\n--- Señal IR recibida ---");
      Serial.printf("Protocolo: %s\n", proto);
      Serial.printf("Dirección: 0x%04X\n", data.address);
      Serial.printf("Comando  : 0x%02X\n", data.command);
      Serial.printf("Flags    : 0x%02X\n", data.flags);
      Serial.printf("RawData  : 0x%08lX\n", data.decodedRawData);
      Serial.printf("Bits     : %d\n", data.numberOfBits);
      Serial.printf("Repetido : %s\n", data.flags & IRDATA_FLAGS_IS_REPEAT ? "SI" : "NO");

      // Solo aceptar comandos si la dirección es la "autorizada"
      if (raw == RAW_ON && cmd == CMD_ON) {  // Botón ON
        ledState = true;
      }
      if (raw == RAW_OFF && cmd == CMD_OFF) {  // Botón OFF
        ledState = false;
      }
      IrReceiver.resume();
    }
  )

  // --- Actualización del LED ---
  digitalWrite(LED_EXTERNAL, ledState ? HIGH : LOW);
}