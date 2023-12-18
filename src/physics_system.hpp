#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "render_system.hpp"

const float GRAVITY_ACCELERATION_FACTOR = 10.0 / 17.5;

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void init(RenderSystem* renderer);
	void step(float elapsed_ms, int dialogue);
	static bool collides(const Entity &entity1, const Entity &entity2);
	bool laser_collides(Motion& motion1, Motion& motion2);
	PhysicsSystem()
	{
	}
private:
	RenderSystem* renderer;
};