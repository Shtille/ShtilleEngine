#include "../include/packed_key.h"
#include "../include/raw_key.h"

namespace {
	// Check minimum requirement
	static_assert(sizeof(sht::satellite_imagery::PackedKey::HashType) >= 8, "Packed key requires more space");

	// Shifts
	const sht::satellite_imagery::PackedKey::HashType kYShift = 28ULL;
	const sht::satellite_imagery::PackedKey::HashType kZShift = 56ULL;
	// Masks
	const sht::satellite_imagery::PackedKey::HashType kXMask = 0x000000000FFFFFFFULL;
	const sht::satellite_imagery::PackedKey::HashType kYMask = 0x00FFFFFFF0000000ULL;
	const sht::satellite_imagery::PackedKey::HashType kZMask = 0xFF00000000000000ULL;

	// Do some checks whether all shifts and masks have been set right
	static_assert((kXMask + 1ULL) == (1ULL << kYShift), "Error in masks and/or shifts");
	static_assert((kXMask + kYMask + 1ULL) == (1ULL << kZShift), "Error in masks and/or shifts");
}

namespace sht {
	namespace satellite_imagery {

		PackedKey::PackedKey()
			: hash_(0ULL)
		{
		}
		PackedKey::PackedKey(const PackedKey& other)
			: hash_(other.hash_)
		{
		}
		PackedKey::PackedKey(const RawKey& raw_key)
		{
			hash_ = Hash(raw_key.x(), raw_key.y(), raw_key.z());
		}
		PackedKey::PackedKey(int x, int y, int z)
		{
			Set(x, y, z);
		}
		void PackedKey::Set(int x, int y, int z)
		{
			hash_ = Hash(x, y, z);
		}
		int PackedKey::x() const
		{
			return static_cast<int>((hash_ & kXMask)           ) - 1;
		}
		int PackedKey::y() const
		{
			return static_cast<int>((hash_ & kYMask) >> kYShift) - 1;
		}
		int PackedKey::z() const
		{
			return static_cast<int>((hash_ & kZMask) >> kZShift) - 1;
		}
		PackedKey& PackedKey::operator =(const PackedKey& other)
		{
			Set(other.x(), other.y(), other.z());
			return *this;
		}
		PackedKey& PackedKey::operator =(const RawKey& other)
		{
			Set(other.x(), other.y(), other.z());
			return *this;
		}
		bool operator == (const PackedKey& lhs, const PackedKey& rhs)
		{
			return lhs.hash_ == rhs.hash_;
		}
		bool operator <  (const PackedKey& lhs, const PackedKey& rhs)
		{
			return lhs.hash_ <  rhs.hash_;
		}
		PackedKey::HashType PackedKey::Hash(int x, int y, int z)
		{
			/*
			Data key is proposed for tile position in quadtree.
			We will increase by 1 each coordinate to avoid storing zero values
			*/
			HashType ux = static_cast<HashType>(x + 1);
			HashType uy = static_cast<HashType>(y + 1);
			HashType uz = static_cast<HashType>(z + 1);
			return (uz << kZShift) | (uy << kYShift) | (ux);
		}

	} // namespace satellite_imagery
} // namespace sht