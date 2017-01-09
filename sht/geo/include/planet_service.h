#pragma once
#ifndef __SHT_GEO_PLANET_SERVICE_H__
#define __SHT_GEO_PLANET_SERVICE_H__

#include "../../graphics/include/image/image.h"

#include <mutex>
#include <condition_variable>
#include <thread>

namespace sht {
	namespace geo {

		class PlanetService {
		public:
			PlanetService();
			virtual ~PlanetService();

			void RunService();
			void StopService();

			bool CheckRegionStatus(int face, int lod, int x, int y);

			const graphics::Image& image();

			virtual bool Initialize();
			virtual void Deinitialize();

		protected:
			virtual bool Execute() = 0;	//!< returns true if texture filling has been completed
			void ObtainTileParameters(int* face, int* lod, int* x, int* y);

			graphics::Image image_;

		private:
			PlanetService(PlanetService&) = delete;
			PlanetService& operator=(PlanetService&) = delete;

			void ThreadFunc();

			std::mutex mutex_;
			std::condition_variable condition_variable_;
			std::thread thread_;
			int face_;
			int lod_;
			int x_;
			int y_;
			bool finishing_;
			bool done_;
			bool has_task_;
		};

	} // namespace geo
} // namespace sht

#endif