#include "universeNode.hpp"
#include "config.hpp"
#include "fdd.hpp"
#include "icecream.hpp"
#include "nodeGenerators/terrainPainterGenerator.hpp"
#include "nodeGenerators/prefabGenerator.hpp"
#include "states/state_playing.hpp"
#include <iostream>
#include <cmath>
#include "nodeGenerators/nullGenerator.hpp"
#include "jsonTools.hpp"

universeNode::~universeNode()
{
	_CL_cameraPosition = nullptr;
	_CL_chunkloader->join();

	for (terrainChunk& chunk : _chunks)
	{
		if (chunk.loaded())
		{
			chunk.unload(State::Playing::savePath().append("nodes").append(std::to_string(_ID)));
		}
	}
}

universeNode::universeNode(const universeNode &u)
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

	_CL_cameraPosition = std::make_shared<fdd>();
	_CL_chunkloader = std::make_shared<std::thread>(std::bind(&universeNode::_CL_chunkloaderFunc, this));
	_CL_mutex = std::make_shared<std::mutex>();
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

metaBlock &universeNode::getTheoreticalBlock(const point3Di &pos)
{
	return getBlock(pos);
}

void universeNode::updateCamera(fdd c)
{
	*_CL_cameraPosition=c;
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
	_CL_mutex->lock();
	fdd localCameraPos = getLocalPos(cameraPos, u);
	if (localCameraPos.distance2D({ 0,0,0,0 }) < (_diameter / 2 + 100)) {
		iUpdateChunks(chunkFromPos(localCameraPos));
	}
	_CL_mutex->unlock();
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

HI2::Color universeNode::getMainColor()
{
	return _mainColor;
}

interactable* universeNode::getClosestInteractable(fdd pos)
{
	double minDist = config::interactableRadius;
	interactable* inter = nullptr;
	for(auto& i : _interactables){
		for(auto& p : i->getPositions()){
			if(double dist = (p + fdd{0.5,0.5,0.5,0}).distance(pos); dist <= minDist){
				minDist = dist;
				inter = i.get();
			}
		}
	}
	return inter;
}

point3Di universeNode::getClosestInteractablePos(fdd pos)
{

	double minDist = config::interactableRadius;
	point3Di inter;
	for(auto& i : _interactables){
		for(auto& p : i->getPositions()){
			if(double dist = (p + fdd{0.5,0.5,0.5,0}).distance(pos); dist <= minDist){
				minDist = dist;
				inter = p.getPoint3Di();
			}
		}
	}
	return inter;
}

std::shared_ptr<thrustSystem> universeNode::getThrustSystem()
{
	return _thrustSystem;
}

void universeNode::_CL_chunkloaderFunc()
{
	fdd lastPos;
	while(_CL_cameraPosition){
		fdd pos = *_CL_cameraPosition;
		if(!_CL_cameraPosition)
			break;
		if(lastPos!=pos){
			updateChunks(pos,this);
			lastPos = pos;
		}
		else{
			if(pos.magnitude() > 1.5*_diameter){
				int t = pos.magnitude()/_diameter * 100;
				if(t > 2000)
					t=2000;
				std::this_thread::sleep_for(std::chrono::milliseconds(t + rand()%300));//will not work when TPing
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand()%30));
		}
	}
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
	if (_parent == nullptr) {
		_depth = 0;
	}
	else {
		_depth = _parent->_depth + 1;
	}
	for (universeNode& u : _children) {
		u._parent = this;
		u.linkChildren();
	}
}

fdd universeNode::getLocalPos(fdd f, universeNode* u) const // returns the fdd(position) f (which is relative to u)
								  // relative to our local node (*this)
{
	assert(!std::isnan(_position.x));
	universeNode* backUp = u;
	if (u == this)
		return f;
	else
	{
		fdd transform = f;
		const universeNode* transformLocal = this;

		while (transformLocal != u) { // while transformLocal isn't f's parent (u)
			if (u->_depth > transformLocal->_depth) {//should move u
				transform += u->_position;
				u = u->_parent;
			}
			else {// move transformLocal
				transform -= transformLocal->_position;
				transformLocal = transformLocal->_parent;
			}
		}
		if(std::isnan(transform.x)){
			getLocalPos(f,backUp);
		}
		return transform;
	}
}

