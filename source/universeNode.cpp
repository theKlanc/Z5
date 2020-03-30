#include "universeNode.hpp"
#include "config.hpp"
#include "fdd.hpp"

#include "nodeGenerators/terrainPainterGenerator.hpp"
#include "nodeGenerators/prefabGenerator.hpp"
#include "states/state_playing.hpp"
#include <iostream>
#include "nodeGenerators/nullGenerator.hpp"


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

universeNode::universeNode(const universeNode& u)
{
	*this = u;
}

universeNode::universeNode(std::string name, double mass, double diameter, fdd pos, fdd com, fdd vel, nodeType type, universeNode* parent, unsigned int id)
{
	_name = name;
	_mass = mass;
	_diameter = diameter;
	_position = pos;
	_centerOfMass = com;
	_velocity = vel;
	_type = type;
	_parent = parent;
	_ID = id;
	if (_parent == nullptr) {
		_depth = 0;
	}
	else {
		_depth = _parent->_depth + 1;
	}
	_chunks = std::vector<terrainChunk>(config::chunkLoadDiameter * config::chunkLoadDiameter * config::chunkLoadDiameter);
	connectGenerator({ {"type","null"} });
	populateColliders();
}

baseBlock& universeNode::getTopBlock(const point2D& pos)
{
	return _generator->getTopBlock(pos);
}

