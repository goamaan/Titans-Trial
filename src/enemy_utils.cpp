//
// Created by justi on 2023-12-07.
//


#include "enemy_utils.hpp"
#include "physics_system.hpp"


static std::default_random_engine rng = std::default_random_engine(std::random_device()());
static std::uniform_real_distribution<float> uniform_dist;

void do_enemy_spawn(float elapsed_ms, RenderSystem* renderer, int ddl) {
    adjust_difficulty(ddl);
    next_enemy_spawn -= elapsed_ms * (5.f/(registry.enemies.components.size()+1)+0.5);
    printf("%f \n", next_enemy_spawn);
    if (next_enemy_spawn > 0.f) {
        return;
    }

    size_t spawns[ENEMY_COUNT] = {
            registry.fireEnemies.components.size(),
            registry.ghouls.components.size(),
            registry.spitterEnemies.components.size(),
            registry.boulders.components.size()
    };

    float random = uniform_dist(rng);
    int selectedEnemy = 0;
    while (selectedEnemy < ENEMY_COUNT) {
        if (spawn_prob[selectedEnemy] >= random && spawns[selectedEnemy] < max_spawns[selectedEnemy]) {
            break;
        }
        selectedEnemy++;
    }
    switch (SpawnableEnemyType(selectedEnemy)) {
        case FIRELINGS: {
            summon_fireling_helper(renderer);
            break;
        }
        case GHOULS: {
            createGhoul(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::GHOUL_ENEMY)));
            break;
        }
        case SPITTERS: {
            createSpitterEnemy(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::SPITTER_ENEMY)));
            break;
        }
        case BOULDERS: {
            summon_boulder_helper(renderer);
            break;
        }
        case ENEMY_COUNT:
            break;
    }
    next_enemy_spawn = (spawn_delay * spawn_delay_variance) + uniform_dist(rng) * (spawn_delay * (1-spawn_delay_variance));
}

void adjust_difficulty(int ddl){
    switch (ddl) {
        case 0:
            max_spawns = {8, 3, 0, 0};
            spawn_prob = {.7, 1, 1, 1};
            spawn_delay = 6000;
            break;
        case 1:
            spitter_projectile_delay_ms = 5000.f;
            max_spawns = {10, 5, 2, 0};
            spawn_prob = {.5, .85, 1, 1};
            spawn_delay = 5000;
            break;
        case 2:
            spitter_projectile_delay_ms = 3500.f;
            max_spawns = {12, 6, 3, 1};
            spawn_prob = {.4, .8, .95, 1};
            spawn_delay = 4000;
            break;
        case 3:
            spitter_projectile_delay_ms = 3000.f;
            max_spawns = {15, 4, 3, 2};
            spawn_prob = {.4, .6, .85, 1};
            spawn_delay = 3500;
            break;
        case 4:
            max_spawns = {0, 0, 0, 0};
            break;
        default:
            spitter_projectile_delay_ms = 2500.f;
            max_spawns = {18, 7, 5, 3};
            spawn_prob = {.3, .6, .8, 1};
            spawn_delay = 3000;
            break;
    }
}


void move_firelings(RenderSystem* renderer)
{
    auto &testAI_container = registry.testAIs;
    for (uint i = 0; i < testAI_container.size(); i++)
    {
        TestAI &testAI = testAI_container.components[i];
        Entity entity = testAI_container.entities[i];
        Motion &motion = registry.motions.get(entity);
        if (testAI.departFromRight && motion.position[0] < 0)
        {
            float squareFactor = rand() % 2 == 0 ? 0.0005 : -0.0005;
            int rightHeight = ENEMY_SPAWN_HEIGHT_IDLE_RANGE + rand() % (window_height_px - ENEMY_SPAWN_HEIGHT_IDLE_RANGE * 2);
            motion.position = vec2(0.0, testAI.c);
            float curveParameter = (float)(rightHeight - testAI.c - window_width_px * window_width_px * squareFactor) / window_width_px;
            testAI.departFromRight = false;
            testAI.a = (float)squareFactor;
            testAI.b = curveParameter;
            motion.dir = 1;
        }
        else if (!testAI.departFromRight && motion.position[0] > window_width_px)
        {
            float squareFactor = rand() % 2 == 0 ? 0.0005 : -0.0005;
            int rightHeight = testAI.a * window_width_px * window_width_px + testAI.b * window_width_px + testAI.c;
            int leftHeight = ENEMY_SPAWN_HEIGHT_IDLE_RANGE + rand() % (window_height_px - ENEMY_SPAWN_HEIGHT_IDLE_RANGE * 2);
            motion.position = vec2(window_width_px, rightHeight);
            float curveParameter = (float)(rightHeight - leftHeight - window_width_px * window_width_px * squareFactor) / window_width_px;
            testAI.departFromRight = true;
            testAI.a = (float)squareFactor;
            testAI.b = curveParameter;
            testAI.c = (float)leftHeight;
            motion.dir = -1;
        }
        float gradient = 2 * testAI.a * motion.position[0] + testAI.b;
        float basicFactor = sqrt(BASIC_SPEED * BASIC_SPEED / (gradient * gradient + 1));
        float direction = testAI.departFromRight ? -1.0 : 1.0;
        motion.velocity = direction * vec2(basicFactor, gradient * basicFactor);
    }
}

