#include <Arduino.h>
// ESTE CODIGO SON DOS PROGRAMAS COMBINADOS QUE QUIERO JUNTAR EN UNO SOLO
// ES UNA MAQUINA Mealy ADAPTADA A Moore PARA EL SEGUIDOR DE LINEA
// 1. programa son las interrupciones con estados que quiero ejecutar
// 2. programa con una maquina Mealy - tabla armada segun programa 1 y con ejecucion de func transicion dummy
// QUIERO EJECUTAR EL ESTADO QUE SE CORRESPONDE, STOP, ACEL O CONTROL.
// DEBE PERMANECER LA FORMA DE LA MAQUINA, SIN USAR CONDICIONALES Y/O SIN SWITCH/CASE
// 

// ===================== LEDs =====================
const int LED_INTERNAL = 2;   // indica estado S
const int LED_EXTERNAL = 13;  // indica A o C

// ===================== Botones (pull-down) =====================
const int BTN_ON  = 19;   // RUN = 1 o 0
const int BTN_SP = 22;   //  SP = 1 o 0

// ===================== Señales de control =====================
volatile bool RUN = 0;    // por interrupción
volatile bool SP  = 1;    // por interrupción

// ===================== Prototipos =====================
void estadoS();
void estadoA();
void estadoC();

void (*estadoActual)() = estadoS;

// ===================== ISR =====================
void IRAM_ATTR ISR_runOn()  { RUN = !RUN; }
void IRAM_ATTR ISR_setPoint() { SP = !SP; }

// ===================== SETUP =====================
void setup() {
    Serial.begin(115200);

    pinMode(LED_INTERNAL, OUTPUT);
    pinMode(LED_EXTERNAL, OUTPUT);

    pinMode(BTN_ON, INPUT);
    pinMode(BTN_SP, INPUT);

    attachInterrupt(digitalPinToInterrupt(BTN_ON),  ISR_runOn,  RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_SP), ISR_setPoint, RISING);
}

// ==================================================
//                    ESTADO STOP
// ==================================================
void estadoS() {
    digitalWrite(LED_INTERNAL, HIGH);  // LED interno ON → S
    digitalWrite(LED_EXTERNAL, LOW);    
    // si RUN = 0   se queda en S
    // si RUN = 1   pasa a A
    // si SP=1      se queda en S
    // si SP=0      se queda en C
}

// ==================================================
//                    ESTADO ACELERAR
// ==================================================
void estadoA() {
    digitalWrite(LED_INTERNAL, HIGH);
    digitalWrite(LED_EXTERNAL, HIGH);
    // si RUN = 0   pasa a S
    // si RUN = 1   se queda en A
    // si SP=1      se queda en A
    // si SP=0      pasa a C
}

// ==================================================
//                    ESTADO CONTROL
// ==================================================
void estadoC() {
    digitalWrite(LED_INTERNAL, LOW);
    digitalWrite(LED_EXTERNAL, HIGH);  // luz fija → C

    // si RUN = 0 pasa a S
    // si RUN = 1 se queda en c
    // si SP=1, pasa a A
    // si SP=0, se queda en C
}

// Enumeracion de los estados
enum estados { CUALQUIERA = -1, S, A, C, CANT_ESTADOS };

// funcion dummy
int nada(int c) {   return 0; }

typedef struct {
    int recibo;
    int (*transicion)(int);
    int prox_estado;
} estado_t;

// lo que recibe (Setpoint Run) - la funcion que ejecuta (dummies) - proximo estado (Stop, Acel, Control)
estado_t stop[] = {
    {   00       , nada  ,   S},
    {   01       , nada  ,   A},
    {   10       , nada  ,   S},
    {   11       , nada  ,   A},
    {CUALQUIERA , nada  ,   S},
};

// lo que recibe (Setpoint Run)
estado_t acel[] = {
    {   00       , nada  ,   S},
    {   01       , nada  ,   C},
    {   10       , nada  ,   S},
    {   11       , nada  ,   A},
    {CUALQUIERA , nada  ,   A},
};

//lo que recibe (Setpoint Run)
estado_t control[] = {
    {   00       , nada  ,   S},
    {   01       , nada  ,   C},
    {   10       , nada  ,   S},
    {   11       , nada  ,   A},
    {CUALQUIERA , nada  ,   C},
};

// Tabla de estados
estado_t *tabla_de_estados[] = {stop, acel, control};

static int transicionar(int c) {
    estado_t *p; 
    static int temp = S;
    for (p = tabla_de_estados[temp]; p->recibo != c && p->recibo != CUALQUIERA; p++)  ;
    (p->transicion)(c); 
    temp = p->prox_estado;

    // Aca ejecuto el estado?

    return 0;
}

// ===================== LOOP =====================
void loop() {
    unsigned int c; // 2b - (Setpoint Run)

    transicionar(c);

    delay(5); // antirrebote
}