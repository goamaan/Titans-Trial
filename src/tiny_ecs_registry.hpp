#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface *> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Solid> solids;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<Gravity> gravities;
	ComponentContainer<TestAI> testAIs;
	ComponentContainer<Collision> collisions;
	ComponentContainer<ParallaxBackground> parallaxBackgrounds;
	ComponentContainer<Player> players;
    ComponentContainer<Boss> boss;
	ComponentContainer<BossSword> bossSwords;
    ComponentContainer<HealthBar> healthBar;
	ComponentContainer<Block> blocks;
	ComponentContainer<Mesh *> meshPtrs;
	ComponentContainer<CollisionMesh *> collisionMeshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
    ComponentContainer<Blank> debugRenderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<SpitterEnemy> spitterEnemies;
	ComponentContainer<SpitterBullet> spitterBullets;
	ComponentContainer<FollowingEnemies> followingEnemies;
	ComponentContainer<Ghoul> ghouls;
	ComponentContainer<Enemies> enemies;
	ComponentContainer<FireEnemy> fireEnemies;
	ComponentContainer<Boulder> boulders;
	ComponentContainer<Collectable> collectables;
	ComponentContainer<Sword> swords;
	ComponentContainer<Gun> guns;
	ComponentContainer<Bullet> bullets;
	ComponentContainer<RocketLauncher> rocketLaunchers;
	ComponentContainer<Rocket> rockets;
	ComponentContainer<GrenadeLauncher> grenadeLaunchers;
	ComponentContainer<Grenade> grenades;
	ComponentContainer<Explosion> explosions;
	ComponentContainer<LaserRifle> laserRifles;
	ComponentContainer<Laser> lasers;
	ComponentContainer<Trident> tridents;
	ComponentContainer<WaterBall> waterBalls;
	ComponentContainer<Weapon> weapons;
	ComponentContainer<WeaponHitBox> weaponHitBoxes;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<AnimationInfo> animated;
    ComponentContainer<GameButton> buttons;
    ComponentContainer<ShowWhenPaused> showWhenPaused;
	ComponentContainer<InGameGUI> inGameGUIs;
	ComponentContainer<LavaPillar> lavaPillars;
	ComponentContainer<Dialogue> dialogues;
	ComponentContainer<DialogueText> dialogueTexts;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		// TODO: A1 add a LightUp component
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&solids);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&gravities);
		registry_list.push_back(&testAIs);
		registry_list.push_back(&parallaxBackgrounds);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
        registry_list.push_back(&boss);
		registry_list.push_back(&bossSwords);
        registry_list.push_back(&healthBar);
		registry_list.push_back(&blocks);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&collisionMeshPtrs);
		registry_list.push_back(&renderRequests);
        registry_list.push_back(&debugRenderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&spitterEnemies);
		registry_list.push_back(&spitterBullets);
		registry_list.push_back(&enemies);
		registry_list.push_back(&fireEnemies);
		registry_list.push_back(&ghouls);
		registry_list.push_back(&followingEnemies);
		registry_list.push_back(&boulders);
		registry_list.push_back(&collectables);
		registry_list.push_back(&swords);
		registry_list.push_back(&guns);
		registry_list.push_back(&bullets);
		registry_list.push_back(&rocketLaunchers);
		registry_list.push_back(&rockets);
		registry_list.push_back(&grenadeLaunchers);
		registry_list.push_back(&grenades);
		registry_list.push_back(&explosions);
		registry_list.push_back(&laserRifles);
		registry_list.push_back(&lasers);
		registry_list.push_back(&tridents);
		registry_list.push_back(&waterBalls);
		registry_list.push_back(&weapons);
		registry_list.push_back(&weaponHitBoxes);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
        registry_list.push_back(&animated);
        registry_list.push_back(&buttons);
        registry_list.push_back(&showWhenPaused);
		registry_list.push_back(&lavaPillars);
        registry_list.push_back(&dialogues);
		registry_list.push_back(&dialogueTexts);
		registry_list.push_back(&inGameGUIs);
	}

	void clear_all_components()
	{
		for (ContainerInterface *reg : registry_list)
			reg->clear();
	}

	void list_all_components()
	{
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface *reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e)
	{
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface *reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e)
	{
		for (ContainerInterface *reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;