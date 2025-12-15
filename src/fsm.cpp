/*
    Maquina de estados:
    * inicia en uno stop y se queda ahi hasta que no cambie RUN
    * cuando RUN = true pasamos a acel en setpoint, en setpoint pasamos a seguirLinea y fuera a ac 
    * nos quedamos en seguirLinea cada timerIsr. Pero cuando pasamos a run = 0 volvemos a stop.  
*/

#include "fsm.hpp"

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

// Punteros a funciones de estado (prototipos vienen de fsm.hpp)
extern void (*acciones_estado[])();

// Estado actual
static int estadoActual = S;

// ==================== FUNCION FSM ====================
int transicionar(int entrada) {
    estado_t* p = tabla_de_estados[estadoActual];

    // busca el el recibo que coincida con la entrada
    while (p->recibo != entrada && p->recibo != CUALQUIERA) p++;

    // ejecuta las funciones de transicion - dummies (como no las ejecutamos lo dejaremos comentado)
    (p->transicion)(entrada);
    
    // actualizo el estado
    estadoActual = p->prox_estado;

    // EJECUTAR ESTADO
    acciones_estado[estadoActual]();

    return estadoActual;
}
