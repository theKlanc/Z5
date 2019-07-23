#pragma once
#include <memory>
#include <vector>
#include <array>
#include "fdd.hpp"
#include "json.hpp"
#include "block.hpp"
#include "terrainChunk.hpp"
#include "nodeGenerator.hpp"

using nlohmann::json;

enum nodeType{
	STAR,
	BLACK_HOLE,
	PLANET_GAS,
	PLANET_ROCK,
	ASTEROID,
	COMET,
	SATELLITE_NATURAL,
	SATELLITE_ARTIFICIAL,
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
	{SATELLITE_NATURAL,"SATELLITE_NATURAL"},
	{SATELLITE_ARTIFICIAL,"SATELLITE_ARTIFICIAL"},
	{SPACE_STATION,"SPACE_STATION"},
	{SPACESHIP,"SPACESHIP"},
})


class universeNode {
public:
	universeNode();
	block& getBlock(const point3Di &pos);
	void setBlock(block* b, const point3Di &pos);

	friend void to_json(nlohmann::json &j, const universeNode &f);
	friend void from_json(const json& j, universeNode& f);

  private:
	terrainChunk& chunkAt(const point3Di &pos);
	void linkChildren();
	
	std::string _name;
	double _mass; // mass in kg
	double _diameter; // diameter in m
	fdd _position;
	fdd _velocity;

	std::array<terrainChunk,config::chunkLoadRadius*config::chunkLoadRadius*config::chunkLoadRadius> _chunks;

	nodeType _type;
	std::vector<universeNode> _children;
	universeNode* _parent;
	nodeGenerator _generator;
  
};

void to_json(json& j, const universeNode& f);
void from_json(const json& j, universeNode& f);