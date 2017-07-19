#pragma once
#ifndef __GAME_MODE_H__
#define __GAME_MODE_H__

enum class GameMode : int {
	kSimplePool,	// 14 red balls and 1 black ball
	kPool,			// 7 filled balls, 7 stripped balls and 1 black ball
	kSnooker		//
};

#endif