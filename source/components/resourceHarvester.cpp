#include "components/resourceHarvester.hpp"
#include "services.hpp"
#include "components/position.hpp"
#include "components/drawable.hpp"
#include "components/placeableBlock.hpp"
#include "components/name.hpp"


void resourceHarvester::use(entt::entity user, entt::entity i)
{
	if(Services::enttRegistry->has<position>(i)){
		auto pos = Services::enttRegistry->get<position>(i);
		auto displacement = point2Dd::fromDirection(pos.pos.r,0.5);
		fdd aim = pos.pos;
		aim.x+=displacement.x;
		aim.y+=displacement.y;
		aim.z+=0.5;
		auto bloc = pos.parent->getBlock(aim);
		if(!bloc.base->visible)
			return;
		pos.parent->removeBlock(aim);

		auto itemEntity = Services::enttRegistry->create();
		auto& drw = Services::enttRegistry->emplace<drawable>(itemEntity);
		drw.zoom = 0.5;
		drw.name = bloc.base->name;
		drw.spr = bloc.base->spr;

		auto& neim = Services::enttRegistry->emplace<name>(itemEntity);
		neim.nameString = bloc.base->name;

		auto& pb = Services::enttRegistry->emplace<std::unique_ptr<item>>(itemEntity);
		pb = std::make_unique<placeableBlock>(*bloc.base);

		auto& posi = Services::enttRegistry->emplace<position>(itemEntity);
		posi.parent = pos.parent;
		posi.parentID = posi.parent->getID();
		posi.pos = aim;

		Services::enttRegistry->emplace<entt::tag<"PERMANENT"_hs>>(itemEntity);
	}
}

bool resourceHarvester::operator==(const item &right) const
{
	return true;
}

nlohmann::json resourceHarvester::getJson() const
{
	return json{{"type","resourceHarvester"},{"item",{{"amount",amount}}}};
}

void from_json(const nlohmann::json &j, resourceHarvester &n)
{
	n.amount = j.at("amount");
}
