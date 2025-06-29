#include "structure_data.h"

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
resource_t d1 = { .name = "dimension_1",
							.base_gain_mantissa = 1, .base_gain_exponent1 = 1,
							.effective_gain_mantissa = 1, .effective_gain_exponent1 = 1,
							.amount_mantissa = 1, .amount_exponent1 = 1 };
effect_t d1_additive = { .name = "dimension_1_additive",
							.resources_affected = &d1, .producers_affected = NULL,
							.additive_bonus_mantissa = 1, .additive_bonus_exponent1 = 0,
							.mult_bonus_mantissa = 0, .mult_bonus_exponent1 = 0 };
effect_t d1_multiplier = { .name = "dimension_1_multiplier",
							.resources_affected = NULL, .producers_affected = &d1_1,
							.additive_bonus_mantissa = 0, .additive_bonus_exponent1 = 0,
							.mult_bonus_mantissa = 2, .mult_bonus_exponent1 = 1 };
producer_t d1_1 = { .name = "dimension_1_producer_1", .group = &d1_g,
							.produced_effect = &d1_additive, .affected_by = &d1_multiplier };
producer_group_t d1_g = { .name = "dimension_1_producer_group",
							.producers = NULL, .producer_count = 0 };
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
game_state_t game_state = { .producer_groups = NULL, .effects = NULL, .resources = NULL,		 // initialise game_state with NULL pointers and 0 counts
							.producer_group_count = 0, .effect_count = 0, .resource_count = 0 }; // ***DONT TOUCH***(unless you know what you're doing and why)
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
