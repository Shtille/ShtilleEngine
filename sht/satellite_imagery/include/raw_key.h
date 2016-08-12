#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_RAW_KEY_H__
#define __SHT_SATELLITE_IMAGERY_RAW_KEY_H__

namespace sht {
	namespace satellite_imagery {

		// Forward declarations
		class PackedKey;

		//! Simple tile key class
		class RawKey {
		public:
			RawKey();
			RawKey(const RawKey& other);
			RawKey(const PackedKey& packed_key);
			RawKey(int x, int y, int z);

			void Set(int x, int y, int z);

			int x() const;
			int y() const;
			int z() const;

			friend bool operator == (const RawKey& lhs, const RawKey& rhs);
			friend bool operator <  (const RawKey& lhs, const RawKey& rhs);

		private:
			int x_;
			int y_;
			int z_;
		};

	} // namespace satellite_imagery
} // namespace sht

#endif