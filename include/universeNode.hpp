#pragma once
#include <memory>
#include <vector>
#include <queue>
#include "fdd.hpp"
#include "json.hpp"
#include "block.hpp"
#include "terrainChunk.hpp"
#include "nodeGenerator.hpp"
#include "HI2.hpp"
#include "thrustSystem.hpp"
#include "interactable.hpp"
#include <optional>

const double G = (6.67408e-11);

using nlohmann::json;

enum class nodeType{
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
	metaBlock& getBlock(const point3Di &pos);
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
	unsigned getDepth();
	std::string getName();
	nodeType getType();
	std::vector<universeNode*> getChildren();
	void addChild(universeNode u);
	void updatePositions(double dt);
	void applyThrusters(double dt);
	fdd getGravityAcceleration(fdd localPosition);
	universeNode& operator=(const universeNode& n);
	void updateThrusters(double dt);


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

	//Physics private obj
	struct
	{
		bool sleeping = false;
	private:
		rp3d::Vector3 contactNormal;
		double maxContactDepth = 0;
		
		friend class physicsEngine;
		friend class universeNode;
	}physicsData;

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

	
	void connectGenerator(std::unique_ptr<nodeGenerator> ng);
	HI2::Color getMainColor();

	interactable* getClosestInteractable(fdd pos);
	point3Di getClosestInteractablePos(fdd pos);

	std::shared_ptr<thrustSystem> getThrustSystem();
private:

	bool shouldDraw(fdd f);
	point3Di chunkFromPos(const fdd& pos);
	point3Di chunkFromPos(const point3Di& pos);
	void connectGenerator(const nlohmann::json& j);

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
	std::optional<fdd> _artificialGravity;

	std::vector<terrainChunk> _chunks; // So big, should be on the heap. So fat, too much for the stack.

	std::vector<std::unique_ptr<interactable>> _interactables;

	nodeType _type;
	std::vector<universeNode> _children;
	universeNode* _parent;
	std::unique_ptr<nodeGenerator> _generator;
	unsigned int _depth;
	unsigned int _ID;
	rp3d::CollisionBody* _collider;
	rp3d::CollisionShape* _collisionShape;

	HI2::Color _mainColor = HI2::Color::White;

	std::shared_ptr<thrustSystem> _thrustSystem = std::make_shared<thrustSystem>();
};


NLOHMANN_JSON_SERIALIZE_ENUM( nodeType, {
	{nodeType::STAR,"STAR"},
	{nodeType::BLACK_HOLE,"BLACK_HOLE"},
	{nodeType::PLANET_GAS,"PLANET_GAS"},
	{nodeType::PLANET_ROCK,"PLANET_ROCK"},
	{nodeType::ASTEROID,"ASTEROID"},
	{nodeType::SATELLITE_NATURAL,"SATELLITE_NATURAL"},
	{nodeType::SATELLITE_ARTIFICIAL,"SATELLITE_ARTIFICIAL"},
	{nodeType::SPACE_STATION,"SPACE_STATION"},
	{nodeType::SPACESHIP,"SPACESHIP"},
})

void to_json(json& j, const universeNode& f);
void from_json(const json& j, universeNode& f);

