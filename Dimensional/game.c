#include "structure_data.h"
#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


game_state_t game_state = { 0 }; // ***DONT TOUCH***(unless you know what you're doing and why)
float precision = 1.0e3f;
int precision_ = 3;

static void exp_norm(exponent_t* exp);
static void exp_sum(exponent_t* expa, exponent_t* expb);
static void exp_mult(exponent_t* expa, exponent_t* expb);
static void e_culm();
static void r_culm();
static void e_r_wipe();
static void r_init(const char* name, exponent_t bexp, exponent_t aexp, resource_group_t* rg, const char* rg_name);
static void e_init(const char* name, resource_group_t* r, effect_group_t* ea, exponent_t babexp, exponent_t bmbexp, effect_group_t* eg, const char* eg_name);
static void r_g_init(const char* name, resource_t* r);
static void e_g_init(const char* name, effect_t* e);
static void r_g_edit(resource_group_t* rg, resource_t r);
static void e_g_edit(effect_group_t* eg, effect_t* e);

void tick_calculate() {
	e_culm();
	r_culm();
	printf("%f\n", game_state.resource_groups->resources[0].amount.m);
	e_r_wipe();
}

static void r_culm() {
	game_state_t* gs = &game_state;
	for (uint32_t i = 0; i < gs->effect_group_count; i++) {
		effect_group_t* eg = &gs->effect_groups[i];
		for (uint32_t j = 0; j < eg->effect_count; j++) {
			effect_t* e = &eg->effects[j];
			if (e->resources_affected) {
				resource_group_t* reg = e->resources_affected;
				for (uint8_t k = 0; k < reg->resource_count; k++) {
					resource_t* sr = &reg->resources[k];
					exp_sum(&sr->effective_gain, &e->effective_additive_bonus);
					exp_mult(&sr->effective_gain, &e->effective_mult_bonus);
					exp_mult(&sr->effective_gain, &sr->base_gain);
					exp_sum(&sr->amount, &sr->effective_gain);
				}
			}
		}
	}
}

static void e_culm() {
	game_state_t* gs = &game_state;
	for (uint32_t i = 0; i < gs->effect_group_count; i++) {
		effect_group_t* eg = &gs->effect_groups[i];
		for (uint32_t j = 0; j < eg->effect_count; j++) {
			effect_t* e = &eg->effects[j];
			if (e->effects_affected) {
				effect_group_t* seg = e->effects_affected;
				for (uint8_t k = 0; k < seg->effect_count; k++) {
					effect_t* se = &seg->effects[k];
					exp_sum(&se->effective_additive_bonus, &e->effective_additive_bonus);
					exp_sum(&se->effective_mult_bonus, &e->effective_additive_bonus);
					exp_mult(&se->effective_additive_bonus, &e->effective_mult_bonus);
					exp_mult(&se->effective_mult_bonus, &e->effective_mult_bonus);
				}
			}
		}
	}
}

static void e_r_wipe() {
	game_state_t* gs = &game_state;
	for (uint32_t i = 0; i < gs->effect_group_count; i++) {
		effect_group_t* eg = &gs->effect_groups[i];
		for (uint32_t j = 0; j < eg->effect_count; j++) {
			effect_t* e = &eg->effects[j];
			e->effective_additive_bonus = e->base_additive_bonus;
			e->effective_mult_bonus = e->base_mult_bonus;
			if (e->resources_affected) {
				resource_group_t* reg = e->resources_affected;
				for (uint8_t k = 0; k < reg->resource_count; k++) {
					resource_t* sr = &reg->resources[k];
					sr->effective_gain = sr->base_gain;
				}
			}
		}
	}
}

wchar_t* exp_to_str(exponent_t exp) {
	static wchar_t buffer[64] = {0};
	wchar_t format[16];
	swprintf_s(format, _countof(format), L"%%.%df", precision_);
	swprintf_s(buffer, _countof(buffer), format, exp.m);
	for (uint8_t i = 0; i < MAX_DEPTH && exp.e[i] != 0.0f; i++) {
		wchar_t temp[32];
		swprintf_s(format, _countof(format), L"E%%.%df", 1);
		swprintf_s(temp, _countof(temp), format, exp.e[i]);
		wcscat_s(buffer, _countof(buffer), temp);
	}
	return buffer;
}

static void exp_norm(exponent_t* exp){
	while (exp->m >= precision) {
		exp->m /= precision;
		exp->e[0] += 1.0f;
	}
	while (exp->m > 0.0f && exp->m < 1.0f) {
		exp->m *= precision;
		exp->e[0] -= 1.0f;
	}
	for (int i = 0; i < MAX_DEPTH - 1; i++) {
		while (exp->e[i] >= precision) {
			exp->e[i] -= precision;
			exp->e[i + 1] += 1.0f;
		}
		while (exp->e[i] < 0.0f) {
			exp->e[i] += precision;
			exp->e[i + 1] -= 1.0f;
		}
	}
}

