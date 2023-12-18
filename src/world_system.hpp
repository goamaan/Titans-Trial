#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>
#include <map>

// #define SDL_MAIN_HANDLED
// #include <SDL.h>
// #include <SDL_mixer.h>

#include "render_system.hpp"
#include "sound_utils.hpp"
#include "weapon_utils.hpp"
#include "ai_system.hpp"
#include "enemy_utils.hpp"
// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods



// Render configs
const float ANIMATION_SPEED_FACTOR = 10.0f;

// Game configuration
static float spitter_projectile_delay_ms = 5000.f;
static float spawn_delay_variance = 0.6;
static size_t spawn_delay = 6000;
const size_t BOSS_MAX_GHOULS = 14;
const size_t BOSS_MAX_SPITTERS = 8;
const float ENEMY_INVULNERABILITY_TIME = 500.f;
const int BOSS_HEALTH = 300;
const std::vector<size_t> BOSS_ACTION_COOLDOWNS = {12000, 2000, 70000, 10000, 2000, 5000};
const float INITIAL_SPITTER_PROJECTILE_DELAY_MS = 1000.f;
const float SPITTER_PROJECTILE_REDUCTION_FACTOR = 5000.f;
const float SPITTER_PROJECTILE_MIN_SIZE = 0.3f;
const uint MAX_JUMPS = 2;
const float BASIC_SPEED = 220.f;
const float JUMP_INITIAL_SPEED = 370.f;
const int ENEMY_SPAWN_HEIGHT_IDLE_RANGE = 50;
const float DDF_PUNISHMENT = 10.f;
const float HEART_START_POS = 70.f;
const float HEART_GAP = 35.f;
const float HEART_Y_CORD = 20.f;
const vec2 POWER_CORD = { 20.f, 60.f };
const vec2 DIFF_BAR_CORD = { 140.f, 750.f };
const vec2 DB_BOSS_CORD = { 140.f, 717.5f };
const vec2 INDICATOR_START_CORD = { 35.f, 710.f };
const float INDICATOR_VELOCITY = 55.f / 100.f;
const vec2 SCORE_CORD = { 1050.f, 700.f };
const float NUMBER_START_POS = 992.f;
const float NUMBER_GAP = 29.f;
const float NUMBER_Y_CORD = 740.f;
const vec2 DB_SATAN_CORD = { 140.f, 725.f };
const float LAVA_PILLAR_SPAWN_DELAY = 15000.f;
const uint MDP_HORIZON = 2;
const float MDP_DISCOUNT_FACTOR = 0.9f;
const float MDP_BASE_REWARD = 100;

enum SpawnableEnemyType {
        FIRELINGS = 0,
        GHOULS = FIRELINGS + 1,
        SPITTERS = GHOULS + 1,
        BOULDERS = SPITTERS + 1,
        ENEMY_COUNT = BOULDERS + 1
};

static std::vector<int> max_spawns;

static std::vector<float> spawn_prob;

class WorldSystem
{
public:
	WorldSystem();
    // somehow pause it activated once
    static bool pause;
    static bool debug;
	static bool mouse_clicked;
	static bool isTitleScreen;
	static int dialogue_screen_active;

	// Creates a window
	GLFWwindow *create_window();

	// starts the game
	void init(RenderSystem *renderer);
	
	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms_since_last_update);
  
	void changeScore(int score);

	TEXTURE_ASSET_ID connectNumber(int digit);

	void show_dialogue(int dialogue_number);
	TEXTURE_ASSET_ID connectDialogue(int digit);

	SOUND_EFFECT effect_to_play(int dialogue_number);

	void update_graphics_all_enemies();

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over() const;

    static void change_pause();


private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
    void on_mouse_click(int button, int action, int mods);

	void clear_enemies();

    static void update_health_bar();

	void motion_helper(Motion& playerMotion);

	// restart level
	void restart_game();

	void save_game();

	void load_game();

	int save_weapon(Entity weapon);

    // creates pause gui
    void create_pause_screen();
	void create_title_screen();
	void create_almanac_screen();
	void create_inGame_GUIs();
	// OpenGL window handle
	GLFWwindow *window;

	// Number of enemies killed, displayed in the window title
	unsigned int points;
	
	// backgrounds
    void create_parallax_background();
	Entity parallax_background;
	Entity parallax_background_color;
	Entity parallax_clouds_far_1;
	Entity parallax_clouds_far_2;
	Entity parallax_clouds_close_1;
	Entity parallax_clouds_close_2;
	Entity parallax_rain_1;
	Entity parallax_rain_2;
	Entity parallax_rain_3;
	Entity parallax_rain_4;
	Entity parallax_moon;
	Entity parallax_lava_1;
	Entity parallax_lava_2;
	Entity parallax_lava_3;

	// Game state
	RenderSystem *renderer;
	Entity player_hero;
    Entity boss;
};
