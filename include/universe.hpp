#pragma once
#include <universeNode.hpp>
#include <entt/entity/registry.hpp>

class universe{
	public:
	private:
		universeNode _worldBase;
		entt::DefaultRegistry enttRegistry;
};