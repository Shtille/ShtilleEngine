#pragma once
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace sht {

	//! Simple singleton class
	template <class T>
	class Singleton {
	public:
		static T * GetInstance() {
			static T instance;
			return &instance;
		}

	protected:
        Singleton() = default;
        Singleton(const Singleton&) = delete;
		virtual ~Singleton() = default;
		Singleton& operator = (const Singleton&) = delete;
	};

	//! Singleton class with instance management
	template <class T>
	class ManagedSingleton {
	public:
		static void CreateInstance() {
			instance_ = new T();
		}
		static void DestroyInstance()
		{
			delete instance_;
			instance_ = nullptr;
		}
		static T * GetInstance() {
			return instance_;
		}

	protected:
        ManagedSingleton() = default;
        ManagedSingleton(const ManagedSingleton&) = delete;
		virtual ~ManagedSingleton() = default;
		ManagedSingleton& operator = (const ManagedSingleton&) = delete;

	private:
		static T * instance_;
	};
	template <class T>
	T* ManagedSingleton<T>::instance_ = nullptr;

	//! Singleton which counting references
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
	int CountingSingleton<T>::instance_counter_ = 0;

} // namespace sht

#endif