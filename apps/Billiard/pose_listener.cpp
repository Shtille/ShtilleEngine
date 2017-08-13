#include "pose_listener.h"

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
	sht::math::Matrix4 pose_matrix;
	pose_matrix.Identity();
	pose_matrix.sa[12] = pose->position.x;
	pose_matrix.sa[13] = pose->position.y;
	pose_matrix.sa[14] = pose->position.z;

	world_matrix_ = local_to_world_matrix_ * pose_matrix;
}