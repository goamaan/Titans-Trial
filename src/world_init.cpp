#include <utility>
#include "world_init.hpp"
#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"


Entity createHero(RenderSystem *renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::HERO);

	registry.players.emplace(entity);
	registry.solids.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::HERO));
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::HERO,
		 EFFECT_ASSET_ID::HERO,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         SPRITE_SCALE.at(TEXTURE_ASSET_ID::HERO),
         SPRITE_OFFSET.at(TEXTURE_ASSET_ID::HERO)});
    registry.debugRenderRequests.emplace(entity);

    registry.gravities.emplace(entity);
	return entity;
}

Entity createBoulder(RenderSystem* renderer, vec2 position, vec2 velocity, float size) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::CIRCLE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.velocity = velocity;
	motion.scale = BOULDER_BB * size;

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.friction_y = .8f;

	registry.gravities.emplace(entity);
	registry.boulders.emplace(entity);
	registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOULDER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			false,
			true,
			motion.scale});

	return entity;
}

Entity createFireing(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.velocity = vec2(0.0, 0.0);
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::FIRE_ENEMY);
    motion.position = position;
	motion.dir = -1;

	registry.colors.insert(entity, {1, .8f, .8f});
	registry.enemies.insert(entity, {
        1,
        2,
        FIRELING_HP,
        FIRELING_HP
    });

	registry.fireEnemies.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::FIRE_ENEMY));
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::FIRE_ENEMY,
		 EFFECT_ASSET_ID::FIRE_ENEMY,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
		 SPRITE_SCALE.at(TEXTURE_ASSET_ID::FIRE_ENEMY),
		 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::FIRE_ENEMY) });

    registry.debugRenderRequests.emplace(entity);
	registry.testAIs.emplace(entity);

	return entity;
}


Entity createBossEnemy(RenderSystem *renderer, vec2 position)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.collisionMeshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto &motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS);
    motion.position = position;
    motion.dir = -1;

    registry.colors.insert(entity, {1, .8f, .8f});
    registry.enemies.insert(entity, {
        11,
        12,
        BOSS_HEALTH,
        BOSS_HEALTH
    });

    AnimationInfo& aniInfo = registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::BOSS));
    aniInfo.oneTimeState = 9;
    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::BOSS,
             EFFECT_ASSET_ID::BOSS,
             GEOMETRY_BUFFER_ID::SPRITE,
             false,
             true,
             SPRITE_SCALE.at(TEXTURE_ASSET_ID::BOSS),
             SPRITE_OFFSET.at(TEXTURE_ASSET_ID::BOSS) });

    registry.debugRenderRequests.emplace(entity);
    auto &bossState = registry.boss.emplace(entity);
    for (int i = 0; i <= (uint) BOSS_STATE::SIZE; i++)
        bossState.cooldowns.push_back(0);

    bossState.hurt_boxes = {
            createWeaponHitBox(renderer, motion.position, vec2(480, 40), {
                    false,
                    false,
                    1,
                    false,
                    true
            }), createWeaponHitBox(renderer, motion.position, vec2(560, 40), {
                    false,
                    false,
                    1,
                    false,
                    true
            })
    };



    return entity;
}

