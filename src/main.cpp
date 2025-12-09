#include <Arduino.h>
//#include <IRremote.hpp>
#include "QTRSensors.h"
#include "drv8833.hpp"
#include "buzzer.hpp"
#include "interrupciones.hpp"
#include "config.hpp"
#include "pid.hpp"
#include "sensores.hpp"
#include "motores.hpp"
#include "fsm.hpp"

/* MAIN.CPP - CORREDOR PID COMPETENCIA ROBOTICA */

// VELOCIDADES  - PORCENTAJE DE PWM (0-100%)
const int32_t maxSpeed  = 100;  // Límite de velocidad - usada para la max correccion y para acelerar - bajarla limita todas las velocidades  
int32_t velocidadAcel = 50;     // arranca suave 50% sube hasta maxSpeed

// delta tiempo fijo en segundos - usado en el PID (TIEMPO_TIMER esta en us)
const float FIXED_DT_S = (float)TIEMPO_TIMER / 1000000.0f;

// Velocidades base de motores - Le aumentamos o disminuimos para realizar correcion  
int32_t motorSpeedIzq = baseSpeed;  int32_t motorSpeedDer = baseSpeed;

// SETPOINT y ZONA MUERTA
uint16_t setpoint = 3500;       // mitad de lectura de sensores - es decir pararnos sobre la linea
uint16_t zonaMuerta = 100;      // zona de mas y menos del setpoint para el estado acelerar 

// Variables auxiliares para PID 
float  lastError = 0;   // Error previo         -   control D
float  integral = 0;    // Acumulador           -   control I
uint32_t lastTime = 0;  // Millis previo        -   delta Tiempo

// CONFIGURACIÓN DE BUZZER
const uint8_t BuzzerPwm = 2;            // Canal PWM 2  (0 y 1 son de motores)
Buzzer buzzer(pinBuzzer, BuzzerPwm);    // objeto buzzer en el pin 17

// CONFIGURACIÓN DE MOTORES
Drv8833 motorDer;   Drv8833 motorIzq; // Objeto de motores izq y der 
const uint8_t motorPWM_Izq = 0; const uint8_t motorPWM_Der = 1; // Canales PWM 0 y 1
const uint32_t freqPWM = 10000;                                 // Frecuencia del PWM = 10KHz
const uint8_t resPWM = 8;                                       // Resolución de 8 bits [0, 255]

// CONFIGURACIÓN SENSORES QTR & LINEA
QTRSensors qtr;                 // Objeto sensor qtr, con 8 sensores S1 a S8
const uint8_t SensorCount = 8;
const uint8_t sensorPins[SensorCount] = {S8, S7, S6, S5, S4, S3, S2, S1};
uint16_t sensorValues[SensorCount];
uint16_t position;

/* // CONTROL IR - comentado por ahora
// ============================
// COMANDOS CONTROL
// ============================
#define CMD_ON_OFF   0x43           //Tecla 'play/pause'
#define RAW_ON_OFF   0xBC43FF00

void leer_IR()
{// Si recibe algo lo leemos/decodificamos - ISR interna (maquina de estado[?])
    if (IrReceiver.decode()) {
        // Guardamos los datos y separamos el dato crudo y el codigo
        auto data = IrReceiver.decodedIRData;
        uint32_t raw = data.decodedRawData;
        uint8_t cmd  = data.command;

        // Si coinciden el codigo crudo y comando entonces arranco o paro el coche
        if (raw == RAW_ON_OFF && cmd == CMD_ON_OFF)   { RUN = !RUN;}

        // Liberamos / salimos de la maquina
        IrReceiver.resume();
    }
}*/

// Punteros a funciones de estado 
void (*acciones_estado[])() = { estadoStop, estadoAcel, estadoControl };
bool stop_done = false;     // recuerda si ejecutamos stop


