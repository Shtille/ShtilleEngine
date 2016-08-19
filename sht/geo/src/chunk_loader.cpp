#include "chunk_loader.h"
#include "chunk_node.h"
#include "../../system/include/tasks/service_task_interface.h"

namespace sht {
	namespace geo {

		class ChunkLoadTask : public ServiceTaskInterface {
		public:
			ChunkLoadTask(int tree_id, ChunkNode * chunk, float urgency)
				: tree_id_(tree_id)
				, chunk_(chunk)
				, urgency_(urgency)
			{
			}
			bool Execute()
			{
				// TODO:
				return true;
			}
			void Notify(bool success)
			{
				//
			}

		private:
			int tree_id_;
			ChunkNode * chunk_;
			float urgency_;
		}

		ChunkLoader::ChunkLoader()
			: thread_(nullptr)
			, finishing_(false)
		{
			RunService();
		}
		ChunkLoader::~ChunkLoader()
		{
			if (thread_)
				StopService();
		}
		void ChunkLoader::SyncLoaderThread()
		{
			std::lock_guard<std::mutex> guard(mutex_);

			// Unload data.
			for (auto chunk : unload_queue_)
			{
				// Only unload the chunk if it's not currently in use.
				// Sometimes a chunk will be marked for unloading, but
				// then is still being used due to a dependency in a
				// neighboring part of the hierarchy.  We want to
				// ignore the unload request in that case.
				if (chunk->parent_ != nullptr && chunk->parent_->split_ == false)
				{
					chunk->unload_data();
				}
			}
			unload_queue_.resize(0);
		}
		void ChunkLoader::RequestChunkLoad(int tree_id, ChunkNode * chunk, float urgency)
		{
			// Don't schedule for load unless our parent already has data.
			if (chunk->parent_ == nullptr || chunk->parent_->data_ != nullptr)
			{
				load_queue_.push_back();
				load_queue_.back().chunk = chunk;
				load_queue_.back().tree_id = tree_id;
				load_queue_.back().urgency = urgency;
			}
		}
		void ChunkLoader::RequestChunkUnload(int tree_id, ChunkNode * chunk)
		{
			unload_queue_.push_back();
			unload_queue_.back().chunk = chunk;
			unload_queue_.back().tree_id = tree_id;
		}
		void ChunkLoader::RequestChunkLoadTexture(int tree_id, ChunkNode * chunk)
		{

		}
		void ChunkLoader::RequestChunkUnloadTexture(int tree_id, ChunkNode * chunk)
		{

		}
		void ChunkLoader::RunService()
		{
			finishing_ = false;
			thread_ = new std::thread(ThreadFuncWrapper, this);
		}
		void ChunkLoader::StopService()
		{
			{//---
				std::lock_guard<std::mutex> guard(mutex_);
				finishing_ = true;
			}//---
			thread_->join();
			delete thread_;
			thread_ = nullptr;

			// Don't forget to clear tasks
			load_queue_.clear();
			unload_queue_.clear();
			retire_list_.clear();
		}
		void ChunkLoader::ThreadFunc()
		{
			volatile bool finishing = false;
			for (;;)
			{
				ServiceTaskInterface * task = nullptr;
				{//---
					std::lock_guard<std::mutex> guard(mutex_);
					finishing = finishing_;
					if (!tasks_.empty())
					{
						// Dequeue front task
						task = tasks_.front();
						tasks_.pop_front();
					}
				}//---

				if (finishing)
					break;

				if (task == nullptr)
				{
					std::this_thread::sleep_for(std::chrono::seconds(0));
					continue;
				}

				// Task exists
				if (task->Execute())
				{
					// Notify about success
					task->Notify(true);
				}
				else
				{
					// Notify about fail
					task->Notify(false);
				}
				delete task;
			}
		}
		void ChunkLoader::ThreadFuncWrapper(Service * instance)
		{
			instance->ThreadFunc();
		}

	} // namespace geo
} // namespace sht