#include "universeNode.hpp"
#include "config.hpp"
#include "fdd.hpp"

#include "nodeGenerators/artificialSatelliteGenerator.hpp"
#include "nodeGenerators/asteroidGenerator.hpp"
#include "nodeGenerators/blackHoleGenerator.hpp"
#include "nodeGenerators/gasPlanetGenerator.hpp"
#include "nodeGenerators/naturalSatelliteGenerator.hpp"
#include "nodeGenerators/rockyPlanetGenerator.hpp"
#include "nodeGenerators/spaceStationGenerator.hpp"
#include "nodeGenerators/spaceshipGenerator.hpp"
#include "nodeGenerators/starGenerator.hpp"

block &universeNode::getBlock(const point3Di &pos) {
	return chunkAt(pos).getBlock(pos);
}

void universeNode::setBlock(block *b, const point3Di &pos) {
	chunkAt(pos).setBlock(b, pos);
}

void universeNode::updateChunks(const fdd &playerPos, universeNode* u)
{
	fdd localPlayerPos = getLocalPos(playerPos,u);
	if(_position.distance2D(localPlayerPos)<(_diameter+100)){
		iUpdateChunks(localPlayerPos);
	}
	for(universeNode& child : _children){
		child.updateChunks(playerPos,u);
	}
}

bool universeNode::operator!=(const universeNode &right) const {
	return _ID != right._ID;
}

bool universeNode::operator==(const universeNode &right) const {
	return _ID == right._ID;
}

void universeNode::iUpdateChunks(const fdd &localPos)
{

}

terrainChunk &universeNode::chunkAt(const point3Di &pos) {
	return _chunks[(pos.x / config::chunkSize % config::chunkLoadRadius *
					config::chunkLoadRadius * config::chunkLoadRadius) +
				   (pos.y / config::chunkSize % config::chunkLoadRadius *
					config::chunkLoadRadius) +
				   (pos.z / config::chunkSize % config::chunkLoadRadius)];
}

void universeNode::linkChildren() {
	for (universeNode &node : _children) {
		node._parent = this;
		node._depth = _depth + 1;
	}
}

fdd universeNode::getLocalPos(
	fdd f, universeNode *u) const // returns the fdd f (which is relative to u)
								  // relative to our local node (*this)
{
	fdd transform;
	universeNode *transformParent = _parent;
	universeNode *pu = u;

	while (pu != transformParent) {
		if (pu == nullptr || (transformParent != nullptr && transformParent->_depth >= pu->_depth)) { //if tP is deeper than pu
			transform += transformParent->_position;
			transformParent = transformParent->_parent;
			continue;
		}
		if (transformParent == nullptr || pu->_depth > transformParent->_depth) {
			f += pu->_position;
			pu = pu->_parent;
		}
	}
	return f -= transform;
}

void to_json(nlohmann::json &j, const universeNode &f) {
	j = json{{"name", f._name},			{"mass", f._mass},
			 {"diameter", f._diameter}, {"type", f._type},
			 {"position", f._position}, {"velocity", f._velocity},
			 {"children", f._children}};
}

void from_json(const json &j, universeNode &f) {
	f._depth = 0;
	f._ID = j.at("id").get<int>();
	f._parent = nullptr;
	f._name = j.at("name").get<std::string>();
	f._type = j.at("type").get<nodeType>();
	f._mass = j.at("mass").get<double>();
	f._diameter = j.at("diameter").get<double>();
	f._position = j.at("position").get<fdd>();
	f._velocity = j.at("velocity").get<fdd>();
	f._children = std::vector<universeNode>();
	for (const nlohmann::json &element : j.at("children")) {
		f._children.push_back(element.get<universeNode>());
	}
	f.linkChildren();
	switch (f._type) {
	case STAR:
		f._generator = std::make_unique<starGenerator>();
		break;
	case BLACK_HOLE:
		f._generator = std::make_unique<blackHoleGenerator>();
		break;
	case PLANET_GAS:
		f._generator = std::make_unique<gasPlanetGenerator>();
		break;
	case PLANET_ROCK:
		f._generator = std::make_unique<rockyPlanetGenerator>();
		break;
	case ASTEROID:
		f._generator = std::make_unique<asteroidGenerator>();
		break;
	case SATELLITE_NATURAL:
		f._generator = std::make_unique<naturalSatelliteGenerator>();
		break;
	case SATELLITE_ARTIFICIAL:
		f._generator = std::make_unique<artificialSatelliteGenerator>();
		break;
	case SPACE_STATION:
		f._generator = std::make_unique<spaceStationGenerator>();
		break;
	case SPACESHIP:
		f._generator = std::make_unique<spaceshipGenerator>();
		break;
	}
}
