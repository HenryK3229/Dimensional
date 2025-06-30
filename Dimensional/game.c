#include "structure_data.h"
#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


game_state_t game_state = { 0 }; // ***DONT TOUCH***(unless you know what you're doing and why)

static void r_init(const char* name, uint8_t bgm, uint8_t bge, uint8_t egm, uint8_t ege, uint8_t am, uint8_t ae, resource_group_t* rg, const char* rg_name);
static void e_init(const char* name, resource_group_t* r, effect_group_t* p, uint8_t abm, uint8_t abe, uint8_t mbm, uint8_t mbe, effect_group_t* eg, const char* eg_name);
static void r_g_init(const char* name, resource_t* r);
static void e_g_init(const char* name, effect_t* e);
static void r_g_edit(resource_group_t* rg, resource_t r);
static void e_g_edit(effect_group_t* eg, effect_t* e);

void tick_calculate() {

}

static void console_print_debug() {
	printf("\nNEW DEBUG PRINT\n");
	printf("Resource Group Count: %u\n", game_state.resource_group_count);
	for (uint8_t i = 0; i < game_state.resource_group_count; i++) {
		resource_group_t* rg = &game_state.resource_groups[i];
		printf("Resource Group: %s, at Index %u\n", rg->name, i);
		printf("  Resource Count: %u\n", rg->resource_count);
		for (uint8_t j = 0; j < rg->resource_count; j++) {
			resource_t* r = &rg->resources[j];
			printf("	Resource: %s, Base Gain: %u.%u, Effective Gain: %u.%u, Amount: %u.%u\n",
				r->name, r->base_gain_mantissa, r->base_gain_exponent1,
				r->effective_gain_mantissa, r->effective_gain_exponent1,
				r->amount_mantissa, r->amount_exponent1);
		}
	}
	printf("Resource Sub Group Count: %u\n", game_state.sub_resource_group_count);
	for (uint8_t i = 0; i < game_state.sub_resource_group_count; i++) {

	}

	printf("Effect Group Count: %u\n", game_state.effect_group_count);
	for (uint8_t i = 0; i < game_state.effect_group_count; i++) {
		effect_group_t* eg = &game_state.effect_groups[i];
		printf("Effect Group: %s, at Index %u\n", eg->name, i);
		printf("  Effect Count: %u\n", eg->effect_count);
		for (uint8_t j = 0; j < eg->effect_count; j++) {
			effect_t* e = &eg->effects[j];
			printf("	Effect: %s, Additive Bonus: %u.%u, Mult Bonus: %u.%u\n",
				e->name, e->additive_bonus_mantissa, e->additive_bonus_exponent1,
				e->mult_bonus_mantissa, e->mult_bonus_exponent1);
		}
	}

	printf("\nDEBUG PRINT END\n\n");
}


static void r_init(const char* name, uint8_t bgm, uint8_t bge, uint8_t egm, uint8_t ege, uint8_t am, uint8_t ae, resource_group_t* rg, const char* rg_name) {
	printf("Initializing resource %s in group %s\n", name, rg_name ? rg_name : (rg ? rg->name : "NULL"));
	resource_t r = { 0 };
	strncpy_s(r.name, sizeof(r.name), name, sizeof(r.name));
	r.name[sizeof(r.name) - 1] = '\0';
	r.base_gain_mantissa = bgm; r.base_gain_exponent1 = bge;
	r.effective_gain_mantissa = egm; r.effective_gain_exponent1 = ege;
	r.amount_mantissa = am; r.amount_exponent1 = ae;
	uint8_t valid_group = 0;
	for (uint8_t i = 0; i < game_state.resource_group_count; i++) {
		if (&game_state.resource_groups[i] == rg) {
			printf("Found valid resource group %s, adding resource\n", rg->name);
			r_g_edit(rg, r);
			valid_group++; }
	}
	if (!valid_group) {
		printf("No valid resource group found, creating new resource group %s\n", rg_name ? rg_name : "NULL");
		if (rg_name) {
			char* str = malloc(strlen(rg_name) + 1);
			if (str) {
				strcpy_s(str, strlen(rg_name) + 1, rg_name);
				r_g_init(str, &r);
			}
			else {
				perror("Failed to allocate memory for resource group name");
				free(str);
				exit(1);
			}
		}
		else {
			perror("New resource group name is NULL, cannot initialize resource group");
			exit(1);
		}
	}
}

