/* MAIN.CPP - CORREDOR PID COMPETENCIA ROBOTICA
 TODO: 
    * Cambiar Float por Double (8b a 32b)
    * Cambiar la tecla run/stop
    * implementar TIMER_ISR
    * modulo control_ir / separar del main
 */
#include <Arduino.h>
#include <IRremote.hpp>
#include "QTRSensors.h"
#include "drv8833.hpp"
#include "buzzer.hpp"
#include "interrupciones.hpp"
#include "config.hpp"
#include "pid.hpp"
#include "sensores.hpp"
#include "motores.hpp"


// ============================
// PINES - LEDS, BOTONES, BOCINA y SENSOR IR
// ============================
const uint8_t pinBuzzer = 17;
const uint8_t ledMotores = 13;          // LED para encender/apagar motores
const uint8_t ledCalibracion = 2;       // LED azul que indica calibración finalizada
const uint8_t BTN_RUN = 19;
const uint8_t BTN_STOP = 22;
const uint8_t IR_PIN = 4;

// VELOCIDADES  - PORCENTAJE DE PWM (0-100%)
const uint8_t maxSpeed  = 100;   // Límite de velocidad
// Velocidades base de motores - Le aumentamos o disminuimos para realizar correcion  
int32_t motorSpeedIzq = baseSpeed;
int32_t motorSpeedDer = baseSpeed; 

// SETPOINT y ZONA MUERTA
uint16_t setpoint = 3500;   // mitad de lectura de sensores - es decir pararnos sobre la linea
uint16_t zonaMuerta = 200;  // zona entre el medio en la cual se considera setpoint ("min y max de setpoint") 

// Variables auxiliares para PID 
float  lastError = 0;   // Error previo         -   control D
float  integral = 0;    // Acumulador           -   control I
uint32_t lastTime = 0;  // Millis previo        -   delta Tiempo

// =================================
// CONFIGURACIÓN DE BUZZER
// =================================
const uint8_t BuzzerPwm = 2;            // Canal PWM 2  (0 y 1 son de motores)
Buzzer buzzer(pinBuzzer, BuzzerPwm);   // objeto buzzer en el pin 17

// ============================
// CONFIGURACIÓN DE MOTORES
// ============================ 
const uint8_t motorPinIN1_Izq = 21;   const uint8_t motorPinIN2_Izq = 18;
const uint8_t motorPinSleep_Izq = 23;

// MOTOR DERECHO
const uint8_t motorPinIN1_Der = 26;   const uint8_t motorPinIN2_Der = 25;
const uint8_t motorPinSleep_Der = 16;

// Ajustes PWM MOTORES
const uint8_t motorPWM_Izq = 0; const uint8_t motorPWM_Der = 1;
const uint32_t freqPWM = 20000; // Frecuencia del PWM = 20KHz
const uint8_t resPWM = 8;       // Resolución de 8 bits = 256 valores posibles [0, 255]

// Objeto de motores izq y der 
Drv8833 motorDer;   Drv8833 motorIzq;

// ============================
// CONFIGURACIÓN SENSORES QTR & LINEA
// ============================
const uint8_t S8 = 14;  const uint8_t S7 = 27;  const uint8_t S6 = 33;  const uint8_t S5 = 32;
const uint8_t S4 = 35;  const uint8_t S3 = 34;  const uint8_t S2 = 39;  const uint8_t S1 = 36;

// Objeto sensor qtr, con 8 sensores S1 a S8 - obtenemos posicion con los sensor values 
QTRSensors qtr;         const uint8_t SensorCount = 8;
const uint8_t sensorPins[SensorCount] = {S8, S7, S6, S5, S4, S3, S2, S1};
uint16_t sensorValues[SensorCount];
uint16_t position;


// ============================
// COMANDOS CONTROL
// ============================
#define CMD_ON_OFF   0x18           //Tecla '2'
#define RAW_ON_OFF   0xE718FF00     //TODO: cambiar a tecla Play/Pause


// ============================
// SETUP
// ============================
void setup() {
    deb(Serial.begin(115200);)   
    mute(buzzer.begin();)         // 2 kHz y 8 bits

    // Configuración motores - pines de direcion, canal de pwm, frecuencia y resolucion
    motorDer.setup(motorPinIN1_Der, motorPinIN2_Der, motorPinSleep_Der, motorPWM_Der, freqPWM, resPWM);
    motorIzq.setup(motorPinIN1_Izq, motorPinIN2_Izq, motorPinSleep_Izq, motorPWM_Izq, freqPWM, resPWM);

    // Configuración pines
    pinMode(ledMotores, OUTPUT);
    pinMode(ledCalibracion, OUTPUT);
    pinMode(BTN_RUN, INPUT);
    pinMode(BTN_STOP, INPUT);

    // Interrupciones de arranque y parada
    attachInterrupt(digitalPinToInterrupt(BTN_RUN), handleRun, RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_STOP), handleStop, RISING);

    // Configuración sensores
    setupSensores(sensorPins, SensorCount);
    
    // Calibración inicial
    calibrarSensores();

    // Inicializar receptor IR
    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
}


// ============================
// LOOP
// ============================
void loop() {
    // Lectura de control IR - Debe estar definido en el platformio
    control_ir(
        // Si recibe algo lo leemos/decodificamos - ISR interna (maquina de estado[?])
        if (IrReceiver.decode()) {
            // Guardamos los datos y separamos el dato crudo y el codigo
            auto data = IrReceiver.decodedIRData;
            uint32_t raw = data.decodedRawData;
            uint8_t cmd  = data.command;

            // Si coinciden el codigo crudo y comando entonces arranco o paro el coche
            if (raw == RAW_ON_OFF && cmd == CMD_ON_OFF)   { RUN = !RUN;    }

            // Liberamos / salimos de la maquina
            IrReceiver.resume();
        }
    )

    // Modo detenido - Como no se levanto el flag de correr entonces detenido
    if (!RUN) {
        digitalWrite(ledMotores, LOW);
        motorIzq.stop();    motorDer.stop();
        return;
    }

    // Enceder led modo corredor
    digitalWrite(ledMotores, HIGH);
    
    // Obtener el tiempo actual
    uint32_t now = tiempoActual();

    // Leer posición de línea (0 = extremo izquierda, 7000 = extremo derecha)    
    position = leerLinea();
    deb(Serial.printf("Posicion=%d\n", position);)  

    // calculo la diferencia entre intantes de tiempo
    float  deltaTime = (now - lastTime) / TIME_DIVISOR; // Convertir a segundos

    // calculo la correccion para los motores segun la posicion y el tiempo actual
    float correcion = calculo_pid(position, deltaTime);
 
    // Control de motores
    controlMotores(correcion);

    // Mover los motores (Avanza, retrocede o para)
    moverMotores(motorSpeedIzq, motorSpeedDer);

    //Guardar el tiempo al finalizar
    lastTime = now;
    deb(Serial.println("\n ---------------------- \n");)
}