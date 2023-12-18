# Team17-DefiantMortals-TitansTrial

Titan's Trial - A 2D Endless survival game

## Attributions

Please see [ATTRIBUTIONS.MD](ATTRIBUTIONS.md).

## Write-up Milestone 4 (Final Game)

Changes from M3_Proposal can be seen highlighted in green in the M4_Proposal document. The additions/changes include:

* Story Elements
* Reloadability
* Advanced AI
* Complex prescribed motion
* Precise collision
* Game balance (read below)

### A note on user interaction feedback

* Make sword always active or give it an attack animation
  * Decided not to because it's part of the functionality
* Create a super rare powerup to deal with the following enemy
  * Decided not to because it needs to be a constant nuisance throughout the level
* Lower attack cooldowns since higher levels are very difficult
  * More and better (powerful) weapons. Lower cooldowns, and higher damage
* Visual change to the background/stage to indicate difficulty change
  * Level indicator at bottom left and there's an overlay before endless mode
* Have some sort of end goal
  * Boss + endless for people who can beat the boss
* More enemies
  * Added boss + ghoul + boulder. Spitter was changed
* Make the sword better
  * Does more damage now (as it it more dangerous to use)
* Define the purpose of the score
  * Remove score until you reach the endless level
* Make grenade launcher shoot multiple grenades (or otherwise buff it since it's the slowest weapon to use)
  * Increased damage

### A note on game balance

The game is based on a dynamic difficulty system, and a new run always starts at the lowest difficulty. Player death is permanent and results in starting from difficulty 1, as inspired by roguelike games. Difficulty increases through levels 1-4, followed by the boss level, and then an endless mode that is much harder. This is how we take care of balancing difficulty through enemies and weapons:

* Dynamic Difficulty Factor & Level
  * Enemy spawning frequency, kinds and numbers
  * Weapon spawning kinds and probability
  * Spitter shoots faster at higher difficulty

* Variable Enemy Health Value
  * Ranged enemy and combat enemy have different hp
  * Boulders can only be dstroyed by explosion (but with only one explosion)

* Variable Weapon Advantages
  * Weapons of mass destruction have longer cooldown time and higher damage (chaos orb grenade and orb rocket launcher)
  * Sword has the highest damage (due to high risk)
  * Weapons that go through platforms have lowest damage (beam staff)

* Boss Status
  * Boss has the highest HP
  * Boss can only be damaged by explosion
  * Boss have variable ways to attack and defence itself

M4 Features:

* Story Elements
  * world_init.cpp:
    * func createDialogue
  * render_system.cpp:
    * func drawDialogueLayer
  * world_system.cpp:
    * func connectDialogue
    * func show_dialogue
    * func effect_to_play
    * func step: - lines 295 - 344
  * physics_system.cpp:
    * func step - lines 151 - 172
* Precise Collision
  * physics_system.cpp - lines 19-123
* Complex prescribed motion
  * weapon_utils.cpp - lines 275 - 370 and 635-657
* Game Balance
  * enemy_utils.cpp
    * func adjust_difficulty
    * func do_enemy_spawn
  * enemy_utils.hpp
  * weapon_utils.cpp
    * func spawn_weapon
* Advanced AI (MDP)
  * enemy_utils.cpp - lines 514-651
* Reloadability
  * world_system.cpp
    * func save_game
    * func save_weapon
    * func load_game
    * step - lines 290-363
* Boss
  * enemy_utils.cpp - lines 322 - 472

## Write-up Milestone 3

Changes from M2_Proposal can be seen highlighted in green in the M3_Proposal document. The changes include:

* Additional implementations
  * GUI for game data including current difficulty, player health, and score
  * Seperate out hitbox and sprite systems
  * Consistent game resolution (can resize, opens as windowed fullscreen)

## M3 Features

* Complete GUI
  * world_init.cpp:
    * func createPlayerHeart
    * func createPowerUpIcon
    * func createDifficultyBar
    * func createDifficultyIndicator
    * func createScore
    * func createNumber
  * world_system.cpp:
    * func create_inGame_GUIs
    * func changeScore
    * func connectNumber
    * func step - lines 202-261
* More Weapons and Powerups
  * world_init.cpp:
    * func createRocketLauncher
    * func createRocket
    * func createGrenadeLauncher
    * func createGrenade
    * func createExplosion
    * func createHeart
    * func createPickaxe
    * func createWingedBoots
    * func createDashBoots
  * world_system.cpp:
    * func on_mouse_click
  * weapon_utils.cpp:
    * everything (refactored)
* New Path Finding Enemy
  * world_init.cpp:
    * func createFollowingEnemy
  * world_system.cpp:
    * func move_tracer
  * ai_system.cpp:
    * everything
* Parallax Background
  * world_init.cpp:
    * func createParallaxItem
  * world_system.cpp:
    * func create_parallax_background
* Powerful Debugging System
  * world_system.cpp:
    * func on_key - lines 1002-1045, 1066-1080
* Total Collision and World System Refactoring
  * world_system.hpp:
    * everything
  * world_system.cpp:
    * func handle_collision
  * physics_system.cpp:
    * everything
* Hitbox/sprite refactoring (smaller changes to most systems)
  * world_init.hpp - lines 28-76
* Complete Tutorial
  * world_system.cpp:
    * create_almanac_screen
  * world_init.cpp:
    * createTooltip
* Different Resolutions
  * world_system.cpp:
    * create_window: lines 101-104
    * on_mouse_move
  * render_system.cpp:
    * drawToScreen: lines 250-265
    * drawScreenLayer

## Write-up Milestone 2

Changes from M1_Proposal_V2 can be seen highlighted in green in the M2_Proposal document. The changes include:

* Instead of adding a boulder as an enemy we decided to make the spitter which shoots a bouncing projectile
* Instead of adding simple path finding, we started working on that with the intention of completing it in milestone 3
  * For our creative component, we pivoted to simple rendering effects, including a screen darkening effect on pause and a pulse effect when the hero is hit
* Early and additional implementations
  * Title screen
  * Simple path finding was started
  
## M2 Features

* Button System
  * world_init.cpp:
    * func createButton
  * render_system.cpp:
    * func drawTextureMesh - lines 82-90
  * world_system.cpp:
    * func on_mouse_click
  * button.fs.glsl
  * button.vs.glsl
* Pause System
  * world_init.cpp:
    * func createHelperText
  * world_system.cpp:
    * func change_pause
  * main.cpp:
    * func main - lines 43-53
* screen.fs.glsl
* screen.vs.glsl
* Spitter Enemy
  * world_system.cpp:
    * func small_spitter_enemy
  * world_init.cpp:
    * func createSpitterEnemy
    * func createSpitterEnemyBullet
  * physics_system.cpp:
    * func step - lines 207-246
* Main Menu
  * world_init.cpp:
    * func createTitleText
  * world_system.cpp:
    * func create_help_screen
    * func create_title_screen
* Projectile Weapon:
  * weapon_utils.cpp
    * everything
  * physics_system.cpp
    * func collides - lines 17-49
* Dynamic Difficulty System
  * world_system.cpp:
    * func step - lines 172-200 268-282
* Health Bar + Invulnerability
  * world_system.cpp:
    * func handle_collision - lines 586-612
  * render_system.cpp:
    * drawTextureMesh - lines 68-73
  * hero.fv.glsl

---

## Write-up Milestone 1

There were few changes that we had to implement to our previous proposal, all changes can be seen highlighted in green in the M1_Proposal_V2 document. The changes include:

* Clarification on enemy - player collision
* Early implementations
  * Mouse + space for weapon use forgoing arrow keys
  * Some improved movement options (double jump + wall jump)
    * To allow better traversal of the level
  * Rudementary scoring system
* Added creative elements
  * Player movement animations
  * Audio Feedback

## Features

* Map asset
  * data/textures/background.png
  * world_init.cpp :
    * func createBackground
* Collision with platform and Boundary
  * physics_system.cpp :
    * func collides
* Map collision placement
  * world_init.cpp :
    * function createBlock
  * world_system.cpp :
    * func restart_game - lines 410-440
  * render_system.cpp :
    * func drawTexturedMesh - lines 78-88
* Player movement
  * world_system.cpp :
    * func on_key - lines 552-589
    * func motion_helper
* Player gravity
  * physics_system.cpp :
    * func step - lines 86-89
* Double Jump + Wall Jump
  * world_system.cpp:
    * func on_key - lines 581-589
    * func handle_collisions - lines 490-498
* Random Movement enemy
  * world_init.cpp :
    * func createEnemy
  * world_system.cpp :
    * func step - lines 283-333
* Random sword spawn
  * world_system.cpp :
    * func step - lines 335-345
    * func handle_collisions - lines 500-517
* Sword and collision with enemy
  * world_system.cpp :
    * func on_key - lines 591-602
    * func handle_collisions - lines 518-528
    * func on_mouse_move
    * func step - lines 200-263
* Sprite Animation
  * render_system.cpp :
    * func drawTexturedMesh - line 59-66
  * world_init.cpp :
    * func createHero - lines 21-24
  * world_system.cpp:
    * func step - lines 265-281
  * animated.fs.glsl
  * animated.vs.glsl
