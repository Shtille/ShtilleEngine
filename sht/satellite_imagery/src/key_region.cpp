#include "../include/key_region.h"

#include "../include/key.h"

#include <cstring>

namespace sht {
    namespace satellite_imagery {

        KeyRegion::KeyRegion()
        {

        }
        KeyRegion::KeyRegion(int min_x, int max_x, int min_y, int max_y, int level, bool is_break)
        : min_x(min_x)
        , max_x(max_x)
        , min_y(min_y)
        , max_y(max_y)
        , level(level)
        , is_break(is_break)
        {
        }
        void KeyRegion::LowDetailed(KeyRegion * out) const
        {
            if (level > 0)
            {
                out->level = level - 1;
                out->min_x = min_x >> 1;
                out->max_x = max_x >> 1;
                out->min_y = min_y >> 1;
                out->max_y = max_y >> 1;
                out->is_break = is_break;
            }
            else
            {
                *out = *this;
            }
        }
        void KeyRegion::HighDetailed(KeyRegion * out) const
        {
            out->level = level + 1;
            out->min_x = min_x << 1;
            out->max_x = max_x << 1;
            out->min_y = min_y << 1;
            out->max_y = max_y << 1;
            out->is_break = is_break;
        }
        bool KeyRegion::IsKeyIn(const DataKey& key) const
        {
            int key_x = key.x();
            int key_y = key.y();
            bool in_x_range = (is_break) ? (key_x >= min_x || key_x <= max_x)
                                         : (key_x >= min_x && key_x <= max_x);
            return (key.z() == level)
                && (key_y >= min_y) && (key_y <= max_y)
                && in_x_range;
        }
        bool operator == (const KeyRegion& lhs, const KeyRegion& rhs)
        {
            return memcmp(&lhs, &rhs, sizeof(KeyRegion)) == 0;
        }
        bool operator < (const KeyRegion& lhs, const KeyRegion& rhs)
        {
            return memcmp(&lhs, &rhs, sizeof(KeyRegion)) < 0;
        }
        
    } // namespace satellite_imagery
} // namespace sht