Entity create_boss_sword(RenderSystem* renderer, vec2 position, int type) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0.f, -0.1f);
	motion.position = position;

	registry.enemies.emplace(entity);
	registry.enemies.get(entity).health = 1;
	registry.enemies.get(entity).hit_animation = -2;
	registry.enemies.get(entity).death_animation = -2;

	registry.bossSwords.emplace(entity);
	registry.bossSwords.get(entity).type = type;

	if (type == 1)
	{
		motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS_SWORD_S);
		registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::BOSS_SWORD_S));
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS_SWORD_S,
			 EFFECT_ASSET_ID::BOSS_SWORD_S,
			 GEOMETRY_BUFFER_ID::SPRITE,
			 false,
			 true,
			 SPRITE_SCALE.at(TEXTURE_ASSET_ID::BOSS_SWORD_S),
			 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::BOSS_SWORD_S) });
	}
	else {
		motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS_SWORD_L);
		registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::BOSS_SWORD_L));
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS_SWORD_L,
			 EFFECT_ASSET_ID::BOSS_SWORD_L,
			 GEOMETRY_BUFFER_ID::SPRITE,
			 false,
			 true,
			 SPRITE_SCALE.at(TEXTURE_ASSET_ID::BOSS_SWORD_L),
			 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::BOSS_SWORD_L) });
	}

	registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createGhoul(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.velocity = vec2(0.f, -0.1f);
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::GHOUL_ENEMY);
    motion.position = position;

	registry.enemies.insert(entity, {
        3,
        4,
        GHOUL_HP,
        GHOUL_HP
    });

	registry.ghouls.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::GHOUL_ENEMY));
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GHOUL_ENEMY,
		 EFFECT_ASSET_ID::GHOUL,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 SPRITE_SCALE.at(TEXTURE_ASSET_ID::GHOUL_ENEMY),
		 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::GHOUL_ENEMY) });

	registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createFollowingEnemy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.velocity = vec2(0.f, 0.f);
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::FOLLOWING_ENEMY);

	registry.enemies.emplace(entity).hittable = false;

	registry.followingEnemies.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::FOLLOWING_ENEMY));
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FOLLOWING_ENEMY,
		 EFFECT_ASSET_ID::FOLLOWING_ENEMY,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 SPRITE_SCALE.at(TEXTURE_ASSET_ID::FOLLOWING_ENEMY),
		 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::FOLLOWING_ENEMY) });

	registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createSpitterEnemy(RenderSystem *renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::SPITTER_ENEMY);
    motion.position = pos;

	SpitterEnemy &spitterEnemy = registry.spitterEnemies.emplace(entity);
	// wait 1s for first shot
	spitterEnemy.timeUntilNextShotMs = INITIAL_SPITTER_PROJECTILE_DELAY_MS;

	registry.enemies.insert(entity, {
        3,
        4,
        SPITTER_HP,
        SPITTER_HP
    });
	registry.colors.insert(entity, {1, .8f, .8f});

	registry.solids.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::SPITTER_ENEMY));
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::SPITTER_ENEMY,
		 EFFECT_ASSET_ID::SPITTER_ENEMY,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         SPRITE_SCALE.at(TEXTURE_ASSET_ID::SPITTER_ENEMY),
         SPRITE_OFFSET.at(TEXTURE_ASSET_ID::SPITTER_ENEMY)});

    registry.debugRenderRequests.emplace(entity);
	registry.gravities.emplace(entity);

	return entity;
}

Entity createSpitterEnemyBullet(RenderSystem *renderer, vec2 pos, float angle)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	auto dir = []() -> int
	{ return rand() % 2 == 0 ? 1 : -1; };
	motion.velocity = {dir() * 300, dir() * (rand() % 300)};
	motion.scale = SPITTER_BULLET_BB;

	SpitterBullet &bullet = registry.spitterBullets.emplace(entity);

	bullet.mass = 1;

	registry.projectiles.emplace(entity);
	AnimationInfo &animationInfo = registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::SPITTER_ENEMY_BULLET));
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::SPITTER_ENEMY_BULLET,
		 EFFECT_ASSET_ID::SPITTER_ENEMY_BULLET,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createMainMenuBackground(RenderSystem *renderer) {
	Entity entity = Entity();
	auto &motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = MAIN_MENU_BG_BB;
    motion.position = {window_width_px/2, window_height_px/2};

    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::TITLE_SCREEN_BG,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE,
             false,
             true,
             motion.scale});

    return entity;
}

