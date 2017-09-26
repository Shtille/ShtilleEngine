#pragma once
#ifndef __GAME_SETTINGS_H__
#define __GAME_SETTINGS_H__

#include "game_mode.h"

struct GameSettings {
	GameMode game_mode;
	unsigned int num_players;
};

#endif