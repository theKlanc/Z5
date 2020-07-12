#include "systems.hpp"
#include "components/health.hpp"
#include "components/projectile.hpp"

void systems::projectileDamage(eventArgs args)
{
	auto [entity, proj, ms] = std::get<std::tuple<entt::entity, entt::entity, double>>(args);
	if(Services::enttRegistry->valid(entity) && Services::enttRegistry->valid(proj)){
		if(!Services::enttRegistry->has<health>(entity))
			return;
		auto& hlth = Services::enttRegistry->get<health>(entity);
		auto& prj = Services::enttRegistry->get<projectile>(proj);

		if(prj.lastCollision.has_value() && *prj.lastCollision == entity)
			return;
		hlth.damage(prj._damage);
		prj.lastCollision = entity;
		prj._remainingPenetration--;
		if(prj._remainingPenetration < 0){
			Services::enttRegistry->destroy(proj);
		}
	}
}

void systems::projectileBounce(eventArgs args)
{
	auto entity = std::get<entt::entity>(args);
	if(Services::enttRegistry->valid(entity)){
		projectile& proj = Services::enttRegistry->get<projectile>(entity);

		proj._remainingBounces--;
		if(proj._remainingBounces<0){
			Services::enttRegistry->destroy(entity);
		}
	}
}
