#pragma once
#include <memory>
#include <vector>
#include <queue>
#include "fdd.hpp"
#include "json.hpp"
#include "block.hpp"
#include "terrainChunk.hpp"
#include "nodeGenerator.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

const double G = (6.67408e-11);

using nlohmann::json;

enum nodeType{
	STAR,
	BLACK_HOLE,
	PLANET_GAS,
	PLANET_ROCK,
	ASTEROID,
	SATELLITE_NATURAL,
	SATELLITE_ARTIFICIAL,
	SPACE_STATION,
	SPACESHIP,
};

class universeNode {
public:
	universeNode() :_chunks(config::chunkLoadDiameter* config::chunkLoadDiameter* config::chunkLoadDiameter){}
	universeNode(const universeNode& u);
	universeNode(std::string name, double mass, double diameter, fdd pos, fdd com, fdd vel, nodeType type,universeNode* parent, unsigned int id);
	baseBlock& getTopBlock(const point2D& pos);
	metaBlock getBlock(const point3Di &pos);
	void setBlock(metaBlock b, const point3Di &pos);
	void updateChunks(const fdd& playerPos, universeNode* u);
	std::vector<universeNode*> nodesToDraw(fdd f,universeNode* u);
	fdd getLocalPos(fdd f,universeNode* u) const;
	fdd getLocalVel(fdd f,universeNode* u) const;
	fdd getPosition();
	fdd getVelocity();
	fdd getCenterOfMass();
	void setVelocity(fdd v);
	void setPosition(fdd p);
	unsigned int getID();
	double getMass();
	double getDiameter();
	std::vector<universeNode*> getChildren();
	void addChild(universeNode u);
	void updatePositions(double dt);
	fdd getGravityAcceleration(fdd localPosition);
	universeNode& operator=(const universeNode& n);

	std::vector<terrainChunk>& getChunks();

	universeNode* getParent();
	unsigned int getHeight(const point2D &pos);
	rp3d::CollisionBody* getNodeCollider();
	std::vector<terrainChunk*> getCollidableChunks(fdd p, const point3Dd& size, universeNode* parent);

	void populateColliders();
	void linkChildren();
	bool findNodeByID(const unsigned int& id, universeNode*& result);
	bool drawBefore(universeNode& r)const;
	void clean();

	bool operator!= (const universeNode& right)const;
	bool operator== (const universeNode& right)const;
	friend void to_json(nlohmann::json &j, const universeNode &f);
	friend void from_json(const json& j, universeNode& f);

	class universeNodeIterator : public std::iterator<std::forward_iterator_tag,universeNode>{
	public:
		universeNodeIterator(){}
		universeNodeIterator(const universeNodeIterator& r);
		universeNodeIterator& operator=(const universeNodeIterator& r);
		bool operator==(const universeNodeIterator& r);
		bool operator!=(const universeNodeIterator& r);
		universeNode& operator*();
		universeNode& operator->();
		universeNodeIterator& operator++();
		universeNodeIterator operator++(int devnull);
	private:
		universeNode* p=nullptr;
		std::queue<universeNode*> q;
		friend class universeNode;
	};
	universeNodeIterator begin();
	universeNodeIterator end();
  private:

	bool shouldDraw(fdd f);
	point3Di chunkFromPos(const fdd& pos);
	point3Di chunkFromPos(const point3Di& pos);
	void connectGenerator();

	void iUpdateChunks(const point3Di& localChunk);
	terrainChunk& chunkAt(const point3Di &pos);
	terrainChunk& getChunk(const point3Di &pos);
	int chunkIndex(const point3Di &pos) const;

	std::string _name;
	double _mass; // mass in kg
	double _diameter; // diameter in m
	fdd _position;
	fdd _centerOfMass;
	fdd _velocity;

	std::vector<terrainChunk> _chunks; // So big, should be on the heap. So fat, too much for the stack.

	nodeType _type;
	std::vector<universeNode> _children;
	universeNode* _parent;
	std::unique_ptr<nodeGenerator> _generator;
	unsigned int _depth;
	unsigned int _ID;
	rp3d::CollisionBody* _collider;
	rp3d::CollisionShape* _collisionShape;
  
};


NLOHMANN_JSON_SERIALIZE_ENUM( nodeType, {
	{STAR,"STAR"},
	{BLACK_HOLE,"BLACK_HOLE"},
	{PLANET_GAS,"PLANET_GAS"},
	{PLANET_ROCK,"PLANET_ROCK"},
	{ASTEROID,"ASTEROID"},
	{SATELLITE_NATURAL,"SATELLITE_NATURAL"},
	{SATELLITE_ARTIFICIAL,"SATELLITE_ARTIFICIAL"},
	{SPACE_STATION,"SPACE_STATION"},
	{SPACESHIP,"SPACESHIP"},
})

void to_json(json& j, const universeNode& f);
void from_json(const json& j, universeNode& f);