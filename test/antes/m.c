#include <stdio.h>
#include <stdlib.h>
/*
 * 1er punto
 * Enumeracion de los estados
 */
enum estados { CUALQUIERA = -1, A, B, CANT_ESTADOS };
/*
 * 2do punto
 * Prototipo de funciones que van a
 * ser utilizadas en las transiciones
 */
int ab(int);
int ba(int);
int nada(int);
/*
 * 3er punto
 * Definicion de la Estructura del estado
 */
/**
 * @brief Estructura que representa la información de un estado
 *
 * Esta estructura es utilizada para definir una sola situación de un estado en
 * particular del autómata.
 * Los elementos son:
 *      - Lo que ingresa como input
 *      - El puntero a una función que será la función de transición
 *      - El próximo estado al que apuntará el puntero.
 */
typedef struct {
    int recibo; /**< @brief Señal de entrada o recepcionada. En el caso particular de
                este programa, la forma de ingreso de la señal es por teclado. Como se
                puede ver, debe ser un número entero, ya que la variable de ingreso es
                del tipo _int_. */
    int (*transicion)(
        int); /**< @brief Puntero a una función que maneja la transición
                      particular de cada caso. Para el caso particular de este programa
                      recibirá las direcciones de las funciones _ab()_, _ba()_ y _nada()_.
                  @param int Recibe como parámetro la señal de ingreso. Debe tener la misma
                  estructura, o forma, que los prototipos a los que va a apuntar. Es por
                  ello, que las funciones de transición son todas del mismo estilo: reciben
                  un _int_.
                  @return int Retorna un valor entero tal cual el prototipo de las funciones 
                  de manejo de transiciones a las cuales apuntará. */
    int prox_estado; /**< @brief Identifica el estado siguiente luego de la
                       ejecución de la función de transición. */
} estado_t;
/*
 * 3er punto - Continuacion
 * Definicion de la situacion en cada uno de los estados
 * lo que recibe - la funcion que ejecuta - proximo estado
 */
// clang-format off
estado_t a[] = {
    {   0       , nada  ,   A}, /**< esta linea es el elemento cero del arreglo a del tipo estados */
    {   1       , ab    ,   B}, /**< el elemento 1 del arreglo a del tipo estados */
    {CUALQUIERA , nada  ,   A}, /**< el elemento 2 del arreglo a del tipo estados */
};

estado_t b[] = {
    {   0       , ba    ,   A}, /**< esta linea es el elemento cero del arreglo b del tipo estados */
    {   1       , nada  ,   B}, /**< el elemento 1 del arreglo b del tipo estados */
    {CUALQUIERA , nada  ,   B}, /**< el elemento 2 del arreglo b del tipo estados */
};
// clang-format on
/*
 * 4to punto
 * Tabla de estados
 */
estado_t *tabla_de_estados[] = {a, b};

/*!
  @brief   La funcion transicionar es la que realiza todo el trabajo de buscar,
  según lo que se haya ingresado como input, la función a ejecutar y el estado
  al que apuntará proximamente el puntero al salir de esta función. La función
  es static porque tiene que mantener su estado mediante llamadas. Un claro
  ejemplo es el puntero a la tabla de estados, no dene inicializarse sirmpre,
  sino que debe mantener el valor con el que salió la última vez de la función
  para poder buscar bién las acciones a seguir dependiendo del estado actual.
  
  @param   c  Es el input que se le dió al estado.
  
  @return  Retorna un entero 0 cuando termina la ejecución de forma correcta.
  Podría haber sido void ya que el resultado de esta función no es utilizado
  para realizar ningún cálculo.
  
  @note    Esta es la función más importante del programa de Autómata Finito.
  Esta es quién corre el puntero entre los estados y colecta tanto las acciones
  a ejecutar como los estados a los que se debe ir.
*/
static int transicionar(int c) {
    /*
     * Esta función básicamente debe:
     * - Crear un puntero y una variable que mantenga el valor del estado entre
     * ejecuciones
     * - Ejecutar un for para mover el puntero.
     * - Una vez movido el puntero, ejecutar la funcion con el puntero a la
     * función.
     * - Guardar el siguiente estado en la variable estática.
     */
    estado_t *p; 
    static int temp = A; 
    printf("Estando en el estado %c recibo un %d se ejecuta ", temp ? 'B' : 'A',
           c);
    for (p = tabla_de_estados[temp]; p->recibo != c && p->recibo != CUALQUIERA;
         p++) 
        ;
    (p->transicion)(c); 
    temp = p->prox_estado; 
    printf(" y se pasa al estado %c\n\n", temp ? 'B' : 'A');
    return 0;
}
/*!
  @brief   La función principal del programa. No recibeb argumentos. Su único
  desempeño es preguntar el input para el ingreso por teclado de un número
  entero.
  @param   X  No recibe ningún parámetro o argumento.
  @return  Retorna como señal de buen funcionamiento un 0.
*/
int main() {
    unsigned int c;
    /* preguntar por teclado el input */
    do {
        printf("input: ");
        scanf("%d", &c);
        transicionar(c);
    } while (c != EOF && c != 115);
    return 0;
}
/*!
  @brief  Esta función, es la que se ejecuta cuando uno se desplaza desde el
  estado A al estado B. Tal cual está programado ahora, esta función de
  transición sucederá cuando, estando en el estado A se ingrese un 1.
  @param  c  El parámetro que recibe es un entero y es el input ingresado por el
  teclado. No es utilizado en esta programación ya que es un chasis. Esta
  función debería levantar una bandera y salir para no perder tiempo en la
  rutina.
  @return Retorna un entero que no es utilizado para cálculos posteriores, así
  es que la función se podría cambiar a void sin mayores cambios.
*/
int ab(int c) {
    printf("ab(c)");
    return 0;
}
/*!
  @brief  Esta función, es la que se ejecuta cuando uno se desplaza desde el
  estado B al estado A. Tal cual está programado ahora, esta función de
  transición sucederá cuando, estando en el estado B se ingrese un 0.
  @param  c  El parámetro que recibe es un entero y es el input ingresado por el
  teclado. No es utilizado en esta programación ya que es un chasis. Esta
  función debería levantar una bandera y salir para no perder tiempo en la
  rutina.
  @return Retorna un entero que no es utilizado para cálculos posteriores, así
  es que la función se podría cambiar a void sin mayores cambios.
*/
int ba(int c) {
    printf("ba(c)");
    return 0;
}
/*!
  @brief  Esta función es una función inerte que no realiza nada, es la que se
  ejecuta cuando estando A se ingresa un 0 o estando en el estado B se ingresa
  un 1, y siempre termina en el mismo estado en el que estaba, tal cual está
  programado ahora.
  @param  c  El parámetro que recibe es un entero y es el input ingresado por el
  teclado. No es utilizado en esta programación ya que es un chasis. Esta
  función debería levantar una bandera y salir para no perder tiempo en la
  rutina.
  @return Retorna un entero que no es utilizado para cálculos posteriores, así
  es que la función se podría cambiar a void sin mayores cambios.
*/
int nada(int c) {
    printf("nada(c)");
    return 0;
}
