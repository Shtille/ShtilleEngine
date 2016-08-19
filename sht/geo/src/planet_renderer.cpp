#include "../include/planet_renderer.h"
#include "chunk_tree.h"
#include "chunk_loader.h"

namespace sht {
	namespace geo {

		PlanetRenderer::PlanetRenderer()
		{
			loader_ = new ChunkLoader();
			for (int i = 0; i < kNumSides; ++i)
				side_[i] = new ChunkTree(i, loader_);
		}
		PlanetRenderer::~PlanetRenderer()
		{
			for (int i = 0; i < kNumSides; ++i)
				delete side_[i];
			delete loader_;
		}
		void PlanetRenderer::Update()
		{
			//for (int i = 0; i < kNumSides; ++i)
				side_[0]->Update();

			// Only one mutex-protected call per frame
			loader_->SyncLoaderThread();
		}
		void PlanetRenderer::Render()
		{
			//for (int i = 0; i < kNumSides; ++i)
				side_[0]->Render();
		}

	} // namespace geo
} // namespace sht