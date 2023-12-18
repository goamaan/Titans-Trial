#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include "ai_system.hpp"
#include <map>
#include <vector>

// These are hard coded to the dimensions of the entity texture

const static std::vector<std::vector<char>> grid_vec = create_grid();

const float CHARACTER_SCALING = 3.0f;
const float BOSS_SCALING = 2.5f;
const float EXPLOSION_SCALING = 2.0f;

const vec2 ENEMY_BB = vec2(26.f, 30.f) * CHARACTER_SCALING;
const vec2 BOULDER_BB = vec2(15.f, 14.f);
const vec2 SWORD_BB = vec2(32.f, 64.f) * 0.7f;
const vec2 GUN_BB = vec2(45.f, 32.f);
const vec2 ARROW_BB = vec2(64.f, 64.f) * 0.5f;
const vec2 ROCKET_LAUNCHER_BB = vec2(64.f, 32.f) * .8f;
const vec2 ROCKET_BB = vec2(16.f, 16.f) * 1.2f;
const vec2 GRENADE_LAUNCHER_BB = vec2(39.f, 39.f) * 0.6f;
const vec2 GRENADE_BB = vec2(39.f, 39.f) * 0.6f;
const vec2 LASER_RIFLE_BB = vec2(32.f, 32.f);
const vec2 LASER_BB = vec2(window_width_px, 92.f * 0.2f);
const vec2 HEART_BB = vec2(16.f, 16.f) * 2.f;
const vec2 WINGED_BOOTS_BB = vec2(1489.f, 1946.f) * .02f;
const vec2 DASH_BOOTS_BB = vec2(27.f, 30.f) * 1.2f;
const vec2 PICKAXE_BB = vec2(55.f, 80.f) * .5f;
const vec2 SPITTER_BULLET_BB = vec2(16.f, 16.f) * 3.f;
const vec2 HELPER_BB = vec2(566, 510) / 1.8f;
const vec2 LAVA_PILLAR_BB = vec2(120, 536);
const vec2 TRIDENT_BB = vec2(16, 32) * 1.5f;
const vec2 MAIN_MENU_BG_BB = vec2(1200, 800);

const int SWORD_DMG = 7;
const int EXPLOSIVE_DMG = 6;
const int DIR_EXPLOSIVE_DMG = 12;
const int ARROW_DMG = 4;
const int LASER_DMG = 3;
const int WATER_BALL_DMG = 10;

const int FIRELING_HP = 4;
const int GHOUL_HP = 8;
const int SPITTER_HP = 12;


const std::vector<vec<2, vec<2,float>>> platforms = {
        // bottom line
        {{window_width_px / 2, window_height_px + 100}, {window_width_px, base_height / 2}},
        // left line
        {{-base_width, 0}, {base_width * 6, window_height_px * 2}},
        // right line
        {{window_width_px + base_width, 0}, {base_width * 6, window_height_px * 2}},
        // left middle platform
        {{base_width * 7.5, base_height * 12}, {base_width * 11, base_height * 2}},
        // top middle platform
        {{window_width_px / 2, base_height * 6}, {base_width * 26, base_height * 2}},
        // right middle platform
        {{window_width_px - base_width * 7.5, base_height * 12}, {base_width * 11, base_height * 2}},
        // bottom middle left platform
        {{base_width * 13, base_height * 18}, {base_width * 10, base_height * 2}},
        // bottom middle right platform
        {{window_width_px - base_width * 13, base_height * 18}, {base_width * 10, base_height * 2}},
        // bottom left padding platform
        {{base_width * 6.5, window_height_px - base_height * 3}, {base_width * 9, base_height * 4}},
        // bottom right padding platform
        {{window_width_px - base_width * 6.5, window_height_px - base_height * 3}, {base_width * 9, base_height * 4}},
        // bottom center padding platform
        {{window_width_px / 2, window_height_px - base_height * 2}, {base_width * 14, base_height * 2}}
};

const std::vector<vec<3, float>> walkable_area = {
        // left middle platform
        {base_width * 7.5, base_height * 11, base_width * 5.5},
        // top middle platform
        {window_width_px / 2, base_height * 5, base_width * 13},
        // right middle platform
        {window_width_px - base_width * 7.5, base_height * 11, base_width * 5.5},
        // bottom middle left platform
        {base_width * 13, base_height * 17, base_width * 5},
        // bottom middle right platform
        {window_width_px - base_width * 15, base_height * 17, base_width * 5},
        // bottom left padding platform
        {base_width * 6.5, window_height_px - base_height * 4, base_width * 4.5},
        // bottom right padding platform
        {window_width_px - base_width * 6.5, window_height_px - base_height * 4, base_width * 4.5},
        // bottom center padding platform
        {window_width_px / 2, window_height_px - base_height * 3, base_width * 7}
};

