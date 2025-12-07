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

/* MAIN.CPP - CORREDOR PID COMPETENCIA ROBOTICA
 TODO: 
    * implementar TIMER_ISR - reemplaza dt por una cantidad fija en donde se debe poder ejecutar el pid
    * modulo control_ir / separar del main  - usar un mejor control/receptor mas ancho
    * Implementar un arranque seguro - evita desviaciones bruscaz por error alto al arrancar
        - acelerar al principio. desde un min (50 - casi detenido) hasta base_speed
        - debe durar unos pocos ms, despues no lo volvemos a hacer y solo usamos el PID 
    * PCB vers ECO:
        - uC arduino NANO o MICRO (ese que posee 8 ADCS y los pwm justos)
        - driver puente h rojo - es comodo de usar y posee dist pin de enable (osea 1 que hace de dos drvs)
        - siguen los sensores qrts, cambiamos el puenteH, uc y bateria (menos peso y espacio)
    * Si cambio el uC, ¿como puedo evitar cambiar este codigo? ¿Separo los pines del main? 
    * Separar las variables o pines del micro en otro programa (menos lineas)
    * Maquina de estados - paso de parado, a inicio seguro, y a correrdor
*/


// VELOCIDADES  - PORCENTAJE DE PWM (0-100%)
const int32_t maxSpeed  = 80;   // Límite de velocidad
// Velocidades base de motores - Le aumentamos o disminuimos para realizar correcion  
int32_t motorSpeedIzq = baseSpeed;
int32_t motorSpeedDer = baseSpeed;

// SETPOINT y ZONA MUERTA
uint16_t setpoint = 3500;       // mitad de lectura de sensores - es decir pararnos sobre la linea
uint16_t zonaMuerta = 50;      // zona para acelerar "min y max de setpoint" 

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
// Objeto de motores izq y der 
Drv8833 motorDer;   Drv8833 motorIzq;
// Ajustes PWM MOTORES
const uint8_t motorPWM_Izq = 0; const uint8_t motorPWM_Der = 1;
const uint32_t freqPWM = 20000; // Frecuencia del PWM = 20KHz
const uint8_t resPWM = 8;       // Resolución de 8 bits = 256 valores posibles [0, 255]

// ============================
// CONFIGURACIÓN SENSORES QTR & LINEA
// ============================
// Objeto sensor qtr, con 8 sensores S1 a S8 - obtenemos posicion con los sensor values 
QTRSensors qtr;
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

// ===================== Prototipos estados  =====================
void estadoStop();
void estadoAcel();
void estadoControl();

// Punteros a funciones de estado
void (*acciones_estado[])() = { estadoStop, estadoAcel, estadoControl };

enum estados { CUALQUIERA = -1, S, A, C, CANT_ESTADOS };

// función dummy
int nada(int c) { return 0; }

// estructura de estado 
typedef struct {
    int recibo;               // combinación SP,RUN (00..11)
    int (*transicion)(int);  // función acción (dummy)
    int prox_estado;         // estado destino
} estado_t;

// ===================== TABLAS =====================
// STOP
estado_t stop[] = {
    {0, nada, S},
    {1, nada, A},
    {2, nada, S},
    {3, nada, A},
    {CUALQUIERA, nada, S},
};

// ACELERAR
estado_t acel[] = {
    {0, nada, S},
    {1, nada, C},
    {2, nada, S},
    {3, nada, A},
    {CUALQUIERA, nada, A},
};

// CONTROL
estado_t control[] = {
    {0, nada, S},
    {1, nada, C},
    {2, nada, S},
    {3, nada, A},
    {CUALQUIERA, nada, C},
};

// tabla general
estado_t* tabla_de_estados[] = { stop, acel, control };
static int estadoActual = S;    // static - recuerda el valor entre llamadas

// FUNCION TRANSICIONAR
static int transicionar(int entrada) {
    estado_t* p = tabla_de_estados[estadoActual];

    // busca el el recibo que coincida con la entrada
    while (p->recibo != entrada && p->recibo != CUALQUIERA) p++;

    //ejecuta las funciones de transicion - dummies
    (p->transicion)(entrada);
    
    // actualizo el estado
    estadoActual = p->prox_estado;

    // EJECUTAR ESTADO
    acciones_estado[estadoActual]();

    return estadoActual;
}
 

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

    // Interrupciones por TIMER - reemplaza dt por una unidad cte
    timer = timerBegin(0, 80, true);                        // Timer 0, divisor de reloj 80
    timerAttachInterrupt(timer, &timerInterrupcion, true);  // función de interrupción
    timerAlarmWrite(timer, TIEMPO_TIMER, true);             // Interrupción por cada deltaTime 
    timerAlarmEnable(timer);                                // Habilitar la alarma
    
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
    static uint8_t c_prev = 255;

    transicionar(c);
}

// ESTADO STOP
void estadoStop() {
    // si RUN = 0   se queda en S
    // si RUN = 1   pasa a A
    // si SP=1      se queda en S
    // si SP=0      se queda en C
    deb(Serial.println("Estado: STOP");)
    
    digitalWrite(ledMotores, LOW);
    motorIzq.stop();
    motorDer.stop();
}

// ESTADO ACEL
void estadoAcel() {
    // si RUN = 0   pasa a S
    // si RUN = 1   se queda en A
    // si SP=1      se queda en A
    // si SP=0      pasa a C
    deb(Serial.println("Estado: ACEL");)

    // Leer posición de línea (0 = extremo izquierda, 7000 = extremo derecha)  
    position = leerLinea();
    deb(Serial.printf("Posicion=%d\n", position);)

    // Mover los motores (Avanza, retrocede o para)
    moverMotores(maxSpeed, maxSpeed);

    // Calculamos si estamos en el setpoint
    actualizarSP(position);

    // Reiniciamos las variables PID
    lastError = 0; integral =0;
}


// ESTADO CONTROL - FUNCION SEGUIDOR LINEA
// todo: CAMBIAR NOW POR TIMER ISR - pasa cada 5 ms
void estadoControl() {
    // si RUN = 0 pasa a S
    // si RUN = 1 se queda en c
    // si SP=1, pasa a A
    // si SP=0, se queda en C
    deb(Serial.println("Estado: CONTROL");)

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
    
    // Calculamos si estamos en el setpoint
    actualizarSP(position);

    deb(Serial.println("\n ---------------------- \n");)
}

/*
    Maquina de estados:
    * inicia en uno stop y se queda ahi hasta que no cambie RUN
    * cuando RUN = true pasamos a acel en setpoint, en setpoint pasamos a seguirLinea y fuera a ac 
    * nos quedamos en seguirLinea cada timerIsr. Pero cuando pasamos a run = 0 volvemos a stop.  
*/