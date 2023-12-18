// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include "ai_system.hpp"
#include "json.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <bitset>
#include <iostream>
#include <map>
#include <fstream>

// Global Variables (?)
vec2 mouse_pos = {0,0};
bool WorldSystem::pause = false;
bool WorldSystem::debug = false;
bool WorldSystem::mouse_clicked = false;
bool WorldSystem::isTitleScreen = true;
int WorldSystem::dialogue_screen_active = 0; // 0 means no dialogue screen is active
std::bitset<2> motionKeyStatus("00");
bool pickupKeyStatus = false;
vec3 player_color;
std::vector<Entity> player_hearts_GUI = { };
Entity powerup_GUI;
Entity difficulty_bar;
Entity indicator;
Entity score_text;
std::vector<Entity> score_GUI = { };
std::vector<Entity> following_enemies = { };

json::JSON state;

/* 
* ddl = Dynamic Difficulty Level
* (0 <= ddf < 100) -> (ddl = 0)
* (100 <= ddf < 200) -> (ddl = 1)
* (200 <= ddf < 300) -> (ddl = 2)
* (300 <= ddf < 400) -> (ddl = 3)
* As soon as ddf reaches 400, change to boss level (ddl = 4), const ddf = 499
* When boss is defeated, ddl = 5, ddf increases from 500 to MAX_FLOAT
* ddl still increases and change the hp of enemy (when implemented)
*/
int ddl;

/*
* ddf = Dynamic Difficulty Factor
* 1000 ms increases 1 ddf (at ddl = 0-3, 5-INF)
* Every enemy death increases 5(?) ddf
*/
float ddf;
float recorded_max_ddf;
bool should_score_prepare_to_show = false;
bool death_skip_dialogue = false;

float lavaPillarTimer = LAVA_PILLAR_SPAWN_DELAY;

// Create the fish world
WorldSystem::WorldSystem()
	: points(0)
{
}