static void exp_sum(exponent_t* expa, exponent_t* expb) {
	int depth_a = 0, depth_b = 0;
	float exp_val_a = 0.0f, exp_val_b = 0.0f;
	while (depth_a < MAX_DEPTH && expa->e[depth_a] != 0.0f) {
		exp_val_a += expa->e[depth_a] * powf(precision, depth_a);
		depth_a++;
	}
	while (depth_b < MAX_DEPTH && expb->e[depth_b] != 0.0f) {
		exp_val_b += expb->e[depth_b] * powf(precision, depth_b);
		depth_b++;
	}
	if (fabsf(exp_val_a - exp_val_b) < 0.01f) {
		expa->m += expb->m;
	}
	else if (exp_val_a > exp_val_b) {
		float delta = exp_val_a - exp_val_b;
		expa->m += expb->m / powf(precision, delta);
	}
	else {
		float delta = exp_val_b - exp_val_a;
		expa->m = expa->m / powf(precision, delta) + expb->m;
		memcpy(expa->e, expb->e, sizeof(float) * MAX_DEPTH);
	}
	exp_norm(expa);
}

static void exp_mult(exponent_t* expa, exponent_t* expb) {
	expa->m *= expb->m;
	for (int i = 0; i < MAX_DEPTH; i++) {
		expa->e[i] += expb->e[i];
	}
	exp_norm(expa);
}

static void console_print_debug() {
	printf("\nNEW DEBUG PRINT\n");
	printf("Resource Group Count: %u\n", game_state.resource_group_count);
	for (uint16_t i = 0; i < game_state.resource_group_count; i++) {
		resource_group_t* rg = &game_state.resource_groups[i];
		printf("Resource Group: %s, at Index %u\n", rg->name, i);
		printf("  Resource Count: %u\n", rg->resource_count);
		for (uint8_t j = 0; j < rg->resource_count; j++) {
			resource_t* r = &rg->resources[j];
			wprintf(L"	Resource: %hs, Base Gain: %ls, Effective Gain: %ls, Amount: %ls\n",
				r->name,
				exp_to_str(r->base_gain),
				exp_to_str(r->effective_gain),
				exp_to_str(r->amount)
				);
		}
	}
	printf("Resource Sub Group Count: %u\n", game_state.sub_resource_group_count);
	for (uint16_t i = 0; i < game_state.sub_resource_group_count; i++) {
		resource_group_t* srg = &game_state.sub_resource_groups[i];
		printf("Sub Resource Group: %s, at Index %u\n", srg->name, i);
		printf("  Resource Count: %u\n", srg->resource_count);
		for (uint8_t j = 0; j < srg->resource_count; j++) {
			resource_t* r = &srg->resources[j];
			wprintf(L"		Sub Resource: %hs, Base Gain: %ls, Effective Gain: %ls, Amount: %ls\n",
				r->name,
				exp_to_str(r->base_gain),
				exp_to_str(r->effective_gain),
				exp_to_str(r->amount)
				);
		}

	}

	printf("Effect Group Count: %u\n", game_state.effect_group_count);
	for (uint16_t i = 0; i < game_state.effect_group_count; i++) {
		effect_group_t* eg = &game_state.effect_groups[i];
		printf("Effect Group: %s, at Index %u\n", eg->name, i);
		printf("  Effect Count: %u\n", eg->effect_count);
		for (uint8_t j = 0; j < eg->effect_count; j++) {
			effect_t* e = &eg->effects[j];
			wprintf(L"	Effect: %hs, Base Additive Bonus: %ls, Effective Additive Bonus: %ls, Base Mult Bonus: %ls, Effective Mult Bonus: %ls\n",
				e->name,
				exp_to_str(e->base_additive_bonus),
				exp_to_str(e->effective_additive_bonus),
				exp_to_str(e->base_mult_bonus),
				exp_to_str(e->effective_mult_bonus)
				);
		}
	}

	printf("Effect Sub Group Count: %u\n", game_state.sub_effect_group_count);
	for (uint16_t i = 0; i < game_state.sub_effect_group_count; i++) {
		effect_group_t* seg = &game_state.sub_effect_groups[i];
		printf("Sub Effect Group: %s, at Index %u\n", seg->name, i);
		printf("  Effect Count: %u\n", seg->effect_count);
		for (uint8_t j = 0; j < seg->effect_count; j++) {
			effect_t* e = &seg->effects[j];
			wprintf(L"		Sub Effect: %hs, Base Additive Bonus: %ls, Effective Additive Bonus: %ls, Base Mult Bonus: %ls, Effective Mult Bonus: %ls\n",
				e->name,
				exp_to_str(e->base_additive_bonus),
				exp_to_str(e->effective_additive_bonus),
				exp_to_str(e->base_mult_bonus),
				exp_to_str(e->effective_mult_bonus)
			);
		}
	}

	printf("\nDEBUG PRINT END\n\n");
}

