/**
 @file prueba_motores.ino
 @brief Test básico de funcionalidad direccional del driver DRV8833 (Puente H). Este test verifica las conexiones de los pines de control (IN1, IN2, SLEEP) y valida las funciones direccionales (adelante, atrás, freno) y de energía (sleep, wake) del motor en modo Digital (sin control PWM).
 @author Legion de Ohm
*/

#include <Arduino.h>

// ------------------ PINES ------------------
// Motor IZQUIERDO (M2)
#define PinSleep_M2 23 ///< Pin SLEEP del driver del motor izquierdo.
#define IN1_M2 18      ///< Pin de entrada IN1 del driver del motor izquierdo.
#define IN2_M2 21      ///< Pin de entrada IN2 del driver del motor izquierdo.

// Motor DERECHO (M1)
#define PinSleep_M1 16 ///< Pin SLEEP del driver del motor derecho.
#define IN1_M1 26      ///< Pin de entrada IN1 del driver del motor derecho.
#define IN2_M1 25      ///< Pin de entrada IN2 del driver del motor derecho.

// ------------------ CLASE DRIVER PUENTE H (SIMPLE) ------------------
/**
 @class DRV8833
 @brief Implementación simplificada del driver DRV8833 para control direccional digital.
 @note Esta clase es local a la prueba y no incluye la funcionalidad de PWM.
*/
class DRV8833 {
public:
    /**
    @brief Constructor de la clase DRV8833.
    @param pinSleep Pin SLEEP del driver.
    @param pinIN1 Pin de entrada IN1.
    @param pinIN2 Pin de entrada IN2.
    */
    DRV8833(uint8_t pinSleep, uint8_t pinIN1, uint8_t pinIN2);

    /** @brief Configura el motor para moverse hacia adelante (HIGH/LOW). */
    void adelante();
    /** @brief Configura el motor para moverse hacia atrás (LOW/HIGH). */
    void atras();
    /** @brief Frena el motor (cortocircuitando, LOW/LOW). */
    void freno();
    /** @brief Pone el driver en modo de bajo consumo (desactiva, SLEEP=LOW). */
    void sleep();
    /** @brief Activa el driver para su uso (SLEEP=HIGH). */
    void wake();

private:
    uint8_t _pinSleep;
    uint8_t _pinIN1;
    uint8_t _pinIN2;
};

// Implementación del constructor
DRV8833::DRV8833(uint8_t pinSleep, uint8_t pinIN1, uint8_t pinIN2)
: _pinSleep(pinSleep), _pinIN1(pinIN1), _pinIN2(pinIN2) {
    pinMode(_pinSleep, OUTPUT);
    pinMode(_pinIN1, OUTPUT);
    pinMode(_pinIN2, OUTPUT);

    digitalWrite(_pinSleep, HIGH); // habilitar el driver al inicio
    freno(); // Inicializar detenido
}

// Métodos de control de dirección
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
    digitalWrite(_pinSleep, LOW);  // desactiva el driver
}

void DRV8833::wake() {
    digitalWrite(_pinSleep, HIGH); // activa el driver
}

// ------------------ OBJETOS GLOBALES ------------------
DRV8833 MotorIzq(PinSleep_M2, IN1_M2, IN2_M2); ///< Instancia para el motor izquierdo.
DRV8833 MotorDer(PinSleep_M1, IN1_M1, IN2_M1); ///< Instancia para el motor derecho.

// ------------------ PROGRAMA PRINCIPAL ------------------
/**
 @brief Inicialización de la comunicación serial.
*/
void setup() {
    Serial.begin(115200);
    Serial.println("Prueba direccional de motores");
}

/**
 @brief Bucle principal de prueba direccional. Ejecuta secuencias de movimiento (adelante y atrás) con pausas de 2 segundos. El código de prueba de Sleep/Wake y Freno está comentado.
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
    delay(2000);
*/
}
