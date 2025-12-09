#pragma once

#ifndef FSM_HPP
#define FSM_HPP
// ==================== Estados ====================
enum estados { CUALQUIERA = -1, S, A, C, CANT_ESTADOS };

// ==================== Interfaz de la FSM ====================
int transicionar(int entrada);

// ==================== Acciones de estado ====================
void estadoStop();
void estadoAcel();
void estadoControl();

#endif
