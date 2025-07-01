#pragma once

#include <stdint.h>

#define MAX_DEPTH 10
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct Exponent {
	float m;
	float e[MAX_DEPTH];
} exponent_t;
typedef struct Resource resource_t;
typedef struct ResourceGroup resource_group_t;
typedef struct Effect effect_t;
typedef struct EffectGroup effect_group_t;
typedef struct GameState game_state_t;
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct Resource {
	char name[32];

	exponent_t base_gain;

	exponent_t effective_gain;

	exponent_t amount;

}resource_t;
typedef struct ResourceGroup {
	char name[32];
	resource_t* resources;
	uint8_t resource_count;
} resource_group_t;
typedef struct Effect {	
	char name[32];

	resource_group_t* resources_affected;
	effect_group_t* effects_affected;

	exponent_t base_additive_bonus;

	exponent_t effective_additive_bonus;

	exponent_t base_mult_bonus;

	exponent_t effective_mult_bonus;

}effect_t;
typedef struct EffectGroup {
	char name[32];
	effect_t* effects;
	uint8_t effect_count;
} effect_group_t;
typedef struct GameState {
	effect_group_t* effect_groups;
	effect_group_t* sub_effect_groups;
	resource_group_t* resource_groups;
	resource_group_t* sub_resource_groups;
	uint32_t effect_group_count;
	uint32_t sub_effect_group_count;
	uint32_t resource_group_count;
	uint32_t sub_resource_group_count;
}game_state_t;
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
extern game_state_t game_state;
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/