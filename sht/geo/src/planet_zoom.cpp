#include "../include/planet_zoom.h"
#include "../include/constants.h"
#include "../../utility/include/camera.h"
#include <cmath>

namespace sht {
	namespace geo {

		PlanetZoom::PlanetZoom(utility::CameraManager * camera_manager, float planet_radius, float farest_distance, float nearest_distance)
        : camera_manager_(camera_manager)
        , current_scale_index_(0)
		{
			num_scales_ = static_cast<int>(log(farest_distance/nearest_distance)/log(2)) + 1;
			distances_ = new float[num_scales_];
			float distance = farest_distance;
			for (int i = 0; i < num_scales_; ++i)
			{
				distances_[i] = distance;
                camera_manager_->Add(MakePoint(distance), kEarthPosition);
				distance *= 0.5f;
			}
            // Make static camera at farest point
            utility::CameraID start_camera_id = 0;
            camera_manager_->PathClear();
            camera_manager_->PathSetStart(start_camera_id, 0.0f, false); // we don't need a time value for cycling
            camera_manager_->SetManualUpdate();
		}
		PlanetZoom::~PlanetZoom()
		{
			delete[] distances_;
		}
		void PlanetZoom::ZoomIn()
		{
            if (current_scale_index_ + 1 < num_scales_)
            {
                ++current_scale_index_;
                // CameraID is a simply index of camera in array
                utility::CameraID camera_id = current_scale_index_;
                camera_manager_->PathAdd(camera_id, 1.0f);
            }
		}
		void PlanetZoom::ZoomOut()
		{
            if (current_scale_index_ > 0)
            {
                --current_scale_index_;
                // CameraID is a simply index of camera in array
                utility::CameraID camera_id = current_scale_index_;
                camera_manager_->PathAdd(camera_id, 1.0f);
            }
		}
        float PlanetZoom::GetZNear() const
        {
            float distance = distances_[current_scale_index_];
            return distance;
        }
        float PlanetZoom::GetZFar() const
        {
            float distance = distances_[current_scale_index_];
            return distance * 1000.0f;
        }
        math::Vector3 PlanetZoom::MakePoint(float distance)
        {
            return math::Vector3(kEarthRadius + distance, 0.0f, 0.0f);
        }

	}
}