void move_boulder(RenderSystem* renderer) {
    for (Entity boulder: registry.boulders.entities) {
        Motion& motion = registry.motions.get(boulder);
        if (motion.velocity.x > 0) {
            motion.angle += M_PI / 64;
        } else if (motion.velocity.x < 0) {
            motion.angle -= M_PI / 64;
        }
    }
}

void move_ghouls(RenderSystem* renderer, Entity player_hero)
{
    float GHOUL_SPEED = 100.f;
    float EDGE_DISTANCE = 0.f;

    registry.motions.get(player_hero);
    for (uint i = 0; i < registry.ghouls.entities.size(); i++) {
        Entity enemy = registry.ghouls.entities[i];
        Motion& enemy_motion = registry.motions.get(enemy);
        AnimationInfo& animation = registry.animated.get(enemy);
        Ghoul& enemy_reg = registry.ghouls.get(enemy);
        //printf("Position: %f\n", enemy_motion.position.x);
        if (enemy_reg.left_x != -1.f && enemy_motion.velocity.x == 0.f && enemy_motion.velocity.y == 0.f && animation.oneTimeState == -1) {
            float direction = max(enemy_motion.position.x - enemy_reg.left_x, enemy_reg.right_x - enemy_motion.position.x);
            direction = direction / abs(direction);
            enemy_motion.velocity.x = direction * GHOUL_SPEED;
            enemy_motion.dir = (int)direction;
        }
            // Reverse direction
        else if (enemy_motion.position.x - enemy_reg.left_x <= EDGE_DISTANCE && enemy_motion.velocity.y == 0.f && enemy_motion.velocity.x != 0.f) {
            enemy_motion.velocity.x = GHOUL_SPEED;
            enemy_motion.dir = 1;
        }
        else if (enemy_reg.right_x - enemy_motion.position.x <= EDGE_DISTANCE && enemy_motion.velocity.y == 0.f && enemy_motion.velocity.x != 0.f) {
            enemy_motion.velocity.x = -1.f * GHOUL_SPEED;
            enemy_motion.dir = -1;
        }
    }
}

