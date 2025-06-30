#pragma once
#include <stdint.h>

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct Resource resource_t;
typedef struct ResourceGroup resource_group_t;
typedef struct Effect effect_t;
typedef struct EffectGroup effect_group_t;
typedef struct GameState game_state_t;
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct Resource {
	char name[32];

	uint8_t base_gain_mantissa;
	uint8_t base_gain_exponent1;

	uint8_t effective_gain_mantissa;
	uint8_t effective_gain_exponent1;

	uint8_t amount_mantissa;
	uint8_t amount_exponent1;

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

	uint8_t additive_bonus_mantissa;
	uint8_t additive_bonus_exponent1;

	uint8_t mult_bonus_mantissa;
	uint8_t mult_bonus_exponent1;
	

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
	uint16_t effect_group_count;
	uint16_t sub_effect_group_count;
	uint16_t resource_group_count;
	uint16_t sub_resource_group_count;
}game_state_t;
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
extern game_state_t game_state;
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/