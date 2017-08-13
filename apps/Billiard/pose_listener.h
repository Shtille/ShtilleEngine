#pragma once
#ifndef __POSE_LISTENER_H__
#define __POSE_LISTENER_H__

#include "joint_pose.h"

#include "math/matrix.h"

class PoseListener : protected JointPoseListenerInterface {
public:
	PoseListener();

	void SetLocalToWorldMatrix(const sht::math::Matrix4& world_to_local_matrix);

	const sht::math::Matrix4& world_matrix() const;

private:
	void OnChange(const JointPose * pose) override;

	sht::math::Matrix4 local_to_world_matrix_;
	sht::math::Matrix4 world_matrix_;
};

#endif