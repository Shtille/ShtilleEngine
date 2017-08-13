#include "animation.h"

#include <assert.h>

AnimationController::AnimationController(JointPoseListenerInterface * listener)
: listener_(listener)
, clip_(nullptr)
, current_index_(0U)
, is_playing_(false)
{

}
AnimationController::~AnimationController()
{

}
void AnimationController::Update(float local_time)
{
	if (is_playing_)
	{
		// Calculate sample time
		float sample_time = local_time * clip_->playback_rate;
		if (clip_->is_looping && sample_time >= clip_->duration)
			sample_time -= clip_->duration;

		// Determine clip samples' indices
		size_t next_index = current_index_ + 1;
		assert(next_index < clip_->keys.size());
		float next_key_time = clip_->keys[next_index].time;
		while (sample_time >= next_key_time)
		{
			++current_index_;
			++next_index;
			assert(next_index < clip_->keys.size());
			next_key_time = clip_->keys[next_index].time;
		}
		if (current_index_ == clip_->keys.size())
			current_index_ = 0U;

		// Interpolate poses
		const AnimationKey& key0 = clip_->keys[current_index_];
		const AnimationKey& key1 = clip_->keys[next_index];
		const float rate = (sample_time - key0.time) / (key1.time - key0.time);
		const float one_minus_rate = 1.0f - rate;
		const JointPose& pose0 = key0.pose;
		const JointPose& pose1 = key1.pose;
		if (clip_->flags & (unsigned short)AnimationClip::Flags::kRotateFlag)
			sht::math::Quaternion::Slerp(pose0.rotation, pose1.rotation, rate, &current_pose_.rotation);
		if (clip_->flags & (unsigned short)AnimationClip::Flags::kTranslateFlag)
			current_pose_.position = one_minus_rate * pose0.position + rate * pose1.position;
		if (clip_->flags & (unsigned short)AnimationClip::Flags::kScaleFlag)
			current_pose_.scale = one_minus_rate * pose0.scale + rate * pose1.scale;

		// Notify listener in the end
		if (listener_)
			listener_->OnChange(&current_pose_);
	}
}
void AnimationController::SetClip(const AnimationClip * clip)
{
	clip_ = clip;
}
void AnimationController::Play()
{
	if (clip_)
		is_playing_ = true;
}
void AnimationController::Stop()
{
	is_playing_ = false;
}
bool AnimationController::IsPlaying() const
{
	return is_playing_;
}