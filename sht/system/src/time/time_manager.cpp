#include "../../include/time/time_manager.h"

namespace sht {
	namespace system {

		void Timer::Reset()
		{
			time_ = 0.0f;
		}
		void Timer::Start()
		{
			enabled_ = true;
		}
		void Timer::Stop()
		{
			enabled_ = false;
		}
		bool Timer::HasExpired() const
		{
			return time_ >= interval_;
		}
		bool Timer::enabled() const
		{
			return enabled_;
		}
		float Timer::interval() const
		{
			return interval_;
		}
		float Timer::time() const
		{
			return time_;
		}
		Timer::Timer(float interval)
		: next_(nullptr)
		, interval_(interval)
		, time_(0.0f)
		, enabled_(false)
		{

		}
		Timer::~Timer()
		{

		}

		void TimeManager::SetFixedFrameTime(float fixed_frame_time)
		{
			fixed_frame_time_ = fixed_frame_time;
		}
		void TimeManager::Update()
		{
			float current_time = clock_.GetTime();
			frame_time_ = current_time - last_time_;
			last_time_ = current_time;

#if defined(_DEBUG) || defined(DEBUG)
			// Clamp update value when debugging step by step
			if (frame_time_ > 1.0f)
				frame_time_ = 0.0166f;
#endif

			// Update all timers
			Timer * timer = timer_head_;
			while (timer)
			{
				if (timer->enabled_)
					timer->time_ += frame_time_;
				timer = timer->next_;
			}

			// Compute current frame rate
			if (fps_counter_time_ < 1.0f)
			{
				fps_counter_count_ += 1.0f;
				fps_counter_time_ += frame_time_;
			}
			else
			{
				frame_rate_ = fps_counter_count_ / fps_counter_time_;
				fps_counter_count_ = 0.0f;
				fps_counter_time_ = 0.0f;
			}
		}
		Timer * TimeManager::AddTimer(float interval)
		{
			Timer * timer = new Timer(interval);
			timer->next_ = timer_head_;
			timer_head_ = timer;
			return timer;
		}
		void TimeManager::RemoveTimer(Timer * removed_timer)
		{
			Timer * prev = nullptr;
			Timer * timer = timer_head_;
			while (timer)
			{
				if (timer == removed_timer)
				{
					if (prev)
						prev->next_ = timer->next_;
					else
						timer_head_ = timer->next_;
					delete timer;
					break;
				}
				prev = timer;
				timer = timer->next_;
			}
		}
		float TimeManager::GetTime() const
		{
			return last_time_;
		}
		float TimeManager::GetFrameTime() const
		{
			return frame_time_;
		}
		float TimeManager::GetFixedFrameTime() const
		{
			return fixed_frame_time_;
		}
		float TimeManager::GetFrameRate() const
		{
			return frame_rate_;
		}
		TimeManager::TimeManager()
		: clock_()
		, timer_head_(nullptr)
		, fixed_frame_time_(1.0f/60.0f)
		, last_time_(0.0f)
		, frame_time_(0.0f)
		, frame_rate_(0.0f)
		, fps_counter_time_(0.0f)
		, fps_counter_count_(0.0f)
		{

		}
		TimeManager::~TimeManager()
		{
			Timer * timer_to_delete = nullptr;
			Timer * timer = timer_head_;
			while (timer)
			{
				timer_to_delete = timer;
				timer = timer->next_;
				delete timer_to_delete;
			}
		}

	} // namespace system
} // namespace sht