void move_tracer(float elapsed_ms_since_last_update, Entity player_hero)
{
    const uint PHASE_IN_STATE = 1;
    const uint PHASE_OUT_STATE = 4;

    Motion& hero_motion = registry.motions.get(player_hero);
    for (uint i = 0; i < registry.followingEnemies.entities.size(); i++) {
        Entity enemy = registry.followingEnemies.entities[i];
        Motion& enemy_motion = registry.motions.get(enemy);
        AnimationInfo& animation = registry.animated.get(enemy);
        FollowingEnemies& enemy_reg = registry.followingEnemies.get(enemy);

        enemy_reg.next_blink_time -= elapsed_ms_since_last_update;
        if (enemy_reg.next_blink_time < 0.f && enemy_reg.blinked == false)
        {
            //Time between blinks
            enemy_reg.next_blink_time = 700.f;

            //enemies.hittable = true;
            //enemy_reg.hittable = true;

            if (enemy_reg.path.size() == 0 && find_map_index(enemy_motion.position) != find_map_index(hero_motion.position)) {
                std::vector<std::vector<char>> vec = grid_vec;
                bfs_follow_start(vec, enemy_motion.position, hero_motion.position, enemy);
            }

            //Don't blink when not moving: next pos in path is same pos as current
            if (enemy_reg.path.size() != 0 && find_index_from_map(enemy_reg.path.back()) == enemy_motion.position) {
                enemy_reg.path.pop_back();
            }
            else if (enemy_reg.path.size() != 0)
            {
                animation.oneTimeState = PHASE_IN_STATE;
                animation.oneTimer = 0;
                vec2 converted_pos = find_index_from_map(enemy_reg.path.back());
                enemy_motion.dir = (converted_pos.x > enemy_motion.position.x) ? -1 : 1;
                enemy_motion.position = converted_pos;
                enemy_reg.path.pop_back();

                //Don't blink when not moving: next loop will be to re-calc the path
                if (enemy_reg.path.size() != 0) {
                    enemy_reg.blinked = true;
                }
            }
        }

        if (enemy_reg.next_blink_time < 0.0f && enemy_reg.blinked == true) {
            enemy_reg.next_blink_time = 100.f;
            animation.oneTimeState = PHASE_OUT_STATE;
            animation.oneTimer = 0;
            //enemy_reg.hittable = false;
            //enemies.hittable = false;
            enemy_reg.blinked = false;
        }
    }
}

void move_spitters(float elapsed_ms_since_last_update, RenderSystem* renderer) {
    const uint SHOOT_STATE = 2;
    const uint SPITTER_FIRE_FRAME = 4;
    const uint WALKING_TIME = 5;
    const float WALKING_SPEED = 100.f;
    float EDGE_DISTANCE = 10.f;
    const float STOP_WALK_TIME = 300.f;

    auto &spitterEnemy_container = registry.spitterEnemies;
    for (uint i = 0; i < spitterEnemy_container.size(); i++)
    {
        SpitterEnemy &spitterEnemy = spitterEnemy_container.components[i];
        spitterEnemy.timeUntilNextShotMs -= elapsed_ms_since_last_update;
        Entity entity = spitterEnemy_container.entities[i];
        Motion &motion = registry.motions.get(entity);
        AnimationInfo &animation = registry.animated.get(entity);

        if (!spitterEnemy.canShoot && spitterEnemy.timeUntilNextShotMs > STOP_WALK_TIME && motion.velocity.y == 0.f && animation.oneTimeState != 2) {
            if (spitterEnemy.left_x != -1.f && motion.velocity.x == 0.f) {
                float direction;
                if (motion.position.x <= spitterEnemy.left_x || motion.position.x >= spitterEnemy.right_x) {
                    direction = max(motion.position.x - spitterEnemy.left_x, spitterEnemy.right_x - motion.position.x);
                }
                else {
                    direction = (rand() % 2) - 0.5f;
                }

                direction = direction / abs(direction);
                motion.velocity.x = direction * WALKING_SPEED;
                motion.dir = (int)direction;
            }
                // Reverse direction
            else if (motion.position.x - spitterEnemy.left_x <= EDGE_DISTANCE && motion.velocity.x != 0.f) {
                motion.velocity.x = WALKING_SPEED;
                motion.dir = 1;
            }
            else if (spitterEnemy.right_x - motion.position.x <= EDGE_DISTANCE && motion.velocity.x != 0.f) {
                motion.velocity.x = -1.f * WALKING_SPEED;
                motion.dir = -1;
            }
        }
        else if (spitterEnemy.canShoot || spitterEnemy.timeUntilNextShotMs <= STOP_WALK_TIME) {
            motion.velocity.x = 0;
        }

        animation.curState = (motion.velocity.x != 0)? 1: 0;

        if (animation.oneTimeState == SHOOT_STATE && (int)floor(animation.oneTimer * ANIMATION_SPEED_FACTOR) == SPITTER_FIRE_FRAME && spitterEnemy.canShoot) {
            Entity spitterBullet = createSpitterEnemyBullet(renderer, motion.position, motion.angle);
            float absolute_scale_x = abs(registry.motions.get(entity).scale[0]);
            if (registry.motions.get(spitterBullet).velocity[0] < 0.0f)
                registry.motions.get(entity).scale[0] = -absolute_scale_x;
            else
                registry.motions.get(entity).scale[0] = absolute_scale_x;
            spitterEnemy.canShoot = false;
        }
        if (spitterEnemy.timeUntilNextShotMs <= 0.f && registry.enemies.get(entity).hitting == true)
        {
            // attack animation
            animation.oneTimeState = SHOOT_STATE;
            animation.oneTimer = 0;
            spitterEnemy.canShoot = true;
            // create bullet at same position as enemy
            spitterEnemy.timeUntilNextShotMs = spitter_projectile_delay_ms;
        }
    }

    // decay spitter bullets
    auto& spitterBullets_container = registry.spitterBullets;
    for (uint i = 0; i < spitterBullets_container.size(); i++)
    {
        SpitterBullet& spitterBullet = spitterBullets_container.components[i];
        Entity entity = spitterBullets_container.entities[i];
        RenderRequest& render = registry.renderRequests.get(entity);
        Motion& motion = registry.motions.get(entity);
        // make bullets smaller over time
        motion.scale = vec2(motion.scale.x / spitterBullet.mass, motion.scale.y / spitterBullet.mass);
        spitterBullet.mass -= elapsed_ms_since_last_update / SPITTER_PROJECTILE_REDUCTION_FACTOR;
        motion.scale = vec2(motion.scale.x * spitterBullet.mass, motion.scale.y * spitterBullet.mass);
        render.scale = motion.scale;
        if (spitterBullet.mass <= SPITTER_PROJECTILE_MIN_SIZE)
        {
            spitterBullet.mass = 0;
            registry.remove_all_components_of(entity);
        }
    }
}

