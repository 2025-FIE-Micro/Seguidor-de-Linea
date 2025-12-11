/**
 @file prueba_sin_modular.cpp
 @brief Código de prueba que integra toda la lógica del seguidor de línea en un solo archivo. Este archivo contiene la configuración de pines, la lógica PID, la selección de perfiles de corredor y las implementaciones de las funciones para sensores y motores, simulando una implementación no modular para fines de prueba y depuración.
 @author Legion de Ohm
*/

#include <Arduino.h>
#include "QTRSensors.h"
#include "drv8833.hpp"
#include "buzzer.hpp"

// ===================================
// MACROS DE DEPURACIÓN (DEBUG)
// ===================================
#ifdef DEBUG
    /** @brief Macro para habilitar la depuración serial (código dentro de 'deb(x)'). */
    #define deb(x) x
#else
    /** @brief Macro para deshabilitar la depuración serial (compila a vacío). */
    #define deb(x)
#endif


// ===================================
// SELECCION DE CORREDOR - CONFIGURACIÓN PID
// ===================================
#define NIGHTFALL 1
#define ARGENTUM  2
#define DIEGO     3

#ifndef CORREDOR
  /** @error Debe definir CORREDOR en platformio.ini (ej: -D CORREDOR=NIGHTFALL) */
  #error "Debe definir CORREDOR en platformio.ini (ej: -D CORREDOR=NIGHTFALL)"
#endif

// Selección de parametros segun corredor
#if (CORREDOR == NIGHTFALL)
    #define USAR_MICROS  1         ///< Usa microsegundos para precisión de tiempo PID
    uint8_t baseSpeed = 95;         ///< Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.1;          ///< Ganancia Crítica (Ku) para Ziegler-Nichols
    const float Tu = 0.9;          ///< Periodo Crítico (Tu) para Ziegler-Nichols
#elif (CORREDOR == ARGENTUM)
    #define USAR_MICROS  0         ///< Usa milisegundos para tiempo PID
    uint8_t baseSpeed = 85;         ///< Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.05;
    const float Tu = 1.100;
#elif (CORREDOR == DIEGO)
    #define USAR_MICROS  0         ///< Usa milisegundos para tiempo PID
    uint8_t baseSpeed = 90;         ///< Velocidad base PORCENTAJE DE PWM (0-100%)
    const float Ku = 0.049;
    const float Tu = 0.72;
#else
  #error "Valor de CORREDOR inválido. Use NIGHTFALL, ARGENTUM o DIEGO."
#endif


// =================================
// CONFIGURACIÓN DE TIEMPOS
// =================================
#if USAR_MICROS
  /** @brief Macro que devuelve el tiempo actual en microsegundos. */
    #define tiempoActual() micros()
    const float TIME_DIVISOR = 1000000.0; // microsegundos → segundos
#else
  /** @brief Macro que devuelve el tiempo actual en milisegundos. */
    #define tiempoActual() millis()
    const float TIME_DIVISOR = 1000.0;    // milisegundos → segundos
#endif


// ============================
// VELOCIDADES 
// ============================
const uint8_t maxSpeed = 100;      ///< Límite de velocidad máxima (PWM 100%)
int32_t motorSpeedIzq = baseSpeed; ///< Velocidad actual del motor izquierdo.
int32_t motorSpeedDer = baseSpeed; ///< Velocidad actual del motor derecho.


// ============================
// CONTROL PID - METODO Ziegler-Nichols
// ============================
#ifdef TEST_PID
    /** @brief Ganancia Proporcional (Kp) en modo Test (igual a Ku). */
    const float Kp = Ku;
    /** @brief Ganancia Integral (Ki) en modo Test (cero). */
    const float Ki = 0;
    /** @brief Ganancia Derivativa (Kd) en modo Test (cero). */
    const float Kd = 0;
    /** @brief Macro que deshabilita el buzzer en modo Test. */
    #define debTestPid(x)
