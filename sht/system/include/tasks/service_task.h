#pragma once
#ifndef __SHT_SYSTEM_TASK_H__
#define __SHT_SYSTEM_TASK_H__

namespace sht {
	namespace system {

		//! Service task class interface
		class ServiceTask {
		public:
			virtual bool Execute() = 0;
			virtual void Notify(bool success) = 0;
		};

	} // namespace system
} // namespace sht

#endif