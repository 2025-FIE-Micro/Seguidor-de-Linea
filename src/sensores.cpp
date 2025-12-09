/*
    programa de lectura y setup de sensores QTR8A
    * calibracion de sensores
    * lectura de linea (B o N)
    * setup - tipo de sensor analogico, pines y nro de sensores
*/

#include <Arduino.h>
#include "sensores.hpp"
#include "motores.hpp"
#include "config.hpp"


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

    // Por seguridad el robot debe estar detenido durante la calibración
    motorIzq.stop();
    motorDer.stop();

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


// ============================
// SETUP DE SENSORES
// ============================
void setupSensores(const uint8_t* sensorPins, uint8_t SensorCount) {
    // Usaremos lectura analógica (ADC)
    qtr.setTypeAnalog();
    
    // Cargamos los pines de sensores QTR
    qtr.setSensorPins(sensorPins, SensorCount);
}
