#include <Arduino.h>
#include "buzzer.hpp"
#include "config.hpp"

// Instanciamos el buzzer (asegurándonos de usar el pin correcto y el canal de PWM)
Buzzer buzzer(pinBuzzer, 2);  // Usamos el pin y canal correspondientes

// Inicializa el buzzer (configura el PWM y la frecuencia)
void setupBuzzer(uint16_t freq, uint8_t resolution ) {
    buzzer.begin(freq , resolution);
}