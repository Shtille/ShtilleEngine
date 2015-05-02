// sht_pair.h	-- by Vladimir Sviridov <v.shtille@gmail.com> 29 July 2014

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Standart array class container implementation.

#ifndef __SHT_PAIR_H__
#define __SHT_PAIR_H__

#include "sht_utility.h"

#pragma pack(push, _CRT_PACKING)

namespace sht {

	template <class FirstType, class SecondType>
	class Pair {
	public:
		Pair() : first(), second() {}
		Pair(const FirstType& y1, const SecondType& y2) : first(y1), second(y2) {}
		Pair(const FirstType& y1) : first(y1), second() {}
		Pair(const Pair& p) : first(p.first), second(p.second) {}

		void operator = (const Pair& p) {
			first = p.first;
			second = p.second;
		}
		bool operator == (const Pair& p) {
			return first == p.first && second == p.second;
		}
		bool operator != (const Pair& p) {
			return !operator ==(p);
		}

		FirstType first;
		SecondType second;
	};

	template <typename FirstType, typename SecondType>
	Pair<FirstType, SecondType> make_pair(const FirstType& y1, const SecondType& y2) {
		return Pair<FirstType, SecondType>(y1, y2);

} // namespace sht

#pragma pack(pop)

#endif