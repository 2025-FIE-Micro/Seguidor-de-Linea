#pragma once

extern uint16_t setpoint;
extern uint16_t zonaMuerta;
extern uint8_t baseSpeed;

// ============================
// CONTROL PID - METODO Ziegler-Nichols
// ============================
extern const float Ku;
extern const float Tu;

extern const float Kp;
extern const float Ki;
extern const float Kd;

float calculo_pid(uint16_t pos, float deltaTime);

void reiniciar_pid();
