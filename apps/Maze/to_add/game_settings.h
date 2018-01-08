#pragma once
#ifndef __GAME_SETTINGS_H__
#define __GAME_SETTINGS_H__

#include "game_mode.h"
#include "game_camera_mode.h"

struct GameSettings {
	GameMode game_mode;
	GameCameraMode camera_mode;
	unsigned int num_players;
	unsigned int max_players;
};

#endif