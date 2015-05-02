#pragma once
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace sht {

	// Simple singleton class
	template <class T>
	class Singleton {
	public:
		static T * GetInstance() {
			static T instance;
			return &instance;
		}

	protected:
		Singleton() {}
		Singleton(const Singleton&) {}
		virtual ~Singleton() {}
		Singleton& operator = (const Singleton&) {}
	};

	// Singleton which counting references
	template <class T>
	class CountingSingleton {
	public:
		static T * GetInstance() {
			if (instance_counter_ == 0)
				instance_ = new T();
			++instance_counter_;
			return instance_;
		}
		static void DeleteInstance() {
			--instance_counter_;
			if (instance_counter_ == 0) {
				delete instance_;
				instance_ = nullptr;
			}
		}

	protected:
		CountingSingleton() {}
		CountingSingleton(const CountingSingleton&) {}
		virtual ~CountingSingleton() {}
		CountingSingleton& operator = (const CountingSingleton&) {}

	private:
		static T * instance_;
		static int instance_counter_;
	};
	template <class T>
	T* CountingSingleton<T>::instance_ = nullptr;
	template <class T>
	T* CountingSingleton<T>::instance_counter_ = 0;

} // namespace sht

#endif