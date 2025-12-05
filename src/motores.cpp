/*
    programa de direccion y control de motores
    * movimiento con correcion para atras
    * movimiento con correcion solamente recta
    * control de motores - setpoint o pid
    * TODO: separar correccion y setpoint  
*/

#include <Arduino.h>
#include "sensores.hpp"
#include "config.hpp"
#include "drv8833.hpp"
#include "motores.hpp"
#include "pid.hpp"

// ============================
// FUNCIONES PARA MOVER MOTORES
// ============================
void moverMotores(int32_t motorSpeedIzq, int32_t motorSpeedDer) {
    deb(Serial.printf("MotorIzq=%d\n", motorSpeedIzq);)
    deb(Serial.printf("MotorDer=%d\n", motorSpeedDer);)

    if      (motorSpeedIzq > 0) {   motorIzq.forward(motorSpeedIzq);        }
    else if (motorSpeedIzq < 0) {   motorIzq.reverse(abs(motorSpeedIzq));   }
    else                        {   motorIzq.stop();    }

    if      (motorSpeedDer > 0) {   motorDer.forward(motorSpeedDer);        }
    else if (motorSpeedDer < 0) {   motorDer.reverse(abs(motorSpeedDer));   }
    else                        {   motorDer.stop();    }   
}

void moverMotoresSinCorrecion(int32_t motorSpeedIzq, int32_t motorSpeedDer) {
    //motorSpeedIzq = map(motorSpeedIzq, -100, 100, 0, 100);
    //motorSpeedDer = map(motorSpeedDer, -100, 100, 0, 100);

    motorSpeedIzq = constrain(motorSpeedIzq, 0, 100);
    motorSpeedDer = constrain(motorSpeedDer, 0, 100);

    deb(Serial.printf("MotorIzq=%d\n", motorSpeedIzq);)
    deb(Serial.printf("MotorDer=%d\n", motorSpeedDer);)

    if      (motorSpeedIzq > 0) {   motorIzq.forward(motorSpeedIzq);        }
    else                        {   motorIzq.stop();    }
    if      (motorSpeedDer > 0) {   motorDer.forward(motorSpeedDer);        }
    else                        {   motorDer.stop();    }   
}

// ============================
// FUNCION CONTROL MOTORES - Zona muerta o Pid
// ============================
void controlMotores(float correcion) {
    // En Zona muerta dejo de acumular y solo acelero
    if (abs(position - setpoint) < zonaMuerta) {
        motorSpeedIzq++;    // aumento las velocidades lentamente
        motorSpeedDer++;
        
        correcion = 0;      // no corregir
        integral = 0;       // dejar de acumular I
    }
    else{
        // Calcular velocidad motores
        motorSpeedIzq = baseSpeed - correcion;
        motorSpeedDer = baseSpeed + correcion;
    }
    // Limitar a rango válido incluyendo negativos
    motorSpeedIzq = constrain(motorSpeedIzq, -maxSpeed, maxSpeed);
    motorSpeedDer = constrain(motorSpeedDer, -maxSpeed, maxSpeed);
}