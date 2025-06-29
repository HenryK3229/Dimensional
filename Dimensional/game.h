#pragma once
#include "resource.h"

#ifndef GAME_H
#define GAME_H


typedef struct GameState game_state_t;


typedef struct GameState {
	producer_t* producers;
	effect_t* effects;
	resource_t* resources;	
	uint16_t effect_count;
	uint16_t producer_count;
	uint16_t resource_count;
}game_state_t;

extern game_state_t game_state;


#endif