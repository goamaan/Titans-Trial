#include "sound_utils.hpp"

// music references
Mix_Music *background_music;
std::vector<Mix_Chunk *> sound_effects;
Mix_Music *dialogue_background_music;
Mix_Music *main_menu_background_music;

bool is_music_muted;

uint init_sound()
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return 1;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return 1;
	}
	Mix_VolumeMusic(60);
	Mix_Volume(-1, 60);

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	dialogue_background_music = Mix_LoadMUS(audio_path("dialogue_bg_music.wav").c_str());
	main_menu_background_music = Mix_LoadMUS(audio_path("main_menu_bg_music.wav").c_str());
	sound_effects.push_back(Mix_LoadWAV(audio_path("hero_hurt.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("hero_jump.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("sword_swing.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("bow_shoot.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("bow_loading.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("staff_fire.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("recharge.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("swoosh.wav").c_str()));
	//Sound Effect by <a href="https://pixabay.com/users/jigokukarano_sisya-39731529/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=168857">jigokukarano_sisya</a> from <a href="https://pixabay.com/sound-effects//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=168857">Pixabay</a>
	sound_effects.push_back(Mix_LoadWAV(audio_path("charge.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("explosion.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("laser_fire.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("laser_reload.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("heal.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("pickaxe.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("dash.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("equipment_drop.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("button_click.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("teleport.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("hades_laugh.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("water_ball_shoot.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("boss_slam.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("boss_teleport.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("boss_summon.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("boss_death.wav").c_str()));
	sound_effects.push_back(Mix_LoadWAV(audio_path("bell.wav").c_str()));

	if (background_music == nullptr || dialogue_background_music == nullptr || std::any_of(sound_effects.begin(), sound_effects.end(), [](Mix_Chunk *effect)
												   { return effect == nullptr; }))
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
				audio_path("music.wav").c_str(),
				audio_path("hero_hurt.wav").c_str(),
				audio_path("sword_swing.wav").c_str(),
				audio_path("hero_jump.wav").c_str(),
				audio_path("bullet_shoot.wav").c_str(),
				audio_path("gun_lever.wav").c_str(),
				audio_path("rocket_launcher_fire.wav").c_str(),
				audio_path("rocket_launcher_reload.wav").c_str(),
				audio_path("grenade_launcher_fire.wav").c_str(),
				audio_path("grenade_launcher_reload.wav").c_str(),
				audio_path("explosion.wav").c_str(),
				audio_path("laser_rifle_fire.wav").c_str(),
				audio_path("laser_rifle_reload.wav").c_str(),
				audio_path("heal.wav").c_str(),
				audio_path("pickaxe.wav").c_str(),
				audio_path("dash.wav").c_str(),
				audio_path("equipment_drop.wav").c_str(),
				audio_path("button_click.wav").c_str(),
				audio_path("button_click.wav").c_str(),
				audio_path("teleport.wav").c_str(),
				audio_path("hades_laugh.wav").c_str(),
				audio_path("water_ball_shoot.wav").c_str());
		return 1;
	}

	return 0;
}

void destroy_sound()
{
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (dialogue_background_music != nullptr)
		Mix_FreeMusic(dialogue_background_music);
	for (Mix_Chunk *effect : sound_effects)
	{
		if (effect != nullptr)
		{
			Mix_FreeChunk(effect);
		}
	}
	Mix_CloseAudio();
}

void play_main_menu_music() {
	Mix_PlayMusic(main_menu_background_music, -1);
	fprintf(stderr, "Loaded main menu music\n");
}

void play_music()
{
	Mix_FadeOutMusic(300);
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");
}

void set_mute_music(bool muted)
{
	if (muted)
	{
		Mix_VolumeMusic(0);
	}
	else
	{
		Mix_VolumeMusic(60);
	}
}

void play_sound(SOUND_EFFECT id)
{
	Mix_PlayChannel(-1, sound_effects[(uint)id], 0);
}

void play_dialogue_music() {
	Mix_FadeOutMusic(300);
	Mix_PlayMusic(dialogue_background_music, -1);
}

void stop_dialogue_music() {
	Mix_FadeOutMusic(100);
	play_music();
}