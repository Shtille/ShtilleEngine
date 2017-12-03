#include "../../include/renderer/cubemap_face_filler.h"

#include "../../include/image/image.h"

#include <cstring>

namespace sht {
	namespace graphics {

		CubemapFaceFiller::CubemapFaceFiller(Image * source_image)
		: source_image_(source_image)
		{

		}
		CubemapFaceFiller::~CubemapFaceFiller()
		{
		}

		CrossCubemapFaceFiller::CrossCubemapFaceFiller(Image * source_image)
		: CubemapFaceFiller(source_image)
		{
		}
		bool CrossCubemapFaceFiller::Fill(int face, Image * image)
		{
			int w = source_image_->width() / 4;
			int h = source_image_->height() / 3;

			image->Allocate(w, h, source_image_->format());

			unsigned char * src_pixels = source_image_->pixels();
			unsigned char * dst_pixels = image->pixels();

			// Texture offset by width and height in pixels
			int dw = 0, dh = 0;

			// Pixels are stored from left bottom corner to right top
			switch (face)
			{
			case 0: // +x front
				dw = w;
				dh = h;
				break;
			case 1: // -x back
				dw = 3 * w;
				dh = h;
				break;
			case 2: // +y up
				dw = 0;
				dh = 0;
				break;
			case 3: // -y down
				dw = 0;
				dh = 2 * h;
				break;
			case 4: // +z right
				dw = 0;
				dh = h;
				break;
			case 5: // -z left
				dw = 2 * w;
				dh = h;
				break;
			}

			int bpp = source_image_->bpp();

			// Copy pixel data
			for (int j = 0; j < h; ++j)
				memcpy(
					dst_pixels + (bpp * (w * j)),
					src_pixels + (bpp * (source_image_->width() * (dh + j) + dw)),
					bpp * w); // copy line

			return true;
		}

		SphereCubemapFaceFiller::SphereCubemapFaceFiller(Image * source_image)
		: CubemapFaceFiller(source_image)
		{
		}
		bool SphereCubemapFaceFiller::Fill(int face, Image * image)
		{
			// TODO
			return true;
		}

	} // namespace graphics
} // namespace sht