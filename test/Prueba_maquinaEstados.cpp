/**
 @file prueba_maquinaEstados.cpp
 @brief Programa de prueba para la Máquina de Estados Finitos (FSM).
 @details Este test valida la lógica de transiciones entre los estados STOP, ACEL y CONTROL 
 utilizando señales de entrada simuladas por botones y feedback visual mediante LEDs.
 @author Legion de Ohm
 */

#include <Arduino.h>

// ===================== LEDs =====================
/** @brief Pin del LED interno de la placa. */
const int LED_INTERNAL = 2;
/** @brief Pin del LED externo para indicar el estado del sistema. */
const int LED_EXTERNAL = 13;

// ===================== Botones (pull-down) =====================
/** @brief Pin del botón para activar el modo RUN (inicio). */
const int BTN_ON  = 19; // RUN toggle
/** @brief Pin del botón para simular la salida del setpoint (modo CONTROL). */
const int BTN_SP  = 22; // SP toggle

// ===================== Señales de control =====================
/** @brief Bandera volátil que indica si el sistema está activo. */
volatile bool RUN = 0;
/** @brief Bandera volátil que indica si el robot está en el setpoint (1) o fuera (0). */
volatile bool SP  = 1;

// ===================== Prototipos estados  =====================
void estadoStop();
void estadoAcel();
void estadoControl();

/** @brief Array de punteros a funciones que vincula los identificadores de estado con su lógica. */
void (*acciones_estado[])() = { estadoStop, estadoAcel, estadoControl };

// ===================== ISR =====================
/**
 @brief ISR para iniciar el modo corredor.
 @details Activa RUN y restablece SP para comenzar en aceleración.
 */
void IRAM_ATTR handleRun() {
    RUN = true;
    SP = true;
}

/**
 @brief ISR para simular el cambio a modo control.
 @details Desactiva la bandera SP para forzar la transición a control PID.
 */
void IRAM_ATTR handleControl(){
    SP = false;
}

/** @enum estadoStop 
 @brief Enumeración de los estados posibles y valores especiales de la tabla. 
 */
enum estadoStop { CUALQUIERA = -1, S, A, C, CANT_estadoStop };

/**
 @brief Función dummy para transiciones que no ejecutan acciones adicionales.
 @param c Valor de entrada.
 @return int Siempre 0.
 */
int nada(int c) { return 0; }

/**
 @struct estado_t
 @brief Estructura de la tabla de transiciones.
 */
typedef struct {
    int recibo;               ///< Valor de entrada esperado (combinación SP, RUN).
    int (*transicion)(int);   ///< Puntero a función de transición.
    int prox_estado;          ///< Siguiente estado tras la coincidencia.
} estado_t;

// ===================== TABLAS =====================
/** @brief Transiciones desde el estado STOP (S). */
estado_t stop[] = {
    {0, nada, S},
    {1, nada, A},
    {2, nada, S},
    {3, nada, A},
    {CUALQUIERA, nada, S},
};

/** @brief Transiciones desde el estado ACELERAR (A). */
estado_t acel[] = {
    {0, nada, S},
    {1, nada, C},
    {2, nada, S},
    {3, nada, A},
    {CUALQUIERA, nada, A},
};

/** @brief Transiciones desde el estado CONTROL (C). */
estado_t control[] = {
    {0, nada, S},
    {1, nada, C},
    {2, nada, S},
    {3, nada, A},
    {CUALQUIERA, nada, C},
};

/** @brief Tabla general que agrupa todas las tablas de estado. */
estado_t* tabla_de_estados[] = { stop, acel, control };

/** @brief Variable estática que almacena el estado actual de la máquina. */
static int estadoActual = S;

/**
 @brief Procesa el cambio de estado basado en la entrada.
 @details Busca en la tabla correspondiente al estado actual hasta hallar una coincidencia.
 @param entrada Combinación binaria de 2 bits (SP | RUN).
 @return int Identificador del nuevo estado.
 */
static int transicionar(int entrada) {
    estado_t* p = tabla_de_estados[estadoActual];

    // busca el recibo que coincida con la entrada
    while (p->recibo != entrada && p->recibo != CUALQUIERA) p++;

    // ejecuta las funciones de transicion - dummies
    (p->transicion)(entrada);
    
    // actualizo el estado
    estadoActual = p->prox_estado;

    // EJECUTAR ESTADO
    acciones_estado[estadoActual]();

    return estadoActual;
}

/** @brief Lógica del estado STOP: Apaga LEDs y muestra mensaje. */
void estadoStop() {
    digitalWrite(LED_INTERNAL, LOW);
    digitalWrite(LED_EXTERNAL, LOW);
    Serial.println("Estado: STOP");
}

/** @brief Lógica del estado ACEL: Enciende ambos LEDs y muestra mensaje. */
void estadoAcel() {
    digitalWrite(LED_INTERNAL, HIGH);
    digitalWrite(LED_EXTERNAL, HIGH);
    Serial.println("Estado: ACEL");
}

/** @brief Lógica del estado CONTROL: Enciende solo LED externo y muestra mensaje. */
void estadoControl() {
    digitalWrite(LED_INTERNAL, LOW);
    digitalWrite(LED_EXTERNAL, HIGH);
    Serial.println("Estado: CONTROL");
}

/**
 @brief Configuración inicial del test de FSM.
 */
void setup() {
    Serial.begin(115200);

    pinMode(LED_INTERNAL, OUTPUT);
    pinMode(LED_EXTERNAL, OUTPUT);

    pinMode(BTN_ON, INPUT);
    pinMode(BTN_SP, INPUT);

    attachInterrupt(digitalPinToInterrupt(BTN_ON),  handleRun,   RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_SP),  handleControl, RISING);

    acciones_estado[estadoActual](); // estado inicial
}

/**
 @brief Ciclo principal que calcula la entrada y ejecuta la transición.
 */
void loop() {
    // Entrada de 2 bits (SP RUN - 00, 01, 10, 11) → 0, 1, 2, 3
    uint8_t c = (SP << 1) | RUN;
    // static uint8_t c_prev = 255; // Variable no utilizada pero mantenida del original

    transicionar(c);

    delay(10);
}