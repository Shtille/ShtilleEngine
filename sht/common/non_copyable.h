#pragma once
#ifndef __SHT_NON_COPYABLE_H__
#define __SHT_NON_COPYABLE_H__

namespace sht {

	//! Non-copyable class interface
	class NonCopyable {
	public:
		NonCopyable() = default;
		virtual ~NonCopyable() = default;

	private:
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator =(const NonCopyable&) = delete;
	};

} // namespace sht

#endif