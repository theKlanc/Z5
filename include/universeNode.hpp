#pragma once
#include <memory>
#include <vector>
#include "fdd.hpp"
#include "json.hpp"
#include "block.hpp"
#include "terrainChunk.hpp"
#include "nodeGenerator.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

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


class universeNode {
public:
	universeNode() :_chunks(config::chunkLoadDiameter* config::chunkLoadDiameter* config::chunkLoadDiameter){}
	block& getBlock(const point3Di &pos);
	void setBlock(block* b, const point3Di &pos);
	void updateChunks(const fdd& playerPos, universeNode* u);
	std::vector<universeNode*> nodesToDraw(fdd f,universeNode* u);
	fdd getLocalPos(fdd f,universeNode* u) const;
	fdd getLocalVel(fdd f,universeNode* u) const;
	fdd getPosition();
	fdd getVelocity();
	void setVelocity(fdd v);

	universeNode* getParent();
	double getMass();
	unsigned int getHeight(const point2D &pos);
	rp3d::CollisionBody* getNodeCollider();
	std::vector<rp3d::CollisionBody*> getTerrainColliders(fdd p, universeNode* parent);

	void populateColliders();
	void linkChildren();
	bool findNodeByID(const unsigned int& id, universeNode*& result);
	bool drawBefore(universeNode& r)const;
	void clean();

	bool operator!= (const universeNode& right)const;
	bool operator== (const universeNode& right)const;
	friend void to_json(nlohmann::json &j, const universeNode &f);
	friend void from_json(const json& j, universeNode& f);

  private:

	bool shouldDraw(fdd f);
	point3Di chunkFromPos(const fdd& pos);


	void iUpdateChunks(const point3Di& localChunk);
	terrainChunk& chunkAt(const point3Di &pos);
	terrainChunk& getChunk(const point3Di &pos);
	int chunkIndex(const point3Di &pos) const;

	std::string _name;
	double _mass; // mass in kg
	double _diameter; // diameter in m
	fdd _position;
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

void to_json(json& j, const universeNode& f);
void from_json(const json& j, universeNode& f);