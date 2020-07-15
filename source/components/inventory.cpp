#include "components/inventory.hpp"
#include <vector>
#include "services.hpp"
#include "components/position.hpp"
#include "components/item.hpp"



inventory::inventory(unsigned size):_items(size),_size(size){}

std::vector<std::optional<entt::entity>>::iterator inventory::begin()
{
	return _items.begin();
}

std::vector<std::optional<entt::entity>>::iterator inventory::end()
{
	return _items.end();
}

bool inventory::add(entt::entity obj)
{
	for(auto it = _items.begin(); it != _items.end();++it){
		if(!it->has_value()){
			if(Services::enttRegistry->has<position>(obj))
				Services::enttRegistry->remove<position>(obj);
			*it = obj;
			return true;
		}
	}//TODO find items of same type and stack

	return false;
}

bool inventory::add(entt::entity obj, std::vector<std::optional<entt::entity>>::iterator position)
{
	if(!position->has_value()){
		*position = obj;
		return true;
	}//TODO stack if possible
	return false;
}

bool inventory::remove(std::vector<std::optional<entt::entity>>::iterator position)
{
	if(position->has_value()){
		position->reset();
		return true;
	}//TODO stack if possible
	return false;
}

unsigned inventory::getSize() const
{
	return _size;
}

std::optional<entt::entity> inventory::operator[](unsigned n)
{
	return _items[n];
}

void inventory::fix()
{
	_items.clear();
	for(auto i : _itemIDs){
		if(i == -1)
			_items.push_back({});
		else
			_items.push_back(item::_itemIDLUT[i]);
	}
	_itemIDs.clear();
}

void to_json(nlohmann::json &j, const inventory &n)
{
	json jj;
	for(auto i : n._items){
		if(i){
			jj.push_back((int)*i);
		}
		else
			jj.push_back(-1);
	}
	j = json{{"size",n.getSize()},{"items",jj}};
}

void from_json(const nlohmann::json &j, inventory &n)
{
	n = inventory(j.at("size").get<unsigned>());
	for(int id : j.at("items")){
		n._itemIDs.push_back(id);
	}
}
