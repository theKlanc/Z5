#include "colliderManager.hpp"

colliderManager::colliderManager()
{
	_colliders.insert(std::make_pair(colliderType::RAMP,customCollider({0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5},{  {2,3,0,1},{5,2,1,4},{3,5,4,0},{4,1,0},{5,3,2}   })));
}

colliderManager::~colliderManager()
{

}

reactphysics3d::CollisionShape *colliderManager::getCollider(colliderType c)
{
	if(c == colliderType::CUBE)
		return &_boxShape;
	else
		return _colliders.at(c).getCollider();
}


customCollider::customCollider(std::vector<float> v, std::vector<std::vector<int>> i)
{
	_verts = v;
	_idx.clear();
	_faces = new rp3d::PolygonVertexArray::PolygonFace[v.size()];
	for(int f = 0; f < i.size(); ++f){
		auto face = i[f];
		_faces[f].indexBase = _idx.size();
		_idx.insert(_idx.end(),face.begin(),face.end());
		_faces[f].nbVertices = face.size();
	}
	_polyArray = new rp3d::PolygonVertexArray(_verts.size()/3, _verts.data(), 3*sizeof(float), _idx.data(),sizeof(int),i.size(),_faces, rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE, rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
	_polyMesh = new rp3d::PolyhedronMesh(_polyArray);
	_convexMesh = new rp3d::ConvexMeshShape(_polyMesh);
}

customCollider::~customCollider()
{
	//delete[] _faces;
}

reactphysics3d::CollisionShape *customCollider::getCollider()
{
	return _convexMesh;
}
