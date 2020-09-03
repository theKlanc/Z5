#include "components/placeableBlock.hpp"
#include "services.hpp"
#include "components/position.hpp"

placeableBlock::placeableBlock(const nlohmann::json &j) : _block(baseBlock::terrainTable[j.at("block")])
{
	amount = j.at("amount");
}

void placeableBlock::use(entt::entity user, entt::entity i)
{
	if(Services::enttRegistry->has<position>(i) && amount > 0){
		auto pos = Services::enttRegistry->get<position>(i);
		auto displacement = point2Dd::fromDirection(pos.pos.r,0.5);
		fdd aim = pos.pos;
		aim.x+=displacement.x;
		aim.y+=displacement.y;

		metaBlock m;
		m.base = &_block;
		m.saveMeta = true;
		pos.parent->setBlock(m,aim);

		amount--;
	}
}

bool placeableBlock::operator==(const item &right) const
{
	return false;
}

nlohmann::json placeableBlock::getJson() const
{
	json pbjson = {{"amount",amount},{"block",_block.ID}};
	return json{{"type","placeableBlock"},{"item",pbjson}};
}

void from_json(const nlohmann::json &j, placeableBlock &n)
{
	n.amount = j.at("amount");
	n._block = baseBlock::terrainTable[j.at("block")];
}
