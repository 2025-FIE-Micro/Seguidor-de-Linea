/**
 @file main.cpp
 @brief Programa principal para el seguidor de línea de competencia (Control PID). Este archivo ensambla todos los módulos del robot: sensores, drivers de motor, buzzer, y la lógica de control PID. Se inicializan todos los periféricos en setup() y se ejecuta el ciclo de control de alta frecuencia en loop().
 @author Legion de Ohm
 */

#include <Arduino.h>
#include "QTRSensors.h"
#include "drv8833.hpp"
#include "buzzer.hpp"
#include "interrupciones.hpp"
#include "config.hpp"
#include "pid.hpp"
#include "sensores.hpp"
#include "motores.hpp"

// ============================
// PINES Y PERIFÉRICOS GLOBALES
// ============================
const uint8_t pinBuzzer = 17;
const uint8_t ledMotores = 13;          // LED para encender/apagar motores (Indicador RUN)
const uint8_t ledCalibracion = 2;       // LED para indicar estado de calibración
const uint8_t BTN_RUN = 19;
const uint8_t BTN_STOP = 22;

// ============================
// DEFINICIÓN DE VARIABLES GLOBALES (Externas)
// ============================

// Velocidades
const int32_t maxSpeed = 80;   // Límite de velocidad PWM
int32_t motorSpeedIzq = baseSpeed;
int32_t motorSpeedDer = baseSpeed; 

// PID y Setpoint
uint16_t setpoint = 3500;       // Valor central de la lectura de sensores (ej. 0 a 7000)
uint16_t zonaMuerta = 50;      // Banda muerta de error alrededor del setpoint
float lastError = 0;           // Último error (Término D)
float integral = 0;            // Acumulador Integral (Término I)
uint32_t lastTime = 0;          // Tiempo de la última iteración (para $\Delta T$)

// Buzzer
const uint8_t BuzzerPwm = 2;    // Canal PWM para el zumbador
Buzzer buzzer(pinBuzzer, BuzzerPwm);   // Objeto Buzzer

// Motores (DRV8833)
const uint8_t motorPinIN1_Izq = 21; const uint8_t motorPinIN2_Izq = 18;
const uint8_t motorPinSleep_Izq = 23;
const uint8_t motorPinIN1_Der = 26; const uint8_t motorPinIN2_Der = 25;
const uint8_t motorPinSleep_Der = 16;
const uint8_t motorPWM_Izq = 0; const uint8_t motorPWM_Der = 1;
const uint32_t freqPWM = 20000; 
const uint8_t resPWM = 8;
Drv8833 motorDer; Drv8833 motorIzq;

// Sensores QTR
const uint8_t S8 = 14; const uint8_t S7 = 27; const uint8_t S6 = 33; const uint8_t S5 = 32;
const uint8_t S4 = 35; const uint8_t S3 = 34; const uint8_t S2 = 39; const uint8_t S1 = 36;
QTRSensors qtr;
const uint8_t SensorCount = 8;
const uint8_t sensorPins[SensorCount] = {S8, S7, S6, S5, S4, S3, S2, S1};
uint16_t sensorValues[SensorCount];
uint16_t position;

// ============================
// SETUP
// ============================
/**
 @brief Función de inicialización de Arduino. Configura todos los periféricos de entrada y salida, inicializa los drivers y establece las interrupciones para el control de inicio/parada.
 @return void
 */
void setup() {
    // Inicialización de periféricos básicos
    deb(Serial.begin(115200);)
    mute(buzzer.begin();) 

    // Configuración de drivers de motores DRV8833
    motorDer.setup(motorPinIN1_Der, motorPinIN2_Der, motorPinSleep_Der, motorPWM_Der, freqPWM, resPWM);
    motorIzq.setup(motorPinIN1_Izq, motorPinIN2_Izq, motorPinSleep_Izq, motorPWM_Izq, freqPWM, resPWM);

    // Configuración pines E/S
    pinMode(ledMotores, OUTPUT);
    pinMode(ledCalibracion, OUTPUT);
    pinMode(BTN_RUN, INPUT);
    pinMode(BTN_STOP, INPUT);

    // Interrupciones de arranque y parada
    // Se adjuntan las funciones ISR a los pines.
    attachInterrupt(digitalPinToInterrupt(BTN_RUN), handleRun, RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_STOP), handleStop, RISING);

    // Configuración sensores QTR
    setupSensores(sensorPins, SensorCount);

    // Calibración inicial
    calibrarSensores();
}


// ============================
// LOOP
// ============================
/**
 @brief Bucle principal de control del robot. Este es el ciclo de control primario. Solo se ejecuta la lógica de seguimiento si la bandera `RUN` está activa.
 @return void
 */
void loop() {
    // 1. MODO DETENIDO (IDLE)
    if (!RUN) {
        digitalWrite(ledMotores, LOW);
        motorIzq.stop();
        motorDer.stop();
        return;
    }

    // MODO CARRERA
    digitalWrite(ledMotores, HIGH);

    // 2. LECTURA DE SENSORES Y TIEMPO
    uint32_t now = tiempoActual();
    
    // Lectura de posición de línea (Input al PID) 
    position = leerLinea(); 
    deb(Serial.printf("Posicion=%d\n", position);)  

    // Cálculo del Delta Tiempo ($\Delta T$)
    float deltaTime = (now - lastTime) / TIME_DIVISOR; // Convertir a segundos

    // 3. CÁLCULO PID
    // La salida del PID es la corrección angular/de trayectoria
    float correcion = calculo_pid(position, deltaTime);

    // 4. ACTUACIÓN Y MOVIMIENTO
    // Mapeo de la corrección a las velocidades de motor (con lógica de zona muerta)
    controlMotores(correcion);

    // Aplicar las velocidades calculadas (permite avance y retroceso)
    moverMotores(motorSpeedIzq, motorSpeedDer);

    // 5. REINICIO DE CICLO
    lastTime = now;
    deb(Serial.println("\n ---------------------- \n");)
}