#else
    /** @brief Ganancia Proporcional (Kp) calculada (0.6 * Ku). */
    const float Kp = 0.6 * Ku;
    /** @brief Ganancia Integral (Ki) calculada. */
    const float Ki = 2 * Kp / Tu;
    /** @brief Ganancia Derivativa (Kd) calculada. */
    const float Kd = Kp * Tu / 8;
    /** @brief Macro que habilita el buzzer en modo normal. */
    #define debTestPid(x) x
#endif

uint16_t setpoint = 3500;           ///< Posición ideal de la línea (centro del rango 0-7000).
uint16_t zonaMuerta = 200;          ///< Rango de error permisible (3500 ± 200) donde no se aplica corrección.
float lastError = 0;                ///< Error previo, usado para el cálculo Derivativo (D).
float integral = 0;                 ///< Acumulador del error, usado para el cálculo Integral (I).
uint32_t lastTime = 0;              ///< Marca de tiempo de la lectura anterior, usada para calcular el delta de tiempo.


// =================================
// CONFIGURACIÓN DE BUZZER
// =================================
debTestPid(
    const uint8_t BuzzerPwm = 2;       ///< Canal PWM para el Buzzer.
    Buzzer buzzer(17, BuzzerPwm);     ///< Instancia del Buzzer.
)

// ============================
// CONFIGURACIÓN DE MOTORES
// ============================ 
const uint8_t motorPinIN1_Izq = 21;   ///< Pin IN1 del motor Izquierdo. 
const uint8_t motorPinIN2_Izq = 18;   ///< Pin IN2 del motor Izquierdo.
const uint8_t motorPinSleep_Izq = 23; ///< Pin Sleep del motor Izquierdo.

// MOTOR DERECHO
const uint8_t motorPinIN1_Der = 26;   ///< Pin IN1 del motor Derecho. 
const uint8_t motorPinIN2_Der = 25;   ///< Pin IN2 del motor Derecho.
const uint8_t motorPinSleep_Der = 16; ///< Pin Sleep del motor Derecho.

// Ajustes PWM MOTORES
const uint8_t motorPWM_Izq = 0;       ///< Canal PWM 0 para el motor Izquierdo. 
const uint8_t motorPWM_Der = 1;       ///< Canal PWM 1 para el motor Derecho.
const uint32_t freqPWM = 20000;       ///< Frecuencia del PWM (20 KHz, recomendada para motores).
const uint8_t resPWM = 8;             ///< Resolución de 8 bits (valores 0-255).

Drv8833 motorDer;                     ///< Instancia del driver del motor Derecho.
Drv8833 motorIzq;                     ///< Instancia del driver del motor Izquierdo.


// ============================
// CONFIGURACIÓN SENSORES QTR & LINEA
// ============================
// Pines de los 8 sensores de línea
const uint8_t S8 = 14;  const uint8_t S7 = 27;  const uint8_t S6 = 33;  const uint8_t S5 = 32;
const uint8_t S4 = 35;  const uint8_t S3 = 34;  const uint8_t S2 = 39;  const uint8_t S1 = 36;

QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

uint16_t position;

/** @brief Enumeración para definir el color de la línea a seguir. */
enum Linea { BLANCA, NEGRA };
Linea linea_competencia = BLANCA;   // CAMBIAR BLANCA o NEGRA de acorde a la linea


// ============================
// LEDS Y BOTONES
// ============================
const uint8_t ledMotores = 13;          ///< LED para indicar que los motores están activos (RUN).
const uint8_t ledCalibracion = 2;       ///< LED azul que indica el proceso de calibración.
const uint8_t BTN_RUN = 19;             ///< Botón para iniciar el algoritmo (con interrupción).
const uint8_t BTN_STOP = 22;            ///< Botón para detener el algoritmo (con interrupción).


// ============================
// INTERRUPCIONES
// ============================
volatile bool RUN = false;
/** @brief ISR: Cambia el estado RUN a true (inicia el movimiento). */
void IRAM_ATTR handleRun() { RUN = true; }
/** @brief ISR: Cambia el estado RUN a false (detiene el movimiento). */
void IRAM_ATTR handleStop() { RUN = false; }