WorldSystem::~WorldSystem()
{
	// Destroy all sound
	destroy_sound();
	
	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace
{
	void glfw_err_cb(int error, const char *desc)
	{
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow *WorldSystem::create_window()
{
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Create the main window (for rendering, keyboard, and mouse input)
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    float new_width = mode->width;
    float new_height = mode->height;
    window = glfwCreateWindow(new_width, new_height, "Titan's Trial", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1}); };
    auto cursor_click_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2)
    { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	auto window_close_redirect = [](GLFWwindow* wnd)
	{ ((WorldSystem*)glfwGetWindowUserPointer(wnd))->save_game(); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, cursor_click_redirect);
	glfwSetWindowCloseCallback(window, window_close_redirect);

	// Initialize all sound
	if (init_sound())
		return nullptr;

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg)
{
	this->renderer = renderer_arg;
	
	// Play main menu background music
	play_main_menu_music();

	// Set all states to default
	if (isTitleScreen) {
		create_title_screen();
	}

}


void WorldSystem::create_title_screen() 
{
	glfwSetWindowTitle(window, "Titan's Trial");
	ScreenState& screen = registry.screenStates.components[0];
	screen.screen_darken_factor = 0;
	isTitleScreen = true;

	pause = false;
	dialogue_screen_active = 0;

	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	//these magic number are just the vertical position of where the buttons are
	createMainMenuBackground(renderer);
	createTitleText(renderer, { window_width_px / 2, 100 });
	createButton(renderer, { window_width_px / 2, 450 }, TEXTURE_ASSET_ID::PLAY, [&]() {load_game(); });
	createButton(renderer, { window_width_px / 2, 550 }, TEXTURE_ASSET_ID::ALMANAC, [&]() {create_almanac_screen(); });
	createButton(renderer, { window_width_px / 2, 650 }, TEXTURE_ASSET_ID::QUIT, [&]() {exit(0); });
}

void WorldSystem::create_almanac_screen() {
	isTitleScreen = true;
	pause = false;
	dialogue_screen_active = 0;

	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	Entity helper = createHelperText(renderer, 1.f);
	Motion& motion = registry.motions.get(helper);
	motion.position = {window_width_px / 2, 150};
	registry.renderRequests.get(helper).visibility = true;

	createSword(renderer, {window_width_px / 6, 300});
	createToolTip(renderer, {window_width_px * 2 / 3, 300}, TEXTURE_ASSET_ID::SWORD_HELPER);
	createGun(renderer, {window_width_px / 6, 350});
	createToolTip(renderer, {window_width_px * 2 / 3, 350}, TEXTURE_ASSET_ID::GUN_HELPER);
	createGrenadeLauncher(renderer, {window_width_px / 6, 400});
	createToolTip(renderer, {window_width_px * 2 / 3, 400}, TEXTURE_ASSET_ID::GRENADE_HELPER);
	createRocketLauncher(renderer, {window_width_px / 6, 450});
	createToolTip(renderer, {window_width_px * 2 / 3, 450}, TEXTURE_ASSET_ID::ROCKET_HELPER);
	createWingedBoots(renderer, {window_width_px / 6, 500});
	createToolTip(renderer, {window_width_px * 2 / 3, 500}, TEXTURE_ASSET_ID::WINGED_BOOTS_HELPER);
	createPickaxe(renderer, {window_width_px / 6, 550});
	createToolTip(renderer, {window_width_px * 2 / 3, 550}, TEXTURE_ASSET_ID::PICKAXE_HELPER);
	createDashBoots(renderer, {window_width_px / 6, 600});
	createToolTip(renderer, {window_width_px * 2 / 3, 600}, TEXTURE_ASSET_ID::DASH_BOOTS_HELPER);
	createLaserRifle(renderer, {window_width_px / 6, 650});
	createToolTip(renderer, {window_width_px * 2 / 3, 650}, TEXTURE_ASSET_ID::LASER_HELPER);
	createTrident(renderer, {window_width_px / 6, 700});
	createToolTip(renderer, {window_width_px * 2 / 3, 700}, TEXTURE_ASSET_ID::TRIDENT_HELPER);

	createButton(renderer, { window_width_px / 2, 760 }, TEXTURE_ASSET_ID::BACK, [&]() {create_title_screen();});
}


// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{
	if (dialogue_screen_active == 0) {
		if (ddl == 4)
		{
			lavaPillarTimer += elapsed_ms_since_last_update;
			if (lavaPillarTimer > LAVA_PILLAR_SPAWN_DELAY) {
				lavaPillarTimer = 0;
				createLavaPillar(renderer, { window_width_px * 0.71,window_height_px + LAVA_PILLAR_BB.y / 2.f });
				createLavaPillar(renderer, { window_width_px * 0.29, window_height_px + LAVA_PILLAR_BB.y / 2.f });
			}
		}

		for (AnimationInfo& animation: registry.animated.components) {
			if (animation.oneTimeState != -1) {
				animation.oneTimer += elapsed_ms_since_last_update / 1000.f;
			}
		}
		
		// internal data update section
		float expectedTimer = registry.players.get(player_hero).invulnerable_timer - elapsed_ms_since_last_update;
		if (expectedTimer <= 0.0f)
		{
			expectedTimer = 0.0f;
			registry.colors.get(player_hero) = player_color;
			registry.players.get(player_hero).invuln_type = INVULN_TYPE::NONE;
		}
		registry.players.get(player_hero).invulnerable_timer = expectedTimer;

		if (registry.players.get(player_hero).invuln_type == INVULN_TYPE::HIT) 
		{
			for (Entity e : player_hearts_GUI) {
				registry.renderRequests.get(e).used_texture = TEXTURE_ASSET_ID::PLAYER_HEART_STEEL;
			}
		}
		else if (registry.players.get(player_hero).invuln_type == INVULN_TYPE::HEAL)
		{
			for (Entity e : player_hearts_GUI) {
				registry.renderRequests.get(e).used_texture = TEXTURE_ASSET_ID::PLAYER_HEART_HEAL;
			}
		}
		else
		{
			for (Entity e : player_hearts_GUI) {
				registry.renderRequests.get(e).used_texture = TEXTURE_ASSET_ID::PLAYER_HEART;
			}
		}

		switch (registry.players.get(player_hero).equipment_type)
		{
			case COLLECTABLE_TYPE::PICKAXE:
				registry.renderRequests.get(powerup_GUI).used_texture = TEXTURE_ASSET_ID::PICKAXE;
				registry.renderRequests.get(powerup_GUI).visibility = true;
				break;
			case COLLECTABLE_TYPE::WINGED_BOOTS:
				registry.renderRequests.get(powerup_GUI).used_texture = TEXTURE_ASSET_ID::WINGED_BOOTS;
				registry.renderRequests.get(powerup_GUI).visibility = true;
				break;
			case COLLECTABLE_TYPE::DASH_BOOTS:
				registry.renderRequests.get(powerup_GUI).used_texture = TEXTURE_ASSET_ID::DASH_BOOTS;
				registry.renderRequests.get(powerup_GUI).visibility = true;
				break;
			default:
				registry.renderRequests.get(powerup_GUI).used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
				registry.renderRequests.get(powerup_GUI).visibility = false;
		}

		changeScore(points);

		ddf = max(ddf, 0.f);
		if (ddf < 100 && ddl != 0)
		{
			ddl = 0;
			if (ddf > recorded_max_ddf) {
				show_dialogue(1);
			}
		}
		else if (ddf >= 100 && ddf < 200 && ddl != 1)
		{
			ddl = 1;
			if (ddf > recorded_max_ddf) {
				show_dialogue(2);
			}
		}
		else if (ddf >= 200 && ddf < 300 && ddl != 2)
		{
			ddl = 2;
			if (ddf > recorded_max_ddf) {
				show_dialogue(3);
			}
		}
		else if (ddf >= 300 && ddf < 400 && ddl != 3)
		{
			ddl = 3;
			if (ddf > recorded_max_ddf) {
				show_dialogue(6);
			}
		}
		else if (ddf >= 400 && ddf < 500 && ddl != 4)
		{
			ddl = 4;
			registry.remove_all_components_of(indicator);
			/*
			registry.renderRequests.get(difficulty_bar).used_texture = TEXTURE_ASSET_ID::DIFFICULTY_BAR_BOSS;
			db_decorator.push_back(createDBFlame(renderer, DB_FLAME_CORD));
			db_decorator.push_back(createDBSkull(renderer, DIFF_BAR_CORD));
			*/
			registry.renderRequests.get(difficulty_bar).used_texture = TEXTURE_ASSET_ID::DB_BOSS_SINGLE;
			registry.renderRequests.get(difficulty_bar).scale.y = 125.f;
			registry.motions.get(difficulty_bar).position = DB_BOSS_CORD;
			clear_enemies();
			boss = createBossEnemy(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::BOSS), 1, false));
			createHealthBar(renderer, boss);
			if (ddf > recorded_max_ddf) {
				show_dialogue(7);
				printf("\nLv4\n");
			}
		}
		else if (ddf >= 500 && ddf < 600 && ddl != 5)
		{
			ddl = 5;
			clear_enemies();
			if (ddf > recorded_max_ddf) {
				show_dialogue(8);
			}
		}
		else if (ddf >= 600)
		{
			ddl = (int) ddf / 100;
		}

		recorded_max_ddf = max(recorded_max_ddf, ddf);
		state["history_max_ddf"] = max((float) state["history_max_ddf"].ToFloat(), ddf);
		points = (points > (unsigned int) INT_MAX) ? INT_MAX : points;
		state["history_max_score"] = max((int) state["history_max_score"].ToInt(), (int) points);

		if (ddl < 4)
			registry.motions.get(indicator).position[0] = 30.f + ddf * INDICATOR_VELOCITY;

		if (ddl == 4)
			ddf = 499.0;
		else
			ddf += elapsed_ms_since_last_update / 1000.f;

		// Updating window title

		std::stringstream title_ss;
		title_ss << "Points: " << points;
		title_ss << "; Dynamic Difficulty Level: " << ddl;
		title_ss << "; Dynamic Difficulty Factor: " << ddf;
		glfwSetWindowTitle(window, title_ss.str().c_str());

		// Remove debug info from the last step
		while (registry.debugComponents.entities.size() > 0)
			registry.remove_all_components_of(registry.debugComponents.entities.back());

		// Removing out of screen entities
		auto &motion_container = registry.motions;

		// Remove entities that leave the screen on the left side
		// Iterate backwards to be able to remove without unterfering with the next object to visit
		// (the containers exchange the last element with the current)
		for (int i = (int)motion_container.components.size() - 1; i >= 0; --i)
		{
			Motion &motion = motion_container.components[i];

			if (motion.position.x + abs(motion.scale.x) < 0.f || motion.position.x - abs(motion.scale.x) > window_width_px || motion.position.y - abs(motion.scale.y) > window_height_px || motion.position.y + abs(motion.scale.y) < 0.f)
			{
				Entity e = motion_container.entities[i];
				if (registry.parallaxBackgrounds.has(e)) {
					ParallaxBackground &bg = registry.parallaxBackgrounds.get(e);
					motion.position = bg.resetPosition;
				}
			}
			
			if (motion.position.y < -250 && (registry.bullets.has(motion_container.entities[i]) || registry.rockets.has(motion_container.entities[i]))) // || registry.waterBalls.has(motion_container.entities[i])
				registry.remove_all_components_of(motion_container.entities[i]);
			else if (registry.lasers.has(motion_container.entities[i]) && (motion.position.x > window_width_px + window_width_px / 2.f || motion.position.x < -window_width_px / 2.f || motion.position.y > window_height_px + window_height_px / 2.f || motion.position.y < -window_height_px / 2.f))
				registry.remove_all_components_of(motion_container.entities[i]);
			else if ((registry.boulders.has(motion_container.entities[i]) || registry.lavaPillars.has(motion_container.entities[i])) && (motion.position.y > window_height_px + motion.scale.y))
				registry.remove_all_components_of(motion_container.entities[i]);
		}

		if (registry.players.get(player_hero).hasWeapon) {
			update_weapon(renderer, elapsed_ms_since_last_update, player_hero, mouse_clicked);
			update_water_balls(elapsed_ms_since_last_update, registry.weapons.get(registry.players.get(player_hero).weapon).type, mouse_clicked);
		} else {
			update_water_balls(elapsed_ms_since_last_update, COLLECTABLE_TYPE::COLLECTABLE_COUNT, mouse_clicked);
		}

		update_equipment(elapsed_ms_since_last_update, player_hero);
		update_dash_boots(elapsed_ms_since_last_update, player_hero, motionKeyStatus, BASIC_SPEED);
		update_pickaxe(elapsed_ms_since_last_update);

		update_grenades(renderer, elapsed_ms_since_last_update);
		update_explosions(elapsed_ms_since_last_update);
		// Animation Stuff
		vec2 playerVelocity = registry.motions.get(player_hero).velocity;
		AnimationInfo &playerAnimation = registry.animated.get(player_hero);
		if (playerVelocity.y > 0)
		{
			playerAnimation.curState = 4;
		}
		else if (playerVelocity.y < 0)
		{
			playerAnimation.curState = 3;
		}
		else if (playerVelocity.x != 0)
		{
			playerAnimation.curState = 2;
		}
		else
		{
			playerAnimation.curState = 0;
		}

		for (int i = 0; i < registry.players.get(player_hero).hp; i++) {
			Entity curHeart = player_hearts_GUI[i];
			registry.renderRequests.get(curHeart).visibility = true;
		}
		for (int i = registry.players.get(player_hero).hp; i < player_hearts_GUI.size(); i++) {
			Entity curHeart = player_hearts_GUI[i];
			registry.renderRequests.get(curHeart).visibility = false;
		}


		update_collectable_timer(elapsed_ms_since_last_update, renderer, ddl);
        move_firelings(renderer);
        move_boulder( renderer);
        move_ghouls(renderer, player_hero);
        move_spitters(elapsed_ms_since_last_update, renderer);
		if (boss && registry.boss.size()) {
			boss_action_decision(player_hero, boss, renderer, elapsed_ms_since_last_update);
		}
        do_enemy_spawn(elapsed_ms_since_last_update, renderer, ddl);
		update_graphics_all_enemies();

		if ((ddl == 2 || ddl == 3) && following_enemies.empty())
		{
			Entity newEnemy = createFollowingEnemy(renderer, find_index_from_map(vec2(12, 8)));
			std::vector<std::vector<char>> vec = grid_vec;
			bfs_follow_start(vec, registry.motions.get(newEnemy).position, registry.motions.get(player_hero).position, newEnemy);
			following_enemies.push_back(newEnemy);
		}
		else if ((ddl == 2 || ddl == 3) && !following_enemies.empty())
		{
            move_tracer(elapsed_ms_since_last_update, player_hero);
		}
		else if ((ddl != 2 && ddl != 3) && !following_enemies.empty())
		{
			registry.remove_all_components_of(following_enemies[0]);
			following_enemies.clear();
		}

		// Processing the hero state
		assert(registry.screenStates.components.size() <= 1);
		ScreenState &screen = registry.screenStates.components[0];

		float min_timer_ms = 3000.f;
		for (Entity entity : registry.deathTimers.entities)
		{
			// progress timer
			DeathTimer &timer = registry.deathTimers.get(entity);
			timer.timer_ms -= elapsed_ms_since_last_update;
			if (timer.timer_ms < min_timer_ms)
			{
				min_timer_ms = timer.timer_ms;
			}

			// restart the game once the death timer expired
			if (timer.timer_ms < 0)
			{
				registry.deathTimers.remove(entity);
				screen.screen_darken_factor = 0;
				death_skip_dialogue = true;
				restart_game();
				return true;
			}
		}

		screen.screen_darken_factor = 1 - min_timer_ms / 3000;
	}
	return true;
}

