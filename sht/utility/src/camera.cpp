#include "../include/camera.h"

#include <assert.h>

namespace sht {
	namespace utility {
	
		Camera::Camera(const vec3& pos, const vec3& target_pos)
		{
			is_position_ = true;
			position_ = new vec3(pos);
			is_target_position_ = true;
			target_position_ = new vec3(target_pos);
			is_orientation_ = true;
			orientation_ = new quat(pos, target_pos);
			need_update_orientation_ = false;
		}
		Camera::Camera(vec3 * pos, const vec3& target_pos)
		{
			is_position_ = false;
			position_ = pos;
			is_target_position_ = true;
			target_position_ = new vec3(target_pos);
			is_orientation_ = true;
			orientation_ = new quat(*pos, target_pos);
			need_update_orientation_ = true;
		}
		Camera::Camera(const vec3& pos, vec3 * target_pos)
		{
			is_position_ = true;
			position_ = new vec3(pos);
			is_target_position_ = false;
			target_position_ = target_pos;
			is_orientation_ = true;
			orientation_ = new quat(pos, *target_pos);
			need_update_orientation_ = true;
		}
		Camera::Camera(vec3 * pos, vec3 * target_pos)
		{
			is_position_ = false;
			position_ = pos;
			is_target_position_ = false;
			target_position_ = target_pos;
			is_orientation_ = true;
			orientation_ = new quat(*pos, *target_pos);
			need_update_orientation_ = true;
		}
		Camera::Camera(const vec3& pos, const quat& orient)
		{
			is_position_ = true;
			position_ = new vec3(pos);
			is_target_position_ = false;
			target_position_ = nullptr;
			is_orientation_ = true;
			orientation_ = new quat(orient);
			need_update_orientation_ = false;
		}
		Camera::Camera(vec3 * pos, const quat& orient)
		{
			is_position_ = false;
			position_ = pos;
			is_target_position_ = false;
			target_position_ = nullptr;
			is_orientation_ = true;
			orientation_ = new quat(orient);
			need_update_orientation_ = false;
		}
		Camera::Camera(const vec3& pos, quat * orient)
		{
			is_position_ = true;
			position_ = new vec3(pos);
			is_target_position_ = false;
			target_position_ = nullptr;
			is_orientation_ = false;
			orientation_ = orient;
			need_update_orientation_ = false;
		}
		Camera::Camera(vec3 * pos, quat * orient)
		{
			is_position_ = false;
			position_ = pos;
			is_target_position_ = false;
			target_position_ = nullptr;
			is_orientation_ = false;
			orientation_ = orient;
			need_update_orientation_ = false;
		}
		Camera::Camera(const Camera& camera)
		{
			(void)operator =(camera);
		}
		Camera::~Camera()
		{
			if (is_position_)
				delete position_;
			if (is_target_position_)
				delete target_position_;
			if (is_orientation_)
				delete orientation_;
		}
		Camera& Camera::operator =(const Camera& camera)
		{
			need_update_orientation_ = camera.need_update_orientation_;
			is_position_ = camera.is_position_;
			is_target_position_ = camera.is_target_position_;
			is_orientation_ = camera.is_orientation_;
			if (is_position_)
				position_ = new vec3(*camera.position_);
			else
				position_ = camera.position_;
			if (is_target_position_)
				target_position_ = new vec3(*camera.target_position_);
			else
				target_position_ = camera.target_position_;
			if (is_orientation_)
				orientation_ = new quat(*camera.orientation_);
			else
				orientation_ = camera.orientation_;
			return *this;
		}
		void Camera::Move(const vec3& translation)
		{
			if (is_position_)
			{
				*position_ += translation;
				Update();
			}
		}
		void Camera::Update()
		{
			if (!need_update_orientation_)
				return;

			orientation_->Set(*position_, *target_position_);
		}
		CameraManager::CameraManager()
		: current_camera_(nullptr)
		, current_path_index_(0)
		, animation_time_(0.0f)
		, is_current_(false)
		, is_path_cycled_(false)
		, need_view_matrix_update_(false)
		, manual_rotation_(false)
		{
		}
		CameraManager::~CameraManager()
		{
			if (is_current_)
				delete current_camera_;
			Clear();
		}
		const math::Matrix4& CameraManager::view_matrix() const
		{
			return view_matrix_;
		}
		void CameraManager::RotateAroundX(f32 angle)
		{
			quat orient(UNIT_X, angle);
			*current_camera_->orientation_ = *current_camera_->orientation_ * orient;
            current_camera_->orientation_->Normalize();
			manual_rotation_ = true;
		}
		void CameraManager::RotateAroundY(f32 angle)
		{
			quat orient(UNIT_Y, angle);
            *current_camera_->orientation_ = *current_camera_->orientation_ * orient;
            current_camera_->orientation_->Normalize();
			manual_rotation_ = true;
		}
		void CameraManager::RotateAroundZ(f32 angle)
		{
			quat orient(UNIT_Z, angle);
			*current_camera_->orientation_ = *current_camera_->orientation_ * orient;
            current_camera_->orientation_->Normalize();
			manual_rotation_ = true;
		}
        void CameraManager::RotateAroundTargetInX(f32 angle)
        {
            assert(current_camera_->target_position_);
            assert(current_camera_->is_position_);
            
            RotateAroundX(-angle);
            f32 distance = (*current_camera_->target_position_ - *current_camera_->position_).Length();
            *current_camera_->position_ = *current_camera_->target_position_
            - distance * current_camera_->orientation_->Direction();
        }
        void CameraManager::RotateAroundTargetInY(f32 angle)
        {
            assert(current_camera_->target_position_);
            assert(current_camera_->is_position_);
            
            RotateAroundY(-angle);
            f32 distance = (*current_camera_->target_position_ - *current_camera_->position_).Length();
            *current_camera_->position_ = *current_camera_->target_position_
                - distance * current_camera_->orientation_->Direction();
        }
        void CameraManager::RotateAroundTargetInZ(f32 angle)
        {
            assert(current_camera_->target_position_);
            assert(current_camera_->is_position_);
            
            RotateAroundZ(-angle);
            f32 distance = (*current_camera_->target_position_ - *current_camera_->position_).Length();
            *current_camera_->position_ = *current_camera_->target_position_
            - distance * current_camera_->orientation_->Direction();
        }
		void CameraManager::Move(const vec3& translation)
		{
			if (current_camera_)
				current_camera_->Move(translation);
		}
		void CameraManager::MakeFree(const vec3& pos, const vec3& target_pos)
		{
			if (is_current_)
				delete current_camera_;
			is_current_ = true;
			current_camera_ = new Camera(pos, target_pos);
            manual_rotation_ = true;
		}
		void CameraManager::MakeFree(const vec3& pos, const quat& orient)
		{
			if (is_current_)
				delete current_camera_;
			is_current_ = true;
			current_camera_ = new Camera(pos, orient);
            manual_rotation_ = true;
		}
		void CameraManager::MakeAttached(vec3 * pos, quat * orient)
		{
			if (is_current_)
				delete current_camera_;
			is_current_ = true;
			current_camera_ = new Camera(pos, orient);
            manual_rotation_ = true;
		}
		void CameraManager::Clear()
		{
			for (auto &c : cameras_)
				delete c;
			cameras_.clear();
			if (!is_current_)
				current_camera_ = nullptr;
		}
		CameraID CameraManager::Add(const vec3& pos, const vec3& target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, target_pos));
			return cam_id;
		}
		CameraID CameraManager::Add(vec3 * pos, const vec3& target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, target_pos));
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3& pos, vec3 * target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, target_pos));
			return cam_id;
		}
		CameraID CameraManager::Add(vec3 * pos, vec3 * target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, target_pos));
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3& pos, const quat& orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, orient));
			return cam_id;
		}
		CameraID CameraManager::Add(vec3 * pos, const quat& orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, orient));
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3& pos, quat * orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, orient));
			return cam_id;
		}
		CameraID CameraManager::Add(vec3 * pos, quat * orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
			cameras_.push_back(new Camera(pos, orient));
			return cam_id;
		}
		void CameraManager::SetCurrent(CameraID cam_id)
		{
			assert(cam_id < static_cast<CameraID>(cameras_.size()));
			if (is_current_)
				delete current_camera_;
			is_current_ = false;
			current_camera_ = cameras_[cam_id];
		}
		void CameraManager::PathClear()
		{
			current_path_index_ = 0;
			animation_time_ = 0.0f;
			paths_.clear();
		}
		void CameraManager::PathSetStart(CameraID cam_id, f32 time, bool target_oriented)
		{
			SetCurrent(cam_id);

			Path path = {cam_id, time, target_oriented};
			paths_.push_back(path);
		}
		void CameraManager::PathAdd(CameraID cam_id, f32 time, bool target_oriented)
		{
			Path path = {cam_id, time, target_oriented};
			paths_.push_back(path);
		}
		void CameraManager::PathSetCycling(bool cycling)
		{
			is_path_cycled_ = cycling;
		}
		void CameraManager::Update(f32 sec)
		{
			int path_size = static_cast<int>(paths_.size());
			if (path_size == 0)
			{
                assert(current_camera_);
                need_view_matrix_update_ = current_camera_->need_update_orientation_;
			}
			else if ((path_size == 1) || (current_path_index_ + 1 == path_size && !is_path_cycled_))
			{
				// Update the single camera
				CameraID cam_id = paths_[0].camera_id;
				cameras_[cam_id]->Update();
				need_view_matrix_update_ = cameras_[cam_id]->need_update_orientation_;
			}
			else
			{
				need_view_matrix_update_ = true;
				// Camera interpolation
				int curr_index = current_path_index_;
				int next_index = current_path_index_ + 1;
				if (next_index >= path_size && is_path_cycled_)
					next_index = 0;
				CameraID curr_cam_id = paths_[curr_index].camera_id;
				CameraID next_cam_id = paths_[next_index].camera_id;
				Camera * curr_camera = cameras_[curr_cam_id];
				Camera * next_camera = cameras_[next_cam_id];

				curr_camera->Update();
				next_camera->Update();

				f32 t = animation_time_ / paths_[next_index].interval;
				quat orient;
				quat::Slerp(*curr_camera->orientation_, *next_camera->orientation_, t, &orient);
				vec3 pos;
				if (paths_[next_index].is_target_oriented)
				{
					// Rotation around target position
					assert(curr_camera->position_);
					assert(curr_camera->target_position_);
					assert(next_camera->target_position_);

					vec3 direction = orient.Direction();
					vec3 target_pos = *curr_camera->target_position_ +
						(*next_camera->target_position_ - *curr_camera->target_position_) * t;
					f32 distance = (*curr_camera->target_position_ - *curr_camera->position_).Length();
					pos = target_pos - (direction * distance);
				}
				else
				{
					// Common case: interpolation between positions
					pos = *curr_camera->position_ +
						(*next_camera->position_ - *curr_camera->position_) * t;
				}
				MakeFree(pos, orient);

				animation_time_ += sec; // increase time
				if (animation_time_ >= paths_[next_index].interval)
				{
					// Transfer is complete
					animation_time_ -= paths_[next_index].interval;
					// Move selection to the next interval
					current_path_index_ = next_index;
				}
			}
			if (need_view_matrix_update_ || manual_rotation_)
			{
				manual_rotation_ = false;
				math::Matrix3 rotation;
				math::QuaternionToMatrix(rotation, *current_camera_->orientation_);
				view_matrix_ = math::ViewMatrix(rotation, *current_camera_->position_);
			}
		}
	
	} // namespace utility
} // namespace sht