void boss_action_decision(Entity player_hero, Entity boss, RenderSystem* renderer, float elapsed_ms){
    Boss& boss_state = registry.boss.get(boss);
    AnimationInfo& info = registry.animated.get(boss);
    // 11 and 12 are hurt and death animation
    if (info.oneTimeState > 10) {
        boss_state.phase = 0;
        boss_state.state = BOSS_STATE::SIZE;
        for (auto hurt_box : boss_state.hurt_boxes) {
            registry.weaponHitBoxes.get(hurt_box).isActive = false;
        }
        return;
    }

    for (float& cd: boss_state.cooldowns)
        if (cd > 0)
            cd -= elapsed_ms;

    switch (boss_state.state) {
        case BOSS_STATE::TELEPORT:
            boss_action_teleport(boss);
            break;
        case BOSS_STATE::SWIPE:
            boss_action_swipe(boss);
            break;
        case BOSS_STATE::SUMMON_GHOULS:
            boss_action_summon(boss, renderer, 0);
            break;
        case BOSS_STATE::SUMMON_SPITTERS:
            boss_action_summon(boss, renderer, 1);
            break;
        case BOSS_STATE::SUMMON_BULLETS:
            boss_action_summon(boss, renderer, 2);
            break;
        case BOSS_STATE::SIZE:
            if (boss_state.cooldowns[(uint) BOSS_STATE::SIZE] <= 0)
                boss_state.state = get_action(player_hero, boss, renderer);
            break;
    }
}
std::vector<int> teleport_unique(vec2 pos) {
    //printf("Boss pos: %f, %f\n", pos.x, pos.y);
    std::vector<int> ret;
    if (pos.x == 600.f) {
        if (pos.y == 73.148148) {
            ret = { 0, 2, 7 };
        }
        else {
            ret = { 0, 1, 2 };
        }
    }
    else if (pos.x == 187.5f) {
        ret = { 7, 2, 1 };
    }
    else {
        ret = { 0, 1, 7 };
    }
    return ret;
}
void boss_action_teleport(Entity boss){
    const int PHASE_OUT = 8;
    const int PHASE_IN = 9;
    Motion& motion = registry.motions.get(boss);
    const std::vector<int> boss_platforms = teleport_unique(motion.position);
    AnimationInfo& info = registry.animated.get(boss);
    Boss& boss_state = registry.boss.get(boss);
    Enemies& enemy_info = registry.enemies.get(boss);
    if (boss_state.phase == 0) {
        play_sound(SOUND_EFFECT::TELEPORT);
        enemy_info.hitting = false;
        enemy_info.hittable = false;
        info.oneTimeState = PHASE_OUT;
        boss_state.phase++;
    } else if(boss_state.phase == 1 && info.oneTimeState == -1) {
        motion.position = getRandomWalkablePos(motion.scale, boss_platforms[rand() % boss_platforms.size()], false);
        boss_state.phase++;
    } else if(boss_state.phase == 2) {
        enemy_info.hittable = true;
        info.oneTimeState = PHASE_IN;
        boss_state.phase++;
    } else if(boss_state.phase == 3 && info.oneTimeState == -1) {
        enemy_info.hitting = true;
        boss_state.phase = 0;
        boss_state.state = BOSS_STATE::SIZE;
    }
}

