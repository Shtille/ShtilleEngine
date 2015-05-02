#pragma once
#ifndef __SHT_SYSTEM_TIME_H__
#define __SHT_SYSTEM_TIME_H__

namespace sht {
	namespace system {

		class UpdateTimer {
		public:
			UpdateTimer();
			~UpdateTimer();

			void Start();
			float GetElapsedTime();

		private:
			class UpdateTimerImpl * impl_;
		};

	} // namespace system
} // namespace sht

#endif