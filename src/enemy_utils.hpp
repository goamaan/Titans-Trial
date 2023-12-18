#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "components.hpp"
#include "world_init.hpp"
#include "world_system.hpp"

static float next_enemy_spawn;

void do_enemy_spawn(float elapsed_ms, RenderSystem* renderer, int ddl);

void adjust_difficulty(int ddl);

void move_firelings(RenderSystem* renderer);

void move_boulder(RenderSystem* renderer);

void move_ghouls(RenderSystem* renderer, Entity player_hero);

void move_tracer(float elapsed_ms_since_last_update, Entity player_hero);

void move_spitters(float elapsed_ms_since_last_update, RenderSystem* renderer);

void summon_boulder_helper(RenderSystem* renderer);

void summon_fireling_helper(RenderSystem* renderer);

void boss_action_decision(Entity player_hero, Entity boss, RenderSystem* renderer, float elapsed_ms);
std::vector<int> teleport_unique(vec2 pos);
void boss_action_teleport(Entity boss);
void boss_action_swipe(Entity boss);
void boss_action_summon(Entity boss, RenderSystem* renderer, uint type);
void boss_action_sword_spawn(bool create, vec2 pos, vec2 scale, RenderSystem* renderer, Entity player_hero);
BOSS_STATE get_action(Entity player_hero, Entity boss, RenderSystem* renderer);
float mdp_helper(vec2 boss_pos, uint num_ghouls, uint num_spitters, uint step_num, std::vector<float> cooldowns, Entity player_hero, Entity boss, RenderSystem* renderer);
float get_action_reward(BOSS_STATE action, vec2 boss_pos, uint num_ghouls, uint num_spitters, uint step_num, std::vector<float> cooldowns, Entity player_hero, Entity boss, RenderSystem* renderer);
float get_reward(vec2 boss_pos_old, uint num_ghouls_old, uint num_spitters_old, vec2 boss_pos, uint num_ghouls, uint num_spitters, Entity player_hero, Entity boss);

