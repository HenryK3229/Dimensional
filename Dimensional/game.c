#include "game.h"
#include "resource.h"
#include <stdlib.h>
#include <stdio.h>

/*
look at resource.h to format 'resource_t', 'effect_t', 'producer_t'
look at game.h to format 'resource_states_t', 'game_state_t'


how to add a Resource 'new_resource':
1.forward declare it without initializing			||| add 'resource_t new_resource;'
2.set values on new line							||| add 'resource_t new_resource = {.name = "new_resource", ...rest of info here};'
3.in g_s_init() add to resource count				||| change 'g_s->count = n;' to 'g_s->count = n + 1;'
4.in g_s_init() add resource where specified		||| add 'g_s->resources[n] = new_resource;' below the last resource
5.repeat 1-4 for EACH resource					
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
how to add an Effect 'new_effect':
1.forward declare without init						||| add 'effect_t new_effect;'
2.set values on new line							||| add 'effect_t new_effect = {.name = "new_effect", 
													|||							.resources_affected = &resource or NULL, .producers_affected = &producer or NULL,
													|||							... rest of info here};
3.in g_s_init() add to effect count					||| change 'g_s->effect_count = n;' to 'g_s->effect_count = n+1;'
4.in g_s_init() add effect where specified			||| add 'g_s->resources[n] = new_effect;' below the last effect
5.repeat 1-4 for EACH effect
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
how to add a Producer 'new_resource_producer':
1.forward declare without init						||| add 'producer_t new_producer;'
2.set_values on new line							||| add 'producer_t new_producer = {.name = "resource_producer_1", .group = &producer_group,
													|||								.produced_effect = &effect or NULL, .affected_by = &effect or NULL};
3.in g_s_init() add producer to group				||| add 'p_g_init(&producer_group, &producer, n + 1)' bewlow the relevant producer group, where n is the number of producers in that group
5.repeat 1-4 for EACH producer
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
how to add a Producer Group 'new_producer_group':
1.forward declare without init						||| add 'producer_group_t new_producer_group;'
2.set values on new line							||| add 'producer_group_t new_producer_group = {.name = "producer_group_name",
													|||												.producers = NULL, .producer_count = 0};'
3.in g_s_init() add to producer group count			||| change 'g_s->producer_group_count = n;' to 'g_s->producer_group_count = n + 1;'
4.in g_s_init() add producer group where specified	||| add 'g_s->producer_groups[n] = new_producer_group;' below the last producer group
5.repeat 1-4 for EACH producer group

*/

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

resource_t d1;

effect_t d1_additive;
effect_t d1_multiplier;

producer_t d1_1;

producer_group_t d1_g;

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

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

producer_t d1_1 = {			.name = "dimension_1_producer_1", .group = &d1_g,
							.produced_effect = &d1_additive, .affected_by = &d1_multiplier };

producer_group_t d1_g = {   .name = "dimension_1_producer_group",
							.producers = NULL, .producer_count = 0 };

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

game_state_t game_state = { .producer_groups = NULL, .effects = NULL, .resources = NULL,		 // initialise game_state with NULL pointers and 0 counts
							.producer_group_count = 0, .effect_count = 0, .resource_count = 0 }; // ***DONT TOUCH***(unless you know what you're doing and why)

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

static void tick_calculate() {

}

static void p_g_init(producer_group_t* p_g, producer_t* p, uint32_t p_c) {
	producer_group_t* g = realloc(p_g->producers, sizeof(producer_t) * p_c);
	if (g) {
		g->producers[p_c] = *p;
	}
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
	g_s->producer_group_count = 1; //CHANGE PRODUCER_GROUP COUNT HERE
	g_s->producer_groups = malloc(sizeof(producer_group_t) * g_s->producer_group_count);
	if (g_s->producer_groups) { //ADD PRODUCER_GROUPS HERE

		g_s->producer_groups[0] = d1_g;
		p_g_init(&d1_g, &d1_1, 1); //ADD PRODUCERS TO GROUP HERE
		

	}
	else {
		exit(1);
	}

}

void game_init(FILE* f) {
	g_s_init();

}

