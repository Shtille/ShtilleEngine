#include "../../include/time/clock.h"

#include <chrono>

namespace sht {
	namespace system {

		class ClockImpl {
			typedef std::chrono::high_resolution_clock clock;
			typedef std::chrono::duration<float> seconds;
		public:
			ClockImpl()
			{
				start_time_ = clock::now();
			}
			void MakeStartPoint()
			{
				start_time_ = clock::now();
			}
			float GetTime()
			{
				end_time_ = clock::now();
				return std::chrono::duration_cast<seconds>(end_time_ - start_time_).count();
			}

		private:
			std::chrono::time_point<clock> start_time_;
			std::chrono::time_point<clock> end_time_;
		};

		Clock::Clock()
		: impl_(new ClockImpl())
		{

		}
		Clock::~Clock()
		{
			delete impl_;
		}
		void Clock::MakeStartPoint()
		{
			return impl_->MakeStartPoint();
		}
		float Clock::GetTime() const
		{
			return impl_->GetTime();
		}

	} // namespace system
} // namespace sht