void WorldSystem::changeScore(int score)
{
	if (score >= 99999) 
	{
		for (Entity n : score_GUI)
			registry.renderRequests.get(n).used_texture = TEXTURE_ASSET_ID::NINE;
	}
	else
	{
		int mask = 10000;
		for (Entity n : score_GUI)
		{
			int digit = score / mask;
			registry.renderRequests.get(n).used_texture = connectNumber(digit);
			if (digit != 0) {
				score -= digit * mask;
			}
			mask /= 10;
		}
	}
}

TEXTURE_ASSET_ID WorldSystem::connectNumber(int digit)
{
	switch (digit)
	{
	case 0:
		return TEXTURE_ASSET_ID::ZERO;
	case 1:
		return TEXTURE_ASSET_ID::ONE;
	case 2:
		return TEXTURE_ASSET_ID::TWO;
	case 3:
		return TEXTURE_ASSET_ID::THREE;
	case 4:
		return TEXTURE_ASSET_ID::FOUR;
	case 5:
		return TEXTURE_ASSET_ID::FIVE;
	case 6:
		return TEXTURE_ASSET_ID::SIX;
	case 7:
		return TEXTURE_ASSET_ID::SEVEN;
	case 8:
		return TEXTURE_ASSET_ID::EIGHT;
	default:
		return TEXTURE_ASSET_ID::NINE;
	}
}

void WorldSystem::show_dialogue(int dialogue_number)
{
	// remove current dialogue screen
	if (dialogue_screen_active != 0) 
	{
		Entity dialogue = registry.dialogues.entities.back();
		registry.remove_all_components_of(dialogue);
		Entity text = registry.dialogueTexts.entities.back();
		registry.remove_all_components_of(text);
		motionKeyStatus.reset();
		if (should_score_prepare_to_show)
		{
			registry.renderRequests.get(difficulty_bar).used_texture = TEXTURE_ASSET_ID::DB_BROKEN_SINGLE;
			registry.motions.get(difficulty_bar).position = DB_SATAN_CORD;
			registry.renderRequests.get(difficulty_bar).scale = { 220.f, 128.f };
			registry.renderRequests.get(score_text).visibility = true;
			for (Entity n : score_GUI)
			{
				registry.renderRequests.get(n).visibility = true;
			}
		}
	}

	dialogue_screen_active = dialogue_number;
	if (dialogue_number != 0) {
		play_sound(effect_to_play(dialogue_number));
		if (dialogue_number != 9) {
			play_dialogue_music();
		}
		createDialogue(renderer, connectDialogue(dialogue_number));
	} else {
		stop_dialogue_music();
	}
}

SOUND_EFFECT WorldSystem::effect_to_play(int dialogue_number) {
	switch (dialogue_number) {
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
			return SOUND_EFFECT::TELEPORT;
			break;
		case 9:
			return SOUND_EFFECT::BELL;
		default: // cases 3, 7, 8. Needed default case to compile
			return SOUND_EFFECT::LAUGH;
			break;
	}
}

TEXTURE_ASSET_ID WorldSystem::connectDialogue(int num)
{
	switch (num)
	{
	case 1:
		return TEXTURE_ASSET_ID::DIALOGUE_1;
	case 2:
		return TEXTURE_ASSET_ID::DIALOGUE_2;
	case 3:
		return TEXTURE_ASSET_ID::DIALOGUE_3;
	case 4:
		return TEXTURE_ASSET_ID::DIALOGUE_4;
	case 5:
		return TEXTURE_ASSET_ID::DIALOGUE_5;
	case 6:
		return TEXTURE_ASSET_ID::DIALOGUE_6;
	case 7:
		return TEXTURE_ASSET_ID::DIALOGUE_7;
	case 8:
		return TEXTURE_ASSET_ID::DIALOGUE_8;
	default:
		return TEXTURE_ASSET_ID::ENDLESS_OVERLAY;
	}
}

void WorldSystem::update_graphics_all_enemies()
{
    for (Entity entity: registry.enemies.entities)
    {
        Enemies& enemy = registry.enemies.get(entity);

        if (!registry.animated.has(entity))
            continue;
        AnimationInfo& animation = registry.animated.get(entity);

        if (animation.oneTimeState == enemy.death_animation && (int)floor(animation.oneTimer * ANIMATION_SPEED_FACTOR) == animation.stateFrameLength[enemy.death_animation]) {
            registry.remove_all_components_of(entity);
            update_health_bar();
        } else if (animation.oneTimeState == enemy.hit_animation && (int)floor(animation.oneTimer * ANIMATION_SPEED_FACTOR) == animation.stateFrameLength[enemy.hit_animation]) {
            enemy.hittable = true;
            enemy.hitting = true;
            if (registry.motions.get(entity).velocity.x != 0)
                registry.motions.get(entity).dir = registry.motions.get(entity).velocity.x > 0 ? 1 : -1;

            if (registry.boss.has(entity)) {
                enemy.hittable = false;
                enemy.hitting = false;
                registry.boss.get(entity).state = BOSS_STATE::TELEPORT;
            }
        }
    }
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	isTitleScreen = false;
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");
	play_music();

	points = 0;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	// Debugging for memory/component leaks
	registry.list_all_components();
	// add bg
	
	create_parallax_background();
	initiate_weapons();


	// Create a new hero
	player_hero = createHero(renderer, { 100, 200 });
	registry.colors.insert(player_hero, { 1, 0.8f, 0.8f });

	Player& player = registry.players.get(player_hero);
	createSword(renderer, registry.motions.get(player_hero).position);

	// global variables at this .cpp to reset, don't forget it!
	motionKeyStatus.reset();
	ddl = -1;
	ddf = 0.f;
	if (!death_skip_dialogue)
	{
		recorded_max_ddf = -1.f;
	}
	else if (recorded_max_ddf >= 500)
	{
		recorded_max_ddf = 499;
	}
	death_skip_dialogue = false;
	should_score_prepare_to_show = false;
	player_color = registry.colors.get(player_hero);
	player_hearts_GUI.clear();
	score_GUI.clear();

	create_inGame_GUIs();

    for(auto value : platforms) {
        createBlock(renderer, value.x, value.y);
    }
	
	// Adds whatever's needed in the pause screen
	create_pause_screen();



	//testing screen dimensions
	/*for (int i = 10; i < window_width_px; i += ENEMY_BB_WIDTH) {
		for (int j = -25; j < window_height_px; j += ENEMY_BB_HEIGHT) {
			createEnemy(renderer, vec2(i, j), 0.0, vec2(0.0, 0.0), vec2(ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT));
		}
	}*/
}

