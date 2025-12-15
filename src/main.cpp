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
const int32_t maxSpeed  = 90;   // Límite de velocidad - usada para la max correccion y para acelerar  
int32_t velocidadAcel = 50;     // arranca suave 50% sube hasta maxSpeed

// SETPOINT y ZONA MUERTA
uint16_t setpoint = 3500;       // mitad de lectura de sensores - es decir pararnos sobre la linea
uint16_t zonaMuerta = 100;      // zona de mas y menos del setpoint para el estado acelerar 

// CONFIGURACIÓN DE BUZZER
const uint8_t BuzzerPwm = 2;            // Canal PWM 2  (0 y 1 son de motores)
Buzzer buzzer(pinBuzzer, BuzzerPwm);    // objeto buzzer en el pin 17

/* // CONTROL IR - comentado por ahora
// ============================
// COMANDOS CONTROL
// ============================
#define CMD_ON_OFF   0x43           //Tecla 'play/pause'
#define RAW_ON_OFF   0xBC43FF00

void leer_IR() {
// Si recibe algo lo leemos/decodificamos
    if (IrReceiver.decode()) {
        // Guardamos los datos y separamos el dato crudo y el codigo
        auto data = IrReceiver.decodedIRData;
        uint8_t cmd  = data.command;

        // Si coinciden el codigo crudo y comando entonces arranco o paro el coche
        if (cmd == CMD_ON_OFF)   { RUN = !RUN;}

        // Liberamos
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
    // Inicializar Serial, Buzzer, Control-IR SOLOS SI se habilitaron en el PLATFORMIO.INI
    deb(Serial.begin(115200);)
    mute(buzzer.begin();)
    //control_ir( IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);)

    // Configuracion motores - pines de direcion, canal de pwm, frecuencia y resolucion
    setupMotores();

    // Configuracion pines
    pinMode(ledMotores, OUTPUT);
    pinMode(ledCalibracion, OUTPUT);
    pinMode(BTN_RUN, INPUT);
    pinMode(BTN_STOP, INPUT);

    // Configuracion interrupciones
    setupInterrupciones(); 

    // Configuracion y calibracion de sensores
    setupSensores();
}

// ============================
// LOOP
// ============================
void loop() {
    // Entrada de 2 bits (SETPOINT RUN - 00, 01, 10, 11) → 0, 1, 2, 3
    uint8_t c = (SETPOINT << 1) | RUN;

    // Realizamos la transicion y ejecutamos su estado 
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

        detenerMotores();
        
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
    
    // Leer posicion de línea (0 = extremo izquierda, 7000 = extremo derecha)  
    position = leerLinea();
    deb(Serial.printf("Posicion=%d\n", position);)

    // Incremento suave de velocidad
    if (velocidadAcel < maxSpeed) velocidadAcel++;

    // Mover motores con aceleracion progresiva
    moverMotores(velocidadAcel, velocidadAcel);

    // Indicador de que estamos en setpoint
    digitalWrite(ledCalibracion, HIGH);

    // Calculamos si estamos en el setpoint
    actualizarSP(position);

    // Reiniciamos las variables PID
    reiniciar_pid();

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

    // Leer posicion de línea (0 = extremo izquierda, 7000 = extremo derecha)    
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