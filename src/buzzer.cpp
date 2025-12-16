#include <Arduino.h>
#include "buzzer.hpp"
#include "config.hpp"

// Constructor
Buzzer::Buzzer(uint8_t buzzerPin, uint8_t pwmChannel) {
  pin = buzzerPin;
  channel = pwmChannel;
}

// Inicializa el buzzer (PWM)
void Buzzer::begin(uint16_t freq, uint8_t resolution) {
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(pin, channel);
}

// Toca una nota (frecuencia en Hz)
void Buzzer::play(uint16_t freq) {
  ledcWriteTone(channel, freq);
}

// Detiene el sonido
void Buzzer::stop() {
  ledcWriteTone(channel, 0);
}

// Instancia del buzzer
Buzzer buzzer(pinBuzzer, 2);  // Usamos el pin y canal correspondientes

// Inicializa el buzzer (configura el PWM y la frecuencia)
void setupBuzzer(uint16_t freq, uint8_t resolution ) {
    buzzer.begin(freq , resolution);
}