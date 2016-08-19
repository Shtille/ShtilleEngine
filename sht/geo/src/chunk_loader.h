#pragma once
#ifndef __SHT_GEO_PLANET_CHUNK_LOADER_H__
#define __SHT_GEO_PLANET_CHUNK_LOADER_H__

//#include "../../system/include/tasks/service.h"
#include "../../common/non_copyable.h"

#include <mutex>
#include <thread>
#include <vector>

namespace sht {
	namespace geo {

		// Forward declarations
		class ChunkNode;

		//! Chunk loader class
		class ChunkLoader : private NonCopyable {
		public:
			ChunkLoader();
			~ChunkLoader();

			void SyncLoaderThread();

			void RequestChunkLoad(int tree_id, ChunkNode * chunk, float urgency);
			void RequestChunkUnload(int tree_id, ChunkNode * chunk);

			void RequestChunkLoadTexture(int tree_id, ChunkNode * chunk);
			void RequestChunkUnloadTexture(int tree_id, ChunkNode * chunk);

		private:
			static void ThreadFuncWrapper(Service * instance);
			void ThreadFunc();
			void RunService();
			void StopService();

			struct LoadInfo {
				ChunkNode * chunk;
				int tree_id;
				float urgency;
			};
			struct Info {
				ChunkNode * chunk;
				int tree_id;
			};

			std::vector<LoadInfo> load_queue_;
			std::vector<Info> unload_queue_;
			std::vector<Info> request_list_;
			std::vector<Info> retire_list_;

			std::mutex mutex_;
			std::thread * thread_;
			bool finishing_;
		};

	} // namespace geo
} // namespace sht

#endif