Entity createParallaxItem(RenderSystem *renderer, vec2 pos, TEXTURE_ASSET_ID texture_id)
{
	Entity entity = Entity();
	vec2 vel;
	if (texture_id == TEXTURE_ASSET_ID::BACKGROUND || texture_id == TEXTURE_ASSET_ID::BACKGROUND_COLOR || texture_id == TEXTURE_ASSET_ID::PARALLAX_MOON)
	{
		vel = vec2();
	}
	else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_CLOUDS_CLOSE)
	{
		// moves to the right
		vel = vec2(10, 0);
	}
	else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_CLOUDS_FAR)
	{
		// moves to the right slowly
		vel = vec2(5, 0);
	}
	else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_RAIN)
	{
		vel = vec2(40, 120);
	} else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_LAVA) 
	{
		vel = vec2(10, 0);
		CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.collisionMeshPtrs.emplace(entity, &mesh);
	}
	vel *= 5;

	auto &motion = registry.motions.emplace(entity);

	motion.angle = 0.f;
	motion.velocity = vel;
	motion.position = pos;
	if (texture_id == TEXTURE_ASSET_ID::BACKGROUND ||
		texture_id == TEXTURE_ASSET_ID::BACKGROUND_COLOR ||
		texture_id == TEXTURE_ASSET_ID::PARALLAX_MOON ||
		texture_id == TEXTURE_ASSET_ID::PARALLAX_CLOUDS_CLOSE ||
		texture_id == TEXTURE_ASSET_ID::PARALLAX_CLOUDS_FAR)
	{
		motion.scale = {pos.x * 2, pos.y * 2};
	}
	else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_LAVA) {
		motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::PARALLAX_LAVA);
	}
	else
	{
		motion.scale = {1200, 800};
	}

	ParallaxBackground &bg = registry.parallaxBackgrounds.emplace(entity);
	if (texture_id == TEXTURE_ASSET_ID::PARALLAX_CLOUDS_CLOSE || texture_id == TEXTURE_ASSET_ID::PARALLAX_CLOUDS_FAR) {
		bg.resetPosition = vec2(-800, 400);
	} else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_RAIN) {
		bg.resetPosition = vec2(400, 0);
	} else if (texture_id == TEXTURE_ASSET_ID::PARALLAX_LAVA) {
		bg.resetPosition = vec2(-600, 813);
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{texture_id,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 texture_id == TEXTURE_ASSET_ID::PARALLAX_LAVA ? SPRITE_SCALE.at(TEXTURE_ASSET_ID::PARALLAX_LAVA) : motion.scale,
		 texture_id == TEXTURE_ASSET_ID::PARALLAX_LAVA ? SPRITE_OFFSET.at(TEXTURE_ASSET_ID::PARALLAX_LAVA) : vec2({0, 0})});
	if (texture_id == TEXTURE_ASSET_ID::PARALLAX_LAVA)
		registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createHelperText(RenderSystem* renderer, float size)
{
    const int PADDING = 150;
    Entity entity = Entity();

    auto &motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = HELPER_BB * size;
    motion.position = {PADDING, window_height_px/2};

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.renderRequests.insert(
            entity,
            {TEXTURE_ASSET_ID::HELPER,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE,
             true,
             false,
             motion.scale});
    registry.showWhenPaused.emplace(entity);
    return entity;
}

Entity createToolTip(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID type) {
    auto entity = Entity();

    Motion &motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.scale = ASSET_SIZE.at(type);
    
	registry.renderRequests.insert(
            entity,
            {type,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE,
             true,
             true,
            motion.scale});

    return entity;
}

Entity createSword(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.position = position;
	motion.scale = SWORD_BB;

	// Add to swords, gravity and render requests
	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::SWORD;
	registry.swords.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::SWORD,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         motion.scale});
	registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createGun(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.position = position;
	motion.scale = GUN_BB;

	// Add to swords, gravity and render requests
	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::GUN;
	registry.guns.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::GUN,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         SPRITE_SCALE.at(TEXTURE_ASSET_ID::GUN),
		 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::GUN)});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createArrow(RenderSystem* renderer, vec2 position, float angle) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	CollisionMesh &collisionMesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.collisionMeshPtrs.emplace(entity, &collisionMesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = angle;
	motion.velocity = vec2(600.f, 0) * mat2({cos(angle), -sin(angle)}, {sin(angle), cos(angle)});
	motion.scale = mesh.original_size * 36.f;

	registry.bullets.emplace(entity);
	registry.weaponHitBoxes.emplace(entity).damage = ARROW_DMG;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
          false,
          true,
          motion.scale});

	return entity;
}