void boss_action_swipe(Entity boss){
    const int SWIPE = 1;
    const int STAND_UP = 10;
    Boss& boss_state = registry.boss.get(boss);
    AnimationInfo& info = registry.animated.get(boss);
    if (boss_state.phase == 0) {
        play_sound(SOUND_EFFECT::BOSS_SLASH);
        info.oneTimeState = SWIPE;
        registry.motions.get(boss_state.hurt_boxes[0]).position = registry.motions.get(boss).position + vec2(0,55);
        registry.motions.get(boss_state.hurt_boxes[1]).position = registry.motions.get(boss).position + vec2(0,15);
        boss_state.phase++;
    } else if (boss_state.phase == 1 && info.oneTimeState != -1) {
        Motion& motion = registry.motions.get(boss);
        int frame = (int)floor(info.oneTimer * ANIMATION_SPEED_FACTOR);
        if (frame == 1) {
            registry.weaponHitBoxes.get(boss_state.hurt_boxes[0]).isActive = true;
        } else if (frame == 3) {
            registry.weaponHitBoxes.get(boss_state.hurt_boxes[1]).isActive = true;
        } else {
            for (auto hurt_box : boss_state.hurt_boxes) {
                registry.weaponHitBoxes.get(hurt_box).isActive = false;
            }
        }
    } else if (boss_state.phase == 1 && info.oneTimeState == -1) {
        info.oneTimeState = STAND_UP;
        boss_state.phase++;
    } else if (boss_state.phase == 2 && info.oneTimeState == -1) {
        boss_state.phase = 0;
        boss_state.state = BOSS_STATE::SIZE;
    }
}

void boss_action_summon(Entity boss, RenderSystem* renderer, uint type){
    const int SUMMON = 6;
    const int STAND_UP = 10;
    AnimationInfo& info = registry.animated.get(boss);
    Boss& boss_state = registry.boss.get(boss);
    if (boss_state.phase == 0) {
        play_sound(SOUND_EFFECT::BOSS_SUMMON);
        info.oneTimeState = SUMMON;
        boss_state.phase++;
    } else if(boss_state.phase == 1 && info.oneTimeState == -1) {
        info.oneTimeState = STAND_UP;
        switch (type) {
            case 0:
                for(int i = 0; i < 3 + rand() % 4; i++) {
                    createGhoul(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::GHOUL_ENEMY)));
                }
                break;
            case 1:
                for(int i = 0; i < 1 + rand() % 3; i++) {
                    createSpitterEnemy(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::SPITTER_ENEMY)));
                }
                break;
            case 2:
                for(int i = 0; i < 10 + rand() % 6; i++) {
                    Motion& motion = registry.motions.get(boss);
                    createSpitterEnemyBullet(renderer, motion.position, motion.angle);
                }
                break;
        }
        boss_state.phase++;
    } else if(boss_state.phase == 2 && info.oneTimeState == -1) {
        boss_state.phase = 0;
        boss_state.state = BOSS_STATE::SIZE;
    }

}



