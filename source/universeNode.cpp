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
#include "states/state_playing.hpp"
#include <iostream>


void universeNode::clean()
{
	Services::physicsMutex.lock();
	delete _collisionShape;
	Services::physicsMutex.unlock();

	for (terrainChunk& chunk : _chunks)
	{
		if (chunk.loaded())
		{
			chunk.unload(State::Playing::savePath().append("nodes").append(std::to_string(_ID)));
		}
	}
	for (universeNode& child : _children)
	{
		child.clean();
	}
}

baseBlock& universeNode::getTopBlock(const point2D& pos)
{
	return _generator->getTopBlock(pos);
}

metaBlock* universeNode::getBlock(const point3Di& pos) {
	terrainChunk& tChunk = chunkAt(pos);
	auto debug = chunkFromPos(pos);
	if (!tChunk.loaded() || tChunk.getPosition() != debug)
		return nullptr;
	return &tChunk.getBlock(pos);
}

void universeNode::setBlock(metaBlock b, const point3Di& pos) {
	if (!chunkAt(pos).loaded())
	{
		chunkAt(pos) = terrainChunk(chunkFromPos({ (double)pos.x,(double)pos.y,(double)pos.z,0 }));
		chunkAt(pos).setLoaded();
	}
	chunkAt(pos).setBlock(b, pos);
}

