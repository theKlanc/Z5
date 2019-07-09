#pragma once
#include <worldNode.hpp>
#include <entt/entity/registry.hpp>

class universe{
	public:
	private:
		worldNode _worldBase;
		entt::DefaultRegistry enttRegistry;
};