void boss_action_sword_spawn(bool create, vec2 pos, vec2 scale, RenderSystem* renderer, Entity player_hero) {
    float SWORD_SPEED1 = 40.f;
    float SWORD_SPEED2 = 80.f;

    if (create) {
        vec2 rad = vec2(scale.x/2.f, scale.y/2.f);
        float angle = ((float)rand() / RAND_MAX) * (2.f * M_PI);
        vec2 spawn_pos = pos + (rad * vec2(cos(angle), sin(angle)));
        //printf("Position: %f\n", angle);
        //create_boss_sword(renderer, spawn_pos, 1);
        create_boss_sword(renderer, spawn_pos, rand() % 2);
    }
    else {
        Motion& hero_motion = registry.motions.get(player_hero);
        auto& sword_container = registry.bossSwords;
        for (uint i = 0; i < sword_container.size(); i++)
        {
            BossSword& sword = sword_container.components[i];
            Entity entity = sword_container.entities[i];
            Motion& motion = registry.motions.get(entity);

            vec2 following_direction = hero_motion.position - motion.position;
            following_direction = following_direction / sqrt(dot(following_direction, following_direction));

            motion.velocity = following_direction * ((sword.type == 1) ? SWORD_SPEED1 : SWORD_SPEED2);
        }
    }

    //if (registry.bossSwords.components.size() < 2)
    //{
    //	//create_boss_sword(renderer, find_index_from_map(vec2(12, 8)), rand() % 2);
    //
    //}
}

BOSS_STATE get_action(Entity player_hero, Entity boss, RenderSystem* renderer) {
    vec2 boss_pos = registry.motions.get(boss).position;
    uint num_ghouls = registry.ghouls.entities.size();
    uint num_spitters = registry.spitterEnemies.entities.size();
    BOSS_STATE action;
    float max_utility = 0;
    for (uint i = 0; i < (uint) BOSS_STATE::SIZE; i++) {
        if (registry.boss.components[0].cooldowns[i] <= 0) {
            float utility = get_action_reward((BOSS_STATE) i, boss_pos, num_ghouls, num_spitters, 0, registry.boss.components[0].cooldowns, player_hero, boss, renderer);
            if (utility > max_utility) {
                max_utility = utility;
                action = (BOSS_STATE) i;
            }
        }
    }
    if (max_utility == 0) {
        action = BOSS_STATE::SIZE;
    } else {
        registry.boss.components[0].cooldowns[(uint) BOSS_STATE::SIZE] = BOSS_ACTION_COOLDOWNS[(uint) BOSS_STATE::SIZE];
        registry.boss.components[0].cooldowns[(uint) action] = BOSS_ACTION_COOLDOWNS[(uint) action];
    }
    return action;
}



float mdp_helper(vec2 boss_pos, uint num_ghouls, uint num_spitters, uint step_num, std::vector<float> cooldowns, Entity player_hero, Entity boss, RenderSystem* renderer) {
    float max_utility = 0;
    if (step_num <= MDP_HORIZON) {
        for (uint i = 0; i < (uint) BOSS_STATE::SIZE; i++) {
            if (cooldowns[i] <= 0) {
                float utility = get_action_reward((BOSS_STATE) i, boss_pos, num_ghouls, num_spitters, step_num, cooldowns, player_hero, boss, renderer);
                if (utility > max_utility) {
                    max_utility = utility;
                }
            }
        }
    }
    return max_utility;
}

