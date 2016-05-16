#pragma once
#ifndef __SHT_GEO_PLANET_ZOOM_H__
#define __SHT_GEO_PLANET_ZOOM_H__

#include "../../math/vector.h"

// Forward declarations
namespace sht {
    namespace utility {
        class CameraManager;
    }
}

namespace sht {
	namespace geo {

		class PlanetZoom {
		public:
			PlanetZoom(utility::CameraManager * camera_manager, float planet_radius, float farest_distance, float nearest_distance);
			~PlanetZoom();

			void ZoomIn();
			void ZoomOut();
            
            float GetZNear() const;
            float GetZFar() const;
            
        protected:
            math::Vector3 MakePoint(float distance);

		private:
            utility::CameraManager * camera_manager_;
			int num_scales_; //!< number of scales
            int current_scale_index_; //!< current scale index
			float * distances_; //!< array of distances
		};

	}
}

#endif