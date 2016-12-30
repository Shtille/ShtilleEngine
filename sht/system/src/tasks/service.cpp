#include "../../include/tasks/service.h"
#include "../../include/tasks/service_task_interface.h"

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
				std::unique_lock<std::mutex> guard(mutex_);
				finishing_ = true;
				condition_variable_.notify_one();
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
			std::unique_lock<std::mutex> guard(mutex_);
			while (!tasks_.empty())
			{
				delete tasks_.front();
				tasks_.pop_front();
			}
			condition_variable_.notify_one();
		}
		void Service::AddTask(ServiceTaskInterface * task)
		{
			std::unique_lock<std::mutex> guard(mutex_);
			tasks_.push_back(task);
			condition_variable_.notify_one();
		}
		void Service::ThreadFunc()
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
					std::unique_lock<std::mutex> guard(mutex_);
					while (!finishing_ && tasks_.empty())
						condition_variable_.wait(guard);
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