#pragma once
#ifndef __SHT_GEO_PLANET_CUBE_H__
#define __SHT_GEO_PLANET_CUBE_H__

#include "../src/planet_tree.h"

#include "../../common/non_copyable.h"
#include "../../utility/include/camera.h"

#include <list>
#include <set>
#include <queue>

namespace sht {
	namespace graphics {
		class Renderer;
		class Shader;
	}
}

namespace sht {
	namespace geo {

		// Forward declarations
		class PlanetTileMesh;
		class PlanetMap;
		class PlanetMapTile;

		struct LodParams {
			int limit;

			math::Vector3 camera_position;
			math::Vector3 camera_front;

			math::Vector3 sphere_plane;
			float sphere_clip;

			float geo_factor;
			float geo_factor_squared;

			float tex_factor;
			float tex_factor_squared;
		};

		//! Planet rendering class
		class PlanetCube final : public NonCopyable {
			friend class PlanetTreeNode;
			friend class PlanetRenderable;

			enum {
				REQUEST_RENDERABLE,
				REQUEST_MAPTILE,
				REQUEST_SPLIT,
				REQUEST_MERGE,
			};

			struct RequestType {
				PlanetTreeNode* mNode;
				int mType;

				RequestType(PlanetTreeNode* node, int type) : mNode(node), mType(type) {};
			};

			class RequestComparePriority {
			public:
				bool operator()(const RequestType& a, const RequestType& b) const;
			};

			typedef std::list<RequestType> RequestQueue;
			typedef std::set<PlanetTreeNode*> NodeSet;
			typedef std::priority_queue<PlanetTreeNode*, std::vector<PlanetTreeNode*>, PlanetTreeNodeCompareLastOpened> NodeHeap;

		public:
			PlanetCube(graphics::Renderer * renderer, graphics::Shader * shader,
				utility::CameraManager * camera, float radius);
			~PlanetCube();

			//! Data video memory objects creation and other things that may fail
			bool Initialize();

			void SetParameters(float fovy_in_radians, int screen_height);

			void Update();
			void Render();

			const float radius() const;
			const int grid_size() const;

			const int GetLodLimit() const;
			const int GetTextureSize() const;

			static math::Matrix3 GetFaceTransform(int face);
			int GetFrameCounter() const;

		protected:
			void SplitQuadTreeNode(PlanetTreeNode* node);
			void MergeQuadTreeNode(PlanetTreeNode* node);
			void Request(PlanetTreeNode* node, int type, bool priority = false);
			void Unrequest(PlanetTreeNode* node);
			void HandleRequests(RequestQueue& requests);
			void HandleRenderRequests();
			void HandleInlineRequests();

			bool HandleRenderable(PlanetTreeNode* node);
			bool HandleMapTile(PlanetTreeNode* node);
			bool HandleSplit(PlanetTreeNode* node);
			bool HandleMerge(PlanetTreeNode* node);

			void PruneTree();
			void RefreshMapTile(PlanetTreeNode* node, PlanetMapTile* tile);

		private:
			graphics::Shader * shader_;
			utility::CameraManager * camera_;

			static constexpr int kNumFaces = 6;
			PlanetTree * faces_[kNumFaces];

			const int grid_size_;
			const float radius_; //!< planet radius
			PlanetTileMesh * tile_;
			PlanetMap * map_;

			RequestQueue inline_requests_;
			RequestQueue render_requests_;
			NodeSet open_nodes_;

			LodParams lod_params_;

			int frame_counter_;
			bool lod_freeze_;
			bool tree_freeze_;
		};

	} // namespace geo
} // namespace sht

#endif