void WorldSystem::save_game() {
	if (!isTitleScreen && !registry.deathTimers.has(player_hero))
	{
		state =
		{
			"mute", is_music_muted,
			"ddl", ddl,
			"ddf", ddf,
			"recorded_max_ddf", recorded_max_ddf,
			"history_max_ddf", state["history_max_ddf"],
			"score", points,
			"history_max_score", state["history_max_score"],
			"hp", registry.players.get(player_hero).hp,
			"player_x", registry.motions.get(player_hero).position.x,
			"player_y", registry.motions.get(player_hero).position.y,
			"weapon", save_weapon(registry.players.get(player_hero).weapon),
			"fire_enemy", json::Array(),
			"ghoul", json::Array(),
			"spitter", json::Array(),
			"spitter_bullet", json::Array(),
			"boulder", json::Array(),
		};

		for (Entity fire_enemy : registry.fireEnemies.entities)
		{
			json::JSON sfes = json::Object();   // single_fire_enemy_save
			sfes =
			{
				"hp", registry.enemies.get(fire_enemy).health,
				"x_pos", registry.motions.get(fire_enemy).position.x,
				"y_pos", registry.motions.get(fire_enemy).position.y,
				"a", registry.testAIs.get(fire_enemy).a,
				"b", registry.testAIs.get(fire_enemy).b,
				"c", registry.testAIs.get(fire_enemy).c,
				"from_right", registry.testAIs.get(fire_enemy).departFromRight,
			};
			state["fire_enemy"].append(sfes);
		}

		for (Entity ghoul : registry.ghouls.entities)
		{
			json::JSON sgs = json::Object();   // single_ghoul_save
			sgs =
			{
				"hp", registry.enemies.get(ghoul).health,
				"x_pos", registry.motions.get(ghoul).position.x,
				"y_pos", registry.motions.get(ghoul).position.y,
				"x_v", registry.motions.get(ghoul).velocity.x,
				"y_v", registry.motions.get(ghoul).velocity.y,
				"dir", registry.motions.get(ghoul).dir,
			};
			state["ghoul"].append(sgs);
		}

		for (Entity spitter : registry.spitterEnemies.entities)
		{
			json::JSON sss = json::Object();   // single_spitter_save
			sss =
			{
				"hp", registry.enemies.get(spitter).health,
				"x_pos", registry.motions.get(spitter).position.x,
				"y_pos", registry.motions.get(spitter).position.y,
				"x_v", registry.motions.get(spitter).velocity.x,
				"y_v", registry.motions.get(spitter).velocity.y,
				"dir", registry.motions.get(spitter).dir,
				"timer", registry.spitterEnemies.get(spitter).timeUntilNextShotMs,
				"shootable", registry.spitterEnemies.get(spitter).canShoot,
				"right_x", registry.spitterEnemies.get(spitter).right_x,
				"left_x", registry.spitterEnemies.get(spitter).left_x,
			};
			state["spitter"].append(sss);
		}

		for (Entity spitter_bullet : registry.spitterBullets.entities)
		{
			json::JSON ssbs = json::Object();   // single_spitter_bullet_save
			ssbs =
			{
				"x_pos", registry.motions.get(spitter_bullet).position.x,
				"y_pos", registry.motions.get(spitter_bullet).position.y,
				"x_v", registry.motions.get(spitter_bullet).velocity.x,
				"y_v", registry.motions.get(spitter_bullet).velocity.y,
				"scale_x", registry.motions.get(spitter_bullet).scale.x,
				"scale_y", registry.motions.get(spitter_bullet).scale.y,
				"angle", registry.motions.get(spitter_bullet).angle,
				"mass", registry.spitterBullets.get(spitter_bullet).mass,
			};
			state["spitter_bullet"].append(ssbs);
		}

		for (Entity boulder : registry.boulders.entities)
		{
			json::JSON sbs = json::Object();   // single_boulder_save
			sbs =
			{
				"x_pos", registry.motions.get(boulder).position.x,
				"y_pos", registry.motions.get(boulder).position.y,
				"x_v", registry.motions.get(boulder).velocity.x,
				"y_v", registry.motions.get(boulder).velocity.y,
				"scale_x", registry.motions.get(boulder).scale.x,
				"scale_y", registry.motions.get(boulder).scale.y,
				"angle", registry.motions.get(boulder).angle,
				"hitting", registry.enemies.get(boulder).hitting,
			};
			state["boulder"].append(sbs);
		}

		std::ofstream out("game_save.json");
		out << state;
		out.close();
	}

	create_title_screen();
}

int WorldSystem::save_weapon(Entity weapon) {
	if (registry.swords.has(weapon))
		return 0;
	else if (registry.guns.has(weapon))
		return 1;
	else if (registry.rocketLaunchers.has(weapon))
		return 2;
	else if (registry.grenadeLaunchers.has(weapon))
		return 3;
	else if (registry.laserRifles.has(weapon))
		return 4;
	else if (registry.tridents.has(weapon))
		return 5;
	else
		return -1;
}

