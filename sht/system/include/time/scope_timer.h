#pragma once
#ifndef __SHT_SYTEM_SCOPE_TIMER_H__
#define __SHT_SYTEM_SCOPE_TIMER_H__

namespace sht {
	namespace system {
	
		// Forward declarations
		class Stream;
		class ScopeTimerImpl;
	
		//! Class for scope time measurement
		class ScopeTimer {
		public:
			explicit ScopeTimer(Stream * stream, const char* format);
			virtual ~ScopeTimer();
			
		private:
			ScopeTimerImpl * impl_; //!< implementation
		};
	
	} // namespace system
} // namespace sht

#endif