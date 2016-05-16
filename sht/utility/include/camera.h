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
		protected:
			explicit Camera(const vec3& pos, const vec3& target_pos);
			explicit Camera(vec3 * pos, const vec3& target_pos);
			explicit Camera(const vec3& pos, vec3 * target_pos);
			explicit Camera(vec3 * pos, vec3 * target_pos);
			explicit Camera(const vec3& pos, const quat& orient);
            explicit Camera(const vec3& pos, const quat& orient, const vec3& target_pos);
			explicit Camera(vec3 * pos, const quat& orient);
			explicit Camera(const vec3& pos, quat * orient);
			explicit Camera(vec3 * pos, quat * orient);

			Camera(const Camera& camera);
			~Camera();

			Camera& operator =(const Camera& camera);

			void Move(const vec3& translation);

			void Update(); //!< update orientation

		private:
			vec3 * position_;
			vec3 * target_position_;
			quat * orientation_;
			bool is_position_;
			bool is_target_position_;
			bool is_orientation_;
			bool need_update_orientation_;
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
            
            const math::Vector3* position() const;
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
			void MakeAttached(vec3 * pos, quat * orient);

			void Clear();
			CameraID Add(const vec3& pos, const vec3& target_pos);
			CameraID Add(vec3 * pos, const vec3& target_pos);
			CameraID Add(const vec3& pos, vec3 * target_pos);
			CameraID Add(vec3 * pos, vec3 * target_pos);
			CameraID Add(const vec3& pos, const quat& orient);
			CameraID Add(vec3 * pos, const quat& orient);
			CameraID Add(const vec3& pos, quat * orient);
			CameraID Add(vec3 * pos, quat * orient);
			void SetCurrent(CameraID cam_id);

			void PathClear();
			//!< time is needed here for cycling case
			void PathSetStart(CameraID cam_id, f32 time, bool target_oriented = false);
			void PathAdd(CameraID cam_id, f32 time, bool target_oriented = false);
			void PathSetCycling(bool cycling);

			void Update(f32 sec);
            
        protected:
            void MakeFreeTargeted(const vec3& pos, const quat& orient, const vec3& target_pos);

		private:
			struct Path {
				CameraID camera_id;
				f32 interval;
				bool is_target_oriented; //!< whether we should rotate around target
			};
			std::vector<Camera*> cameras_;
			std::vector<Path> paths_;
			f32 animation_time_; //!< time to interpolate cameras
			Camera * current_camera_; 	//!< pointer to current camera (to single or standart)
			int current_path_index_;
			math::Matrix4 view_matrix_;
			bool is_current_;			//!< is using single camera (obsolete?)
			bool is_path_cycled_;
			bool need_view_matrix_update_;
			bool manual_rotation_;
		};
	
	} // namespace utility
} // namespace sht

#endif