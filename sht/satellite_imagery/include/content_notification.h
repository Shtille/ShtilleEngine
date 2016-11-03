#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CONTENT_NOTIFICATION_H__
#define __SHT_SATELLITE_IMAGERY_CONTENT_NOTIFICATION_H__

namespace sht {
    namespace satellite_imagery {

    	// Forward declarations
    	class DataKey;
    	class Content;

    	//! Content notification class
    	class ContentNotification {
    	public:
    		explicit ContentNotification(const DataKey& key, const Content& content);

    		const DataKey& key() const;
    		const Content& content() const;

    	private:
			const DataKey& key_;
			const Content& content_;
    	};

    } // namespace satellite_imagery
} // namespace sht

#endif