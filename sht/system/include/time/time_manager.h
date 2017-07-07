#pragma once
#ifndef __SHT_SYSTEM_TIME_MANAGER_H__
#define __SHT_SYSTEM_TIME_MANAGER_H__

#include "common/singleton.h"
#include "clock.h"

namespace sht {
	namespace system {

		//! Managed timer class (timer is initially disabled)
		class Timer {
			friend class TimeManager;
		public:
			void Reset(); //!< sets time to 0
			void Start(); //!< enables
			void Stop(); //!< disables

			bool HasExpired() const;
			bool enabled() const;
			float interval() const;
			float time() const;

		private:
			Timer(float interval);
			~Timer();

			Timer * next_;
			float interval_;
			float time_;
			bool enabled_;
		};

		//! Time manager class
		class TimeManager : public ManagedSingleton<TimeManager> {
			friend class ManagedSingleton<TimeManager>;
		public:

			void SetFixedFrameTime(float fixed_frame_time);

			void Update();

			Timer * AddTimer(float interval);
			void RemoveTimer(Timer * removed_timer);

			float GetTime() const;
			float GetFrameTime() const; //!< real frame time
			float GetFixedFrameTime() const; //!< fixed frame time for updates
			float GetFrameRate() const; //!< also known as FPS

		private:
			TimeManager();
			~TimeManager();

			Clock clock_;
			Timer * timer_head_;
			float fixed_frame_time_;		//!< our engine uses fixed time steps, so this just shares the value
			float last_time_;
			float frame_time_;				//!< time between two updates
			float frame_rate_;				//!< frames per second (FPS)
			float fps_counter_time_;		//!< for counting FPS
			float fps_counter_count_;		//!< for counting FPS
		};

	} // namespace system
} // namespace sht

#endif