/**
 @file config.cpp
 @brief Implementación de las constantes de configuración global y lógica de selección de perfil.
 *Nota:* Las variables globales (extern) se definen aquí con su valor inicial, dependiendo del perfil de corredor seleccionado en platformio.ini.
 @author Legion de Ohm
 */

#include "config.hpp"


// ===================================
// LINEA NEGRA o BLANCA - CAMBIAR EN PLATFORMIO.INI
// ===================================

/**
 @brief Selecciona el modo de detección de línea: Negro o Blanco. Si la bandera `LINEA_NEGRA` está definida en platformio.ini, el seguidor de linea buscará la línea negra. De lo contrario, buscará la línea blanca. El valor asignado se utiliza para configurar la calibración de los sensores.
 @var linea_competencia
 */
#ifdef LINEA_NEGRA
    Linea linea_competencia = NEGRA;
#else
    Linea linea_competencia = BLANCA;
#endif 


// ===================================
// SELECCION DE CORREDOR - CAMBIAR EN PLATFORMIO.INI
// ===================================
#if (CORREDOR == NIGHTFALL)
    /** @brief Perfil NIGHTFALL: Ajustes para motores de 500 RPM. */
    #define USAR_MICROS   0         ///< Define si se usan micros() o millis(). (0 = millis)
    uint8_t baseSpeed = 90;         ///< Velocidad base de los motores (PWM 0-100%).
    const float Ku = 0.045;         ///< Ganancia crítica (Ku) para este perfil.
    const float Tu = 0.5;           ///< Período crítico (Tu) para este perfil.
#elif (CORREDOR == ARGENTUM)
    /** @brief Perfil ARGENTUM: Ajustes para motores de 800 RPM. */    
    #define USAR_MICROS   0         ///< Define si se usan micros() o millis(). (0 = millis)
    uint8_t baseSpeed = 78;         ///< Velocidad base de los motores (PWM 0-100%).
    const float Ku = 0.05;          ///< Ganancia crítica (Ku) para este perfil.
    const float Tu = 0.31;          ///< Período crítico (Tu) para este perfil.
#elif (CORREDOR == DIEGO)
    /** @brief Perfil DIEGO: Ajustes para motores de 800 RPM. */
    #define USAR_MICROS   0         ///< Define si se usan micros() o millis(). (0 = millis)
    uint8_t baseSpeed = 70;         ///< Velocidad base de los motores (PWM 0-100%).
    const float Ku = 0.05;          ///< Ganancia crítica (Ku) para este perfil.
    const float Tu = 0.38;          ///< Período crítico (Tu) para este perfil.
#else
  #error "Valor de CORREDOR inválido. Use NIGHTFALL, ARGENTUM o DIEGO."
#endif


// ============================
// CONTROL PID - METODO Ziegler-Nichols
// ============================

/**
 @brief Lógica para la asignación de constantes finales PID. Si `TEST_PID` está definido, usa las constantes de prueba (solo Kp = Ku y el periodo Tu). Si no está definido, calcula las constantes finales usando las fórmulas derivadas del método de Ziegler-Nichols (PID clásico).
 */
#ifdef TEST_PID
    const float Kp = Ku;         ///< Kp se establece a Ku para el test de oscilación.
    const float Ki = 0;          ///< Ki se deshabilita para el test.
    const float Kd = 0;          ///< Kd se deshabilita para el test.
    #define MUTEAR               ///< Se recomienda mutear el robot durante la fase de test.
#else
    /**
     @brief Fórmulas para el cálculo de las constantes PID basadas en Ziegler-Nichols (PID Clásico). La fórmula usada es: $ Kp = 0.6 \cdot K_u $; $ K_i = 2 \cdot K_p / T_u $; $ K_d = K_p \cdot T_u / 8 $. Esto resulta en una respuesta de amortiguamiento del 75%.
    */
    const float Kp = 0.6 * Ku;
    const float Ki = 2 * Kp / Tu;
    const float Kd = Kp * Tu / 8;
#endif