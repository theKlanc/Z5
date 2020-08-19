#pragma once
#include "entt/entity/registry.hpp"
#include "json.hpp"
#include <optional>

struct inventory{
	inventory(){}
	inventory(unsigned size);

	std::vector<std::optional<entt::entity>>::iterator begin();
	std::vector<std::optional<entt::entity>>::iterator end();

	bool add(entt::entity obj);
	bool add(entt::entity obj, std::vector<std::optional<entt::entity>>::iterator position);

	bool remove(std::vector<std::optional<entt::entity>>::iterator position);

	unsigned getSize() const;

	std::optional<entt::entity> operator[](unsigned n);

	void fix();
	private:
	std::vector<std::optional<entt::entity>> _items;
	std::vector<int> _itemIDs;
	unsigned _size;


	friend void to_json(nlohmann::json& j, const inventory& n);
	friend void from_json(const nlohmann::json& j, inventory& n);
};

void to_json(nlohmann::json& j, const inventory& n);
void from_json(const nlohmann::json& j, inventory& n);