#include "../include/update_timer.h"
#include <chrono>

namespace sht {
	namespace system {

		class UpdateTimerImpl {
			typedef std::chrono::high_resolution_clock clock;
			typedef std::chrono::duration<float> seconds;
		public:
			void Start()
			{
				last_time_ = clock::now();
			}
			float GetElapsedTime()
			{
				current_time_ = clock::now();
				float time = std::chrono::duration_cast<seconds>(current_time_ - last_time_).count();
				last_time_ = current_time_;
				return time;
			}

		private:
			std::chrono::time_point<clock> current_time_;
			std::chrono::time_point<clock> last_time_;
		};

		UpdateTimer::UpdateTimer()
		{
			impl_ = new UpdateTimerImpl;
		}
		UpdateTimer::~UpdateTimer()
		{
			delete impl_;
		}
		void UpdateTimer::Start()
		{
			impl_->Start();
		}
		float UpdateTimer::GetElapsedTime()
		{
			return impl_->GetElapsedTime();
		}

	} // namespace system
} // namespace sht