#pragma once
#ifndef __SHT_SYSTEM_SERVICE_H__
#define __SHT_SYSTEM_SERVICE_H__

#include <mutex>
#include <condition_variable>
#include <thread>
#include <list>

namespace sht {
	namespace system {

		class ServiceTaskInterface;

		//! Service class
		class Service {
		public:
			Service();
			virtual ~Service();

			void RunService();
			void StopService();

			void ClearTasks();
			void AddTask(ServiceTaskInterface * task);

		protected:
			virtual void ThreadFunc();

			std::mutex mutex_;
			std::condition_variable condition_variable_;
			std::thread * thread_;
			std::list<ServiceTaskInterface*> tasks_;
			bool finishing_;

		private:
			Service(Service&) = delete;
			Service& operator=(Service&) = delete;

			static void ThreadFuncWrapper(Service * instance);
		};

	} // namespace system
} // namespace sht

#endif