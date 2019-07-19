#pragma once
#include <memory>
#include <vector>
#include "fdd.hpp"
#include "json.hpp"

using nlohmann::json;

enum nodeType{
	STAR,
	BLACK_HOLE,
	PLANET_GAS,
	PLANET_ROCK,
	ASTEROID,
	COMET,
	ARTIFICIAL_SATELLITE,
	SPACE_STATION,
	SPACESHIP,
};

NLOHMANN_JSON_SERIALIZE_ENUM( nodeType, {
	{STAR,"STAR"},
	{BLACK_HOLE,"BLACK_HOLE"},
	{PLANET_GAS,"PLANET_GAS"},
	{PLANET_ROCK,"PLANET_ROCK"},
	{ASTEROID,"ASTEROID"},
	{COMET,"COMET"},
	{ARTIFICIAL_SATELLITE,"ARTIFICIAL_SATELLITE"},
	{SPACE_STATION,"SPACE_STATION"},
	{SPACESHIP,"SPACESHIP"},
})


class universeNode {
public:
	universeNode();
	universeNode(const json& j, universeNode* parent = nullptr);
	friend void to_json(nlohmann::json &j, const universeNode &f);
  protected:
	
	std::string _name;
	double _mass; // mass in kg
	double _diameter; // diameter in m
	fdd _position;
	fdd _velocity;

	const nodeType _type;
	std::vector<universeNode> _children;
	universeNode* _parent;
  
};

void to_json(json& j, const fdd& f);
