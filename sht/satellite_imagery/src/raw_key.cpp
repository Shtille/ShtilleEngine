#include "../include/raw_key.h"
#include "../include/packed_key.h"

namespace sht {
	namespace satellite_imagery {

		RawKey::RawKey()
		{
		}
		RawKey::RawKey(const RawKey& other)
			: x_(other.x_)
			, y_(other.y_)
			, z_(other.z_)
		{
		}
		RawKey::RawKey(const PackedKey& packed_key)
			: x_(packed_key.x())
			, y_(packed_key.y())
			, z_(packed_key.z())
		{
		}
		RawKey::RawKey(int x, int y, int z)
			: x_(x)
			, y_(y)
			, z_(z)
		{
		}
		void RawKey::Set(int x, int y, int z)
		{
			x_ = x;
			y_ = y;
			z_ = z;
		}
		int RawKey::x() const
		{
			return x_;
		}
		int RawKey::y() const
		{
			return y_;
		}
		int RawKey::z() const
		{
			return z_;
		}
		bool operator == (const RawKey& lhs, const RawKey& rhs)
		{
			return (lhs.z_ == rhs.z_) && (lhs.x_ == rhs.x_) && (lhs.y_ == rhs.y_);
		}
		bool operator <  (const RawKey& lhs, const RawKey& rhs)
		{
			return	(lhs.z_ <  rhs.z_) ||
					(lhs.z_ == rhs.z_) && (lhs.x_ <  rhs.x_) ||
					(lhs.z_ == rhs.z_) && (lhs.x_ == rhs.x_) && (lhs.y_ < rhs.y_);
		}

	} // namespace satellite_imagery
} // namespace sht