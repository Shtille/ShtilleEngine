#ifndef __SHT_UTILITY_CAMERA_H__
#define __SHT_UTILITY_CAMERA_H__

#include "../../common/types.h"
#include "../../math/sht_math.h"

#include <vector>

namespace sht {
	namespace utility {

		//! Standart camera class
		class Camera {
			friend class CameraManager;
        public:
            explicit Camera(const vec3& pos, const vec3& target_pos);
            explicit Camera(vec3 * pos, const vec3& target_pos);
            explicit Camera(const vec3& pos, vec3 * target_pos);
            explicit Camera(vec3 * pos, vec3 * target_pos);
            explicit Camera(const vec3& pos, const quat& orient);
            explicit Camera(const vec3& pos, const quat& orient, const vec3& target_pos);
            explicit Camera(vec3 * pos, const quat& orient);
            explicit Camera(const vec3& pos, quat * orient);
            explicit Camera(vec3 * pos, quat * orient);
            explicit Camera(const quat& orient, vec3 * target_pos, float distance);
            Camera(const Camera& camera);
            Camera();
            ~Camera();
            
            Camera& operator =(const Camera& camera);
            
		protected:
            
            void Set(const vec3& pos, const vec3& target_pos);
            void Set(vec3 * pos, const vec3& target_pos);
            void Set(const vec3& pos, vec3 * target_pos);
            void Set(vec3 * pos, vec3 * target_pos);
            void Set(const vec3& pos, const quat& orient);
            void Set(const vec3& pos, const quat& orient, const vec3& target_pos);
            void Set(vec3 * pos, const quat& orient);
            void Set(const vec3& pos, quat * orient);
            void Set(vec3 * pos, quat * orient);
            void Set(const quat& orient, vec3 * target_pos, float distance);

			void Move(const vec3& translation);

			void Update(); //!< update orientation

		private:
			vec3 * position_ptr_;
			vec3 * target_position_ptr_;
			quat * orientation_ptr_;
            vec3 position_;
            vec3 target_position_;
            quat orientation_;
            float distance_;
			bool is_position_; //!< owns position or refers to external pointer
			bool is_target_position_; //!< owns target position or refers to external pointer
			bool is_orientation_; //!< owns orientation or refers to external pointer
			bool need_update_orientation_;
			bool need_update_position_; //!< the most rare case
		};

		typedef unsigned int CameraID;

		//! Camera manager class
		// NOTE: current mechanics doesn't allow to delete cameras,
		// because the main usage of camera IDs is in scripts.
		class CameraManager {
		public:
			CameraManager();
			~CameraManager();

			const math::Matrix4& view_matrix() const;
            
            const math::Vector3 * position() const;
            const math::Quaternion * orientation() const;
            
            math::Vector3 GetForward() const;
            math::Vector3 GetUp() const;
            math::Vector3 GetSide() const;
            
            math::Vector3 GetDirection() const;
            bool animated() const;
            
            void SetManualUpdate();

			void RotateAroundX(f32 angle);
			void RotateAroundY(f32 angle);
			void RotateAroundZ(f32 angle);
            void RotateAroundTargetInX(f32 angle);
            void RotateAroundTargetInY(f32 angle);
            void RotateAroundTargetInZ(f32 angle);
			void Move(const vec3& translation);

			void MakeFree(const vec3& pos, const vec3& target_pos);
			void MakeFree(const vec3& pos, const quat& orient);
            void MakeFree(CameraID camera_id);
            void MakeFreeTargeted(const vec3& pos, const quat& orient, const vec3& target_pos);
			void MakeAttached(vec3 * pos, quat * orient);

			void Clear();
			CameraID Add(const vec3& pos, const vec3& target_pos);
			CameraID Add(vec3 * pos, const vec3& target_pos);
			CameraID Add(const vec3& pos, vec3 * target_pos);
			CameraID Add(vec3 * pos, vec3 * target_pos);
			CameraID Add(const vec3& pos, const quat& orient);
            CameraID Add(const vec3& pos, const quat& orient, const vec3& target_pos);
			CameraID Add(vec3 * pos, const quat& orient);
			CameraID Add(const vec3& pos, quat * orient);
			CameraID Add(vec3 * pos, quat * orient);
			CameraID Add(const quat& orient, vec3 * target_pos, float distance);
            CameraID AddAsCurrent();
			void SetCurrent(CameraID cam_id);

			void PathClear();
			//!< time is needed here for cycling case
			void PathSetStart(CameraID cam_id, f32 time, bool target_oriented = false);
			void PathAdd(CameraID cam_id, f32 time, bool target_oriented = false);
			void PathSetCycling(bool cycling);

			void Update(f32 sec);

		private:
			struct Path {
				CameraID camera_id;
				f32 interval;
				bool is_target_oriented; //!< whether we should rotate around target
			};
			std::vector<Camera> cameras_;
			std::vector<Path> paths_;
			f32 animation_time_; //!< time to interpolate cameras
            Camera current_camera_; //!< current single camera object
			Camera * current_camera_ptr_; 	//!< pointer to current camera (to single or standart)
			int current_path_index_;
			math::Matrix4 view_matrix_;
			bool is_current_;			//!< is using single camera
			bool is_path_cycled_;
			bool need_view_matrix_update_;
			bool manual_rotation_;
		};
	
	} // namespace utility
} // namespace sht

#endif