void WorldSystem::load_game() {
	std::ifstream in("game_save.json");
	std::stringstream buffer;
	buffer << in.rdbuf();
	std::string jsonString = buffer.str();
	if (jsonString != "")
	{
		state = json::JSON::Load(jsonString);
		if (state["mute"].ToBool())
		{
			is_music_muted = true;
			set_mute_music(is_music_muted);
		}
		// restart after loading mute status
		restart_game();

		ddl = state["ddl"].ToInt();
		ddf = state["ddf"].ToFloat();
		recorded_max_ddf = state["recorded_max_ddf"].ToFloat();
		switch (ddl)
		{
			case 4:
				ddf = 400.f;
				recorded_max_ddf = 399.f;
				ddl = 3;
				break;
			case 5:
				registry.remove_all_components_of(indicator);
				registry.renderRequests.get(difficulty_bar).used_texture = TEXTURE_ASSET_ID::DB_BROKEN_SINGLE;
				registry.motions.get(difficulty_bar).position = DB_SATAN_CORD;
				registry.renderRequests.get(difficulty_bar).scale = { 220.f, 128.f };
				registry.renderRequests.get(score_text).visibility = true;
				for (Entity n : score_GUI)
				{
					registry.renderRequests.get(n).visibility = true;
				}
				break;
		}
		points = state["score"].ToInt();
		Player& player = registry.players.get(player_hero);
		player.hp = state["hp"].ToInt();
		int weapon = state["weapon"].ToInt();
		registry.motions.get(player_hero).position = { state["player_x"].ToFloat(), state["player_y"].ToFloat() };
		if (weapon == 0)
			collect(createSword(renderer, { 0.f, 0.f }), player_hero);
		else if (weapon == 1)
			collect(createGun(renderer, { 0.f, 0.f }), player_hero);
		else if (weapon == 2)
			collect(createRocketLauncher(renderer, { 0.f, 0.f }), player_hero);
		else if (weapon == 3)
			collect(createGrenadeLauncher(renderer, { 0.f, 0.f }), player_hero);
		else if (weapon == 4)
			collect(createLaserRifle(renderer, { 0.f, 0.f }), player_hero);
		else if (weapon == 5)
			collect(createTrident(renderer, { 0.f, 0.f }), player_hero);

		for (int i = 0; i < state["fire_enemy"].size(); i++)
		{
			json::JSON sfe = state["fire_enemy"][i];
			if (!sfe["hp"].ToInt() <= 0)
			{
				Entity nfe = createFireing(renderer, {sfe["x_pos"].ToFloat(), sfe["y_pos"].ToFloat()});
				Enemies &nfe_basic = registry.enemies.get(nfe);
				nfe_basic.health = sfe["hp"].ToInt();
				nfe_basic.hittable = true;
				nfe_basic.hitting = true;
				TestAI &nfe_ai = registry.testAIs.get(nfe);
				nfe_ai.a = sfe["a"].ToFloat();
				nfe_ai.b = sfe["b"].ToFloat();
				nfe_ai.c = sfe["c"].ToFloat();
				nfe_ai.departFromRight = sfe["from_right"].ToBool();
			}
		}

		for (int i = 0; i < state["ghoul"].size(); i++)
		{
			json::JSON sg = state["ghoul"][i];
			if (!sg["hp"].ToInt() <= 0)
			{
				Entity ng = createGhoul(renderer, { sg["x_pos"].ToFloat(), sg["y_pos"].ToFloat() });
				Enemies& ng_basic = registry.enemies.get(ng);
				ng_basic.health = sg["hp"].ToInt();
				ng_basic.hittable = true;
				ng_basic.hitting = true;
				Motion& ng_mo = registry.motions.get(ng);
				ng_mo.velocity = { sg["x_v"].ToFloat(), sg["y_v"].ToFloat() };
				ng_mo.dir = { static_cast<int>(sg["dir"].ToInt()) };
			}
		}

		for (int i = 0; i < state["spitter"].size(); i++)
		{
			json::JSON ss = state["spitter"][i];
			if (!ss["hp"].ToInt() <= 0)
			{
				Entity ns = createSpitterEnemy(renderer, { ss["x_pos"].ToFloat(), ss["y_pos"].ToFloat() });
				Enemies &ns_basic = registry.enemies.get(ns);
				ns_basic.health = ss["hp"].ToInt();
				ns_basic.hittable = true;
				ns_basic.hitting = true;
				SpitterEnemy& ns_info = registry.spitterEnemies.get(ns);
				ns_info.canShoot = ss["shootable"].ToBool();
				ns_info.timeUntilNextShotMs = ss["timer"].ToFloat();
				ns_info.left_x = ss["left_x"].ToFloat();
				ns_info.right_x = ss["right_x"].ToFloat();
				Motion& ns_mo = registry.motions.get(ns);
				ns_mo.velocity = { ss["x_v"].ToFloat(), ss["y_v"].ToFloat() };
				ns_mo.dir = ss["dir"].ToInt();
			}
		}

		for (int i = 0; i < state["spitter_bullet"].size(); i++)
		{
			json::JSON ssb = state["spitter_bullet"][i];
			Entity nsb = createSpitterEnemyBullet(renderer, { ssb["x_pos"].ToFloat(), ssb["y_pos"].ToFloat() }, ssb["angle"].ToFloat());
			registry.spitterBullets.get(nsb).mass = ssb["mass"].ToFloat();
			Motion& nsb_mo = registry.motions.get(nsb);
			nsb_mo.scale = { ssb["scale_x"].ToFloat(), ssb["scale_y"].ToFloat() };
			nsb_mo.velocity = { ssb["x_v"].ToFloat(), ssb["y_v"].ToFloat() };
		}

		for (int i = 0; i < state["boulder"].size(); i++)
		{
			json::JSON sb = state["boulder"][i];
			Entity nb = createBoulder(renderer, { sb["x_pos"].ToFloat(), sb["y_pos"].ToFloat() }, { sb["x_v"].ToFloat(), sb["y_v"].ToFloat() }, 3.f);
			registry.enemies.get(nb).hitting = sb["hitting"].ToBool();
			Motion& nb_mo = registry.motions.get(nb);
			nb_mo.scale = { sb["scale_x"].ToFloat(), sb["scale_y"].ToFloat() };
		}

		registry.players.get(player_hero).invuln_type = INVULN_TYPE::HEAL;
		registry.players.get(player_hero).invulnerable_timer = 3000.f;
	}
	else
	{
		restart_game();
		state =
		{
			"history_max_ddf", 0.f,
			"history_max_score", 0,
		};
	}
}

void WorldSystem::create_pause_screen() {
    createButton(renderer, {18, 18}, TEXTURE_ASSET_ID::MENU, [&](){change_pause();});
    createButton(renderer, {window_width_px / 2, window_height_px / 2}, TEXTURE_ASSET_ID::BACK, [&]() {save_game(); }, false);
    createHelperText(renderer, 1.4f);
}

void WorldSystem::create_parallax_background() {
	parallax_background_color = createParallaxItem(renderer, {600, 400}, TEXTURE_ASSET_ID::BACKGROUND_COLOR);
	parallax_moon = createParallaxItem(renderer, {580, 400}, TEXTURE_ASSET_ID::PARALLAX_MOON);
	parallax_clouds_far_1 = createParallaxItem(renderer, {600, 400}, TEXTURE_ASSET_ID::PARALLAX_CLOUDS_FAR);
	parallax_clouds_far_2 = createParallaxItem(renderer, {-600, 400}, TEXTURE_ASSET_ID::PARALLAX_CLOUDS_FAR);
	parallax_clouds_close_1 = createParallaxItem(renderer, {600, 400}, TEXTURE_ASSET_ID::PARALLAX_CLOUDS_CLOSE);
	parallax_clouds_close_2 = createParallaxItem(renderer, {-600, 400}, TEXTURE_ASSET_ID::PARALLAX_CLOUDS_CLOSE);
	parallax_rain_1 = createParallaxItem(renderer, {0, 1200}, TEXTURE_ASSET_ID::PARALLAX_RAIN);
	parallax_rain_2 = createParallaxItem(renderer, {600, 800}, TEXTURE_ASSET_ID::PARALLAX_RAIN);
	parallax_rain_3 = createParallaxItem(renderer, {400, 400}, TEXTURE_ASSET_ID::PARALLAX_RAIN);
	parallax_rain_4 = createParallaxItem(renderer, {800, 100}, TEXTURE_ASSET_ID::PARALLAX_RAIN);
	parallax_background = createParallaxItem(renderer, {600, 400}, TEXTURE_ASSET_ID::BACKGROUND);
	parallax_lava_1 = createParallaxItem(renderer, {600, 813}, TEXTURE_ASSET_ID::PARALLAX_LAVA);
	parallax_lava_2 = createParallaxItem(renderer, {-600, 813}, TEXTURE_ASSET_ID::PARALLAX_LAVA);
	parallax_lava_3 = createParallaxItem(renderer, {-1200, 813}, TEXTURE_ASSET_ID::PARALLAX_LAVA);
}

void WorldSystem::create_inGame_GUIs() {
	float heartPosition = HEART_START_POS;
	for (int i = 0; i < registry.players.get(player_hero).hp_max; i++) {
		player_hearts_GUI.push_back(createPlayerHeart(renderer, { heartPosition, HEART_Y_CORD }));
		heartPosition += HEART_GAP;
	}
	powerup_GUI = createPowerUpIcon(renderer, POWER_CORD);
	difficulty_bar = createDifficultyBar(renderer, DIFF_BAR_CORD);
	indicator = createDifficultyIndicator(renderer, INDICATOR_START_CORD);
	score_text = createScore(renderer, SCORE_CORD);
	float numberPosition = NUMBER_START_POS;
	for (int i = 0; i < 5; i++) {
		score_GUI.push_back(createNumber(renderer, { numberPosition, NUMBER_Y_CORD }));
		numberPosition += NUMBER_GAP;
	}
}

// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	
	// Loop over all collisions detected by the physics system
	auto &collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++)
	{
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other_entity;

		if (registry.players.has(entity))
		{
			Player& player = registry.players.get(entity);

			// Checking Player - Enemies collisions
			if ((
                    (registry.enemies.has(entity_other) && registry.enemies.get(entity_other).hitting) ||
                    (registry.weaponHitBoxes.has(entity_other) && registry.weaponHitBoxes.get(entity_other).isActive && registry.weaponHitBoxes.get(entity_other).hurtsHero) ||
                    registry.spitterBullets.has(entity_other)
                ) && registry.players.get(player_hero).invulnerable_timer <= 0.0f && !registry.gravities.get(player_hero).dashing)
			{
				// remove 1 hp
				player.hp -= 1;
				registry.players.get(player_hero).invulnerable_timer = max(3000.f, registry.players.get(player_hero).invulnerable_timer);
				registry.players.get(player_hero).invuln_type = INVULN_TYPE::HIT;
				play_sound(SOUND_EFFECT::HERO_DEAD);
				if (ddl < 4) ddf -= (player.hp_max - player.hp) * DDF_PUNISHMENT;

				if (registry.spitterBullets.has(entity_other))
					registry.remove_all_components_of(entity_other);

				// initiate death unless already dying
				if (player.hp == 0 && !registry.deathTimers.has(entity))
				{
					registry.deathTimers.emplace(entity);
					
					if (player.hasWeapon) {
						if (registry.grenadeLaunchers.has(player.weapon))
							for(Entity line: registry.grenadeLaunchers.get(player.weapon).trajectory)
								registry.remove_all_components_of(line);
						registry.remove_all_components_of(player.weapon);
					}
					player.hasWeapon = false;

					Motion &motion = registry.motions.get(player_hero);
					motion.angle = M_PI / 2;
					motion.velocity = vec2(0, 100);
					registry.colors.get(player_hero) = vec3(1, 0, 0);
				}
			}
			// Checking Player - Collectable collision
			else if (registry.collectables.has(entity_other))
			{
				if (!registry.deathTimers.has(entity) && pickupKeyStatus)
				{
					collect(entity_other, player_hero);
				}
			}
		}
		else if (registry.weaponHitBoxes.has(entity))
		{
			if ((registry.enemies.has(entity_other) && registry.enemies.get(entity_other).hittable) && registry.weaponHitBoxes.get(entity).isActive && registry.weaponHitBoxes.get(entity).hurtsEnemy)
			{
				if (registry.enemies.has(entity_other) && !registry.boulders.has(entity_other)) {
					//printf("Health: %d, Damage: %d\n", registry.enemies.get(entity_other).health, registry.weaponHitBoxes.get(entity).damage);
					Enemies& enemy = registry.enemies.get(entity_other);
					enemy.health -= registry.weaponHitBoxes.get(entity).damage;
					enemy.hittable = false;
					enemy.hitting = false;
					if (!registry.fireEnemies.has(entity_other))
						registry.motions.get(entity_other).dir = registry.motions.get(entity).position.x < registry.motions.get(entity_other).position.x ? -1 : 1;
					if (enemy.health <= 0) {
						points += 10;
						if (ddl != 4)
						{
							ddf += 5.f;
						}
						registry.animated.get(entity_other).oneTimeState = enemy.death_animation;
                        if (registry.boss.has(entity_other)) {
                            play_sound(SOUND_EFFECT::BOSS_DEATH);
                        }
					} else {
						registry.animated.get(entity_other).oneTimeState = enemy.hit_animation;
						registry.animated.get(entity_other).oneTimer = 0;
					}
				}
				
				if (registry.bullets.has(entity) || registry.rockets.has(entity) || registry.grenades.has(entity)) {
					if (registry.rockets.has(entity) || registry.grenades.has(entity))
						explode(renderer, registry.motions.get(entity).position, entity);
					registry.remove_all_components_of(entity);
				} else if (registry.explosions.has(entity) && registry.boulders.has(entity_other)) {
					registry.remove_all_components_of(entity_other);
				}
			} else if (registry.blocks.has(entity_other) && (registry.bullets.has(entity) || registry.rockets.has(entity))) {
				if (registry.rockets.has(entity))
					explode(renderer, registry.motions.get(entity).position, entity);
				registry.remove_all_components_of(entity);
			} else if (registry.swords.has(entity) && registry.spitterBullets.has(entity_other)) {
				registry.remove_all_components_of(entity_other);
			}
		}
		else if (registry.blocks.has(entity))
		{
			if (registry.solids.has(entity_other)) {
				Motion& block_motion = registry.motions.get(entity);
				Motion& solid_motion = registry.motions.get(entity_other);
				vec2 scale1 = vec2({abs(block_motion.scale.x), abs(block_motion.scale.y)}) / 2.f;
				vec2 scale2 = vec2({abs(solid_motion.scale.x), abs(solid_motion.scale.y)}) / 2.f;
				float vCollisionDepth = scale1.y + scale2.y - abs(block_motion.position.y - solid_motion.position.y);
				float hCollisionDepth = scale1.x + scale2.x - abs(block_motion.position.x - solid_motion.position.x);
				if (vCollisionDepth > 0 && (hCollisionDepth <= 0 || vCollisionDepth < hCollisionDepth)) {
					if (solid_motion.position.y < block_motion.position.y && (solid_motion.velocity.y > 0 || registry.waterBalls.has(entity_other))) {
						solid_motion.position.y = block_motion.position.y - scale1.y - scale2.y;
						solid_motion.velocity.y = 0;
					} else if (solid_motion.position.y > block_motion.position.y && (solid_motion.velocity.y < 0 || registry.waterBalls.has(entity_other))) {
						solid_motion.position.y = block_motion.position.y + scale1.y + scale2.y;
						solid_motion.velocity.y = 0;
					}
				} else {
					if (solid_motion.position.x < block_motion.position.x) {
						solid_motion.position.x = block_motion.position.x - scale1.x - scale2.x;
					} else {
						solid_motion.position.x = block_motion.position.x + scale1.x + scale2.x;
					}
				}

				if ((solid_motion.position.y <= block_motion.position.y - block_motion.scale.y / 2.f - solid_motion.scale.y / 2.f)) {
					if (registry.players.has(entity_other)) {
						registry.players.get(entity_other).jumps = MAX_JUMPS + (registry.players.get(entity_other).equipment_type == COLLECTABLE_TYPE::WINGED_BOOTS ? 1 : 0);
					} else if (registry.ghouls.has(entity_other) && registry.ghouls.get(entity_other).left_x == -1.f) {
						registry.animated.get(entity_other).oneTimeState = 5;
						registry.animated.get(entity_other).oneTimer = 0;
						registry.colors.insert(entity_other, {1, .8f, .8f});
						registry.ghouls.get(entity_other).left_x = block_motion.position.x - scale1.x;
						registry.ghouls.get(entity_other).right_x = block_motion.position.x + scale1.x;
					} else if (registry.spitterEnemies.has(entity_other) && registry.spitterEnemies.get(entity_other).left_x == -1.f) {
						//registry.colors.insert(entity_other, { 1, .8f, .8f });
						registry.animated.get(entity_other).oneTimeState = 3;
						registry.animated.get(entity_other).oneTimer = 0;
						registry.spitterEnemies.get(entity_other).left_x = max(block_motion.position.x - scale1.x, 70.f);
						registry.spitterEnemies.get(entity_other).right_x = min(block_motion.position.x + scale1.x, 1125.f);
					} else if (registry.waterBalls.has(entity_other)) {
						solid_motion.angle = M_PI/2;
						solid_motion.position.y = block_motion.position.y - scale1.y - scale2.x;
					}
				} else if (solid_motion.position.x <= block_motion.position.x - block_motion.scale.x / 2.f - solid_motion.scale.x / 2.f) {
					if (registry.players.has(entity_other) && motionKeyStatus.test(0) && registry.players.get(entity_other).equipment_type == COLLECTABLE_TYPE::PICKAXE && solid_motion.position.y > 0) {
						use_pickaxe(player_hero, 0, MAX_JUMPS);
					} else if (registry.ghouls.has(entity_other)) {
						registry.motions.get(entity_other).velocity.x = -registry.motions.get(entity_other).velocity.x;
						registry.motions.get(entity_other).dir = -registry.motions.get(entity_other).dir;
					} else if (registry.waterBalls.has(entity_other)) {
						solid_motion.angle = 0;
					}
				} else if (solid_motion.position.x >= block_motion.position.x + block_motion.scale.x / 2.f + solid_motion.scale.x / 2.f) {
					if (registry.players.has(entity_other) && motionKeyStatus.test(1) && registry.players.get(entity_other).equipment_type == COLLECTABLE_TYPE::PICKAXE && solid_motion.position.y > 0) {
						use_pickaxe(player_hero, 1, MAX_JUMPS);
					} else if (registry.ghouls.has(entity_other)) {
						registry.motions.get(entity_other).velocity.x = -registry.motions.get(entity_other).velocity.x;
						registry.motions.get(entity_other).dir = -registry.motions.get(entity_other).dir;
					} else if (registry.waterBalls.has(entity_other)) {
						solid_motion.angle = M_PI;
					}
				} else {
					if (registry.waterBalls.has(entity_other)) {
						solid_motion.angle = -M_PI/2;
						solid_motion.position.y = block_motion.position.y + scale1.y + scale2.x;
					}
				}

				if (registry.waterBalls.has(entity_other)) {
					registry.waterBalls.get(entity_other).drawing = false;
					registry.waterBalls.get(entity_other).state = -1;
					for (Entity line: registry.waterBalls.get(entity_other).trajectory)
						registry.remove_all_components_of(line);
					registry.animated.get(entity_other).oneTimeState = 2;
					registry.animated.get(entity_other).oneTimer = 0;
					registry.weaponHitBoxes.get(entity_other).isActive = false;
					solid_motion.velocity = {0, 0};
				}
			}
			else if (registry.projectiles.has(entity_other))
			{
				Motion& block_motion = registry.motions.get(entity);
				Motion& projectile_motion = registry.motions.get(entity_other);
				Projectile& projectile = registry.projectiles.get(entity_other);
				vec2 scale1 = vec2({abs(block_motion.scale.x), abs(block_motion.scale.y)}) / 2.f;
				vec2 scale2 = vec2({abs(projectile_motion.scale.x), abs(projectile_motion.scale.y)}) / 2.f;
				float vCollisionDepth = (scale1.y + scale2.y) - abs(projectile_motion.position.y - block_motion.position.y);
				float hCollisionDepth = (scale1.x + scale2.x) - abs(projectile_motion.position.x - block_motion.position.x);
				if (vCollisionDepth < hCollisionDepth) {
					projectile_motion.velocity.y = -projectile_motion.velocity.y;
					projectile_motion.position.y += vCollisionDepth * (projectile_motion.position.y < block_motion.position.y ? -1 : 1);
				} else {
					projectile_motion.velocity.x = -projectile_motion.velocity.x;
					projectile_motion.position.x += hCollisionDepth * (projectile_motion.position.x < block_motion.position.x ? -1 : 1);
				}
				projectile_motion.velocity = vec2(projectile_motion.velocity.x * projectile.friction_x, projectile_motion.velocity.y * projectile.friction_y);
			} 
		} else if (registry.parallaxBackgrounds.has(entity) && registry.renderRequests.get(entity).used_texture == TEXTURE_ASSET_ID::PARALLAX_LAVA) {
			if (registry.bullets.has(entity_other) || registry.rockets.has(entity_other) || registry.grenades.has(entity_other) || registry.spitterBullets.has(entity_other) || registry.collectables.has(entity_other) || registry.waterBalls.has(entity_other)) {
				if (registry.waterBalls.has(entity_other)) {
					registry.waterBalls.get(entity_other).drawing = false;
					for (Entity line: registry.waterBalls.get(entity_other).trajectory)
						registry.remove_all_components_of(line);
				}
				registry.remove_all_components_of(entity_other);
			} else if (registry.players.has(entity_other) && !registry.deathTimers.has(entity_other)) {
				// Scream, reset timer, and make the hero fall
				registry.deathTimers.emplace(entity_other);
				Player& player = registry.players.get(entity_other);
			
				if (player.hasWeapon) {
					if (registry.grenadeLaunchers.has(player.weapon))
						for(Entity line: registry.grenadeLaunchers.get(player.weapon).trajectory)
							registry.remove_all_components_of(line);
					registry.remove_all_components_of(player.weapon);
				}
				player.hasWeapon = false;
				player.invulnerable_timer = max(3000.f, registry.players.get(player_hero).invulnerable_timer);
				player.invuln_type = INVULN_TYPE::HIT;

				play_sound(SOUND_EFFECT::HERO_DEAD);
				Motion &motion = registry.motions.get(player_hero);
				motion.angle = M_PI / 2;
				motion.velocity = vec2(0, 100);
				registry.colors.get(player_hero) = vec3(1, 0, 0);
			} else if (registry.boulders.has(entity_other)) {
				registry.gravities.remove(entity_other);
				registry.motions.get(entity_other).velocity = {0, 50};
				registry.enemies.get(entity_other).hitting = false;
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::motion_helper(Motion &playerMotion)
{
	std::bitset<2>& lodged = registry.gravities.get(player_hero).lodged;
	float rightFactor = motionKeyStatus.test(0) && !lodged.test(0) && !lodged.test(1) ? 1 : 0;
	float leftFactor = motionKeyStatus.test(1) && !lodged.test(0) && !lodged.test(1) ? -1 : 0;
	playerMotion.velocity[0] = BASIC_SPEED * (rightFactor + leftFactor);
	if (!pause && dialogue_screen_active == 0) {
		if (playerMotion.velocity.x < 0)
			playerMotion.dir = -1;
		else if (playerMotion.velocity.x > 0)
			playerMotion.dir = 1;
	}
}

void WorldSystem::clear_enemies()
{
	std::vector<Entity> justKillThem = { };
	for (uint i = 0; i < registry.enemies.size(); i++)
	{
		Entity enemy = registry.enemies.entities[i];
		if (!(registry.followingEnemies.has(enemy) || registry.boss.has(enemy))) {
			justKillThem.push_back(registry.enemies.entities[i]);
		}
	}
	for (Entity e : justKillThem) registry.remove_all_components_of(e);
}

void WorldSystem::update_health_bar()
{
    for(Entity e : registry.healthBar.entities) {
        if (!registry.enemies.has(registry.healthBar.get(e).owner)) {
            registry.remove_all_components_of(registry.healthBar.get(e).bar);
            registry.remove_all_components_of(e);
			ddf = 500;
			should_score_prepare_to_show = true;
        }
    }
}


// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{
	if (isTitleScreen) {
		
		return;
	}
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        change_pause();
		play_sound(SOUND_EFFECT::BUTTON_CLICK);
    }

	if (key == GLFW_KEY_E && action == GLFW_PRESS && dialogue_screen_active != 0) {
		bool endlessStarted = dialogue_screen_active == 8;
		show_dialogue(0);
		if (endlessStarted) {
			show_dialogue(9);
		}
	}

	if (!registry.deathTimers.has(player_hero) && dialogue_screen_active == 0)
	{
		Motion &playerMotion = registry.motions.get(player_hero);

		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			if (registry.players.get(player_hero).equipment_type == COLLECTABLE_TYPE::DASH_BOOTS)
				check_dash_boots(player_hero, 0);
			motionKeyStatus.set(0);
		}
		else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			motionKeyStatus.reset(0);
			registry.gravities.get(player_hero).lodged.reset(0);
		}
		else if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			if (registry.players.get(player_hero).equipment_type == COLLECTABLE_TYPE::DASH_BOOTS)
				check_dash_boots(player_hero, 1);
			motionKeyStatus.set(1);
		}
		else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			motionKeyStatus.reset(1);
			registry.gravities.get(player_hero).lodged.reset(1);
		}

		if (!registry.gravities.get(player_hero).dashing)
			motion_helper(playerMotion);

		if ((key == GLFW_KEY_W || key == GLFW_KEY_SPACE) && action == GLFW_PRESS && !pause && !registry.gravities.get(player_hero).dashing)
		{
			if (registry.players.get(player_hero).jumps > 0)
			{
				playerMotion.velocity[1] = -JUMP_INITIAL_SPEED;
				registry.players.get(player_hero).jumps--;
				play_sound(SOUND_EFFECT::HERO_JUMP);
				if (registry.gravities.get(player_hero).lodged.test(0))
					disable_pickaxe(player_hero, 0, JUMP_INITIAL_SPEED / GRAVITY_ACCELERATION_FACTOR);
				else if (registry.gravities.get(player_hero).lodged.test(1))
					disable_pickaxe(player_hero, 1, JUMP_INITIAL_SPEED / GRAVITY_ACCELERATION_FACTOR);
			}
		} else if (key == GLFW_KEY_1 && action == GLFW_PRESS && !pause && debug) {
			if (mod == GLFW_MOD_SHIFT) {
                summon_fireling_helper(renderer);
			} else {
				createSword(renderer, registry.motions.get(player_hero).position);
			}
		} else if (key == GLFW_KEY_2 && action == GLFW_PRESS && !pause && debug) {
			if (mod == GLFW_MOD_SHIFT) {
                createGhoul(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::GHOUL_ENEMY)));
			} else {
				createGun(renderer, registry.motions.get(player_hero).position);
			}
		} else if (key == GLFW_KEY_3 && action == GLFW_PRESS && !pause && debug) {
			if (mod == GLFW_MOD_SHIFT) {
                createSpitterEnemy(renderer, getRandomWalkablePos(ASSET_SIZE.at(TEXTURE_ASSET_ID::SPITTER_ENEMY)));
			} else {
				createGrenadeLauncher(renderer, registry.motions.get(player_hero).position);
			}
		} else if (key == GLFW_KEY_4 && action == GLFW_PRESS && !pause && debug) {
			if (mod == GLFW_MOD_SHIFT) {
                summon_boulder_helper(renderer);
			} else {
				createRocketLauncher(renderer, registry.motions.get(player_hero).position);
			}
		} else if (key == GLFW_KEY_5 && action == GLFW_PRESS && !pause && debug) {
			createLaserRifle(renderer, registry.motions.get(player_hero).position);
		} else if (key == GLFW_KEY_6 && action == GLFW_PRESS && !pause && debug) {
			createTrident(renderer, registry.motions.get(player_hero).position);
		} else if (key == GLFW_KEY_7 && action == GLFW_PRESS && !pause && debug) {
			createHeart(renderer, registry.motions.get(player_hero).position);
		} else if (key == GLFW_KEY_8 && action == GLFW_PRESS && !pause && debug) {
			createWingedBoots(renderer, registry.motions.get(player_hero).position);
		} else if (key == GLFW_KEY_9 && action == GLFW_PRESS && !pause && debug) {
			createPickaxe(renderer, registry.motions.get(player_hero).position);
		} else if (key == GLFW_KEY_0 && action == GLFW_PRESS && !pause && debug) {
			createDashBoots(renderer, registry.motions.get(player_hero).position);
		}

		if (key == GLFW_KEY_I && action == GLFW_PRESS && debug)
		{
			if (registry.players.get(player_hero).invuln_type != INVULN_TYPE::HEAL)
			{
				registry.players.get(player_hero).invulnerable_timer = 12550821.f;
				registry.players.get(player_hero).invuln_type = INVULN_TYPE::HEAL;
			}
			else
			{
				registry.players.get(player_hero).invulnerable_timer = 0.f;
				registry.players.get(player_hero).invuln_type = INVULN_TYPE::NONE;
			}
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS && debug)
		{
            if (mod == GLFW_MOD_SHIFT) {
                registry.remove_all_components_of(boss);
            } else {
                clear_enemies();
            }
            update_health_bar();
		}

		if (key == GLFW_KEY_S && action == GLFW_PRESS && !pause && dialogue_screen_active == 0 && !registry.gravities.get(player_hero).dashing) {
			pickupKeyStatus = true;
		}

		if (key == GLFW_KEY_S && action == GLFW_RELEASE && !pause && dialogue_screen_active == 0 && !registry.gravities.get(player_hero).dashing) {
			pickupKeyStatus = false;
		}

		if (key == GLFW_KEY_C && action == GLFW_PRESS && debug) {
			show_dialogue(1);
		}

	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
        pause = false;
		dialogue_screen_active = 0;
		restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		debug = !debug;
	}
	
	if (key == GLFW_KEY_COMMA && action == GLFW_RELEASE && debug)
	{
		if (ddl < 4)
		{
			ddf = (ddl - 1) * 100;
		}
	}
	if (key == GLFW_KEY_PERIOD && action == GLFW_RELEASE && debug)
	{
		if (ddl != 4)
		{
			ddf = (ddl + 1) * 100;
		}
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS && debug && ddl == 4) {
		ddf = 500;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_M) {
		is_music_muted = !is_music_muted;
		set_mute_music(is_music_muted);
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    // Calculate the scaling factors for X and Y

    // Scale the mouse position
    float ox = 0, oy = 0;
	int new_w = w, new_h = h;

    float aspect_ratio = window_width_px / (float) window_height_px; // 16:9
    float new_aspect_ratio = w / (float) h;

    if (aspect_ratio < new_aspect_ratio) {
        new_w = h * aspect_ratio;
        ox = (w-new_w)/2.0;
        // w = new_w;
    } else {
        new_h = w / aspect_ratio;
        oy = (h-new_h) / 2.0;
        // h = new_h;
    }

	mouse_pos.x = (mouse_position.x - ox) / new_w * window_width_px;
    mouse_pos.y = (mouse_position.y - oy) / new_h * window_height_px;

	if (!registry.deathTimers.has(player_hero) && !pause && dialogue_screen_active == 0)
		for (Entity weapon : registry.weapons.entities)
			update_weapon_angle(renderer, weapon, mouse_pos, mouse_clicked);
}

