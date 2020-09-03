#include "components/hand.hpp"
#include "services.hpp"
#include "components/position.hpp"


void hand::nextItem(inventory& inv)
{
	index = (index + 1) % toolbarSize;
	select(inv);
}

void hand::previousItem(inventory& inv)
{
	if(index == 0)
		index = toolbarSize-1;
	else
		index--;
	select(inv);
}

void hand::select(inventory& inv)
{
	if(_item)
		Services::enttRegistry->remove<position>(*_item);
	_item = inv[index];
}

void to_json(nlohmann::json &j, const hand &n)
{
	j=nlohmann::json{{"index",n.index},{"tsize",n.toolbarSize}};
}

void from_json(const nlohmann::json &j, hand &n)
{
	n.index = j.at("index");
	n.toolbarSize = j.at("tsize");
}
