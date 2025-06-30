#include "structure_data.h"
#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


game_state_t game_state = { 0 }; // ***DONT TOUCH***(unless you know what you're doing and why)

static resource_t r_init(const char* name, uint8_t bgm, uint8_t bge, uint8_t egm, uint8_t ege, uint8_t am, uint8_t ae, resource_group_t* rg);
static effect_t e_init(const char* name, resource_group_t* r, producer_group_t* p, uint8_t abm, uint8_t abe, uint8_t mbm, uint8_t mbe, effect_group_t* eg);
static producer_t p_init(const char* name, effect_group_t* ep, effect_group_t* ab, producer_group_t* pg);
static void r_g_init(const char* name, resource_t* r);
static void e_g_init(const char* name, effect_t* e);
static void p_g_init(const char* name, producer_t* p);
static resource_group_t r_g_edit(resource_group_t rg, resource_t* r);
static effect_group_t e_g_edit(effect_group_t eg, effect_t* e);
static producer_group_t p_g_edit(producer_group_t pg, producer_t* p);

void tick_calculate() {

}

static resource_t r_init(const char* name, uint8_t bgm, uint8_t bge, uint8_t egm, uint8_t ege, uint8_t am, uint8_t ae, resource_group_t* rg) {
	resource_t r = { 0 };
	strncpy_s(r.name, sizeof(r.name), name, sizeof(r.name));
	r.name[sizeof(r.name) - 1] = '\0';
	r.base_gain_mantissa = bgm; r.base_gain_exponent1 = bge;
	r.effective_gain_mantissa = egm; r.effective_gain_exponent1 = ege;
	r.amount_mantissa = am; r.amount_exponent1 = ae;
	uint8_t valid_group = 0;
	for (uint8_t i = 0; i < game_state.resource_group_count; i++) {
		if (&game_state.resource_groups[i] == rg) { *rg = r_g_edit(*rg, &r); valid_group++; }
	}
	if (!valid_group) {
		char* str = malloc(snprintf(NULL, 0, "%s", rg->name) + 1);
		r_g_init(str, &r);
	}
	else { 
		exit(1); 
	}
	return r;

}

static effect_t e_init(const char* name, resource_group_t* r, producer_group_t* p, uint8_t abm, uint8_t abe, uint8_t mbm, uint8_t mbe, effect_group_t* eg) {
	effect_t e = { 0 };
	strncpy_s(e.name, sizeof(e.name), name, sizeof(e.name));
	e.resources_affected = r; e.producers_affected = p;
	e.additive_bonus_mantissa = abm; e.additive_bonus_exponent1 = abe;
	e.mult_bonus_mantissa = mbm; e.mult_bonus_exponent1 = mbe;
	uint8_t valid_group = 0;
	for (uint8_t i = 0; i < game_state.effect_group_count; i++) {
		if (&game_state.effect_groups[i] == eg) { *eg = e_g_edit(*eg, &e); valid_group++; }
	}
	if (!valid_group) {
		char* str = malloc(snprintf(NULL, 0, "%s", eg->name) + 1);
		e_g_init(str, &e);
	}
	else {
		exit(1);
	}
	return e;
}

static producer_t p_init(const char* name, effect_group_t* ep, effect_group_t* ab, producer_group_t* pg) {
	producer_t p = { 0 };
	strncpy_s(p.name, sizeof(p.name), name, sizeof(p.name));
	p.produced_effect = ep; p.affected_by = ab;
	p.group = pg;
	uint8_t valid_group = 0;
	for (uint8_t i = 0; i < game_state.producer_group_count; i++) {
		if (&game_state.producer_groups[i] == pg) { *pg = p_g_edit(*pg, &p); valid_group++; }
	}
	if (!valid_group) {
		char* str = malloc(snprintf(NULL, 0, "%s", pg->name) + 1);
		p_g_init(str, &p);
	}
	else {
		exit(1);
	}
	return p;
}

static void r_g_init(const char* name, resource_t* r) {
	resource_group_t rg = { 0 };
	rg.resource_count = 1;
	rg.resources = malloc(sizeof(resource_t));
	if (rg.resources) {
		rg.resources[0] = *r;
		game_state.resource_group_count++;
		resource_group_t* temp = realloc(game_state.resource_groups, (sizeof(resource_group_t) * game_state.resource_group_count));
		if (temp) {
			game_state.resource_groups = temp;
			game_state.resource_groups[game_state.resource_group_count - 1] = rg;
		}
		else {
			exit(1);
		}
	}
	else {
		exit(1);
	}
}

static void e_g_init(const char* name, effect_t* e) {
	effect_group_t eg = { 0 };
	eg.effect_count = 1;
	eg.effects = malloc(sizeof(effect_t));
	if (eg.effects) {
		eg.effects[0] = *e;
		game_state.effect_groups++;
		effect_group_t* temp = realloc(game_state.effect_groups, (sizeof(effect_group_t) * game_state.effect_group_count));
		if (temp) {
			game_state.effect_groups = temp;
			game_state.effect_groups[game_state.effect_group_count - 1] = eg;
		}
		else {
			exit(1);
		}
	}
	else {
		exit(1);
	}
}

static void p_g_init(const char* name, producer_t* p) {
	producer_group_t pg = { 0 };
	pg.producer_count = 1;
	pg.producers = malloc(sizeof(producer_t));
	if (pg.producers) {
		pg.producers[0] = *p;
		game_state.producer_group_count++;
		producer_group_t* temp = realloc(game_state.producer_groups, (sizeof(producer_group_t) * game_state.producer_group_count));
		if (temp) {
			game_state.producer_groups = temp;
			game_state.producer_groups[-1] = pg;
		}
		else {
			exit(1);
		}
	}
	else {
		exit(1);
		}
}

static resource_group_t r_g_edit(resource_group_t rg, resource_t* r) {
	uint8_t c = rg.resource_count;
	resource_t* temp = realloc(rg.resources, sizeof(resource_t) * (c + 1));
	if (temp) {
		rg.resources = temp;
		rg.resources[-1] = *r;
		rg.resource_count++;
	}
	else {
		exit(1);
	}
	return rg;
}

static effect_group_t e_g_edit(effect_group_t eg, effect_t* e) {
	uint8_t c = eg.effect_count;
	effect_t* temp = realloc(eg.effects, sizeof(effect_t) * (c + 1));
	if (temp) {
		eg.effects = temp;
		eg.effects[-1] = *e;
		eg.effect_count++;
	}
	else {
		exit(1);
	}
	return eg;
}

static producer_group_t p_g_edit(producer_group_t pg, producer_t* p) {
	uint8_t c = pg.producer_count;
	producer_t* temp = realloc(pg.producers, sizeof(producer_t) * (c + 1));
	if (temp) {
		pg.producers = temp;
		pg.producers[-1] = *p;
		pg.producer_count++;
	}
	else {
		exit(1);
	}
	return pg;
}

static void g_s_init() {

}

void game_init() {
	g_s_init();

}