Entity createRocketLauncher(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.position = position;
	motion.scale = ROCKET_LAUNCHER_BB;

	// Add to swords, gravity and render requests
	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::ROCKET_LAUNCHER;
	registry.rocketLaunchers.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::ROCKET_LAUNCHER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createRocket(RenderSystem* renderer, vec2 position, float angle) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = angle;
	motion.velocity = vec2(500.f, 0) * mat2({cos(angle), -sin(angle)}, {sin(angle), cos(angle)});
	motion.scale = ROCKET_BB;

	registry.rockets.emplace(entity);
	registry.weaponHitBoxes.emplace(entity).damage = DIR_EXPLOSIVE_DMG;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ROCKET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			false,
			true,
			motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createGrenadeLauncher(RenderSystem *renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto &motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.position = position;
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::GRENADE_LAUNCHER);

	// Add to swords, gravity and render requests
	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::GRENADE_LAUNCHER;
	registry.grenadeLaunchers.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::GRENADE_LAUNCHER));
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::GRENADE_LAUNCHER,
		 EFFECT_ASSET_ID::GRENADE_ORB,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 SPRITE_SCALE.at(TEXTURE_ASSET_ID::GRENADE_LAUNCHER),
		 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::GRENADE_LAUNCHER)});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createGrenade(RenderSystem* renderer, vec2 position, vec2 velocity) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.velocity = velocity;
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::GRENADE);

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.friction_x = .6f;
	projectile.friction_y = .6f;

	registry.grenades.emplace(entity);
	registry.weaponHitBoxes.emplace(entity).damage = DIR_EXPLOSIVE_DMG;
	registry.gravities.emplace(entity);
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::GRENADE));
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GRENADE,
			EFFECT_ASSET_ID::GRENADE_ORB,
			GEOMETRY_BUFFER_ID::SPRITE,
			false,
			true,
		SPRITE_SCALE.at(TEXTURE_ASSET_ID::GRENADE),
		SPRITE_OFFSET.at(TEXTURE_ASSET_ID::GRENADE)});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createExplosion(RenderSystem *renderer, vec2 position, float size)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion &motion = registry.motions.emplace(entity);
	motion.position = position + size * SPRITE_OFFSET.at(TEXTURE_ASSET_ID::EXPLOSION);
	motion.scale = size * ASSET_SIZE.at(TEXTURE_ASSET_ID::EXPLOSION);

	registry.explosions.emplace(entity);
	auto &hitbox = registry.weaponHitBoxes.emplace(entity);
	hitbox.hurtsHero = true;
    hitbox.damage = EXPLOSIVE_DMG;

	AnimationInfo& info = registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::EXPLOSION));
	info.oneTimeState = 0;

	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::EXPLOSION,
		 EFFECT_ASSET_ID::EXPLOSION,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 size * SPRITE_SCALE.at(TEXTURE_ASSET_ID::EXPLOSION),
		 size * SPRITE_OFFSET.at(TEXTURE_ASSET_ID::EXPLOSION)});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createLaserRifle(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);
	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.scale = LASER_RIFLE_BB;

	// Add to swords, gravity and render requests
	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::LASER_RIFLE;
	registry.laserRifles.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LASER_RIFLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale });
	registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createLaser(RenderSystem* renderer, vec2 position, float angle) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = angle;
	motion.velocity = vec2(2000, 0) * mat2({ cos(angle), -sin(angle) }, { sin(angle), cos(angle) });
	motion.scale = LASER_BB;

	registry.lasers.emplace(entity);
	registry.weaponHitBoxes.emplace(entity).damage = LASER_DMG;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LASER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			false,
			true,
			motion.scale });
	registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createTrident(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);
	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.scale = TRIDENT_BB;

	// Add to swords, gravity and render requests
	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::TRIDENT;
	registry.tridents.emplace(entity);
	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TRIDENT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale });
	registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createWaterBall(RenderSystem* renderer, vec2 position, float angle) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = angle;
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::WATER_BALL);

	AnimationInfo& animation = registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::WATER_BALL));
	animation.curState = 1;

	registry.waterBalls.emplace(entity);
	registry.solids.emplace(entity);
	registry.weaponHitBoxes.emplace(entity).damage = WATER_BALL_DMG;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WATER_BALL,
			EFFECT_ASSET_ID::WATER_BALL,
			GEOMETRY_BUFFER_ID::SPRITE,
			false,
			true,
			SPRITE_SCALE.at(TEXTURE_ASSET_ID::WATER_BALL),
			SPRITE_OFFSET.at(TEXTURE_ASSET_ID::WATER_BALL)});
	registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createHeart(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = HEART_BB;

	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::HEART;

	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::HEART,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createPickaxe(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = PICKAXE_BB;

	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::PICKAXE;

	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::PICKAXE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createWingedBoots(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = WINGED_BOOTS_BB;

	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::WINGED_BOOTS;

	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::WINGED_BOOTS,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createDashBoots(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = DASH_BOOTS_BB;

	Collectable& collectable = registry.collectables.emplace(entity);
	collectable.type = COLLECTABLE_TYPE::DASH_BOOTS;

	registry.gravities.emplace(entity);
	registry.solids.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::DASH_BOOTS,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createBlock(RenderSystem* renderer, vec2 pos, vec2 size)
{
	auto entity = Entity();
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = size;
	fill_grid((std::vector<std::vector<char>> &) grid_vec, pos, size);
	registry.blocks.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         false,
         motion.scale});
    registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createWeaponHitBox(RenderSystem* renderer, vec2 pos, vec2 size, WeaponHitBox hitBoxInfo)
{
	auto entity = Entity();
	CollisionMesh &mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion &motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = size;
	registry.weaponHitBoxes.insert(entity, hitBoxInfo);
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::COLOURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         motion.scale});
    registry.debugRenderRequests.emplace(entity);

	return entity;
}

Entity createButton(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID type, std::function<void ()> callback, bool visibility) {
    auto entity = Entity();

    Motion &motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.scale = ASSET_SIZE.at(type);
    GameButton &button = registry.buttons.emplace(entity);
    button.clicked = false;
    button.callback = std::move(callback);
    registry.renderRequests.insert(
            entity,
            {type,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE,
             true,
             visibility,
            motion.scale});
    if (!visibility) {
        registry.showWhenPaused.emplace(entity);
    }

    return entity;
}

Entity createTitleText(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::TITLE_TEXT);
	motion.position = pos;

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TITLE_TEXT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
         false,
         true,
         motion.scale});
	registry.showWhenPaused.emplace(entity);
	return entity;
}

