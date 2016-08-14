#include "../include/content.h"

namespace sht {
    namespace satellite_imagery {

        const std::string& Content::data() const
        {
            return data_;
        }
        std::string& Content::data()
        {
            return data_;
        }

    } // namespace satellite_imagery
} // namespace sht