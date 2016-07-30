#pragma once
#ifndef __SHT_SYSTEM_SERVICE_H__
#define __SHT_SYSTEM_SERVICE_H__

#include <mutex>
#include <thread>
#include <list>

namespace sht {
	namespace system {

		class ServiceTask;

		//! Service class
		class Service {
		public:
			Service();
			virtual ~Service();

			void RunService();
			void StopService();

			void ClearTasks();

		protected:
			virtual void ThreadFunc();

			std::mutex mutex_;
			std::thread * thread_;
			std::list<ServiceTask*> tasks_;
			bool finishing_;

		private:
			Service(Service&) = delete;
			Service& operator=(Service&) = delete;

			static void ThreadFuncWrapper(Service * instance);
		};

	} // namespace system
} // namespace sht

#endif