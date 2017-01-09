#include "earth_service.h"

#include "../../sht/thirdparty/libsaim/include/saim.h"

EarthService::EarthService()
{
}
EarthService::~EarthService()
{
}
bool EarthService::Initialize()
{
	if (saim_init("", nullptr, 0) != 0)
		return false;
	// Create image object
	image_.Allocate(256, 256, sht::graphics::Image::Format::kRGB8);
	// Some library settings
	saim_set_bitmap_cache_size(50);
	saim_set_target(image_.pixels(), image_.width(), image_.height(), image_.bpp());
	return true;
}
void EarthService::Deinitialize()
{
	saim_cleanup();
}
bool EarthService::Execute()
{
	int face, lod, x, y;
	ObtainTileParameters(&face, &lod, &x, &y);

	const float inv_scale = 2.0f / (float)(1 << lod);
	const float position_x = -1.f + inv_scale * x;
	const float position_y = -1.f + inv_scale * y;

	double u_min = static_cast<double>(position_x);
	double v_min = static_cast<double>(position_y);
	double u_max = static_cast<double>(position_x + inv_scale);
	double v_max = static_cast<double>(position_y + inv_scale);

	// Render
	return saim_render_mapped_cube(face, u_min, v_min, u_max, v_max) == 0;
}