const std::map<TEXTURE_ASSET_ID, vec2 > ASSET_SIZE = {
        { TEXTURE_ASSET_ID::SPITTER_ENEMY, {16 * CHARACTER_SCALING, 24 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::QUIT,{204, 56} },
        { TEXTURE_ASSET_ID::QUIT_PRESSED,{204, 56} },
        { TEXTURE_ASSET_ID::ALMANAC,{204, 56} },
        { TEXTURE_ASSET_ID::ALMANAC_PRESSED,{204, 56} },
        { TEXTURE_ASSET_ID::BACK,{204, 56} },
        { TEXTURE_ASSET_ID::BACK_PRESSED,{204, 56} },
        { TEXTURE_ASSET_ID::MENU,{30, 32} },
        { TEXTURE_ASSET_ID::MENU_PRESSED,{30, 32} },
        { TEXTURE_ASSET_ID::PLAY, {204, 56}},
        { TEXTURE_ASSET_ID::PLAY_PRESSED, {204, 56}},
        { TEXTURE_ASSET_ID::SWORD_HELPER, {180, 17}},
        { TEXTURE_ASSET_ID::GUN_HELPER, {263, 17}},
        { TEXTURE_ASSET_ID::GRENADE_HELPER, {534, 17}},
        { TEXTURE_ASSET_ID::ROCKET_HELPER, {311, 17}},
        { TEXTURE_ASSET_ID::LASER_HELPER, {301, 17}},
        { TEXTURE_ASSET_ID::WINGED_BOOTS_HELPER, {249, 17}},
        { TEXTURE_ASSET_ID::PICKAXE_HELPER, {333, 17}},
        { TEXTURE_ASSET_ID::DASH_BOOTS_HELPER, {316, 17}},
        { TEXTURE_ASSET_ID::CONTINUE_HELPER, {311, 21}},
        { TEXTURE_ASSET_ID::TITLE_TEXT, {600, 120}},
        { TEXTURE_ASSET_ID::HERO, {15*CHARACTER_SCALING, 16*CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::FIRE_ENEMY, {14 * CHARACTER_SCALING, 16 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::GHOUL_ENEMY, {13 * CHARACTER_SCALING, 20 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::FOLLOWING_ENEMY, {12 * CHARACTER_SCALING, 12 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::BOSS, {32 * BOSS_SCALING, 60 * BOSS_SCALING}},
        { TEXTURE_ASSET_ID::BOSS_SWORD_S, {19, 21}},
        { TEXTURE_ASSET_ID::BOSS_SWORD_L, {19, 21}},
        { TEXTURE_ASSET_ID::SPITTER_ENEMY, {16*CHARACTER_SCALING, 24*CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::EXPLOSION, {60, 55}},
        { TEXTURE_ASSET_ID::EXPLOSION, {60, 55}},
        { TEXTURE_ASSET_ID::PLAYER_HEART, {40, 40}},
        { TEXTURE_ASSET_ID::PARALLAX_LAVA, {1200, 42}},
        { TEXTURE_ASSET_ID::LAVA_PILLAR, vec2(LAVA_PILLAR_BB.x * 0.4f, LAVA_PILLAR_BB.y)},
        { TEXTURE_ASSET_ID::HEALTH_BAR, vec2(62, 19) * 3.f},
        { TEXTURE_ASSET_ID::HEALTH_BAR_HEALTH, vec2(56, 3) * 3.f},
        { TEXTURE_ASSET_ID::WATER_BALL, {30, 20}},
        { TEXTURE_ASSET_ID::GRENADE_LAUNCHER, GRENADE_LAUNCHER_BB},
        { TEXTURE_ASSET_ID::GRENADE, GRENADE_BB},
        { TEXTURE_ASSET_ID::TRIDENT_HELPER, {464, 17}}
};

const std::map<TEXTURE_ASSET_ID, vec2 > SPRITE_SCALE = {
        { TEXTURE_ASSET_ID::HERO, {52*CHARACTER_SCALING, 21*CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::FIRE_ENEMY, {-48 * CHARACTER_SCALING, 32 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::GHOUL_ENEMY, {50 * CHARACTER_SCALING, 28 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::FOLLOWING_ENEMY, {30 * CHARACTER_SCALING, 30 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::SPITTER_ENEMY, {57 * CHARACTER_SCALING, 39 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::BOSS, {222 * BOSS_SCALING, 119 * BOSS_SCALING}},
        { TEXTURE_ASSET_ID::BOSS_SWORD_S, {19, 21}},
        { TEXTURE_ASSET_ID::BOSS_SWORD_L, {19, 21}},
        { TEXTURE_ASSET_ID::EXPLOSION, {100, 92}},
        { TEXTURE_ASSET_ID::PARALLAX_LAVA, {1200, 800}},
        { TEXTURE_ASSET_ID::LAVA_PILLAR, LAVA_PILLAR_BB},
        { TEXTURE_ASSET_ID::WATER_BALL, {64, 64}},
        { TEXTURE_ASSET_ID::GRENADE_LAUNCHER, GRENADE_LAUNCHER_BB},
        { TEXTURE_ASSET_ID::GRENADE, GRENADE_BB},
        { TEXTURE_ASSET_ID::GUN, GUN_BB}
};

const std::map<TEXTURE_ASSET_ID, vec2 > SPRITE_OFFSET = {
        { TEXTURE_ASSET_ID::HERO, {10 * CHARACTER_SCALING, -1 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::FIRE_ENEMY, { 0 * CHARACTER_SCALING, -4 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::GHOUL_ENEMY, { 0 * CHARACTER_SCALING, -2 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::FOLLOWING_ENEMY, { -1 * CHARACTER_SCALING, -2 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::SPITTER_ENEMY, {-10 * CHARACTER_SCALING, -6 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::BOSS, {0 * BOSS_SCALING, -29.5 * BOSS_SCALING}},
        { TEXTURE_ASSET_ID::BOSS_SWORD_S, {0, 0}},
        { TEXTURE_ASSET_ID::BOSS_SWORD_L, {0, 0}},
        { TEXTURE_ASSET_ID::EXPLOSION, {0, -8}},
        { TEXTURE_ASSET_ID::PARALLAX_LAVA, {0, -378}},
        { TEXTURE_ASSET_ID::LAVA_PILLAR, {0,-80}},
        { TEXTURE_ASSET_ID::WATER_BALL, {-17,0}},
        { TEXTURE_ASSET_ID::GRENADE_LAUNCHER, {4 * CHARACTER_SCALING, -2 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::GRENADE, {0 * CHARACTER_SCALING, -2 * CHARACTER_SCALING}},
        { TEXTURE_ASSET_ID::GUN, {-2 * CHARACTER_SCALING, 0 * CHARACTER_SCALING}}
};

const std::map<TEXTURE_ASSET_ID, AnimationInfo> ANIMATION_INFO = {
        {TEXTURE_ASSET_ID::HERO, {
                13,
                {9, 1, 8, 4, 4, 4, 16, 4, 8, 4, 14, 2, 8},
                0,
                16
        }},
        {TEXTURE_ASSET_ID::FIRE_ENEMY, {
            3,
            {4, 5, 10},
            0,
            10
        }},
       {TEXTURE_ASSET_ID::GHOUL_ENEMY, {
            6,
            {4, 9, 7, 4, 7, 11},
            1,
            11
        }},
        {TEXTURE_ASSET_ID::FOLLOWING_ENEMY, {
            5,
            {6, 4, 6, 6, 5},
            0,
            6
        }},
        {TEXTURE_ASSET_ID::SPITTER_ENEMY, {
            5,
            {6, 7, 8, 3, 8},
            0,
            8
        }},
        {TEXTURE_ASSET_ID::BOSS, {
                13,
                {9,9,2,8,8,16,14,1,9,9,13,4,36},
                0,
                36
        }},
        {TEXTURE_ASSET_ID::BOSS_SWORD_S, {
                1,
                {1},
                0,
                1
        }},
        {TEXTURE_ASSET_ID::BOSS_SWORD_L, {
                1,
                {1},
                0,
                1
        }},
        {TEXTURE_ASSET_ID::EXPLOSION, {
            1,
            {6},
            0,
            6
        }},
        {TEXTURE_ASSET_ID::SPITTER_ENEMY_BULLET, {
            1,
            {4},
            0,
            4
        }},
        {TEXTURE_ASSET_ID::LAVA_PILLAR, {
            1,
            {4},
            0,
            4
        }},
        {TEXTURE_ASSET_ID::WATER_BALL, {
            3,
            {4, 5, 5},
            0,
            5
        }},
       {TEXTURE_ASSET_ID::GRENADE_LAUNCHER, {
            1,
            {20},
            0,
            20
        }},
       {TEXTURE_ASSET_ID::GRENADE, {
            1,
            {20},
            0,
            20
        }}
};
// the player
Entity createHero(RenderSystem *renderer, vec2 pos);
// the enemy
Entity createFireing(RenderSystem *renderer, vec2 position);
Entity createBoulder(RenderSystem *renderer, vec2 position, vec2 velocity, float size);
// the ghoul enemy
Entity createGhoul(RenderSystem* renderer, vec2 position);
// the following & teleporting enemy
Entity createFollowingEnemy(RenderSystem* renderer, vec2 position);
// the boss
Entity createBossEnemy(RenderSystem *renderer, vec2 pos);
Entity create_boss_sword(RenderSystem* renderer, vec2 pos, int size);
// spitter enemy
Entity createSpitterEnemy(RenderSystem *renderer, vec2 pos);
// spitter enemy bullet
Entity createSpitterEnemyBullet(RenderSystem *renderer, vec2 pos, float angle);
// the sword
Entity createSword(RenderSystem *renderer, vec2 position);
// the gun
Entity createGun(RenderSystem* renderer, vec2 position);
// the bullet
Entity createArrow(RenderSystem* renderer, vec2 position, float angle);

Entity createRocketLauncher(RenderSystem* renderer, vec2 position);

Entity createRocket(RenderSystem* renderer, vec2 position, float angle);

Entity createGrenadeLauncher(RenderSystem* renderer, vec2 position);

Entity createGrenade(RenderSystem* renderer, vec2 position, vec2 velocity);

Entity createExplosion(RenderSystem* renderer, vec2 position, float size);

Entity createLaserRifle(RenderSystem* renderer, vec2 position);

Entity createLaser(RenderSystem* renderer, vec2 position, float angle);

Entity createTrident(RenderSystem* renderer, vec2 position);

Entity createWaterBall(RenderSystem* renderer, vec2 position, float angle);

Entity createHeart(RenderSystem* renderer, vec2 position);

Entity createWingedBoots(RenderSystem* renderer, vec2 position);

Entity createDashBoots(RenderSystem* renderer, vec2 position);

Entity createPickaxe(RenderSystem* renderer, vec2 position);

Entity createMainMenuBackground(RenderSystem* renderer);

// the parallax backgrounds
Entity createParallaxItem(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture_id);
// the helper text during pause
Entity createHelperText(RenderSystem* renderer, float size);
Entity createToolTip(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID type);
Entity createBlock(RenderSystem* renderer, vec2 pos, vec2 size);
// the ui button
Entity createButton(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID type, std::function<void ()> callback, bool visibility = true);
Entity createWeaponHitBox(RenderSystem* renderer, vec2 pos, vec2 size, WeaponHitBox hitBoxInfo);
Entity createHurtBox(RenderSystem* renderer, vec2 pos, vec2 size);
Entity createTitleText(RenderSystem* renderer, vec2 pos);
Entity createLine(RenderSystem* renderer, vec2 pos, vec2 offset, vec2 scale, float angle);
Entity createPlayerHeart(RenderSystem* renderer, vec2 pos);
Entity createPowerUpIcon(RenderSystem* renderer, vec2 pos);
Entity createDifficultyBar(RenderSystem* renderer, vec2 pos);
Entity createDifficultyIndicator(RenderSystem* renderer, vec2 pos);
Entity createScore(RenderSystem* renderer, vec2 pos);
Entity createNumber(RenderSystem* renderer, vec2 pos);
Entity createDBFlame(RenderSystem* renderer, vec2 pos);
Entity createDBSkull(RenderSystem* renderer, vec2 pos);
Entity createDBSatan(RenderSystem* renderer, vec2 pos);
Entity createLavaPillar(RenderSystem* renderer, vec2 pos);
Entity createHealthBar(RenderSystem* renderer, Entity owner);
Entity createDialogue(RenderSystem* renderer, TEXTURE_ASSET_ID texture_id);

vec2 getRandomWalkablePos(vec2 char_scale, int platform = -1, bool randomness = true);