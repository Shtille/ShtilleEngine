#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CONTENT_NOTIFICATION_H__
#define __SHT_SATELLITE_IMAGERY_CONTENT_NOTIFICATION_H__

#include "../../common/non_copyable.h"

namespace sht {
    namespace satellite_imagery {

    	// Forward declarations
    	class RawKey;
    	class Content;

    	//! Content notification class
    	class ContentNotification : public NonCopyable {
    	public:
    		explicit ContentNotification(const RawKey& key, const Content& content);

    		const RawKey& key() const;
    		const Content& content() const;

    	private:
			const RawKey& key_;
			const Content& content_;
    	};

    } // namespace satellite_imagery
} // namespace sht

#endif