// ============================
// FUNCION CALIBRAR
// ============================
/**
 @brief Realiza la calibración de los sensores QTR para establecer máximos y mínimos. Involucra un feedback audible con el buzzer, detiene los motores y usa el ledCalibracion para señalizar el proceso. Realiza 300 lecturas.
*/
void calibrarSensores() {
    debTestPid(
        buzzer.play(NOTE_A4);
        delay(150);
        buzzer.stop();
    );

    motorIzq.stop();
    motorDer.stop();

    digitalWrite(ledCalibracion, HIGH);
    deb(Serial.println("Calibrando sensores..."); )

    for (uint16_t i = 0; i < 300; i++) { qtr.calibrate(); } 
    digitalWrite(ledCalibracion, LOW);
    deb(Serial.println("Calibracion lista!");)

    debTestPid(
        buzzer.play(NOTE_C5);
        delay(200);
        buzzer.stop();
    )
}


// ============================
// LECTURA DE POSICIÓN
// ============================
/**
 @brief Lee los sensores y calcula la posición ponderada de la línea.
 @return uint16_t La posición de la línea (0=izquierda extrema, 7000=derecha extrema).
*/
uint16_t leerLinea() {
    if (linea_competencia == BLANCA)
        position = qtr.readLineWhite(sensorValues);
    else
        position = qtr.readLineBlack(sensorValues);
    return position;
}


// ============================
// FUNCION MOVER MOTORES
// ============================
/**
 @brief Mueve los motores aplicando la velocidad y dirección (avance/reversa/parada).
 @param motorSpeedIzq Velocidad del motor izquierdo (positiva = avance, negativa = reversa).
 @param motorSpeedDer Velocidad del motor derecho (positiva = avance, negativa = reversa).
*/
void moverMotores(int16_t motorSpeedIzq, int16_t motorSpeedDer) {
    deb(Serial.printf("MotorIzq=%d\n", motorSpeedIzq);)
    deb(Serial.printf("MotorDer=%d\n", motorSpeedDer);)

    if (motorSpeedIzq > 0) { motorIzq.forward(motorSpeedIzq); }
    else if (motorSpeedIzq < 0) { motorIzq.reverse(abs(motorSpeedIzq)); }
    else { motorIzq.stop(); }

    if (motorSpeedDer > 0) { motorDer.forward(motorSpeedDer); }
    else if (motorSpeedDer < 0) { motorDer.reverse(abs(motorSpeedDer)); }
    else { motorDer.stop(); } 
}


// ============================
// FUNCION CALCULO DE PID
// ============================
/**
 @brief Calcula el valor de corrección PID (Proporcional, Integral, Derivativo).
 @param pos Posición actual de la línea leída por los sensores.
 @param now Marca de tiempo actual para el cálculo del delta de tiempo.
 @return float El valor de corrección (output) a aplicar a los motores. $$ output = K_p \cdot error + K_d \cdot \frac{d(error)}{dt} + K_i \cdot \int error \cdot dt $$
*/
float calculo_pid(uint16_t pos, uint32_t now) {
    // Calcular deltaTime: tiempo transcurrido desde la última lectura
    float deltaTime = (now - lastTime) / TIME_DIVISOR; // Convertir a segundos
    deb(Serial.printf("deltaTime=%.6f\n", deltaTime);)

    // Calcular el error: diferencia entre la posición actual y el setpoint (centro)
    float error = pos - setpoint;               // Valores entre -3500 y 3500
deb(Serial.printf("error=%.6f\n", error);)

    // Calcular derivativo (tasa de cambio del error)
    float derivativo = (error - lastError) / deltaTime;

    // Actualizar el último error proporcional
    lastError = error;

    // Calcular integral (acumulación del error a lo largo del tiempo)
    integral += error * deltaTime;

    // Calcular la salida del PID
    float output = (error * Kp) + (derivativo * Kd) + (integral * Ki);

    deb(Serial.printf("PID=%.6f\n", output);)
    return output;
}


