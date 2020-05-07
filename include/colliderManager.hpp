#pragma once
#include "HI2.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include <unordered_map>


enum class colliderType{
	CUBE = 0,
	RAMP = 1,
	HALF = 2,
};

class customCollider{
	public:
		customCollider(std::vector<float> v,std::vector<std::vector<int>> i);
		~customCollider();
		rp3d::CollisionShape* getCollider();
	private:

		std::vector<float> _verts;
		std::vector<int> _idx;

		rp3d::PolygonVertexArray* _polyArray;
		rp3d::PolyhedronMesh* _polyMesh;
		rp3d::ConvexMeshShape* _convexMesh;
		rp3d::PolygonVertexArray::PolygonFace *_faces = nullptr;
};

class colliderManager {
  public:
	colliderManager();
	~colliderManager();
	rp3d::CollisionShape* getCollider(colliderType c);

  private:
	rp3d::BoxShape _boxShape = rp3d::BoxShape({ 0.5,0.5,0.5 });
	std::unordered_map<colliderType, customCollider*> _colliders;
};