metaBlock& universeNode::getBlock(const point3Di& pos) {
	terrainChunk& tChunk = chunkAt(pos);
	if (tChunk.isValid(pos))
	{
		return tChunk.getBlock(pos);
	}
	//else if(wouldBeValid){
	//	return _generator.getBlock()
	//}
	else {
		return metaBlock::nullBlock;
	}
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

universeNode::universeNodeIterator universeNode::begin()
{
	universeNodeIterator u;
	u.p = this;
	return u;
}

universeNode::universeNodeIterator universeNode::end()
{
	return universeNodeIterator();
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

void universeNode::connectGenerator(const nlohmann::json& j)
{
	if (j.contains("type")) {
		std::string type = j.at("type");
		if (type == "prefab") {
			_generator = std::make_unique<prefabGenerator>(j.at("generator").get<prefabGenerator>());
		}
		else if (type == "terrainPainter") {
			_generator = std::make_unique<terrainPainterGenerator>(j.at("generator").get<terrainPainterGenerator>());
		}
		else {
			_generator = std::make_unique<nullGenerator>();
		}
	}
	else {
		_generator = std::make_unique<terrainPainterGenerator>(_ID,_diameter);
	}
}

void universeNode::connectGenerator(std::unique_ptr<nodeGenerator> ng)
{
	_generator = std::move(ng);
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
	return _chunks[(z * config::chunkLoadDiameter * config::chunkLoadDiameter) + (y * config::chunkLoadDiameter) + x];
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

	return _chunks[(z * config::chunkLoadDiameter * config::chunkLoadDiameter) + (y * config::chunkLoadDiameter) + x];
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
	return (z * config::chunkLoadDiameter * config::chunkLoadDiameter) + (y * config::chunkLoadDiameter) + x;
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

fdd universeNode::getCenterOfMass()
{
	return _centerOfMass;
}

void universeNode::setVelocity(fdd v)
{
	_velocity = v;
}

void universeNode::setPosition(fdd p)
{
	_position = p;
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

void universeNode::addChild(universeNode u)
{
	_children.push_back(u);
}

void universeNode::updatePositions(double dt)
{
	if (!physicsData.sleeping)
	{
		_position += _velocity * dt;
	}
	for (universeNode& child : _children)
	{
		child.updatePositions(dt);
	}
}

fdd universeNode::getGravityAcceleration(fdd localPosition)
{
	fdd magicGravity = { 0,0,(localPosition.z > 0 ? -1 : 1)* (G * (_mass / ((_diameter / 2) * (_diameter / 2)))),0 };
	fdd realGravity = (_centerOfMass - localPosition).setMagnitude(G * (_mass / ((_diameter / 2) * (_diameter / 2))));
	double magicFactor = 1;
	double distance = _centerOfMass.distance(localPosition);
	if (distance > _diameter / 2)
	{
		magicFactor = distance - _diameter / 2;
		magicFactor /= 10;
		if (magicFactor > 1)
			magicFactor = 1;
		magicFactor = 1 - magicFactor;
	}

	return magicGravity * magicFactor + realGravity * (1 - magicFactor);
}

universeNode& universeNode::operator=(const universeNode& u)
{
	_position = u._position;
	_ID = u._ID;
	_centerOfMass = u._centerOfMass;
	_children = u._children;
	_chunks = u._chunks;
	_collider = u._collider;
	_collisionShape = u._collisionShape;
	_depth = u._depth;
	_diameter = u._diameter;
	_type = u._type;
	_velocity = u._velocity;
	_parent = u._parent;
	_mass = u._mass;
	_name = u._name;
	physicsData = u.physicsData;
	nlohmann::json jTemp;
	to_json(jTemp, *u._generator.get());
	connectGenerator(jTemp);
	return *this;
}

std::vector<terrainChunk>& universeNode::getChunks()
{
	return _chunks;
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

unsigned universeNode::getDepth()
{
	return _depth;
}

std::string universeNode::getName()
{
	return _name;
}

nodeType universeNode::getType()
{
	return _type;
}

unsigned int universeNode::getHeight(const point2D& pos)
{
	return _generator->getHeight(pos);
}

rp3d::CollisionBody* universeNode::getNodeCollider()
{
	return _collider;
}

std::vector<terrainChunk*> universeNode::getCollidableChunks(fdd p, const point3Dd& size, universeNode* parent)
{
	std::vector<terrainChunk*> candidateBodies;
	//fem 3 llistes de coordenades, afegim a akestes i despres iterem per totes les combinacions
	p = getLocalPos(p, parent);


	std::vector<int> posXlist;
	posXlist.push_back(floor(p.x));
	std::vector<int> posYlist;
	posYlist.push_back(floor(p.y));
	std::vector<int> posZlist;
	posZlist.push_back(floor(p.z));
	if (size.x != 0 && floor(p.x / config::chunkSize) != floor((p.x + size.x) / config::chunkSize))
	{
		posXlist.push_back(floor(p.x + size.x));
	}
	if (size.y != 0 && floor(p.y / config::chunkSize) != floor((p.y + size.y) / config::chunkSize))
	{
		posYlist.push_back(floor(p.y + size.y));
	}
	if (size.z != 0 && floor(p.z / config::chunkSize) != floor((p.z + size.z) / config::chunkSize))
	{
		posZlist.push_back(floor(p.z + size.z));
	}

	for (int x : posXlist)
	{
		for (int y : posYlist)
		{
			for (int z : posZlist)
			{
				auto& chunk = chunkAt({ x,y,z });
				if (chunk.loaded())
				{
					candidateBodies.push_back(&chunk);
				}
			}
		}
	}
	return candidateBodies;
}

void universeNode::populateColliders()
{
	Services::physicsMutex.lock();
	_collider = Services::collisionWorld->createCollisionBody(rp3d::Transform::identity());

	_collisionShape = new rp3d::BoxShape(rp3d::Vector3{ (rp3d::decimal)(_diameter / 2),(rp3d::decimal)(_diameter / 2),(rp3d::decimal)(_diameter / 2) });
	_collider->addCollisionShape(_collisionShape, rp3d::Transform::identity());
	Services::physicsMutex.unlock();
	for (universeNode& u : _children) {
		u.populateColliders();
	}
}


universeNode::universeNodeIterator::universeNodeIterator(const universeNode::universeNodeIterator& r)
{
	p = r.p;
	q = r.q;
}

universeNode::universeNodeIterator& universeNode::universeNodeIterator::operator=(const universeNode::universeNodeIterator& r)
{
	p = r.p;
	q = r.q;
	return *this;
}

bool universeNode::universeNodeIterator::operator==(const universeNode::universeNodeIterator& r)
{
	return p == r.p && q == r.q;
}

bool universeNode::universeNodeIterator::operator!=(const universeNode::universeNodeIterator& r)
{
	return p != r.p || q != r.q;
}

universeNode& universeNode::universeNodeIterator::operator*()
{
	return *p;
}

universeNode& universeNode::universeNodeIterator::operator->()
{
	return *p;
}

universeNode::universeNodeIterator& universeNode::universeNodeIterator::operator++()
{
	for (auto* child : p->getChildren()) {
		q.push(child);
	}
	if (q.empty()) {
		p = nullptr;
	}
	else {
		p = q.front();
		q.pop();
	}
	return *this;
}

universeNode::universeNodeIterator universeNode::universeNodeIterator::operator++(int devnull)
{
	universeNodeIterator old(*this);
	operator++();
	return old;
}


void to_json(nlohmann::json& j, const universeNode& f) {
	j = json{ {"name", f._name},			{"mass", f._mass},
			 {"diameter", f._diameter}, {"type", f._type},
			 {"position", f._position},{"CoM", f._centerOfMass}, {"velocity", f._velocity},
			 {"children", f._children},{"id",f._ID},{"generator",*f._generator.get()} };
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
	if (j.contains("CoM"))
	{
		f._centerOfMass = j.at("CoM").get<fdd>();
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
	nlohmann::json jt;
	if (j.contains("generator")) {
		jt = j.at("generator");
	}
	f.connectGenerator(jt);
	f.populateColliders();
}