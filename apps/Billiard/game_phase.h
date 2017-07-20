#pragma once
#ifndef __GAME_PHASE_H__
#define __GAME_PHASE_H__

enum class GamePhase : int {
	kRackRemoving,
	kCueTargeting,
	kBallsRolling,	// main game phase
};

#endif