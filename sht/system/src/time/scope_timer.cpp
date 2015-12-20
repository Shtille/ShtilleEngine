#include "../include/scope_timer.h"

#include "../include/stream/stream.h"

#include <chrono>

namespace sht {
	namespace system {
	
		//! Scope timer class implementation
		class ScopeTimerImpl {
			typedef std::chrono::high_resolution_clock clock;
			typedef std::chrono::duration<float> seconds;
		public:
			explicit ScopeTimerImpl(Stream * stream, const char* format)
			: stream_(stream)
			, format_(format)
			{
				start_time_ = clock::now();
			}
			virtual ~ScopeTimerImpl()
			{
				end_time_ = clock::now();
				float time = std::chrono::duration_cast<seconds>(end_time_ - start_time_).count();
				stream_->PrintString(format_, time);
			}
			
		private:
			Stream * stream_; //!< logging stream pointer
			const char* format_;
			std::chrono::time_point<clock> start_time_;
			std::chrono::time_point<clock> end_time_;
		};
		
		ScopeTimer::ScopeTimer(Stream * stream, const char* format)
		{
			impl_ = new ScopeTimerImpl(stream, format);
		}
		ScopeTimer::~ScopeTimer()
		{
			delete impl_;
		}
	
	} // namespace system
} // namespace sht