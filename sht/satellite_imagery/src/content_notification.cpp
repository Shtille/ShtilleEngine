#include "../include/content_notification.h"

namespace sht {
    namespace satellite_imagery {

        ContentNotification::ContentNotification(const DataKey& key, const Content& content, bool success)
            : key_(key)
            , content_(content)
            , success_(success)
        {
        }
        const DataKey& ContentNotification::key() const
        {
            return key_;
        }
        const Content& ContentNotification::content() const
        {
            return content_;
        }
        const bool ContentNotification::success() const
        {
            return success_;
        }

    } // namespace satellite_imagery
} // namespace sht