void universeNode::updateChunks(const fdd& cameraPos, universeNode* u) {
	fdd localCameraPos = getLocalPos(cameraPos, u);
	if (localCameraPos.distance2D({ 0,0,0,0 }) < (_diameter / 2 + 100)) {
		iUpdateChunks(chunkFromPos(localCameraPos));
	}
	for (universeNode& child : _children) {
		child.updateChunks(cameraPos, u);
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
	return point3Di{ (int)round((double)pos.x / config::chunkSize),
					(int)round((double)pos.y / config::chunkSize),
					(int)round((double)pos.z / config::chunkSize) };
}

point3Di universeNode::chunkFromPos(const point3Di& pos)
{
	return point3Di{ (int)floor((double)pos.x / config::chunkSize),
					(int)floor((double)pos.y / config::chunkSize),
					(int)floor((double)pos.z / config::chunkSize) };
}

void universeNode::iUpdateChunks(const point3Di& localChunk) {
	for (int x = localChunk.x - floor(config::chunkLoadDiameter / 2);
		x < localChunk.x + ceil(config::chunkLoadDiameter / 2); ++x) {
		for (int y = localChunk.y - floor(config::chunkLoadDiameter / 2);
			y < localChunk.y + ceil(config::chunkLoadDiameter / 2); ++y) {
			for (int z = localChunk.z - floor(config::chunkLoadDiameter / 2);
				z < localChunk.z + ceil(config::chunkLoadDiameter / 2); ++z) {
				point3Di chunkPos{ x % config::chunkLoadDiameter, y % config::chunkLoadDiameter, z % config::chunkLoadDiameter };
				terrainChunk& chunk = getChunk(chunkPos);
				if (chunk != point3Di{ x,y,z } || !chunk.loaded()) {
					if (chunk.loaded())
					{
						chunk.unload(State::Playing::savePath().append("nodes").append(std::to_string(_ID)));
					}
					std::filesystem::path newChunkPath(State::Playing::savePath().append("nodes").append(std::to_string(_ID)).append(std::to_string(x)).append(std::to_string(y)).append(std::to_string(z)).concat(".z5c"));
					if (std::filesystem::exists(newChunkPath))//if file already exists, load
					{
						chunk.load(newChunkPath, { x,y,z });
					}
					else {
						chunk = _generator->getChunk(point3Di{ x,y,z });
					}
				}
			}
		}
	}
}

terrainChunk& universeNode::chunkAt(const point3Di& pos) {
	int x = (int(floor((double)pos.x / config::chunkSize)) % config::chunkLoadDiameter);
	if (x < 0)
		x += config::chunkLoadDiameter;
	int y = (int(floor((double)pos.y / config::chunkSize)) % config::chunkLoadDiameter);
	if (y < 0)
		y += config::chunkLoadDiameter;
	int z = (int(floor((double)pos.z / config::chunkSize)) % config::chunkLoadDiameter);
	if (z < 0)
		z += config::chunkLoadDiameter;
	return _chunks[(x * config::chunkLoadDiameter * config::chunkLoadDiameter) + (y * config::chunkLoadDiameter) + z];
}

terrainChunk& universeNode::getChunk(const point3Di& pos)
{
	int x = pos.x;
	if (x < 0)
		x += config::chunkLoadDiameter;
	int y = pos.y;
	if (y < 0)
		y += config::chunkLoadDiameter;
	int z = pos.z;
	if (z < 0)
		z += config::chunkLoadDiameter;

	return _chunks[(x * config::chunkLoadDiameter * config::chunkLoadDiameter) + (y * config::chunkLoadDiameter) + z];
}

int universeNode::chunkIndex(const point3Di& pos) const
{
	int x = (pos.x / config::chunkSize % config::chunkLoadDiameter);
	if (x < 0)
		x += config::chunkLoadDiameter;
	int y = (pos.y / config::chunkSize % config::chunkLoadDiameter);
	if (y < 0)
		y += config::chunkLoadDiameter;
	int z = (pos.z / config::chunkSize % config::chunkLoadDiameter);
	if (z < 0)
		z += config::chunkLoadDiameter;
	return (x * config::chunkLoadDiameter * config::chunkLoadDiameter) + (y * config::chunkLoadDiameter) + z;
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

fdd universeNode::getLocalPos(fdd f, universeNode* u) const // returns the fdd(position) f (which is relative to u)
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
				f += u->_position;
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

fdd universeNode::getLocalVel(fdd f, universeNode* u) const
{
	if (u == this)
		return f;
	else
	{
		fdd transform{ 0,0,0,0 };
		const universeNode* transformLocal = this;

		while (transformLocal != u) { // while transformLocal isn't u (f's parent)
			if (transformLocal->_depth - u->_depth > 1) {//should move u
				f += u->_velocity;
				u = u->_parent;
			}
			else {// move transformLocal
				transform += transformLocal->_velocity;
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

fdd universeNode::getVelocity()
{
	return _velocity;
}

void universeNode::setVelocity(fdd v)
{
	_velocity = v;
}

unsigned universeNode::getID()
{
	return _ID;
}

std::vector<universeNode*> universeNode::getChildren()
{
	std::vector<universeNode*> result;
	for (universeNode& u : _children)
	{
		result.push_back(&u);
	}
	return result;
}

void universeNode::updatePositions(double dt)
{
	_position += _velocity * dt;
	for (universeNode& child : _children)
	{
		child.updatePositions(dt);
	}
}

fdd universeNode::getGravityAcceleration(fdd localPosition)
{
	fdd magicGravity = { 0,0,(localPosition.z > 0 ? -1 : 1)* (G * (_mass / ((_diameter / 2) * (_diameter / 2)))),0 };
	fdd realGravity = (_centerOfMass - localPosition).setMagnitude(G * (_mass / ((_diameter / 2) * (_diameter / 2))));
	double factorMagic = 1;
	double distance = _centerOfMass.distance(localPosition);
	if (distance > _diameter / 2)
	{
		factorMagic = distance - _diameter / 2;
		factorMagic /= 10;
		if (factorMagic > 1)
			factorMagic = 1;
		factorMagic = 1 - factorMagic;
	}

	return magicGravity * factorMagic + realGravity * (1 - factorMagic);
}

universeNode* universeNode::getParent()
{
	return _parent;
}

double universeNode::getMass()
{
	return _mass;
}

double universeNode::getDiameter()
{
	return _diameter;
}

unsigned int universeNode::getHeight(const point2D& pos)
{
	return 6;
}

rp3d::CollisionBody* universeNode::getNodeCollider()
{
	return _collider;
}

std::vector<rp3d::CollisionBody*> universeNode::getTerrainColliders(fdd p, universeNode* parent)
{
	std::vector<rp3d::CollisionBody*> candidateBodies;
	//fem 3 llistes de coordenades, afegim a akestes i despres iterem per totes les combinacions
	p.x=floor(p.x);
	p.y=floor(p.y);
	p.z=floor(p.z);
	std::vector<int> posXlist;
	posXlist.push_back(p.x);
	std::vector<int> posYlist;
	posYlist.push_back(p.y);
	std::vector<int> posZlist;
	posZlist.push_back(p.z);
	if (chunkFromPos({ p.x,0,0, 0 }).x > chunkFromPos({ p.x - 1,0,0, 0 }).x)
	{
		posXlist.push_back(p.x - 1);
	}
	if (chunkFromPos({ p.x,0,0, 0 }).x < chunkFromPos({ p.x + 1,0,0, 0 }).x)
	{
		posXlist.push_back(p.x + 1);
	}
	if (chunkFromPos({ 0,p.y,0, 0 }).y > chunkFromPos({ 0,p.y - 1,0, 0 }).y)
	{
		posYlist.push_back(p.y - 1);
	}
	if (chunkFromPos({ 0,p.y,0, 0 }).y < chunkFromPos({ 0,p.y + 1,0, 0 }).y)
	{
		posYlist.push_back(p.y + 1);
	}
	if (chunkFromPos({ 0,0,p.z, 0 }).z > chunkFromPos({ 0,0,p.z - 1, 0 }).z)
	{
		posZlist.push_back(p.z - 1);
	}
	if (chunkFromPos({ 0,0,p.z,0 }).z < chunkFromPos({ 0,0,p.z + 1, 0 }).z)
	{
		posZlist.push_back(p.z + 1);
	}

	for (int x : posXlist)
	{
		for (int y : posYlist)
		{
			for (int z : posZlist)
			{
				auto chunk = chunkAt({ x,y,z });
				if (chunk.loaded())
				{
					candidateBodies.push_back(chunkAt({ x,y,z }).getCollider());
				}
			}
		}
	}
	return candidateBodies;
}

void universeNode::populateColliders()
{
	rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
	rp3d::Transform transform(initPosition, initOrientation);
	Services::physicsMutex.lock();
	_collider = Services::collisionWorld->createCollisionBody(transform);

	_collisionShape = new rp3d::BoxShape(rp3d::Vector3{ (rp3d::decimal)(_diameter / 2),(rp3d::decimal)(_diameter / 2),(rp3d::decimal)(_diameter / 2) });
	_collider->addCollisionShape(_collisionShape, transform);
	Services::physicsMutex.unlock();
	for (universeNode& u : _children) {
		u.populateColliders();
	}
}

void to_json(nlohmann::json& j, const universeNode& f) {
	j = json{ {"name", f._name},			{"mass", f._mass},
			 {"diameter", f._diameter}, {"type", f._type},
			 {"position", f._position}, {"velocity", f._velocity},
			 {"children", f._children},{"id",f._ID} };
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
	if (j.contains("com"))
	{
		f._centerOfMass = j.at("com").get<fdd>();
	}
	else
	{
		f._centerOfMass = { 0,0,0,0 };
	}
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
		f._generator = std::make_unique<rockyPlanetGenerator>(f._ID, f._diameter);
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
	f.populateColliders();
}