float get_action_reward(BOSS_STATE action, vec2 boss_pos, uint num_ghouls, uint num_spitters, uint step_num, std::vector<float> cooldowns, Entity player_hero, Entity boss, RenderSystem* renderer) {
    cooldowns[(uint) action] = BOSS_ACTION_COOLDOWNS[(uint) action];
    for (float& cd: cooldowns)
        if (cd > 0)
            cd -= BOSS_ACTION_COOLDOWNS[(uint) BOSS_STATE::SIZE];
    float reward = 0;
    switch (action) {
        case BOSS_STATE::TELEPORT: {
            vec2 new_pos = getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS), 0, false);
            if (new_pos != boss_pos) {
                reward += (get_reward(boss_pos, num_ghouls, num_spitters, new_pos, num_ghouls, num_spitters, player_hero, boss) +
                        MDP_DISCOUNT_FACTOR * mdp_helper(new_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer)) / 3.f;
            }

            new_pos = getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS), 1, false);
            if (new_pos != boss_pos) {
                reward += (get_reward(boss_pos, num_ghouls, num_spitters, new_pos, num_ghouls, num_spitters, player_hero, boss) +
                        MDP_DISCOUNT_FACTOR * mdp_helper(new_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer)) / 3.f;
            }

            new_pos = getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS), 2, false);
            if (new_pos != boss_pos) {
                reward += (get_reward(boss_pos, num_ghouls, num_spitters, new_pos, num_ghouls, num_spitters, player_hero, boss) +
                        MDP_DISCOUNT_FACTOR * mdp_helper(new_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer)) / 3.f;
            }

            new_pos = getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS), 7, false);
            if (new_pos != boss_pos) {
                reward += (get_reward(boss_pos, num_ghouls, num_spitters, new_pos, num_ghouls, num_spitters, player_hero, boss) +
                        MDP_DISCOUNT_FACTOR * mdp_helper(new_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer)) / 3.f;
            }
            break;
        } case BOSS_STATE::SWIPE: {
            vec2 pos_dif = {abs(boss_pos.x - registry.motions.get(player_hero).position.x), abs(boss_pos.y - registry.motions.get(player_hero).position.y)};
            float x_penalty = std::pow(std::pow(MDP_BASE_REWARD, 1.f/20.f), min(pos_dif.x, 300.f) - 280);
            float y_penalty = std::pow(std::pow(MDP_BASE_REWARD, 1.f/20.f), min(pos_dif.y, 60.f) - 40);
            reward = MDP_BASE_REWARD - x_penalty - y_penalty + MDP_DISCOUNT_FACTOR * mdp_helper(boss_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer);
            break;
        } case BOSS_STATE::SUMMON_GHOULS: {
            for (uint i = 3; i <= 3 + 4; i++)
                reward += (get_reward(boss_pos, num_ghouls, num_spitters, boss_pos, num_ghouls + i, num_spitters, player_hero, boss) + MDP_DISCOUNT_FACTOR * mdp_helper(boss_pos, num_ghouls + i, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer)) / 5.f;
            break;
        } case BOSS_STATE::SUMMON_SPITTERS: {
            for (uint i = 1; i <= 1 + 3; i++)
                reward += (get_reward(boss_pos, num_ghouls, num_spitters, boss_pos, num_ghouls, num_spitters + i, player_hero, boss) + MDP_DISCOUNT_FACTOR * mdp_helper(boss_pos, num_ghouls, num_spitters + i, step_num + 1, cooldowns, player_hero, boss, renderer)) / 4.f;
            break;
        } case BOSS_STATE::SUMMON_BULLETS: {
            vec2 line_pos = (boss_pos + registry.motions.get(player_hero).position) / 2.f;
            vec2 line_scale = {sqrt(dot(boss_pos - registry.motions.get(player_hero).position, boss_pos - registry.motions.get(player_hero).position)), 1.f};
            float line_angle = atan2(registry.motions.get(player_hero).position.y - boss_pos.y, registry.motions.get(player_hero).position.x - boss_pos.x);
            Entity line = createLine(renderer, line_pos, {0, 0}, line_scale, line_angle);
            registry.lasers.emplace(line);
            registry.collisionMeshPtrs.emplace(line, &(renderer->getCollisionMesh(GEOMETRY_BUFFER_ID::SPRITE)));
            for (Entity block: registry.blocks.entities) {
                if (PhysicsSystem::collides(line, block)) {
                    registry.remove_all_components_of(line);
                    reward = MDP_BASE_REWARD / 1000.f + MDP_DISCOUNT_FACTOR * mdp_helper(boss_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer);
                    return reward;
                }
            }
            registry.remove_all_components_of(line);
            reward = MDP_BASE_REWARD / 2.5f + MDP_DISCOUNT_FACTOR * mdp_helper(boss_pos, num_ghouls, num_spitters, step_num + 1, cooldowns, player_hero, boss, renderer);
            break;
        }
    }
    return reward;
}