Entity createPlayerHeart(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::PLAYER_HEART);
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER_HEART,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 false,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createLine(RenderSystem* renderer, vec2 pos, vec2 offset, vec2 scale, float angle) {
	Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.positionOffset = offset;
	motion.scale = scale;
	motion.globalAngle = angle;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LINE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale });

	return entity;
}

Entity createPowerUpIcon(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 40.f, 40.f};
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 false,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createDifficultyBar(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 220.f, 40.f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DIFFICULTY_BAR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 true,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createDifficultyIndicator(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = M_PI;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 34.56f, 30.72f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::INDICATOR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 true,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createScore(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 140.f, 29.f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SCORE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 false,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createNumber(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 20.f, 29.f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ZERO,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 false,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createDBFlame(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 180.f, 75.f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DB_BOSS_FLAME,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 true,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createDBSkull(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 70.f, 60.f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DB_BOSS_SKULL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 true,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createDBSatan(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 210.f, 140.f };
	motion.position = pos;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DB_SATAN,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 true,
		 true,
		 motion.scale });

	registry.inGameGUIs.emplace(entity);

	return entity;
}

Entity createLavaPillar(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	CollisionMesh& mesh = renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.collisionMeshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, -700.f };
	motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::LAVA_PILLAR);
	registry.gravities.emplace(entity);
	registry.enemies.emplace(entity).hittable = false;
	registry.animated.emplace(entity, ANIMATION_INFO.at(TEXTURE_ASSET_ID::LAVA_PILLAR));
	registry.lavaPillars.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LAVA_PILLAR, // TEXTURE_COUNT indicates that no txture is needed
		 EFFECT_ASSET_ID::LAVA_PILLAR,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 SPRITE_SCALE.at(TEXTURE_ASSET_ID::LAVA_PILLAR),
		 SPRITE_OFFSET.at(TEXTURE_ASSET_ID::LAVA_PILLAR) });
	registry.debugRenderRequests.emplace(entity);
	return entity;
}

