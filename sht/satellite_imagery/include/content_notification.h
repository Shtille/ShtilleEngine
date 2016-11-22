#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CONTENT_NOTIFICATION_H__
#define __SHT_SATELLITE_IMAGERY_CONTENT_NOTIFICATION_H__

#include "key.h"
#include "../../common/notification.h"

namespace sht {
    namespace satellite_imagery {

    	// Forward declarations
    	//class DataKey;
    	class Content;

    	//! Content notification class
    	class ContentNotification : public NotificationInterface {
    	public:
    		explicit ContentNotification(const DataKey& key, const Content& content, bool success);

    		const DataKey& key() const;
    		const Content& content() const;
            const bool success() const;

    	private:
			const DataKey& key_;
			const Content& content_;
            bool success_;
    	};

    } // namespace satellite_imagery
} // namespace sht

#endif