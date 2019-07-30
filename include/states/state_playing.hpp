#pragma once
#include "state_base.hpp"
#include "universeNode.hpp"
#include "entt/entity/registry.hpp"
#include "block.hpp"
#include <vector>

namespace State {
	class Playing : public virtual State_Base {
	  public:
		Playing();
		Playing(gameCore &gc, std::string );

		void input() override;
		void update(float dt) override;
		void draw() override;

	  private:
		entt::entity player;
		entt::entity camera;

		void loadTerrainTable();
		std::vector<block> _terrainTable;
		universeNode _universeBase;
		entt::DefaultRegistry _enttRegistry;
		
	};
} // namespace State
