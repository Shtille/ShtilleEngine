#include "../../include/image/image.h"
#include "../../../system/include/stream/file_stream.h"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace {
	typedef unsigned char RGBE[4];
	constexpr int R = 0;
	constexpr int G = 1;
	constexpr int B = 2;
	constexpr int E = 3;
	constexpr int MINELEN = 8; // minimum scanline length for encoding
	constexpr int MAXELEN = 0x7fff; // maximum scanline length for encoding

	float ConvertComponent(int expo, int val)
	{
		float v = val / 256.0f;
		float d = powf(2.0f, (float)expo);
		return v * d;
	}

	void WorkOnRGBE(RGBE *scan, int len, float *cols)
	{
		while (len-- > 0)
		{
			int expo = scan[0][E] - 128;
			cols[0] = ConvertComponent(expo, scan[0][R]);
			cols[1] = ConvertComponent(expo, scan[0][G]);
			cols[2] = ConvertComponent(expo, scan[0][B]);
			cols += 3;
			scan++;
		}
	}

	bool OldDecrunch(RGBE *scanline, int len, FILE *file)
	{
		int i;
		int rshift = 0;
		
		while (len > 0)
		{
			scanline[0][R] = fgetc(file);
			scanline[0][G] = fgetc(file);
			scanline[0][B] = fgetc(file);
			scanline[0][E] = fgetc(file);
			if (feof(file))
				return false;

			if (scanline[0][R] == 1 &&
				scanline[0][G] == 1 &&
				scanline[0][B] == 1)
			{
				for (i = scanline[0][E] << rshift; i > 0; i--)
				{
					memcpy(&scanline[0][0], &scanline[-1][0], 4);
					scanline++;
					len--;
				}
				rshift += 8;
			}
			else
			{
				scanline++;
				len--;
				rshift = 0;
			}
		}
		return true;
	}

	bool Decrunch(RGBE *scanline, int len, FILE *file)
	{
		int  i, j;
						
		if (len < MINELEN || len > MAXELEN)
			return OldDecrunch(scanline, len, file);

		i = fgetc(file);
		if (i != 2)
		{
			fseek(file, -1, SEEK_CUR);
			return OldDecrunch(scanline, len, file);
		}

		scanline[0][G] = fgetc(file);
		scanline[0][B] = fgetc(file);
		i = fgetc(file);

		if (scanline[0][G] != 2 || scanline[0][B] & 128)
		{
			scanline[0][R] = 2;
			scanline[0][E] = i;
			return OldDecrunch(scanline + 1, len - 1, file);
		}

		// read each component
		for (i = 0; i < 4; i++)
		{
		    for (j = 0; j < len; )
		    {
				unsigned char code = fgetc(file);
				if (code > 128)
				{ // run
				    code &= 127;
				    unsigned char val = fgetc(file);
				    while (code--)
						scanline[j++][i] = val;
				}
				else
				{	// non-run
				    while(code--)
						scanline[j++][i] = fgetc(file);
				}
			}
	    }

		return feof(file) ? false : true;
	}
}

namespace sht {
	namespace graphics {

		bool Image::SaveHdr(const char *filename)
		{
			assert(!"not implemented yet");
			return true;
		}
		bool Image::LoadHdr(const char *filename)
		{
			int i;
			char str[200];
			FILE *file;

			file = fopen(filename, "rb");
			if (!file)
				return false;

			fread(str, 10, 1, file);
			if (memcmp(str, "#?RADIANCE", 10)) {
				fclose(file);
				return false;
			}

			fseek(file, 1, SEEK_CUR);

			char cmd[200];
			i = 0;
			char c = 0, oldc;
			while(true) {
				oldc = c;
				c = fgetc(file);
				if (c == 0xa && oldc == 0xa)
					break;
				cmd[i++] = c;
			}

			char reso[200];
			i = 0;
			while(true) {
				c = fgetc(file);
				reso[i++] = c;
				if (c == 0xa)
					break;
			}

			int w, h;
			if (!sscanf(reso, "-Y %i +X %i", &h, &w))
			{
				fclose(file);
				return false;
			}

			width_ = w;
			height_ = h;
			channels_ = 3;
			data_type_ = DataType::kFloat;
			bpp_ = channels_ * sizeof(float);
			format_ = Format::kRGB32;

			float *cols = new float[width_ * height_ * bpp_];
			pixels_ = reinterpret_cast<u8*>(cols);

			RGBE *scanline = new RGBE[w];
			if (!scanline)
			{
				fclose(file);
				return false;
			}

			// convert image 
			for (int y = h - 1; y >= 0; y--) {
				if (Decrunch(scanline, w, file) == false)
					break;
				WorkOnRGBE(scanline, w, cols);
				cols += w * 3;
			}

			delete [] scanline;
			fclose(file);

			return true;
		}
		bool Image::LoadFromBufferHdr(const u8* buffer, size_t length)
		{
			assert(!"haven't been implemented yet");
			return true;
		}

	} // namespace graphics
} // namespace sht