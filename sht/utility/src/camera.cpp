#include "../include/camera.h"

#include <assert.h>

namespace sht {
	namespace utility {
	
		Camera::Camera(const vec3& pos, const vec3& target_pos)
		{
            Set(pos, target_pos);
		}
		Camera::Camera(const vec3 * pos, const vec3& target_pos)
		{
            Set(pos, target_pos);
		}
		Camera::Camera(const vec3& pos, const vec3 * target_pos)
		{
            Set(pos, target_pos);
		}
		Camera::Camera(const vec3 * pos, const vec3 * target_pos)
		{
			Set(pos, target_pos);
		}
		Camera::Camera(const vec3& pos, const quat& orient)
		{
            Set(pos, orient);
		}
        Camera::Camera(const vec3& pos, const quat& orient, const vec3& target_pos)
        {
            Set(pos, orient, target_pos);
        }
		Camera::Camera(const vec3 * pos, const quat& orient)
		{
			Set(pos, orient);
		}
		Camera::Camera(const vec3& pos, const quat * orient)
		{
			Set(pos, orient);
		}
		Camera::Camera(const vec3 * pos, const quat * orient)
		{
			Set(pos, orient);
		}
        Camera::Camera(const quat& orient, const vec3 * target_pos, float distance)
        {
            Set(orient, target_pos, distance);
        }
		Camera::Camera(const Camera& camera)
		{
			(void)operator =(camera);
		}
        Camera::Camera()
        {
            
        }
		Camera::~Camera()
		{
		}
		Camera& Camera::operator =(const Camera& camera)
		{
            need_update_position_ = camera.need_update_position_;
			need_update_orientation_ = camera.need_update_orientation_;
			is_position_ = camera.is_position_;
			is_target_position_ = camera.is_target_position_;
			is_orientation_ = camera.is_orientation_;
            distance_ = camera.distance_;
			if (is_position_)
            {
				position_ = camera.position_;
                position_ptr_ = &position_;
            }
			else
				position_ptr_ = camera.position_ptr_;
			if (is_target_position_)
            {
				target_position_ = camera.target_position_;
                target_position_ptr_ = &target_position_;
            }
			else
				target_position_ptr_ = camera.target_position_ptr_;
			if (is_orientation_)
            {
				orientation_ = camera.orientation_;
                orientation_ptr_ = &orientation_;
            }
			else
				orientation_ptr_ = camera.orientation_ptr_;
			return *this;
		}
        void Camera::Set(const vec3& pos, const vec3& target_pos)
        {
            is_position_ = true;
            position_ = pos;
            position_ptr_ = &position_;
            is_target_position_ = true;
            target_position_ = target_pos;
            target_position_ptr_ = &target_position_;
            is_orientation_ = true;
            orientation_.Set(pos, target_pos);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = false;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3 * pos, const vec3& target_pos)
        {
            is_position_ = false;
            position_ptr_ = pos;
            is_target_position_ = true;
            target_position_ = target_pos;
            target_position_ptr_ = &target_position_;
            is_orientation_ = true;
            orientation_.Set(*pos, target_pos);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = true;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3& pos, const vec3 * target_pos)
        {
            is_position_ = true;
            position_ = pos;
            position_ptr_ = &position_;
            is_target_position_ = false;
            target_position_ptr_ = target_pos;
            is_orientation_ = true;
            orientation_.Set(pos, *target_pos);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = true;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3 * pos, const vec3 * target_pos)
        {
            is_position_ = false;
            position_ptr_ = pos;
            is_target_position_ = false;
            target_position_ptr_ = target_pos;
            is_orientation_ = true;
            orientation_.Set(*pos, *target_pos);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = true;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3& pos, const quat& orient)
        {
            is_position_ = true;
            position_ = pos;
            position_ptr_ = &position_;
            is_target_position_ = false;
            target_position_ptr_ = nullptr;
            is_orientation_ = true;
            orientation_.Set(orient);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = false;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3& pos, const quat& orient, const vec3& target_pos)
        {
            is_position_ = true;
            position_ = pos;
            position_ptr_ = &position_;
            is_target_position_ = true;
            target_position_ = target_pos;
            target_position_ptr_ = &target_position_;
            is_orientation_ = true;
            orientation_.Set(orient);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = false;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3 * pos, const quat& orient)
        {
            is_position_ = false;
            position_ptr_ = pos;
            is_target_position_ = false;
            target_position_ptr_ = nullptr;
            is_orientation_ = true;
            orientation_.Set(orient);
            orientation_ptr_ = &orientation_;
            need_update_orientation_ = false;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3& pos, const quat * orient)
        {
            is_position_ = true;
            position_ = pos;
            position_ptr_ = &position_;
            is_target_position_ = false;
            target_position_ptr_ = nullptr;
            is_orientation_ = false;
            orientation_ptr_ = orient;
            need_update_orientation_ = false;
            need_update_position_ = false;
        }
        void Camera::Set(const vec3 * pos, const quat * orient)
        {
            is_position_ = false;
            position_ptr_ = pos;
            is_target_position_ = false;
            target_position_ptr_ = nullptr;
            is_orientation_ = false;
            orientation_ptr_ = orient;
            need_update_orientation_ = false;
            need_update_position_ = false;
        }
        void Camera::Set(const quat& orient, const vec3 * target_pos, float distance)
        {
            is_position_ = true;
            position_ = *target_pos - orient.Direction() * distance;
            position_ptr_ = &position_;
            is_target_position_ = false;
            target_position_ptr_ = target_pos;
            is_orientation_ = true;
            orientation_.Set(orient);
            orientation_ptr_ = &orientation_;
            distance_ = distance;
            need_update_orientation_ = false;
            need_update_position_ = true;
        }
		void Camera::Move(const vec3& translation)
		{
			if (is_position_)
			{
				position_ += translation;
				Update();
			}
		}
		void Camera::Update()
		{
            if (need_update_orientation_ && target_position_ptr_)
                orientation_.Set(*position_ptr_, *target_position_ptr_);

            if (need_update_position_ && target_position_ptr_)
                position_ = *target_position_ptr_ - orientation_ptr_->Direction() * distance_;
		}
		CameraManager::CameraManager()
		: animation_time_(0.0f)
		, current_camera_ptr_(nullptr)
		, current_path_index_(0)
		, is_current_(false)
		, is_path_cycled_(false)
		, need_view_matrix_update_(false)
		, manual_rotation_(false)
		{
		}
		CameraManager::~CameraManager()
		{
			Clear();
		}
		const math::Matrix4& CameraManager::view_matrix() const
		{
			return view_matrix_;
		}
        const math::Vector3* CameraManager::position() const
        {
            if (current_camera_ptr_)
                return current_camera_ptr_->position_ptr_;
            else
                return nullptr;
        }
        const math::Quaternion * CameraManager::orientation() const
        {
            if (current_camera_ptr_)
                return current_camera_ptr_->orientation_ptr_;
            else
                return nullptr;
        }
        math::Vector3 CameraManager::GetForward() const
        {
            return math::Vector3(-view_matrix_.e13, -view_matrix_.e23, -view_matrix_.e33);
        }
        math::Vector3 CameraManager::GetUp() const
        {
            return math::Vector3(view_matrix_.e12, view_matrix_.e22, view_matrix_.e32);
        }
        math::Vector3 CameraManager::GetSide() const
        {
            return math::Vector3(view_matrix_.e11, view_matrix_.e21, view_matrix_.e31);
        }
        math::Vector3 CameraManager::GetDirection() const
        {
            assert(current_camera_ptr_);
            return current_camera_ptr_->orientation_ptr_->Direction();
        }
        bool CameraManager::animated() const
        {
            return need_view_matrix_update_;
        }
        void CameraManager::SetManualUpdate()
        {
            manual_rotation_ = true;
        }
		void CameraManager::RotateAroundX(f32 angle)
		{
			quat orient(UNIT_X, angle);
            assert(current_camera_ptr_->is_orientation_);
			current_camera_ptr_->orientation_ = *current_camera_ptr_->orientation_ptr_ * orient;
            current_camera_ptr_->orientation_.Normalize();
			manual_rotation_ = true;
		}
		void CameraManager::RotateAroundY(f32 angle)
		{
			quat orient(UNIT_Y, angle);
            assert(current_camera_ptr_->is_orientation_);
            current_camera_ptr_->orientation_ = *current_camera_ptr_->orientation_ptr_ * orient;
            current_camera_ptr_->orientation_.Normalize();
			manual_rotation_ = true;
		}
		void CameraManager::RotateAroundZ(f32 angle)
		{
			quat orient(UNIT_Z, angle);
            assert(current_camera_ptr_->is_orientation_);
            current_camera_ptr_->orientation_ = *current_camera_ptr_->orientation_ptr_ * orient;
            current_camera_ptr_->orientation_.Normalize();
			manual_rotation_ = true;
		}
        void CameraManager::RotateAroundTargetInX(f32 angle)
        {
            assert(is_current_);
            assert(current_camera_ptr_->is_target_position_);
            assert(current_camera_ptr_->is_position_);
            
            RotateAroundX(-angle);
            f32 distance = (*current_camera_ptr_->target_position_ptr_ - *current_camera_ptr_->position_ptr_).Length();
            current_camera_ptr_->position_ = *current_camera_ptr_->target_position_ptr_
                - distance * current_camera_ptr_->orientation_ptr_->Direction();
        }
        void CameraManager::RotateAroundTargetInY(f32 angle)
        {
            assert(is_current_);
            assert(current_camera_ptr_->is_target_position_);
            assert(current_camera_ptr_->is_position_);
            
            RotateAroundY(-angle);
            f32 distance = (*current_camera_ptr_->target_position_ptr_ - *current_camera_ptr_->position_ptr_).Length();
            current_camera_ptr_->position_ = *current_camera_ptr_->target_position_ptr_
                - distance * current_camera_ptr_->orientation_ptr_->Direction();
        }
        void CameraManager::RotateAroundTargetInZ(f32 angle)
        {
            assert(is_current_);
            assert(current_camera_ptr_->is_target_position_);
            assert(current_camera_ptr_->is_position_);
            
            RotateAroundZ(-angle);
            f32 distance = (*current_camera_ptr_->target_position_ptr_ - *current_camera_ptr_->position_ptr_).Length();
            current_camera_ptr_->position_ = *current_camera_ptr_->target_position_ptr_
                - distance * current_camera_ptr_->orientation_ptr_->Direction();
        }
		void CameraManager::Move(const vec3& translation)
		{
			if (is_current_)
				current_camera_ptr_->Move(translation);
            manual_rotation_ = true; // need to update matrix anyway
		}
		void CameraManager::MakeFree(const vec3& pos, const vec3& target_pos)
		{
			is_current_ = true;
			current_camera_.Set(pos, target_pos);
            current_camera_ptr_ = &current_camera_;
            manual_rotation_ = true;
		}
		void CameraManager::MakeFree(const vec3& pos, const quat& orient)
		{
			is_current_ = true;
			current_camera_.Set(pos, orient);
            current_camera_ptr_ = &current_camera_;
            manual_rotation_ = true;
		}
        void CameraManager::MakeFree(CameraID camera_id)
        {
            assert(camera_id < static_cast<CameraID>(cameras_.size()));
            is_current_ = true;
            const Camera& camera = cameras_[camera_id];
            current_camera_.Set(*camera.position_ptr_, *camera.target_position_ptr_);
            current_camera_ptr_ = &current_camera_;
            manual_rotation_ = true;
        }
        void CameraManager::MakeFreeTargeted(const vec3& pos, const quat& orient, const vec3& target_pos)
        {
            is_current_ = true;
            current_camera_.Set(pos, orient, target_pos);
            current_camera_ptr_ = &current_camera_;
            manual_rotation_ = true;
        }
		void CameraManager::MakeAttached(const vec3 * pos, const quat * orient)
		{
			is_current_ = true;
			current_camera_.Set(pos, orient);
            current_camera_ptr_ = &current_camera_;
            manual_rotation_ = true;
		}
        void CameraManager::MakeAttached(const quat& orient, const vec3 * target_pos, float distance)
        {
            is_current_ = true;
            current_camera_.Set(orient, target_pos, distance);
            current_camera_ptr_ = &current_camera_;
            manual_rotation_ = true;
        }
		void CameraManager::Clear()
		{
			cameras_.clear();
			if (!is_current_)
				current_camera_ptr_ = nullptr;
		}
		CameraID CameraManager::Add(const vec3& pos, const vec3& target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, target_pos);
			cameras_.push_back(camera);
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3 * pos, const vec3& target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, target_pos);
            cameras_.push_back(camera);
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3& pos, const vec3 * target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, target_pos);
            cameras_.push_back(camera);
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3 * pos, const vec3 * target_pos)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, target_pos);
            cameras_.push_back(camera);
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3& pos, const quat& orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, orient);
            cameras_.push_back(camera);
			return cam_id;
		}
        CameraID CameraManager::Add(const vec3& pos, const quat& orient, const vec3& target_pos)
        {
            CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, orient, target_pos);
            cameras_.push_back(camera);
            return cam_id;
        }
		CameraID CameraManager::Add(const vec3 * pos, const quat& orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, orient);
            cameras_.push_back(camera);
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3& pos, const quat * orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, orient);
            cameras_.push_back(camera);
			return cam_id;
		}
		CameraID CameraManager::Add(const vec3 * pos, const quat * orient)
		{
			CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(pos, orient);
            cameras_.push_back(camera);
			return cam_id;
		}
        CameraID CameraManager::Add(const quat& orient, const vec3 * target_pos, float distance)
        {
            CameraID cam_id = static_cast<CameraID>(cameras_.size());
            Camera camera(orient, target_pos, distance);
            cameras_.push_back(camera);
            return cam_id;
        }
        CameraID CameraManager::AddAsCurrent()
        {
            assert(is_current_); // make sure camera is valid
            CameraID cam_id = static_cast<CameraID>(cameras_.size());
            cameras_.push_back(current_camera_);
            return cam_id;
        }
		void CameraManager::SetCurrent(CameraID cam_id)
		{
			assert(cam_id < static_cast<CameraID>(cameras_.size()));
			is_current_ = false;
			current_camera_ptr_ = &cameras_[cam_id];
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
				assert(current_camera_ptr_ && "camera hasn't been set");
                current_camera_ptr_->Update();
				need_view_matrix_update_ = current_camera_ptr_->need_update_position_ 
                                        || current_camera_ptr_->need_update_orientation_;
			}
			else if ((path_size == 1) || (current_path_index_ + 1 == path_size && !is_path_cycled_))
			{
				// Update the single camera
				CameraID cam_id = paths_[0].camera_id;
				cameras_[cam_id].Update();
                need_view_matrix_update_ = cameras_[cam_id].need_update_position_ 
                                        || cameras_[cam_id].need_update_orientation_;
			}
			else
			{
				need_view_matrix_update_ = true;
                animation_time_ += sec; // increase time
                int curr_index = current_path_index_;
                int next_index = current_path_index_ + 1;
                if (next_index >= path_size && is_path_cycled_)
                    next_index = 0;
                bool animation_finished = false;
                if (animation_time_ >= paths_[next_index].interval)
                {
                    // Transfer is complete
                    animation_time_ -= paths_[next_index].interval;
                    // Move selection to the next interval
                    current_path_index_ = next_index;
                    curr_index = current_path_index_;
                    next_index = current_path_index_ + 1;
                    if (next_index >= path_size)
                    {
                        if (is_path_cycled_)
                            next_index = 0;
                        else
                            animation_finished = true;
                    }
                }
                if (!animation_finished)
                {
                    // Camera interpolation
                    CameraID curr_cam_id = paths_[curr_index].camera_id;
                    CameraID next_cam_id = paths_[next_index].camera_id;
                    Camera * curr_camera = &cameras_[curr_cam_id];
                    Camera * next_camera = &cameras_[next_cam_id];

                    curr_camera->Update();
                    next_camera->Update();

                    f32 t = animation_time_ / paths_[next_index].interval;
                    quat orient;
                    quat::Slerp(*curr_camera->orientation_ptr_, *next_camera->orientation_ptr_, t, &orient);
                    vec3 pos;
                    if (paths_[next_index].is_target_oriented)
                    {
                        // Rotation around target position
                        assert(curr_camera->position_ptr_);
                        assert(curr_camera->target_position_ptr_);
                        assert(next_camera->target_position_ptr_);

                        vec3 direction = orient.Direction();
                        vec3 target_pos = *curr_camera->target_position_ptr_ +
                            (*next_camera->target_position_ptr_ - *curr_camera->target_position_ptr_) * t;
                        f32 distance = (*curr_camera->target_position_ptr_ - *curr_camera->position_ptr_).Length();
                        pos = target_pos - (direction * distance);
                    }
                    else
                    {
                        // Common case: interpolation between positions
                        pos = *curr_camera->position_ptr_ +
                            (*next_camera->position_ptr_ - *curr_camera->position_ptr_) * t;
                    }
                    if (curr_camera->is_target_position_ && next_camera->is_target_position_)
                    {
                        // Make target oriented but saving orientation
                        MakeFreeTargeted(pos, orient, *curr_camera->target_position_ptr_);
                    }
                    else
                        MakeFree(pos, orient);
                }
			}
			if (need_view_matrix_update_ || manual_rotation_)
			{
				manual_rotation_ = false;
				math::Matrix3 rotation;
				math::QuaternionToMatrix(rotation, *current_camera_ptr_->orientation_ptr_);
				view_matrix_ = math::ViewMatrix(rotation, *current_camera_ptr_->position_ptr_);
			}
		}
	
	} // namespace utility
} // namespace sht