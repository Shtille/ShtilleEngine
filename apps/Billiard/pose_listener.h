#pragma once
#ifndef __POSE_LISTENER_H__
#define __POSE_LISTENER_H__

#include "joint_pose.h"

#include "math/matrix.h"

class PoseListener : public JointPoseListenerInterface {
public:
	PoseListener();

	void SetLocalToWorldMatrix(const sht::math::Matrix4& local_to_world_matrix);

	const sht::math::Matrix4& world_matrix() const;

private:
	void OnChange(const JointPose * pose) override;

	sht::math::Matrix4 local_to_world_matrix_;
	sht::math::Matrix4 world_matrix_;
};

#endif