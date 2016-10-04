#pragma once
#ifndef __SHT_GEO_PLANET_TREE_H__
#define __SHT_GEO_PLANET_TREE_H__

#include "../../math/vector.h"
#include "../../common/non_copyable.h"

namespace sht {
	namespace geo {

		// Forward declarations
		class PlanetTree;
		class PlanetCube;
		class PlanetMap;
		class PlanetMapTile;
		class PlanetRenderable;

		//! Planet tree node class
		class PlanetTreeNode : public NonCopyable {
			friend class PlanetCube;
			friend class PlanetRenderable;
			friend class PlanetTreeNodeCompareLastOpened;
		public:
			enum Slot { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

			explicit PlanetTreeNode(PlanetTree * tree);
			virtual ~PlanetTreeNode();

			const float GetPriority() const;

			bool IsSplit();

			void AttachChild(PlanetTreeNode * child, int position);
			void DetachChild(int position);

			void PropagateLodDistances();

			bool PrepareMapTile(PlanetMap* map);
			void CreateMapTile(PlanetMap* map);
			void DestroyMapTile();

			void CreateRenderable(PlanetMapTile* map);
			void DestroyRenderable();

			bool WillRender();
			int Render();
			void RenderSelf();

		private:
			PlanetTree * owner_; //!< owner face tree

			PlanetMapTile * map_tile_;
			PlanetRenderable * renderable_;

			int lod_; //!< level of detail
			int x_;
			int y_;

			int last_rendered_;
			int last_opened_;

			bool has_children_;
			bool page_out_;

			bool request_page_out_;
			bool request_map_tile_;
			bool request_renderable_;
			bool request_split_;
			bool request_merge_;

			int parent_slot_;
			static constexpr int kNumChildren = 4;
			PlanetTreeNode * parent_;
			PlanetTreeNode * children_[kNumChildren];
		};

		//! Planet tree class
		class PlanetTree : public NonCopyable {
			friend class PlanetTreeNode;
			friend class PlanetRenderable;
		public:
			explicit PlanetTree(PlanetCube * cube, int face);
			virtual ~PlanetTree();

			void Render();

		private:
			PlanetCube * cube_;
			PlanetTreeNode * root_;
			int face_;
		};

		// Quadtree node comparison for LOD age.
		class PlanetTreeNodeCompareLastOpened {
		public:
			bool operator()(const PlanetTreeNode* a, const PlanetTreeNode* b) const {
				return (a->last_opened_ > b->last_opened_);
			}
		};

	} // namespace geo
} // namespace sht

#endif