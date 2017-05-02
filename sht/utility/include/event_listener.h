#pragma once
#ifndef __SHT_SYSTEM_EVENT_LISTENER_H__
#define __SHT_SYSTEM_EVENT_LISTENER_H__

namespace sht {
	namespace utility {

		class Event;

		//! Event listener interface class
		class EventListenerInterface {
		public:
			virtual void OnEvent(const Event * event) = 0;
		};

	} // namespace utility
} // namespace sht

#endif