// ============================
// FUNCION CONTROL MOTORES - Zona muerta o Pid
// ============================
/**
 @brief Aplica la corrección PID a las velocidades de los motores o aplica la lógica de Zona Muerta. Si la posición está dentro de la Zona Muerta, la corrección se anula y la velocidad base se incrementa lentamente. De lo contrario, aplica la corrección.
 @param correcion Valor de corrección obtenido del cálculo PID.
*/
void controlMotores(float correcion) {
    // En Zona muerta (cerca del setpoint)
    if (abs(position - setpoint) < zonaMuerta) {
        motorSpeedIzq++;    // Aumento las velocidades lentamente (estrategia de aceleración)
        motorSpeedDer++;
 
        correcion = 0;      // No aplicar corrección (mantener rumbo recto)
        integral = 0;       // Reiniciar la integral para evitar el windup
    }
    else{
        // Fuera de la Zona Muerta, aplicar corrección PID
        motorSpeedIzq = baseSpeed - correcion; // El error positivo frena el izquierdo (gira a la derecha)
        motorSpeedDer = baseSpeed + correcion; // El error positivo acelera el derecho
    }
    // Limitar la velocidad para que no exceda el rango [-maxSpeed, maxSpeed]
    motorSpeedIzq = constrain(motorSpeedIzq, -maxSpeed, maxSpeed);
    motorSpeedDer = constrain(motorSpeedDer, -maxSpeed, maxSpeed);
}


// ============================
// SETUP
// ============================
/**
 @brief Función de inicialización: configura todos los módulos (Motores, Sensores, Interrupciones) y realiza la calibración.
*/
void setup() {
    deb(Serial.begin(115200);) 
    debTestPid(buzzer.begin();)         // Inicializa el PWM para el Buzzer

    // Configuración motores Drv8833
    motorDer.setup(motorPinIN1_Der, motorPinIN2_Der, motorPinSleep_Der, motorPWM_Der, freqPWM, resPWM);
    motorIzq.setup(motorPinIN1_Izq, motorPinIN2_Izq, motorPinSleep_Izq, motorPWM_Izq, freqPWM, resPWM);

    // Configuración sensores QTR
    qtr.setTypeAnalog(); // Se asume el modo Analógico para esta implementación
    const uint8_t sensorPins[SensorCount] = {S8, S7, S6, S5, S4, S3, S2, S1};
    qtr.setSensorPins(sensorPins, SensorCount);

    // Configuración de pines misceláneos
    pinMode(ledMotores, OUTPUT);
    pinMode(ledCalibracion, OUTPUT);
    pinMode(BTN_RUN, INPUT);
    pinMode(BTN_STOP, INPUT);

    // Interrupciones de arranque y parada
    attachInterrupt(digitalPinToInterrupt(BTN_RUN), handleRun, RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_STOP), handleStop, RISING);

    // Calibración inicial
    calibrarSensores();
}


// ============================
// LOOP
// ============================
/**
 @brief Bucle principal de ejecución del seguidor de línea. Ejecuta el ciclo de control: lee la posición, calcula la corrección PID, ajusta las velocidades y mueve los motores, siempre que el flag RUN sea verdadero.
*/
void loop() {
    // Detiene los motores y apaga el LED si el flag RUN es falso
    if (!RUN) {
        digitalWrite(ledMotores, LOW);
        motorIzq.stop();
        motorDer.stop();
        return; // Sale del loop
    }

    // Enceder led modo corredor
    digitalWrite(ledMotores, HIGH);

    // Obtener el tiempo actual (para cálculo de dT)
    uint32_t now = tiempoActual();

    // Leer posición de línea    
    position = leerLinea();

    deb(Serial.printf("Posicion=%d\n", position);)  

    // Cálculo de corrección PID
    float correcion = calculo_pid(position, now);

    // Control de motores (aplica corrección o zona muerta)
    controlMotores(correcion);

    // Mover los motores (Avanza, retrocede o para)
    moverMotores(motorSpeedIzq, motorSpeedDer);

    // Guardar el tiempo actual para la siguiente iteración
    lastTime = now;
    deb(Serial.println("\n ---------------------- \n");)
}