static void r_init(const char* name, exponent_t bexp, exponent_t aexp, resource_group_t* rg, const char* rg_name) {
	printf("Initializing resource %s in group %s\n", name, rg_name ? rg_name : (rg ? rg->name : "NULL"));
	resource_t r = { 0 };
	strncpy_s(r.name, sizeof(r.name), name, sizeof(r.name));
	r.name[sizeof(r.name) - 1] = '\0';
	r.base_gain = bexp;
	r.effective_gain = bexp;
	r.amount = aexp;
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

static void e_init(const char* name, resource_group_t* r, effect_group_t* ea, exponent_t babexp, exponent_t bmbexp, effect_group_t* eg, const char* eg_name) {
	printf("Initializing effect %s in group %s\n", name, eg_name ? eg_name : (eg ? eg->name : "NULL"));
	effect_t e = { 0 };
	strncpy_s(e.name, sizeof(e.name), name, sizeof(e.name));
	e.resources_affected = r; e.effects_affected = ea;
	e.base_additive_bonus = babexp;
	e.effective_additive_bonus = babexp;
	e.base_mult_bonus = bmbexp;
	e.effective_mult_bonus = bmbexp;
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
	printf("Creating sub-resource group from index %u to %u in group %s\n", si, ei, rg->name);
	uint8_t c = ei - si;
	resource_group_t nrg = { 0 };
	nrg.resource_count = c;
	nrg.resources = malloc(sizeof(resource_t) * c);
	if (nrg.resources) {
		printf("Sub-resource group %s will have %u resources\n", rg->name, c);
		for (uint8_t i = 0; i < c; i++) {
			printf("Adding resource %s to sub-resource group %s\n", rg->resources[si + i].name, rg->name);
			nrg.resources[i] = rg->resources[si + i];
		}
		resource_group_t* temp = realloc(game_state.sub_resource_groups, (sizeof(resource_group_t) * (game_state.sub_resource_group_count + 1)));
		if (temp) {
			printf("Sub-resource group %s created at index %u\n", nrg.name, game_state.sub_resource_group_count);
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
	printf("Creating sub-effect group from index %u to %u in group %s\n", si, ei, eg->name);
	uint8_t c = ei - si;
	effect_group_t neg = { 0 };
	neg.effect_count = c + 1;
	neg.effects = malloc(sizeof(effect_t) * (c + 1));
	if (neg.effects) {
		printf("Sub-effect group %s will have %u effects\n", eg->name, c + 1);
		for (uint8_t i = 0; i <= c; i++) {
			printf("Adding effect %s to sub-effect group %s\n", eg->effects[si + i].name, eg->name);
			neg.effects[i] = eg->effects[si + i];
		}
		effect_group_t* temp = realloc(game_state.sub_effect_groups, (sizeof(effect_group_t) * (game_state.sub_effect_group_count + 1)));
		if (temp) {
			printf("Sub-effect group %s created at index %u\n", neg.name, game_state.sub_effect_group_count);
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
	r_init("Point",
		(exponent_t){.m = 1.0, .e = {[0] = 1}},
		(exponent_t){.m = 1.0, .e = {[0] = 1}},
		NULL, "d1_g");

	e_init("d1_e1", &gs->resource_groups[0], NULL,
		(exponent_t){.m = 1.0, .e = {[0] = 1}}, 
		(exponent_t){.m = 1.0, .e = {[0] = 1}},
		NULL, "d1_e_g");
	e_init("d1_e2", &gs->resource_groups[0], NULL,
		(exponent_t){.m = 0, .e = {0}},
		(exponent_t){.m = 1.5, .e = {[0] = 1}},
		&gs->effect_groups[0], NULL);
	e_init("|e1-e1|_e1", NULL, &gs->sub_effect_groups[e_g_range(&gs->effect_groups[0], 0, 0)],
		(exponent_t){.m = 0, .e = {0}},
		(exponent_t){.m = 1.5, .e = {[0] = 1}},
		NULL, "|e1-e1|_e_g");
	e_g_edit(&gs->sub_effect_groups[0], &gs->effect_groups[1].effects[0]);
	console_print_debug();

}

void game_init() {
	g_s_init();
}

