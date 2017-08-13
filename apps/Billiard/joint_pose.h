#pragma once
#ifndef __JOINT_POSE_H__
#define __JOINT_POSE_H__

#include "math/vector.h"
#include "math/quaternion.h"

struct JointPose {
	sht::math::Quaternion rotation;
	sht::math::Vector3 position;
	float scale;
};

class JointPoseListenerInterface {
public:
	virtual void OnChange(const JointPose * pose) = 0;
};

#endif