static void e_init(const char* name, resource_group_t* r, effect_group_t* ea, uint8_t abm, uint8_t abe, uint8_t mbm, uint8_t mbe, effect_group_t* eg, const char* eg_name) {
	printf("Initializing effect %s in group %s\n", name, eg_name ? eg_name : (eg ? eg->name : "NULL"));
	effect_t e = { 0 };
	strncpy_s(e.name, sizeof(e.name), name, sizeof(e.name));
	e.resources_affected = r; e.effects_affected = ea;
	e.additive_bonus_mantissa = abm; e.additive_bonus_exponent1 = abe;
	e.mult_bonus_mantissa = mbm; e.mult_bonus_exponent1 = mbe;
	uint8_t valid_group = 0;
	for (uint8_t i = 0; i < game_state.effect_group_count; i++) {
		if (&game_state.effect_groups[i] == eg) {
			printf("Found valid effect group %s, adding effect\n", eg->name);
			e_g_edit(eg, &e);
			valid_group++; 
		}
	}
	if (!valid_group) {
		printf("No valid effect group found, creating new effect group %s\n", eg_name ? eg_name : "NULL");
		if (eg_name) {
			char* str = malloc(strlen(eg_name) + 1);
			if (str) {
				strcpy_s(str, strlen(eg_name) + 1, eg_name);
				e_g_init(str, &e);
			}
			else {
				perror("Failed to allocate memory for effect group name");
				free(str);
				exit(1);
			}
		}
		else {
			perror("New effect group name is NULL, cannot initialize effect group");
			exit(1);
		}
	}

}

static void r_g_init(const char* name, resource_t* r) {
	printf("Initializing resource group %s\n", name);
	resource_group_t rg = { 0 };
	rg.resource_count = 1;
	strncpy_s(rg.name, sizeof(rg.name), name, sizeof(rg.name));
	rg.resources = malloc(sizeof(resource_t));
	if (rg.resources) {;
		rg.resources[0] = *r;
		resource_group_t* temp = realloc(game_state.resource_groups, (sizeof(resource_group_t) * (game_state.resource_group_count + 1)));
		if (temp) {

			game_state.resource_groups = temp;
			game_state.resource_group_count++;
			game_state.resource_groups[game_state.resource_group_count - 1] = rg;
			printf("Resource group %s initialized at index %u\n", rg.name, game_state.resource_group_count - 1);

		}
		else {
			perror("Failed to allocate memory for resource groups in g_s");
			free(temp);
			exit(1);
		}
	}
	else {
		perror("Failed to allocate memory for new resource group");
		free(rg.resources);
		exit(1);
	}
}

static void e_g_init(const char* name, effect_t* e) {
	printf("Initializing effect group %s\n", name);
	effect_group_t eg = { 0 };
	eg.effect_count = 1;
	strncpy_s(eg.name, sizeof(eg.name), name, sizeof(eg.name));
	eg.effect_count = 1;
	eg.effects = malloc(sizeof(effect_t));
	if (eg.effects) {
		eg.effects[0] = *e;
		effect_group_t* temp = realloc(game_state.effect_groups, (sizeof(effect_group_t) * (game_state.effect_group_count + 1)));
		if (temp) {
			game_state.effect_groups = temp;
			game_state.effect_group_count++;
			game_state.effect_groups[game_state.effect_group_count - 1] = eg;
			printf("Effect group %s initialized at index %u\n", eg.name, game_state.effect_group_count - 1);
		}
		else {
			perror("Failed to allocate memory for effect group for g_s");
			free(temp);
			exit(1);
		}
	}
	else {
		perror("Failed to allocate memory for effect group");
		free(eg.effects);
		exit(1);
	}
}

