#pragma once
#ifndef __SHT_SYSTEM_SERVICE_TASK_INTERFACE_H__
#define __SHT_SYSTEM_SERVICE_TASK_INTERFACE_H__

namespace sht {
	namespace system {

		//! Service task class interface
		class ServiceTaskInterface {
		public:
			virtual ~ServiceTaskInterface() = default;

			virtual bool Execute() = 0;
			virtual void Notify(bool success) = 0;

		private:
			ServiceTaskInterface(const ServiceTaskInterface&) = delete;
			ServiceTaskInterface& operator =(const ServiceTaskInterface&) = delete;
		};

	} // namespace system
} // namespace sht

#endif