#include "../include/content_notification.h"

namespace sht {
    namespace satellite_imagery {

        ContentNotification::ContentNotification(const RawKey& key, const Content& content)
            : key_(key)
            , content_(content)
        {
        }
        const RawKey& ContentNotification::key() const
        {
            return key_;
        }
        const Content& ContentNotification::content() const
        {
            return content_;
        }

    } // namespace satellite_imagery
} // namespace sht