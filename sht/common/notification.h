#pragma once
#ifndef __SHT_NOTIFICATION_H__
#define __SHT_NOTIFICATION_H__

#include <unordered_map>
#include <mutex>

namespace sht {
	
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
			std::lock_guard<std::mutex> guard(mutex_);
			observers_.insert(std::make_pair(obj, f));
		}
		//! Remove observer from list. Complexity: O(1)
		void RemoveObserver(ObserverInterface *obj)
		{
			std::lock_guard<std::mutex> guard(mutex_);
			observers_.erase(obj);
		}
		//! Notify observers about event. Complexity: O(N)
		/*! Automatically dispatches notification. */
		void Notify(NotificationInterface* notification)
		{
			mutex_.lock();
			for (auto &p: observers_)
			{
				ObserverInterface* obj = p.first;
				NotificationFunc f = p.second;
				(obj->*f)(notification);
			}
			mutex_.unlock();
			if (notification)
			{
				delete notification;
			}
		}
		//! Notify observers about event. Complexity: O(N)
		void NotifyWithoutDispatch(NotificationInterface* notification)
		{
			std::lock_guard<std::mutex> guard(mutex_);
			for (auto &p: observers_)
			{
				ObserverInterface* obj = p.first;
				NotificationFunc f = p.second;
				(obj->*f)(notification);
			}
		}
		
	private:
		std::unordered_map<ObserverInterface*, NotificationFunc> observers_;
		std::mutex mutex_;
	};
	
} // namespace sht

#endif