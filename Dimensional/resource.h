#pragma once
#include <stdint.h>


typedef struct Resource resource_t;
typedef struct Producer producer_t;
typedef struct Effect effect_t;


typedef struct Resource {
	char name[32];

	uint8_t base_gain_mantissa;
	uint8_t base_gain_exponent1;

	uint8_t effective_gain_mantissa;
	uint8_t effective_gain_exponent1;

	uint8_t amount_mantissa;
	uint8_t amount_exponent1;

}resource_t;

typedef struct Producer {
	char name[32];
	char type[32];

	effect_t* produced_effect;
	effect_t* affected_by;

} producer_t;

typedef struct Effect {	
	char name[32];

	resource_t* resources_affected;
	producer_t* producers_affected;

	uint8_t additive_bonus_mantissa;
	uint8_t additive_bonus_exponent1;

	uint8_t mult_bonus_mantissa;
	uint8_t mult_bonus_exponent1;
	

}effect_t;

extern resource_t d1;
extern producer_t d1_1;
extern effect_t d1_multiplier;
