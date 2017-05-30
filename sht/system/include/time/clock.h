#pragma once
#ifndef __SHT_SYSTEM_CLOCK_H__
#define __SHT_SYSTEM_CLOCK_H__

namespace sht {
	namespace system {

		class ClockImpl;

		class Clock {
		public:
			Clock();
			~Clock();

			void MakeStartPoint(); //!< makes current time as a start point
			float GetTime() const; //!< returns difference between current time and start time in seconds

		private:
			ClockImpl * impl_;
		};

	} // namespace system
} // namespace sht

#endif