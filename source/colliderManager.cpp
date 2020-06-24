#include "colliderManager.hpp"
#include "services.hpp"

colliderManager::colliderManager()
{
	_boxShape = Services::physicsCommon.createBoxShape({0.5,0.5,0.5});
	customCollider* temp = new customCollider({0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5},{  {2,3,0,1},{5,2,1,4},{3,5,4,0},{4,1,0},{5,3,2}   });
	_colliders.insert(std::make_pair(colliderType::RAMP,temp));
	temp = new customCollider({-0.5,-0.5,-0.5, 0.5,-0.5,-0.5, 0.5,0.5,-0.5,-0.5,0.5,-0.5,-0.5,-0.5,0, 0.5,-0.5,0, 0.5,0.5,0, -0.5,0.5,0},{  {0,1,2,3},{1,5,6,2},{3,2,6,7},{0,3,7,4},{4,5,1,0},{5,4,7,6}   });
	_colliders.insert(std::make_pair(colliderType::HALF,temp));
}

colliderManager::~colliderManager()
{
	Services::physicsCommon.destroyBoxShape(_boxShape);
	for(auto tmp :_colliders){
		delete tmp.second;
	}
}

reactphysics3d::CollisionShape *colliderManager::getCollider(colliderType c)
{
	return _boxShape;
	if(c == colliderType::CUBE)
		return _boxShape;
	else
		return _colliders.at(c)->getCollider();
}


customCollider::customCollider(std::vector<float> v, std::vector<std::vector<int>> i)
{
	_verts = v;
	_idx.clear();
	_faces = new rp3d::PolygonVertexArray::PolygonFace[v.size()];
	for(int f = 0; f < i.size(); ++f){
		std::vector<int> face = i[f];
		_faces[f].indexBase = _idx.size();
		_idx.insert(_idx.end(),face.begin(),face.end());
		_faces[f].nbVertices = face.size();
	}
	_polyArray = new rp3d::PolygonVertexArray(_verts.size()/3, _verts.data(), 3*sizeof(float), _idx.data(),sizeof(int),i.size(),_faces, rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE, rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
	_polyMesh = Services::physicsCommon.createPolyhedronMesh(_polyArray);
	_convexMesh = Services::physicsCommon.createConvexMeshShape(_polyMesh);
}

customCollider::~customCollider()
{
	Services::physicsCommon.destroyConvexMeshShape(_convexMesh);
	Services::physicsCommon.destroyPolyhedronMesh(_polyMesh);
	delete _polyArray;
	delete _faces;
}

reactphysics3d::CollisionShape *customCollider::getCollider()
{
	return _convexMesh;
}