static void r_g_edit(resource_group_t* rg, resource_t r) {
	uint8_t c = rg->resource_count;
	resource_t* temp = realloc(rg->resources, sizeof(resource_t) * (c + 1));
	if (temp) {
		printf("Editing resource group %s, adding resource %s\n", rg->name, r.name);
		rg->resources = temp;
		rg->resources[c] = r;
		rg->resource_count = c + 1;
		printf("Resource group %s now has resource %s\n", rg->name, rg->resources[c].name);
	}
	else {
		perror("Failed to allocate memory for resource group");
		exit(1);
	}
}

static void e_g_edit(effect_group_t* eg, effect_t* e) {
	uint8_t c = eg->effect_count;
	effect_t* temp = realloc(eg->effects, sizeof(effect_t) * (c + 1));
	if (temp) {
		printf("Editing effect group %s, adding effect %s\n", eg->name, e->name);
		eg->effects = temp;
		eg->effects[c] = *e;
		eg->effect_count++;
		printf("Effect group %s now has effect %s\n", eg->name, eg->effects[c].name);
	}
	else {
		perror("Failed to allocate memory for effect group");
		exit(1);
	}}

static uint8_t r_g_range(resource_group_t* rg, uint8_t si, uint8_t ei) {
	uint8_t c = ei - si;
	resource_group_t nrg = { 0 };
	nrg.resource_count = c;
	nrg.resources = malloc(sizeof(resource_t) * c);
	if (nrg.resources) {
		for (uint8_t i = 0; i < c; i++) {
			nrg.resources[i] = rg->resources[si + i];
		}
		resource_group_t* temp = realloc(game_state.sub_resource_groups, (sizeof(resource_group_t) * (game_state.sub_resource_group_count + 1)));
		if (temp) {
			game_state.sub_resource_groups = temp;
			game_state.sub_resource_groups[game_state.sub_resource_group_count] = nrg;
			game_state.sub_resource_group_count++;
			return game_state.sub_resource_group_count - 1;
		}
		else {
			perror("Failed to allocate memory for sub-resource groups");
			free(temp);
			exit(1);
		}
	}
	else
	{
		perror("Failed to allocate memory for new resource group range");
		free(nrg.resources);
		exit(1);
	}
}

static uint8_t e_g_range(effect_group_t* eg, uint8_t si, uint8_t ei) {
	uint8_t c = ei - si;
	effect_group_t neg = { 0 };
	neg.effect_count = c;
	neg.effects = malloc(sizeof(effect_t) * c);
	if (neg.effects) {
		for (uint8_t i = 0; i < c; i++) {
			neg.effects[i] = eg->effects[si + i];
		}
		effect_group_t* temp = realloc(game_state.sub_effect_groups, (sizeof(effect_group_t) * (game_state.sub_effect_group_count + 1)));
		if (temp) {
			game_state.sub_effect_groups = temp;
			game_state.sub_effect_groups[game_state.sub_effect_group_count] = neg;
			game_state.sub_effect_group_count++;
			return game_state.sub_effect_group_count - 1;
		}
		else {
			perror("Failed to allocate memory for sub-effect groups");
			free(temp);
			exit(1);
		}
	}
	else {
		perror("Failed to allocate memory for new effect group range");
		free(neg.effects);
		exit(1);
	}

}

static void g_s_init() {
	game_state_t* gs = &game_state;
	console_print_debug();
	r_init("d1", 1, 0, 1, 0, 1, 0, NULL, "d1_group");
	r_init("d2", 1, 0, 1, 0, 1, 0, &gs->resource_groups[0], NULL);
	e_init("e1", &gs->resource_groups[0], NULL, 1, 0, 1, 0, NULL, "e1_group");
	console_print_debug();
	e_init("e2", &gs->resource_groups[0], NULL, 1, 0, 1, 0, &gs->effect_groups[0], NULL);
}

void game_init() {
	g_s_init();

}

