#pragma once
#ifndef __SHT_SYSTEM_NOFTIFICATION_H__
#define __SHT_SYSTEM_NOFTIFICATION_H__

#include <unordered_map>

namespace sht {
	namespace system {
	
		//! Notification interface class
		class NotificationInterface {
		public:
			NotificationInterface() = default;
			virtual ~NotificationInterface() = default;
		};
		
		//! Observer interface class
		class ObserverInterface {
		public:
			ObserverInterface() = default;
			virtual ~ObserverInterface() = default;
		};
		
		typedef void (ObserverInterface::*NotificationFunc)(NotificationInterface*);
		
		//! Notification center class
		class NotificationCenter {
		public:
			NotificationCenter() = default;
			virtual ~NotificationCenter() = default;
			
			//! Add observer to list. Complexity: O(1)
			void AddObserver(ObserverInterface *obj, NotificationFunc f)
			{
				observers_.insert(std::make_pair(obj, f));
			}
			//! Remove observer from list. Complexity: O(1)
			void RemoveObserver(ObserverInterface *obj)
			{
				observers_.erase(obj);
			}
			//! Notify observers about event. Complexity: O(N)
			/*! Automatically dispatches notification. */
			void Notify(NotificationInterface* notification)
			{
				for (auto &p: observers_)
				{
					ObserverInterface* obj = p.first;
					NotificationFunc f = p.second;
					(obj->*f)(notification);
				}
				if (notification)
				{
					delete notification;
				}
			}
			
		private:
			std::unordered_map<ObserverInterface*, NotificationFunc> observers_;
		};
	
	} // namespace system
} // namespace sht

#endif