#include "../../include/tasks/service.h"
#include "../../include/tasks/service_task.h"

#include <chrono>

namespace sht {
	namespace system {

		Service::Service()
			: thread_(nullptr)
			, finishing_(false)
		{
		}
		Service::~Service()
		{
			if (thread_)
				StopService();
		}
		void Service::RunService()
		{
			finishing_ = false;
			thread_ = new std::thread(ThreadFuncWrapper, this);
		}
		void Service::StopService()
		{
			{//---
				std::lock_guard<std::mutex> guard(mutex_);
				finishing_ = true;
			}//---
			thread_->join();
			delete thread_;
			thread_ = nullptr;

			// Don't forget to clear tasks
			while (!tasks_.empty())
			{
				delete tasks_.front();
				tasks_.pop_front();
			}
		}
		void Service::ClearTasks()
		{
			std::lock_guard<std::mutex> guard(mutex_);
			while (!tasks_.empty())
			{
				delete tasks_.front();
				tasks_.pop_front();
			}
		}
		void Service::ThreadFunc()
		{
			volatile bool finishing = false;
			for (;;)
			{
				ServiceTask * task = nullptr;
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
		void Service::ThreadFuncWrapper(Service * instance)
		{
			instance->ThreadFunc();
		}

	} // namespace system
} // namespace sht