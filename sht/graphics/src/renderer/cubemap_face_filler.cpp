#include "../../include/renderer/cubemap_face_filler.h"

#include "../../include/image/image.h"
#include "math/sht_math.h"

#include <cstring>
#include <cmath>

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

			if (w != h)
				return false;

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

		static math::Matrix3 GetFaceTransform(int face)
		{
			switch (face)
			{
			default:
			case 0: // +X
				return math::Matrix3(
					0.0f, 0.0f, 1.0f,
					0.0f, -1.0f, 0.0f,
					1.0f, 0.0f, 0.0f);
			case 1: // -X
				return math::Matrix3(
					0.0f, 0.0f, -1.0f,
					0.0f, -1.0f, 0.0f,
					-1.0f, 0.0f, 0.0f);
			case 2: // +Y
				return math::Matrix3(
					1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1.0f,
					0.0f, 1.0f, 0.0f);
			case 3: // -Y
				return math::Matrix3(
					1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f,
					0.0f, -1.0f, 0.0f);
			case 4: // +Z
				return math::Matrix3(
					1.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f,
					0.0f, 0.0f, -1.0f);
			case 5: // -Z
				return math::Matrix3(
					-1.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f,
					0.0f, 0.0f, 1.0f);
			}
		}
		SphereCubemapFaceFiller::SphereCubemapFaceFiller(Image * source_image, int face_width)
		: CubemapFaceFiller(source_image)
		, face_width_(face_width)
		{
		}
		bool SphereCubemapFaceFiller::Fill(int face, Image * image)
		{
			int w = face_width_;
			int h = face_width_;

			image->Allocate(w, h, source_image_->format());

			unsigned char * src_pixels = source_image_->pixels();
			unsigned char * dst_pixels = image->pixels();

			int bpp = source_image_->bpp();

			math::Matrix3 face_matrix = GetFaceTransform(face);
			for (int j = 0; j < h; ++j)
			{
				float y = (float)j / (float)(h-1);
				for (int i = 0; i < w; ++i)
				{
					float x = (float)i / (float)(w-1);
					vec3 face_point = face_matrix * vec3(2.0f * x - 1.0f, 2.0f * y - 1.0f, 1.0f);
					face_point.Normalize();
					float angle_x = (atan2(face_point.z, face_point.x) * 0.5f / math::kPi) + 0.5f; // [-Pi; Pi] -> [0; 1]
					float angle_y = (asin(-face_point.y) / math::kPi) + 0.5f; // [-Pi/2; Pi/2] -> [0; 1]
					int src_i = static_cast<int>(angle_x * (float)source_image_->width());
					int src_j = static_cast<int>(angle_y * (float)source_image_->height());
					src_i = math::Clamp(src_i, 0, source_image_->width() - 1);
					src_j = math::Clamp(src_j, 0, source_image_->height() - 1);
					memcpy(
						dst_pixels + bpp * (w * j + i),
						src_pixels + bpp * (source_image_->width() * src_j + src_i),
						bpp); // copy sample
				}
			}

			return true;
		}

	} // namespace graphics
} // namespace sht