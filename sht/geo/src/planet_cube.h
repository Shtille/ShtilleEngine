#pragma once
#ifndef __SHT_GEO_PLANET_CUBE_H__
#define __SHT_GEO_PLANET_CUBE_H__

#include "planet_tree.h"

#include "../../math/vector.h"
#include "../../common/non_copyable.h"

#include <list>
#include <set>
#include <queue>

namespace sht {
	namespace geo {

		// Forward declarations
		class QuadTree;
		class QuadTreeNode;

		//! Planet cube class
		class PlanetCube : public NonCopyable {
			friend class PlanetRenderable;
			friend class QuadTreeNode;

			enum {
				REQUEST_RENDERABLE,
				REQUEST_MAPTILE,
				REQUEST_SPLIT,
				REQUEST_MERGE,
			};

			struct Request {
				QuadTreeNode* mNode;
				int mType;

				Request(QuadTreeNode* node, int type) : mNode(node), mType(type) {}
			};

			class RequestComparePriority {
			public:
				bool operator()(const Request& a, const Request& b) const;
			};

			typedef std::list<Request> RequestQueue;
			typedef std::set<QuadTreeNode*> NodeSet;
			typedef std::priority_queue<QuadTreeNode*, std::vector<QuadTreeNode*>, QuadTreeNodeCompareLastOpened> NodeHeap;

		public:
			explicit PlanetCube();
			virtual ~PlanetCube();

			void Update();
			void Render();

			void SetParameters();
			const double getScale() const;
			inline int getFrameCounter();

		protected:
			void SplitQuadTreeNode(QuadTreeNode* node);
			void MergeQuadTreeNode(QuadTreeNode* node);

			void Request(QuadTreeNode* node, int type, bool priority = false);
			void Unrequest(QuadTreeNode* node);
			void HandleRequests(RequestQueue& queue);
			void HandleRenderRequests();
			void HandleInlineRequests();

			bool HandleRenderable(QuadTreeNode* node);
			bool HandleMapTile(QuadTreeNode* node);
			bool HandleSplit(QuadTreeNode* node);
			bool HandleMerge(QuadTreeNode* node);

			void PruneTree();
			void RefreshMapTile(QuadTreeNode* node, PlanetMapTile* tile);

		private:
			RequestQueue mInlineRequests;
			RequestQueue mRenderRequests;
			QuadTree * mFaces[6];
			NodeSet mOpenNodes;

			int mFrameCounter;
		};

	} // namespace geo
} // namespace sht

#endif