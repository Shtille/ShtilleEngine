#pragma once
#ifndef __SHT_GEO_PLANET_TREE_H__
#define __SHT_GEO_PLANET_TREE_H__

#include "../../math/vector.h"
#include "../../common/non_copyable.h"

namespace sht {
	namespace geo {

		class PlanetCube;

		class QuadTreeNode {
		public:
			enum Slot { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

			QuadTreeNode(PlanetCube * cube);
			~QuadTreeNode();

			void AttachChild(QuadTreeNode* child, int position);
			void DetachChild(int position);
			bool IsSplit();

			void PropagateLodDistances();

			const double getPriority() const;

			bool PrepareMapTile(PlanetMap* map);
			void CreateMapTile(PlanetMap* map);
			void DestroyMapTile();

			void CreateRenderable(PlanetMapTile* map);
			void DestroyRenderable();

			bool WillRender();
			int Render(RenderQueue* queue, PlanetLODConfiguration& lod);

		private:
			int mFace;
			int mLOD;
			int mX;
			int mY;

			int mLastRendered;
			int mLastOpened;

			bool mHasChildren;

			bool mPageOut;

			bool mRequestPageOut;
			bool mRequestMapTile;
			bool mRequestRenderable;
			bool mRequestSplit;
			bool mRequestMerge;

			PlanetMapTile* mMapTile;
			PlanetRenderable* mRenderable;

			PlanetCube* mCube;

			int mParentSlot;
			QuadTreeNode* mParent;
			QuadTreeNode* mChildren[4];
		};

		//! Planet cube side quadtree class
		class QuadTree : public NonCopyable {
		public:
			explicit QuadTree(PlanetCube * cube, int face);
			virtual ~QuadTree();

		private:
			QuadTreeNode * mRoot;
		};

		// Quadtree node comparison for LOD age.
		class QuadTreeNodeCompareLastOpened {
		public:
			bool operator()(const QuadTreeNode* a, const QuadTreeNode* b) const {
				return (a->mLastOpened > b->mLastOpened);
			}
		};

	} // namespace geo
} // namespace sht

#endif