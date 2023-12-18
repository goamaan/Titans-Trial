#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class AISystem
{

public:
	void step(float elapsed_ms);

	AISystem() {}
};

vec2 find_map_index(vec2 pos);

vec2 find_index_from_map(vec2 pos);

void bfs_follow_start(std::vector<std::vector<char>>& vec, vec2 pos_chase, vec2 pos_prey, Entity& chaser);

void fill_grid(std::vector<std::vector<char>>&, vec2, vec2);

std::vector<std::vector<char>> create_grid();

//void astar_follow_start(std::vector<std::vector<char>>& vec, vec2 pos_chase, vec2 pos_prey, Entity& chaser);
//void point_checker(vec2& point);