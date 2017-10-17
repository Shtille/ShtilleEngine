#pragma once
#ifndef __GAME_CAMERA_MODE_H__
#define __GAME_CAMERA_MODE_H__

enum class GameCameraMode : unsigned short {
	kOverview, // from above
	kAttached // camera locked with cue
};

#endif