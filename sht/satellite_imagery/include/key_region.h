#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_KEY_REGION_H__
#define __SHT_SATELLITE_IMAGERY_KEY_REGION_H__

namespace sht {
    namespace satellite_imagery {

        class DataKey;
    
        //! Key region
        // NOTE: this region works only with one or none breaks
        class KeyRegion {
        public:
            int min_x;
            int max_x;
            int min_y;
            int max_y;
            int level;
            bool is_break;

            KeyRegion();
            KeyRegion(int min_x, int max_x, int min_y, int max_y, int level, bool is_break);
            
            void LowDetailed(KeyRegion * out) const;
            void HighDetailed(KeyRegion * out) const;

            bool IsKeyIn(const DataKey& key) const;
            
            friend bool operator == (const KeyRegion& lhs, const KeyRegion& rhs);
            friend bool operator < (const KeyRegion& lhs, const KeyRegion& rhs);
        };
        
    } // namespace satellite_imagery
} // namespace sht

#endif