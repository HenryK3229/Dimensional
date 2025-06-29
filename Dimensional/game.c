#include "structure_data.h"
#include <stdlib.h>
#include <stdio.h>

#define TICK_RATE_HTZ 60
#define TICK_RATE_MS (1000 / TICK_RATE_HTZ)



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

