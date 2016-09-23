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

		//! Planet tree node class
		class PlanetTreeNode : public NonCopyable {
		public:
			explicit PlanetTreeNode(PlanetTree * tree);
			virtual ~PlanetTreeNode();

			void Render();

		private:
			PlanetTree * owner_; //!< owner face tree
			int lod_; //!< level of detail
			int x_;
			int y_;

			// Temporary not available
			static constexpr int kNumChildren = 4;
			PlanetTreeNode * parent_;
			PlanetTreeNode * children_[kNumChildren];
		};

		//! Planet tree class
		class PlanetTree : public NonCopyable {
			friend class PlanetTreeNode;
		public:
			explicit PlanetTree(PlanetCube * cube, int face);
			virtual ~PlanetTree();

			void Update();
			void Render();

		private:
			PlanetCube * cube_;
			PlanetTreeNode * root_;
			int face_;
		};

	} // namespace geo
} // namespace sht

#endif