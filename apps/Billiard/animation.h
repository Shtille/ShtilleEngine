#pragma once
#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "joint_pose.h"

#include <vector>

struct AnimationKey {
	float time;
	JointPose pose;
};

struct AnimationClip {
	enum class Flags : unsigned short {
		kNoFlags = 0,
		kRotateFlag = 1,
		kTranslateFlag = 2,
		kScaleFlag = 4
	};
	std::vector<AnimationKey> keys;
	float duration;
	float playback_rate;
	unsigned short flags;
	bool is_looping;
};

class AnimationController {
public:
	AnimationController(JointPoseListenerInterface * listener);
	~AnimationController();

	void Update(float local_time);

	void SetClip(const AnimationClip * clip);

	void Play();
	void Stop();
	bool IsPlaying() const;

private:
	JointPoseListenerInterface * listener_;
	const AnimationClip * clip_;
	JointPose current_pose_;
	size_t current_index_;
	bool is_playing_;
};

#endif