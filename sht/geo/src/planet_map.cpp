#include "planet_map.h"
#include "planet_map_tile.h"
#include "planet_tree.h"

#include "../../graphics/include/renderer/renderer.h"

#include "../../../thirdparty/libsaim/include/saim.h"

namespace sht {
	namespace geo {

		PlanetMap::PlanetMap(graphics::Renderer * renderer)
			: renderer_(renderer)
			, image_(nullptr)
			, albedo_texture_(nullptr)
			, step_(0)
		{
		}
		PlanetMap::~PlanetMap()
		{
			Deinitialize();
			if (image_)
				delete image_;
		}
		bool PlanetMap::Initialize()
		{
			if (saim_init("", nullptr, 0) != 0)
				return false;
			// Create image object
			image_ = new graphics::Image();
			image_->Allocate(256, 256, graphics::Image::Format::kRGB8);
			memset(image_->pixels(), 0xFF, image_->width() * image_->height() * image_->bpp()); // temporary
			// Some library settings
			saim_set_bitmap_cache_size(50);
			saim_set_target(image_->pixels(), image_->width(), image_->height(), image_->bpp());
			return true;
		}
		void PlanetMap::Deinitialize()
		{
			saim_cleanup();
		}
		void PlanetMap::ResetTile()
		{
			if (step_ > 1)
				renderer_->DeleteTexture(albedo_texture_);
			step_ = 0;
		}
		bool PlanetMap::PrepareTile(PlanetTreeNode * node)
		{
			bool step_done = true;
			switch (step_)
			{
			case 0:
				// Albedo texture generation step
				{
					const float inv_scale = 2.0f / (float)(1 << node->lod_);
					const float position_x = -1.f + inv_scale * node->x_;
					const float position_y = -1.f + inv_scale * node->y_;

					// Calculate corner point normals
					double u_min = static_cast<double>(position_x);
					double v_min = static_cast<double>(position_y);
					double u_max = static_cast<double>(position_x + inv_scale);
					double v_max = static_cast<double>(position_y + inv_scale);
					// Render
					if (saim_render_mapped_cube(node->owner_->face_, u_min, v_min, u_max, v_max) > 0)
						step_done = false;
				}
				break;
			case 1:
				// Albedo texture creation step
				renderer_->AddTextureFromImage(albedo_texture_, *image_, graphics::Texture::Wrap::kClampToEdge);
				break;
			case 2:
				// Height texture generation step
				// TODO
				break;
			}
			if (step_done)
				++step_;
			return step_ > 2;
		}
		PlanetMapTile * PlanetMap::FinalizeTile(PlanetTreeNode * node)
		{
			step_ = 0;
			return new PlanetMapTile(this, node, albedo_texture_);
		}
		graphics::Renderer * PlanetMap::renderer()
		{
			return renderer_;
		}

	} // namespace geo
} // namespace sht