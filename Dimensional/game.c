#include "game.h"
#include "resource.h"
#include <stdlib.h>
#include <stdio.h>

/*
look at resource.h to format 'resource_t', 'effect_t', 'producer_t'
look at game.h to format 'resource_states_t', 'game_state_t'


how to add a Resource 'new_resource':
1.forward declare it without initializing		||| add 'resource_t new_resource;'
2.set values on new line						||| add 'resource_t new_resource = {.name = "new_resource", ...rest of info here};'
3.in g_s_init() add to resource count			||| change 'g_s->count = n;' to 'g_s->count = n + 1;'
4.in g_s_init() add resource where specified	||| add 'g_s->resources[n - 1] = new_resource;' below the last resource
5.repeat 1-4 for EACH resource					

how to add an Effect 'new_effect':
1.forward declare without init					||| add 'effect_t new_effect;'
2.set values on new line						||| add 'effect_t new_effect = {.name = "new_effect", 
												|||							.resources_affected = &resource or NULL, .producers_affected = &producer or NULL,
												|||							... rest of info here};
3.in g_s_init() add to effect count				||| change 'g_s->effect_count = n;' to 'g_s->effect_count = n+1;'
4.in g_s_init() add effect where specified		||| add 'g_s->resources[n - 1] = new_effect;' below the last effect
5.repeat 1-4 for EACH effect

how to add a Producer 'new_resource_producer':
1.forward declare withput init					||| add 'producer_t new_producer;'
2.set_values on new line						||| add 'producer_t new_producer = {.name = "resource_producer_1", .type = "resource_producer",
												|||								.produced_effect = &effect or NULL, .affected_by = &effect or NULL};
3.in g_s_init() add to producer_count			||| change 'g_s->producer_count = n;' to 'g_s->producer_count = n+1;'
4.in g_s_init() add producer where specified	||| add 'g_s->producers[n - 1] = new_producer;' below the last producer
5.repeat 1-5 for EACH producer



*/

/*_______________________________________________________________________________________________________________*/

resource_t d1;

effect_t d1_additive;
effect_t d1_multiplier;

producer_t d1_1;

/*_______________________________________________________________________________________________________________*/

resource_t d1 = {			.name = "dimension_1",
							.base_gain_mantissa = 1, .base_gain_exponent1 = 1,
							.effective_gain_mantissa = 1, .effective_gain_exponent1 = 1,
							.amount_mantissa = 1, .amount_exponent1 = 1 };

effect_t d1_additive = {	.name = "dimension_1_additive",
							.resources_affected = &d1, .producers_affected = NULL,
							.additive_bonus_mantissa = 1, .additive_bonus_exponent1 = 0,
							.mult_bonus_mantissa = 0, .mult_bonus_exponent1 = 0};
effect_t d1_multiplier ={	.name = "dimension_1_multiplier",
							.resources_affected = NULL, .producers_affected = &d1_1,
							.additive_bonus_mantissa = 0, .additive_bonus_exponent1 = 0,
							.mult_bonus_mantissa = 2, .mult_bonus_exponent1 = 1};

producer_t d1_1 = {			.name = "dimension_1_producer_1",
							.type = "dimension_1_producer",
							.produced_effect = &d1_additive, .affected_by = &d1_multiplier };

/*_______________________________________________________________________________________________________________*/

game_state_t game_state = { .producers = NULL, .effects = NULL, .resources = NULL, 
							.producer_count = 0, .effect_count = 0, .resource_count = 0};
/*_______________________________________________________________________________________________________________*/

static void tick_calculate() {

}

static void g_s_init() {
	game_state_t* g_s = &game_state;
	g_s->resource_count = 1; //CHANGE RESOURCE COUNT HERE
	g_s->resources = malloc(sizeof(resource_t) * g_s->resource_count);
	if (g_s->resources) { //ADD RESOURCES HERE
		g_s->resources[0] = d1;
	}
	else {
		exit(1);
	}
	g_s->effect_count = 2; //CHANGE EFFECT COUNT HERE
	g_s->effects = malloc(sizeof(effect_t) * g_s->effect_count);
	if (g_s->effects) { //ADD EFFECTS HERE
		g_s->effects[0] = d1_additive;
		g_s->effects[1] = d1_multiplier;
	}
	else {
		exit(1);
	}
	g_s->producer_count = 1; //CHANGE PRODUCER COUNT HERE
	g_s->producers = malloc(sizeof(producer_t) * g_s->producer_count);
	if (g_s->producers) { //ADD PRODUCERS HERE
		g_s->producers[0] = d1_1;
	}
	else {
		exit(1);
	}
}

void game_init(FILE* f) {
	g_s_init();

}