void WorldSystem::on_mouse_click(int key, int action, int mods){
    if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		mouse_clicked = true;
		for (Entity entity: registry.buttons.entities) {
			Motion &button = registry.motions.get(entity);
        	GameButton &buttonInfo = registry.buttons.get(entity);
        	RenderRequest &buttonRender = registry.renderRequests.get(entity);
			if (abs(button.position.x - mouse_pos.x) < button.scale.x / 2 && abs(button.position.y - mouse_pos.y) < button.scale.y / 2 && buttonRender.visibility) {
				buttonInfo.clicked = true;
                play_sound(SOUND_EFFECT::BUTTON_CLICK);
				return;
			}
		}
		if (!pause && dialogue_screen_active == 0 && registry.players.size() > 0 && !registry.gravities.get(player_hero).dashing) {
			for (Entity weapon : registry.weapons.entities)
				do_weapon_action(renderer, weapon, mouse_pos);
		}
	} else if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		mouse_clicked = false;
		weapon_mouse_release();
		for (Entity entity: registry.buttons.entities) {
        	GameButton &buttonInfo = registry.buttons.get(entity);
			if (buttonInfo.clicked) {
				buttonInfo.clicked = false;
            	buttonInfo.callback();
				return;
			}
		}
	}
}

// pause/unpauses game and show pause screen entities
void WorldSystem::change_pause() {
    pause = !pause;
    for (Entity e : registry.showWhenPaused.entities) {
        registry.renderRequests.get(e).visibility = pause;
    }
}
