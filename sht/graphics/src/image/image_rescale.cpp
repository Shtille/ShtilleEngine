#include "../../include/image/image.h"

#include <assert.h>
#include <algorithm>
#include <memory.h>

// static functions
namespace {
	template <typename T>
	T RoundToPowerOfTwo(T x)
	{
		T n = 1;
		while (n < x)
			n <<= 1;
		return n;
	}
	void InterpolateSamplesR8(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		unsigned char r1 = c1[0];
		unsigned char r2 = c2[0];
		unsigned char r = r1 + static_cast<unsigned char>(static_cast<float>(r2-r1) * alpha);
		out[0] = r;
	}
	void InterpolateSamplesRG8(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		unsigned char r1 = c1[0];
		unsigned char g1 = c1[1];
		unsigned char r2 = c2[0];
		unsigned char g2 = c2[1];
		unsigned char r = r1 + static_cast<unsigned char>(static_cast<float>(r2-r1) * alpha);
		unsigned char g = g1 + static_cast<unsigned char>(static_cast<float>(g2-g1) * alpha);
		out[0] = r;
		out[1] = g;
	}
	void InterpolateSamplesRGB8(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		unsigned char r1 = c1[0];
		unsigned char g1 = c1[1];
		unsigned char b1 = c1[2];
		unsigned char r2 = c2[0];
		unsigned char g2 = c2[1];
		unsigned char b2 = c2[2];
		unsigned char r = r1 + static_cast<unsigned char>(static_cast<float>(r2-r1) * alpha);
		unsigned char g = g1 + static_cast<unsigned char>(static_cast<float>(g2-g1) * alpha);
		unsigned char b = b1 + static_cast<unsigned char>(static_cast<float>(b2-b1) * alpha);
		out[0] = r;
		out[1] = g;
		out[2] = b;
	}
	void InterpolateSamplesRGBA8(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		unsigned char r1 = c1[0];
		unsigned char g1 = c1[1];
		unsigned char b1 = c1[2];
		unsigned char a1 = c1[3];
		unsigned char r2 = c2[0];
		unsigned char g2 = c2[1];
		unsigned char b2 = c2[2];
		unsigned char a2 = c2[3];
		unsigned char r = r1 + static_cast<unsigned char>(static_cast<float>(r2-r1) * alpha);
		unsigned char g = g1 + static_cast<unsigned char>(static_cast<float>(g2-g1) * alpha);
		unsigned char b = b1 + static_cast<unsigned char>(static_cast<float>(b2-b1) * alpha);
		unsigned char a = a1 + static_cast<unsigned char>(static_cast<float>(a2-a1) * alpha);
		out[0] = r;
		out[1] = g;
		out[2] = b;
		out[3] = a;
	}
	void InterpolateSamplesR16(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		assert(!"not implemented yet");
	}
	void InterpolateSamplesRG16(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		assert(!"not implemented yet");
	}
	void InterpolateSamplesRGB16(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		assert(!"not implemented yet");
	}
	void InterpolateSamplesRGBA16(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		assert(!"not implemented yet");
	}
	void InterpolateSamplesR32(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		float r1 = reinterpret_cast<const float*>(c1)[0];
		float r2 = reinterpret_cast<const float*>(c2)[0];
		float r = r1 + (r2-r1) * alpha;
		reinterpret_cast<float*>(out)[0] = r;
	}
	void InterpolateSamplesRG32(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		float r1 = reinterpret_cast<const float*>(c1)[0];
		float g1 = reinterpret_cast<const float*>(c1)[1];
		float r2 = reinterpret_cast<const float*>(c2)[0];
		float g2 = reinterpret_cast<const float*>(c2)[1];
		float r = r1 + (r2-r1) * alpha;
		float g = g1 + (g2-g1) * alpha;
		reinterpret_cast<float*>(out)[0] = r;
		reinterpret_cast<float*>(out)[1] = g;
	}
	void InterpolateSamplesRGB32(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		float r1 = reinterpret_cast<const float*>(c1)[0];
		float g1 = reinterpret_cast<const float*>(c1)[1];
		float b1 = reinterpret_cast<const float*>(c1)[2];
		float r2 = reinterpret_cast<const float*>(c2)[0];
		float g2 = reinterpret_cast<const float*>(c2)[1];
		float b2 = reinterpret_cast<const float*>(c2)[2];
		float r = r1 + (r2-r1) * alpha;
		float g = g1 + (g2-g1) * alpha;
		float b = b1 + (b2-b1) * alpha;
		reinterpret_cast<float*>(out)[0] = r;
		reinterpret_cast<float*>(out)[1] = g;
		reinterpret_cast<float*>(out)[2] = b;
	}
	void InterpolateSamplesRGBA32(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out)
	{
		float r1 = reinterpret_cast<const float*>(c1)[0];
		float g1 = reinterpret_cast<const float*>(c1)[1];
		float b1 = reinterpret_cast<const float*>(c1)[2];
		float a1 = reinterpret_cast<const float*>(c1)[3];
		float r2 = reinterpret_cast<const float*>(c2)[0];
		float g2 = reinterpret_cast<const float*>(c2)[1];
		float b2 = reinterpret_cast<const float*>(c2)[2];
		float a2 = reinterpret_cast<const float*>(c2)[3];
		float r = r1 + (r2-r1) * alpha;
		float g = g1 + (g2-g1) * alpha;
		float b = b1 + (b2-b1) * alpha;
		float a = a1 + (a2-a1) * alpha;
		reinterpret_cast<float*>(out)[0] = r;
		reinterpret_cast<float*>(out)[1] = g;
		reinterpret_cast<float*>(out)[2] = b;
		reinterpret_cast<float*>(out)[3] = a;
	}
	void InterpolateSamples(const unsigned char* c1, const unsigned char* c2, float alpha, unsigned char* out, sht::graphics::Image::Format format)
	{
		using namespace sht::graphics;
		switch (format)
		{
		case Image::Format::kI8:
		case Image::Format::kA8:
		case Image::Format::kL8:
		case Image::Format::kR8:
			InterpolateSamplesR8(c1, c2, alpha, out);
			break;
		case Image::Format::kLA8:
		case Image::Format::kRG8:
			InterpolateSamplesRG8(c1, c2, alpha, out);
			break;
		case Image::Format::kRGB8:
			InterpolateSamplesRGB8(c1, c2, alpha, out);
			break;
		case Image::Format::kRGBA8:
			InterpolateSamplesRGBA8(c1, c2, alpha, out);
			break;

		case Image::Format::kI16:
		case Image::Format::kA16:
		case Image::Format::kL16:
		case Image::Format::kR16:
			InterpolateSamplesR16(c1, c2, alpha, out);
			break;
		case Image::Format::kLA16:
		case Image::Format::kRG16:
			InterpolateSamplesRG16(c1, c2, alpha, out);
			break;
		case Image::Format::kRGB16:
			InterpolateSamplesRGB16(c1, c2, alpha, out);
			break;
		case Image::Format::kRGBA16:
			InterpolateSamplesRGBA16(c1, c2, alpha, out);
			break;

		case Image::Format::kI32:
		case Image::Format::kA32:
		case Image::Format::kL32:
		case Image::Format::kR32:
			InterpolateSamplesR32(c1, c2, alpha, out);
			break;
		case Image::Format::kLA32:
		case Image::Format::kRG32:
			InterpolateSamplesRG32(c1, c2, alpha, out);
			break;
		case Image::Format::kRGB32:
			InterpolateSamplesRGB32(c1, c2, alpha, out);
			break;
		case Image::Format::kRGBA32:
			InterpolateSamplesRGBA32(c1, c2, alpha, out);
			break;

		default:
			assert(!"not implemented yet");
			break;
		}
	}
}