fdd universeNode::getLocalVel(fdd f, universeNode* u) const
{
	assert(!std::isnan(_velocity.x));
	universeNode* backUp = u;
	if (u == this)
		return f;
	else
	{
		fdd transform = f;
		const universeNode* transformLocal = this;

		while (transformLocal != u) { // while transformLocal isn't f's parent (u)
			if (u->_depth > transformLocal->_depth) {//should move u
				transform += u->_velocity;
				u = u->_parent;
			}
			else {// move transformLocal
				transform -= transformLocal->_velocity;
				transformLocal = transformLocal->_parent;
			}
		}
		if(std::isnan(transform.x)){
			getLocalPos(f,backUp);
		}
		return transform;
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

void universeNode::addChild(universeNode& u)
{
	u._depth = _depth+1;
	if(u._parent!=this && u._parent != nullptr){
		u._position = getLocalPos(u._position,u._parent);
		u._velocity = getLocalVel(u._velocity,u._parent);
	}
	_children.push_back(u);
}

universeNode *universeNode::calculateBestParent(fdd pos, unsigned ID)
{
	std::vector<universeNode*> candidates;
	if(_parent!=nullptr)
	{
		candidates.push_back(_parent);
		const auto& brethren = _parent->getChildren();
		candidates.insert(candidates.end(),brethren.begin(),brethren.end());
	}
	else{
		candidates.push_back(this);
	}
	for(auto& child : getChildren()){
		if(child->getID() != ID)
			candidates.push_back(child);
	}
	// is first < second ?
	return *std::max_element(candidates.begin(),candidates.end(),[pos, this](universeNode* a, universeNode* b){
		bool isInsideA = a->getTheoreticalBlock(pos.getPoint3Di()) == metaBlock::nullBlock;
		bool isInsideB = b->getTheoreticalBlock(pos.getPoint3Di()) == metaBlock::nullBlock;
		if(isInsideA && !isInsideB)
			return false;
		else if(!isInsideA && isInsideB)
			return true;
		else{
			if(isInsideA){
				return a->getDiameter() > b->getDiameter();
			}
			//else
			fdd accA = a->getGravityAcceleration(a->getLocalPos(pos,this));
			fdd accB = b->getGravityAcceleration(b->getLocalPos(pos,this));
			return accB.magnitude()>accA.magnitude();
		}
		;
	});
}

void universeNode::removeChild(unsigned ID)
{
	auto iter = std::find_if(_children.begin(),_children.end(),[ID](universeNode& n){return ID == n._ID;});
	universeNode node;
	if(iter != _children.end()){
		_children.erase(iter);
	}
}

void universeNode::updatePositions(double dt)
{
	if (!physicsData.sleeping)
	{
		assert(!std::isnan(dt));
		assert(!std::isnan(_velocity.x));
		assert(!std::isnan(_position.x));
		_position += _velocity * dt;
	}
	for (universeNode& child : _children)
	{
		child.updatePositions(dt);
	}
}

void universeNode::applyThrusters(double dt)
{
	auto [thrust, position] = _thrustSystem->getThrust(dt);
	_velocity+=(thrust/_mass)*dt;
	if(_velocity.magnitude()> 0.1)
		physicsData.sleeping=false;
}

fdd universeNode::getGravityAcceleration(fdd localPosition)
{
	if(_artificialGravity)
		return *_artificialGravity;

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

void universeNode::updateThrusters(double dt)
{
    if(_thrustSystem)
		_thrustSystem->update(dt);
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
	return &*_collider;
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
	_collider = std::shared_ptr<rp3d::CollisionBody>(Services::collisionWorld->createCollisionBody(rp3d::Transform::identity()),[=](rp3d::CollisionBody* cb){
		Services::physicsMutex.lock();
		Services::collisionWorld->destroyCollisionBody(cb);
		Services::physicsMutex.unlock();
	});

	_collisionShape = std::make_shared<rp3d::BoxShape>(rp3d::Vector3{(rp3d::decimal)(_diameter / 2),(rp3d::decimal)(_diameter / 2),(rp3d::decimal)(_diameter / 2) });
	_collider->addCollisionShape(&*_collisionShape, rp3d::Transform::identity());
	Services::physicsMutex.unlock();
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

universeNode::universeNodeIterator universeNode::universeNodeIterator::operator++(int)
{
	universeNodeIterator old(*this);
	operator++();
	return old;
}


void to_json(nlohmann::json& j, const universeNode& f) {
    json interactablesJson;
    for(auto& i : f._interactables){
        interactablesJson.push_back(i->getJson());
    }
	j = json{ {"name", f._name},			{"mass", f._mass},
			 {"diameter", f._diameter}, {"type", f._type},
			 {"position", f._position},{"CoM", f._centerOfMass}, {"velocity", f._velocity},
			 {"children", f._children},{"id",f._ID},{"generator",*f._generator.get()},{"color",f._mainColor},{"thrustSystem",*f._thrustSystem},{"interactables",interactablesJson}};
	if(f._artificialGravity)
		j.emplace("artificial_gravity",*f._artificialGravity);
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
	if(j.contains("artificial_gravity")){
		f._artificialGravity = j.at("artificial_gravity").get<fdd>();
	}
	f._velocity = j.at("velocity").get<fdd>();
	f._children = std::vector<universeNode>();
	for (const nlohmann::json& element : j.at("children")) {
		f._children.emplace_back(element.get<universeNode>());
	}
	if(j.contains("interactables")){
	   for (const nlohmann::json& element : j.at("interactables")) {
		   f._interactables.push_back(getInteractableFromJson(element));
		   f._interactables.back()->setParent(&f);
	   }
	}
	nlohmann::json jt;
	if (j.contains("generator")) {
		jt = j.at("generator");
	}
	if(j.contains("color")){
		f._mainColor = j.at("color").get<HI2::Color>();
	}
	else{
		if(f._ID == 0){
			f._mainColor = HI2::Color::Pink;
		}
		if(f._ID == 1){
			f._mainColor = HI2::Color::Orange;
		}
		if(f._ID == 2){
			f._mainColor = HI2::Color::LightGrey;
		}
		if(f._ID == 3){
			f._mainColor = HI2::Color::Yellow;
		}
		if(f._ID == 4){
			f._mainColor = HI2::Color::Green;
		}
		if(f._ID == 5){
			f._mainColor = HI2::Color::Red;
		}
		if(f._ID == 6){
			f._mainColor = HI2::Color::Brown;
		}
		if(f._ID == 7){
			f._mainColor = HI2::Color::Brown;
		}
		if(f._ID == 8){
			f._mainColor = HI2::Color::Blue;
		}
		if(f._ID == 9){
			f._mainColor = HI2::Color::Blue;
		}
		if(f._ID == 10){
			f._mainColor = HI2::Color::LightGrey;
		}
	}
	f.connectGenerator(jt);
	f.populateColliders();
	if(j.contains("thrustSystem")){
	    auto ts = std::make_shared<thrustSystem>(j.at("thrustSystem").get<thrustSystem>());
	    ts->setParent(&f);
		f._thrustSystem = ts;
	}
    f._CL_cameraPosition = std::make_shared<fdd>();
	f._CL_chunkloader = std::make_shared<std::thread>(std::bind(&universeNode::_CL_chunkloaderFunc, &f));
	f._CL_mutex = std::make_shared<std::mutex>();
}

universeNode& universeNode::operator=(const universeNode& u)
{
	_position = u._position;
	_ID = u._ID;
	_centerOfMass = u._centerOfMass;
	_children = std::move(u._children);
	_chunks = std::move(u._chunks);
	_collider = u._collider;
	_collisionShape = u._collisionShape;
	_depth = u._depth;
	_diameter = u._diameter;
	_type = u._type;
	_velocity = u._velocity;
	_parent = u._parent;
	_mass = u._mass;
	_name = u._name;
	_mainColor = u._mainColor;
	_thrustSystem = u._thrustSystem;
	_thrustSystem->setParent(this);
	_artificialGravity = u._artificialGravity;

	for(auto& i : u._interactables){
		_interactables.push_back(getInteractableFromJson(i->getJson()));
		_interactables.back()->setParent(this);
	}

	physicsData = u.physicsData;

    _CL_cameraPosition = std::make_shared<fdd>();
    _CL_chunkloader = std::make_shared<std::thread>(std::bind(&universeNode::_CL_chunkloaderFunc, this));
	_CL_mutex = std::make_shared<std::mutex>();

	nlohmann::json jTemp;
	to_json(jTemp, *u._generator.get());
	connectGenerator(jTemp);
	return *this;
}