// ============================
// SETUP
// ============================
void setup() {
    // Inicializar Serial, Buzzer, Control SOLOS SI se habilitaron en el PLATFORMIO.INI
    deb(Serial.begin(115200);)   
    mute(buzzer.begin();)         // 2 kHz y 8 bits
    //control_ir( IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);)

    // Configuración motores - pines de direcion, canal de pwm, frecuencia y resolucion
    motorDer.setup(motorPinIN1_Der, motorPinIN2_Der, motorPinSleep_Der, motorPWM_Der, freqPWM, resPWM);
    motorIzq.setup(motorPinIN1_Izq, motorPinIN2_Izq, motorPinSleep_Izq, motorPWM_Izq, freqPWM, resPWM);

    // Configuración pines
    pinMode(ledMotores, OUTPUT);
    pinMode(ledCalibracion, OUTPUT);
    pinMode(BTN_RUN, INPUT);
    pinMode(BTN_STOP, INPUT);

    // Interrupciones FISICAS de arranque y parada
    attachInterrupt(digitalPinToInterrupt(BTN_RUN), handleRun, RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_STOP), handleStop, RISING);

    // Interrupciones por TIMER - Creamos el Timer 0, dividimos los 80MHz en 80 - cada 1us
    timer = timerBegin(0, 80, true);

    // Asocia la función de interrupción (ISR)
    timerAttachInterrupt(timer, &timerInterrupcion, true);
    
    // Configura cada cuánto se activa la interrupcion
    timerAlarmWrite(timer, TIEMPO_TIMER, true);
   
    // Enciende el timer para que empiece a generar las interrupciones
    timerAlarmEnable(timer);
    
    // Configuración sensores
    setupSensores(sensorPins, SensorCount);
    
    // Calibración inicial
    calibrarSensores();
}


// ============================
// LOOP
// ============================
void loop() {
    // Entrada de 2 bits (SETPOINT RUN - 00, 01, 10, 11) → 0, 1, 2, 3
    uint8_t c = (SETPOINT << 1) | RUN;
    
    transicionar(c);
}


// ESTADO STOP
void estadoStop() {
    // si RUN = 0   se queda en S
    // si RUN = 1   pasa a A
    // si SP=1      se queda en S
    // si SP=0      se queda en C

    if (!stop_done) {                  // solo ejecuta una vez
        deb(Serial.println("Estado: STOP");)

        digitalWrite(ledMotores, LOW);
        digitalWrite(ledCalibracion, LOW);
        motorIzq.stop();
        motorDer.stop();
        velocidadAcel = 50;

        stop_done = true;
        deb(Serial.println("\n ---------------------- \n");)
    }
}


// ESTADO ACEL
void estadoAcel() {
    // si RUN = 0   pasa a S
    // si RUN = 1   se queda en A
    // si SP=1      se queda en A
    // si SP=0      pasa a C
    deb(Serial.println("Estado: ACEL");)
    stop_done = false; // para que cuando vuelva a STOP se ejecute 1 vez
    
    // Leer posición de línea (0 = extremo izquierda, 7000 = extremo derecha)  
    position = leerLinea();
    deb(Serial.printf("Posicion=%d\n", position);)

     // Incremento suave de velocidad
    if (velocidadAcel < maxSpeed) velocidadAcel++;

    // Mover motores con aceleración progresiva
    moverMotores(velocidadAcel, velocidadAcel);

    // Indicador de que estamos en setpoint
    digitalWrite(ledCalibracion, HIGH);

    // Calculamos si estamos en el setpoint
    actualizarSP(position);

    // Reiniciamos las variables PID
    lastError = 0; integral =0;
    deb(Serial.println("\n ---------------------- \n");)
}


// ESTADO CONTROL - FUNCION SEGUIDOR LINEA
void estadoControl() {
    // si RUN = 0 pasa a S
    // si RUN = 1 se queda en c
    // si SP=1, pasa a A
    // si SP=0, se queda en C

    // Ejecutar solo cuando el timer indique el tick
    if (!has_expired) return;

    deb(Serial.println("Estado: CONTROL");)
    has_expired = false;    // ya paso un tick (timer isr) entonces debo reiniciarlo
    stop_done = false;      // cuando vuelva a STOP se ejecute 1 vez

    // Enceder led modo corredor
    digitalWrite(ledMotores, HIGH);

    // Apagar led cal - indicamos que no estamos en Setpoint 
    digitalWrite(ledCalibracion, LOW);

    // Leer posición de línea (0 = extremo izquierda, 7000 = extremo derecha)    
    position = leerLinea();
    deb(Serial.printf("Posicion=%d\n", position);)

    // calculo la correccion para los motores segun la posicion y el delta tiempo (timer isr) 
    float correcion = calculo_pid(position, FIXED_DT_S);
 
    // Calculamos si estamos en el setpoint
    actualizarSP(position);

    // Control de motores
    controlMotores(correcion);

    // Mover los motores (Avanza, retrocede o para)
    moverMotores(motorSpeedIzq, motorSpeedDer);

    deb(Serial.println("\n ---------------------- \n");)
}