namespace sht {
	namespace graphics {

		void Image::Rescale(int w, int h)
		{
			if (width_ == w && height_ == h)
				return;

			unsigned char * new_data = new unsigned char[w * h * bpp_];
			unsigned char * samples = new unsigned char[bpp_ * 7];

			for (int dh2 = 0; dh2 < h; ++dh2)
			{
				float rh = (float)dh2 / (float)h;
				float y = rh * (float)height_;
				int dh = (int)y;
				int dh1 = std::min<int>(dh + 1, height_ - 1);
				float ry = y - (float)dh; // fract part of y
				for (int dw2 = 0; dw2 < w; ++dw2)
				{
					float rw = (float)dw2 / (float)w;
					float x = rw * (float)width_;
					int dw = (int)x;
					int dw1 = std::min<int>(dw + 1, width_ - 1);
					float rx = x - (float)dw; // fract part of x

					// We will use bilinear interpolation
					memcpy(samples           , pixels_ + ((dw  + width_ * dh ) * bpp_), bpp_);
					memcpy(samples + bpp_    , pixels_ + ((dw1 + width_ * dh ) * bpp_), bpp_);
					memcpy(samples + bpp_ * 2, pixels_ + ((dw  + width_ * dh1) * bpp_), bpp_);
					memcpy(samples + bpp_ * 3, pixels_ + ((dw1 + width_ * dh1) * bpp_), bpp_);

					InterpolateSamples(samples           , samples + bpp_    , rx, samples + bpp_ * 4, format_);
					InterpolateSamples(samples + bpp_ * 2, samples + bpp_ * 3, rx, samples + bpp_ * 5, format_);
					InterpolateSamples(samples + bpp_ * 4, samples + bpp_ * 5, ry, samples + bpp_ * 6, format_);
					memcpy(new_data + ((dw2 + w * dh2) * bpp_), samples + bpp_ * 6, bpp_);
				}
			}

			delete[] samples;
			delete[] pixels_;
			pixels_ = new_data;
			width_ = w;
			height_ = h;
		}
		void Image::MakePowerOfTwo()
		{
			if (((width_ & (width_-1)) != 0) || ((height_ & (height_-1)) != 0))
			{
				int w2 = RoundToPowerOfTwo(width_);
				int h2 = RoundToPowerOfTwo(height_);
				unsigned char * new_data = new unsigned char[w2 * h2 * bpp_];
				unsigned char * samples = new unsigned char[bpp_ * 7];

				for (int dh2 = 0; dh2 < h2; ++dh2)
				{
					float rh = (float)dh2 / (float)h2;
					float y = rh * (float)height_;
					int dh = (int)y;
					int dh1 = std::min<int>(dh + 1, height_ - 1);
					float ry = y - (float)dh; // fract part of y
					for (int dw2 = 0; dw2 < w2; ++dw2)
					{
						float rw = (float)dw2 / (float)w2;
						float x = rw * (float)width_;
						int dw = (int)x;
						int dw1 = std::min<int>(dw + 1, width_ - 1);
						float rx = x - (float)dw; // fract part of x

						// We will use bilinear interpolation
						memcpy(samples           , pixels_ + ((dw  + width_ * dh ) * bpp_), bpp_);
						memcpy(samples + bpp_    , pixels_ + ((dw1 + width_ * dh ) * bpp_), bpp_);
						memcpy(samples + bpp_ * 2, pixels_ + ((dw  + width_ * dh1) * bpp_), bpp_);
						memcpy(samples + bpp_ * 3, pixels_ + ((dw1 + width_ * dh1) * bpp_), bpp_);

						InterpolateSamples(samples           , samples + bpp_    , rx, samples + bpp_ * 4, format_);
						InterpolateSamples(samples + bpp_ * 2, samples + bpp_ * 3, rx, samples + bpp_ * 5, format_);
						InterpolateSamples(samples + bpp_ * 4, samples + bpp_ * 5, ry, samples + bpp_ * 6, format_);
						memcpy(new_data + ((dw2 + w2 * dh2) * bpp_), samples + bpp_ * 6, bpp_);
					}
				}

				delete[] samples;
				delete[] pixels_;
				pixels_ = new_data;
				width_ = w2;
				height_ = h2;
			}
		}

	} // namespace graphics
} // namespace sht