Entity createHealthBar(RenderSystem* renderer, Entity owner) {
    auto entity = Entity();


    Motion& motion = registry.motions.emplace(entity);
    motion.position = vec2(window_width_px/2, window_height_px-40);
    motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::HEALTH_BAR_HEALTH);
    HealthBar& healthBar = registry.healthBar.emplace(entity);
    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::HEALTH_BAR_HEALTH,
              EFFECT_ASSET_ID::HEALTH_BAR,
              GEOMETRY_BUFFER_ID::SPRITE,
              false,
              true,
              motion.scale});

    auto bar = Entity();
    Motion& motion2 = registry.motions.emplace(bar);
    motion2.position = motion.position;
    motion2.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::HEALTH_BAR);
    registry.renderRequests.insert(
            bar,
            { TEXTURE_ASSET_ID::HEALTH_BAR,
              EFFECT_ASSET_ID::TEXTURED,
              GEOMETRY_BUFFER_ID::SPRITE,
              false,
              true,
              motion2.scale });

    healthBar.bar = bar;
    healthBar.owner = owner;

    return entity;
}

Entity createDialogue(RenderSystem* renderer, TEXTURE_ASSET_ID texture_id) {
	Entity text = Entity();

	auto& text_motion = registry.motions.emplace(text);
	text_motion.angle = 0.f;
	text_motion.velocity = { 0.f, 0.f };
	text_motion.position = { 600, 300 };
	text_motion.scale = ASSET_SIZE.at(TEXTURE_ASSET_ID::CONTINUE_HELPER);

	registry.renderRequests.insert(
		text,
		{ TEXTURE_ASSET_ID::CONTINUE_HELPER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 text_motion.scale });

	registry.dialogueTexts.emplace(text);

    registry.debugRenderRequests.emplace(text);

	Entity entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -3000.f, 0.f };
	motion.position = { 1600, window_height_px / 2 };
	motion.scale = { window_width_px, window_height_px };

	registry.renderRequests.insert(
		entity,
		{ texture_id,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 false,
		 true,
		 motion.scale });

	registry.dialogues.emplace(entity);

    registry.debugRenderRequests.emplace(entity);

	return entity;
}

vec2 getRandomWalkablePos(vec2 char_scale, int platform, bool randomness) {
    vec3 values = walkable_area.at(platform == -1 ? rand() % walkable_area.size() : platform);
    float no_over_edge = values[2] - char_scale.x/2.f;
    // first tries to stay inside platform. If platform too small we let it overflow. Then pick a random offset position
    float rand_offset = (no_over_edge > 0 ? no_over_edge : values[2]) * ((double)rand() / RAND_MAX) * (rand() % 2 == 0 ? 1.f : -1.f);
    //printf("value: %f,",rand_offset);
    return {values.x+(randomness ? rand_offset : 0), values.y-char_scale.y/2.f};
}