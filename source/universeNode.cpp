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

block& universeNode::getBlock(const point3Di& pos) {
	terrainChunk &tChunk = chunkAt(pos);
	return tChunk.getBlock(pos);
}

void universeNode::setBlock(block* b, const point3Di& pos) {
	chunkAt(pos).setBlock(b, pos);
}

void universeNode::updateChunks(const fdd& playerPos, universeNode* u) {
	fdd localPlayerPos = getLocalPos(playerPos, u);
	if (localPlayerPos.distance2D({ 0,0,0,0 }) < (_diameter + 100)) {
		iUpdateChunks(chunkFromPos(localPlayerPos));
	}
	for (universeNode& child : _children) {
		child.updateChunks(playerPos, u);
	}
}

bool universeNode::shouldDraw(fdd f) {
	return (f.distance2D({ 0,0,0,0 }) < _diameter / 2 + 60);
}

std::vector<universeNode*> universeNode::nodesToDraw(fdd f, universeNode* u)
{
	std::vector<universeNode*> result;
	fdd localPos = getLocalPos(f, u);
	if (shouldDraw(localPos)) {
		result.push_back(this);
	}
	for (universeNode& child : _children) {
		std::vector<universeNode*> temp = child.nodesToDraw(f, u);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	return result;
}

bool universeNode::findNodeByID(const unsigned int& id, universeNode*& result)
{
	if (_ID == id) {
		result = this;
		return true;
	}
	for (universeNode& u : _children) {
		if (u.findNodeByID(id, result))
			return true;
	}
	result = nullptr;
	return false;
}

bool universeNode::drawBefore(universeNode& r) const
{
	fdd localPos = getLocalPos(r._position, r._parent);
	return std::fmod(_position.z, 1.0f) < std::fmod(localPos.z, 1.0f);

}

bool universeNode::operator!=(const universeNode& right) const {
	return _ID != right._ID;
}

bool universeNode::operator==(const universeNode& right) const {
	return _ID == right._ID;
}

point3Di universeNode::chunkFromPos(const fdd& pos) {
	return point3Di{ int(pos.x) / config::chunkSize,
					int(pos.y) / config::chunkSize,
					int(pos.z) / config::chunkSize };
}

void universeNode::iUpdateChunks(const point3Di& localChunk) {
	for (int x = localChunk.x - (config::chunkLoadRadius / 2) + 2;
		x < localChunk.x + (config::chunkLoadRadius / 2) - 2; ++x) {
		for (int y = localChunk.y - (config::chunkLoadRadius / 2) + 2;
			y < localChunk.y + (config::chunkLoadRadius / 2) - 2; ++y) {
			for (int z = localChunk.z - (config::chunkLoadRadius / 2) + 2;
				z < localChunk.z + (config::chunkLoadRadius / 2) - 2; ++z) {
				point3Di chunkPos{ x, y, z };
				terrainChunk& chunk = chunkAt(chunkPos);

				if (chunk != chunkPos || !chunk.loaded()) {
					/*if()//if file already exists, load
					{
						chunk.load();
					}
					else{*/
					chunk=terrainChunk();_generator->getChunk(chunkPos);
					//}
				}
			}
		}
	}
}

terrainChunk& universeNode::chunkAt(const point3Di& pos) {
	int x = (pos.x / config::chunkSize % config::chunkLoadRadius);
	if (x < 0)
		x += config::chunkLoadRadius;
	int y = (pos.y / config::chunkSize % config::chunkLoadRadius);
	if (y < 0)
		y += config::chunkLoadRadius;
	int z = (pos.z / config::chunkSize % config::chunkLoadRadius);
	if (z < 0)
		z += config::chunkLoadRadius;
	return _chunks[(x * config::chunkLoadRadius * config::chunkLoadRadius) + (y * config::chunkLoadRadius) + z];
}

int universeNode::chunkIndex(const point3Di& pos) const
{
	int x = (pos.x / config::chunkSize % config::chunkLoadRadius);
	if (x < 0)
		x += config::chunkLoadRadius;
	int y = (pos.y / config::chunkSize % config::chunkLoadRadius);
	if (y < 0)
		y += config::chunkLoadRadius;
	int z = (pos.z / config::chunkSize % config::chunkLoadRadius);
	if (z < 0)
		z += config::chunkLoadRadius;
	return (x * config::chunkLoadRadius * config::chunkLoadRadius) + (y * config::chunkLoadRadius) + z;
}

void universeNode::linkChildren() {
	for (universeNode& u : _children) {
		u._parent = this;
		u.linkChildren();
	}
	if (_parent == nullptr) {
		_depth = 0;
	}
	else {
		_depth = _parent->_depth + 1;
	}
	for (universeNode& u : _children) {
		u.linkChildren();
	}
}


//fdd universeNode::getLocalPosOLD(
//	fdd f, universeNode* u) const // returns the fdd f (which is relative to u)
//								  // relative to our local node (*this)
//{
//	fdd transform;
//	const universeNode* transformParent = _parent;
//	universeNode* pu = u;
//
//	while ((pu == nullptr && transformParent == nullptr) || pu->_parent != transformParent) {
//		if (pu == nullptr || pu->_parent == nullptr || (transformParent != nullptr &&
//			transformParent->_depth >=
//			pu->_depth)) { // if tP is deeper than pu
//			transform += transformParent->_position;
//			transformParent = transformParent->_parent;
//		}
//		else if (transformParent == nullptr ||
//			pu->_depth > transformParent->_depth) {
//			f += pu->_position;
//			pu = pu->_parent;
//		}
//	}
//	return f -= transform;
//}

fdd universeNode::getLocalPos(fdd f, universeNode* u) const // returns the fdd f (which is relative to u)
								  // relative to our local node (*this)
{
	if (u == this)
		return f;
	else
	{
		fdd transform{ 0,0,0,0 };
		const universeNode* transformLocal = this;

		while (transformLocal != u) { // while transformLocal isn't u (f's parent)
			if (transformLocal->_depth - u->_depth > 1) {//should move u
				f += u->_parent->_position;
				u = u->_parent;
			}
			else {// move transformLocal
				transform += transformLocal->_position;
				transformLocal = transformLocal->_parent;
			}
		}
		return f - transform;
	}
}

fdd universeNode::getPosition()
{
	return _position;
}

universeNode* universeNode::getParent()
{
	return _parent;
}

void to_json(nlohmann::json& j, const universeNode& f) {
	j = json{ {"name", f._name},			{"mass", f._mass},
			 {"diameter", f._diameter}, {"type", f._type},
			 {"position", f._position}, {"velocity", f._velocity},
			 {"children", f._children} };
}

void from_json(const json& j, universeNode& f) {
	f._depth = 0;
	f._ID = j.at("id").get<unsigned int>();
	f._parent = nullptr;
	f._name = j.at("name").get<std::string>();
	f._type = j.at("type").get<nodeType>();
	f._mass = j.at("mass").get<double>();
	f._diameter = j.at("diameter").get<double>();
	f._position = j.at("position").get<fdd>();
	f._velocity = j.at("velocity").get<fdd>();
	f._children = std::vector<universeNode>();
	for (const nlohmann::json& element : j.at("children")) {
		f._children.push_back(element.get<universeNode>());
	}
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
