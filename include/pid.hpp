#pragma once

extern uint16_t setpoint;
extern uint16_t zonaMuerta;

float calculo_pid(uint16_t pos, float deltaTime);

void reiniciar_pid();