float get_reward(vec2 boss_pos_old, uint num_ghouls_old, uint num_spitters_old, vec2 boss_pos, uint num_ghouls, uint num_spitters, Entity player_hero, Entity boss) {
    if (num_ghouls_old != num_ghouls) {
        return MDP_BASE_REWARD / 7.f * (num_ghouls - num_ghouls_old) * (1 - min(std::pow(2, num_ghouls_old / (float) BOSS_MAX_GHOULS) - 1, (double) 1));
    } else if (num_spitters_old != num_spitters) {
        return MDP_BASE_REWARD / 4.f * (num_spitters - num_spitters_old) * (1 - min(std::pow(2, num_spitters_old / (float) BOSS_MAX_SPITTERS) - 1, (double) 1));
    } else {
        Player& player = registry.players.get(player_hero);
        if (player.hasWeapon && registry.swords.has(player.weapon)) {
            Motion& player_motion = registry.motions.get(player_hero);
            float x_buffer = max(abs(boss_pos_old.x - player_motion.position.x) - registry.motions.get(boss).scale.x / 2, 0.f);
            float y_buffer = max(abs(boss_pos_old.y - player_motion.position.y) - registry.motions.get(boss).scale.y / 2, 0.f);
            float player_dist_old = sqrt(dot(vec2(x_buffer, y_buffer), vec2(x_buffer, y_buffer)));
            x_buffer = max(abs(boss_pos.x - player_motion.position.x) - registry.motions.get(boss).scale.x / 2, 0.f);
            y_buffer = max(abs(boss_pos.y - player_motion.position.y) - registry.motions.get(boss).scale.y / 2, 0.f);
            float player_dist = sqrt(dot(vec2(x_buffer, y_buffer), vec2(x_buffer, y_buffer)));
            if (player_dist_old < 150)
                return MDP_BASE_REWARD * 1000;
            else {
                return MDP_BASE_REWARD * (1 - min(std::pow(2, player_dist_old / 300.f) - 1, (double) 1)) * max(min((player_dist - player_dist_old) / 100.f, 1.f), -1.f);
            }
        } else {
            Motion& player_motion = registry.motions.get(player_hero);
            float x_buffer = max(abs(boss_pos_old.x - player_motion.position.x) - registry.motions.get(boss).scale.x / 2, 0.f);
            float y_buffer = max(abs(boss_pos_old.y - player_motion.position.y) - registry.motions.get(boss).scale.y / 2, 0.f);
            float player_dist_old = sqrt(dot(vec2(x_buffer, y_buffer), vec2(x_buffer, y_buffer)));
            x_buffer = max(abs(boss_pos.x - player_motion.position.x) - registry.motions.get(boss).scale.x / 2, 0.f);
            y_buffer = max(abs(boss_pos.y - player_motion.position.y) - registry.motions.get(boss).scale.y / 2, 0.f);
            float player_dist = sqrt(dot(vec2(x_buffer, y_buffer), vec2(x_buffer, y_buffer)));
            return MDP_BASE_REWARD * (1 - min(std::pow(2, player_dist / 300.f) - 1, (double) 1)) * max(min((player_dist_old - player_dist) / 100.f, 1.f), -1.f);
        }
    }
}


void summon_boulder_helper(RenderSystem* renderer) {
    float x_pos = uniform_dist(rng) * (window_width_px - 120) + 60;
    float x_speed = 50 + 100 * uniform_dist(rng);
    x_speed = uniform_dist(rng) > 0.5 ? x_speed : -x_speed;
    float size = 3 + uniform_dist(rng);
    createBoulder(renderer, {x_pos, 0}, {x_speed, 0}, size);
}

void summon_fireling_helper(RenderSystem* renderer){
    float squareFactor = rand() % 2 == 0 ? 0.0005 : -0.0005;
    int leftHeight = ENEMY_SPAWN_HEIGHT_IDLE_RANGE + rand() % (window_height_px - ENEMY_SPAWN_HEIGHT_IDLE_RANGE * 2);
    int rightHeight = ENEMY_SPAWN_HEIGHT_IDLE_RANGE + rand() % (window_height_px - ENEMY_SPAWN_HEIGHT_IDLE_RANGE * 2);
    float curveParameter = (float)(rightHeight - leftHeight - window_width_px * window_width_px * squareFactor) / window_width_px;
    Entity newEnemy = createFireing(renderer, vec2(window_width_px, rightHeight));
    TestAI &enemyTestAI = registry.testAIs.get(newEnemy);
    enemyTestAI.departFromRight = true;
    enemyTestAI.a = (float)squareFactor;
    enemyTestAI.b = curveParameter;
    enemyTestAI.c = (float)leftHeight;
}
