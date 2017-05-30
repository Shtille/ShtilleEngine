#pragma once
#ifndef __SHT_GRAPHICS_RESOURCE_H__
#define __SHT_GRAPHICS_RESOURCE_H__

namespace sht {
	namespace graphics {

		// Base graphics resource class for using in resource manager
		class Resource
		{
		public:
			virtual ~Resource() = default;
		};

	} // namespace graphics 
} // namespace sht

#endif