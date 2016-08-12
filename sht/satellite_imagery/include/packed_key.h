#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_PACKED_KEY_H__
#define __SHT_SATELLITE_IMAGERY_PACKED_KEY_H__

namespace sht {
	namespace satellite_imagery {

		// Forward declarations
		class RawKey;

		//! Packed tile key class
		class PackedKey {
		public:
			typedef unsigned long long HashType;

			PackedKey();
			PackedKey(const PackedKey& other);
			PackedKey(const RawKey& raw_key);
			PackedKey(int x, int y, int z);

			void Set(int x, int y, int z);

			int x() const;
			int y() const;
			int z() const;

			friend bool operator == (const PackedKey& lhs, const PackedKey& rhs);
			friend bool operator <  (const PackedKey& lhs, const PackedKey& rhs);

			static HashType Hash(int x, int y, int z);

		private:
			HashType hash_;
		};

	} // namespace satellite_imagery
} // namespace sht

#endif