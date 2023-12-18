#pragma once

#include <iostream>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "common.hpp"

#ifndef SOUND_UTILS_HPP
#define SOUND_UTILS_HPP

extern bool is_music_muted;

#endif

uint init_sound();
void destroy_sound();


enum class SOUND_EFFECT {
    HERO_DEAD = 0,
    HERO_JUMP = HERO_DEAD + 1,
    SWORD_SWING = HERO_JUMP + 1,
    BULLET_SHOOT = SWORD_SWING + 1,
    GUN_LEVER = BULLET_SHOOT + 1,
    ROCKET_LAUNCHER_FIRE = GUN_LEVER + 1,
    ROCKET_LAUNCHER_RELOAD = ROCKET_LAUNCHER_FIRE + 1,
    GRENADE_LAUNCHER_FIRE = ROCKET_LAUNCHER_RELOAD + 1,
    GRENADE_LAUNCHER_RELOAD = GRENADE_LAUNCHER_FIRE + 1,
    EXPLOSION = GRENADE_LAUNCHER_RELOAD + 1,
    LASER_RIFLE_FIRE = EXPLOSION + 1,
    LASER_RIFLE_RELOAD = LASER_RIFLE_FIRE + 1,
    HEAL = LASER_RIFLE_RELOAD + 1,
    PICKAXE = HEAL + 1,
    DASH = PICKAXE + 1,
    EQUIPMENT_DROP = DASH + 1,
    BUTTON_CLICK = EQUIPMENT_DROP + 1,
    TELEPORT = BUTTON_CLICK + 1,
    LAUGH = TELEPORT + 1,
    WATER_BALL_SHOOT = LAUGH + 1,
    BOSS_SLASH = WATER_BALL_SHOOT + 1,
    BOSS_TP = BOSS_SLASH + 1,
    BOSS_SUMMON = BOSS_TP + 1,
    BOSS_DEATH = BOSS_SUMMON + 1,
    BELL = BOSS_DEATH + 1
};

void play_main_menu_music();
void play_music();
void play_sound(SOUND_EFFECT id);
void set_mute_music(bool mute);
void play_dialogue_music();
void stop_dialogue_music();