/**
 @file prueba_motores.cpp
 @brief Programa de prueba para el control direccional de motores mediante el driver DRV8833.
 @details Este test verifica el funcionamiento básico de los puentes H, validando los estados 
 lógicos de los pines de dirección (adelante, atrás, freno) y el pin de habilitación (sleep/wake).
 @author Legion de Ohm
 */

#include <Arduino.h>

// ------------------ PINES ------------------
/** @name Pines Motor IZQUIERDO */
///@{
#define PinSleep_M2 23 ///< Pin de habilitación (Sleep) para motor izquierdo.
#define IN1_M2 18      ///< Pin de dirección 1 para motor izquierdo.
#define IN2_M2 21      ///< Pin de dirección 2 para motor izquierdo.
///@}

/** @name Pines Motor DERECHO */
///@{
#define PinSleep_M1 16 ///< Pin de habilitación (Sleep) para motor derecho.
#define IN1_M1 26      ///< Pin de dirección 1 para motor derecho.
#define IN2_M1 25      ///< Pin de dirección 2 para motor derecho.
///@}

// ------------------ CLASE DRIVER PUENTE H ------------------
/**
 @class DRV8833
 @brief Clase local para el control simplificado del driver de motores.
 @details Gestiona los estados lógicos de los pines de entrada del puente H para controlar el giro.
 */
class DRV8833 {
public:
    /**
     @brief Constructor de la clase DRV8833.
     @param pinSleep Pin GPIO de control de energía.
     @param pinIN1 Pin GPIO de entrada de dirección 1.
     @param pinIN2 Pin GPIO de entrada de dirección 2.
     */
    DRV8833(uint8_t pinSleep, uint8_t pinIN1, uint8_t pinIN2);

    /** @brief Configura los pines para girar el motor hacia adelante. */
    void adelante();
    /** @brief Configura los pines para girar el motor hacia atrás. */
    void atras();
    /** @brief Pone ambas entradas en LOW para detener el motor. */
    void freno();
    /** @brief Pone el pin de sleep en LOW para desactivar el driver (bajo consumo). */
    void sleep();
    /** @brief Pone el pin de sleep en HIGH para reactivar el driver. */
    void wake();   

private:
    uint8_t _pinSleep; ///< Pin de control de reposo.
    uint8_t _pinIN1;   ///< Pin de entrada 1.
    uint8_t _pinIN2;   ///< Pin de entrada 2.
};

// constructor de la clase
DRV8833::DRV8833(uint8_t pinSleep, uint8_t pinIN1, uint8_t pinIN2)
: _pinSleep(pinSleep), _pinIN1(pinIN1), _pinIN2(pinIN2) {
    pinMode(_pinSleep, OUTPUT);
    pinMode(_pinIN1, OUTPUT);
    pinMode(_pinIN2, OUTPUT);

    digitalWrite(_pinSleep, HIGH); // habilitar el driver
    freno();
}

void DRV8833::adelante() {
    digitalWrite(_pinIN1, HIGH);
    digitalWrite(_pinIN2, LOW);
}

void DRV8833::atras() {
    digitalWrite(_pinIN1, LOW);
    digitalWrite(_pinIN2, HIGH);
}

void DRV8833::freno() {
    digitalWrite(_pinIN1, LOW);
    digitalWrite(_pinIN2, LOW);
}

void DRV8833::sleep() {
    digitalWrite(_pinSleep, LOW);  // desactiva el driver
}

void DRV8833::wake() {
    digitalWrite(_pinSleep, HIGH); // activa el driver
}

// ------------------ OBJETOS ------------------
/** @brief Instancia para el control del motor izquierdo. */
DRV8833 MotorIzq(PinSleep_M2, IN1_M2, IN2_M2);
/** @brief Instancia para el control del motor derecho. */
DRV8833 MotorDer(PinSleep_M1, IN1_M1, IN2_M1);

// ------------------ PROGRAMA ------------------
/**
 @brief Configuración inicial del test de motores.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("Prueba direccional de motores");
}

/**
 @brief Ciclo de prueba de movimiento.
 @details Alterna entre marcha adelante y marcha atrás cada 2 segundos. 
 Contiene bloques comentados para pruebas de Sleep/Wake y Freno.
 */
void loop() {
    Serial.println("Adelante");
    MotorIzq.adelante();
    MotorDer.adelante();
    delay(2000);

    Serial.println("Atrás");
    MotorIzq.atras();
    MotorDer.atras();
    delay(2000);

/*
    Serial.println("Sleep");
    MotorIzq.sleep();
    MotorDer.sleep();
    delay(2000);

    Serial.println("Wake + Adelante");
    MotorIzq.wake();
    MotorDer.wake();
    MotorIzq.adelante();
    MotorDer.adelante();
    delay(2000);

    MotorIzq.freno();
    MotorDer.freno();
    delay(2000);*/
}
