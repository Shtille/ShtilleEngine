#include "pose_listener.h"

#include "math/sht_math.h"

PoseListener::PoseListener()
{
	local_to_world_matrix_.Identity();
	world_matrix_.Identity();
}
void PoseListener::SetLocalToWorldMatrix(const sht::math::Matrix4& local_to_world_matrix)
{
	local_to_world_matrix_ = local_to_world_matrix;
	world_matrix_ = local_to_world_matrix_;
}
const sht::math::Matrix4& PoseListener::world_matrix() const
{
	return world_matrix_;
}
void PoseListener::OnChange(const JointPose * pose)
{
	sht::math::Matrix3 rotation_matrix;
	sht::math::Matrix4 pose_matrix;

	sht::math::QuaternionToMatrix(rotation_matrix, pose->rotation);

	pose_matrix = sht::math::OrientationMatrix(rotation_matrix, pose->position);

	world_matrix_ = local_to_world_matrix_ * pose_matrix;
}