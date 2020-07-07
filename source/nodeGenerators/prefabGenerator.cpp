#include "nodeGenerators/prefabGenerator.hpp"

prefabGenerator::prefabGenerator(std::string s)
{
	_prefab = prefab(s);
}

terrainChunk prefabGenerator::getChunk(const point3Di& indexedChunkPos)
{
	point3Di pos = indexedChunkPos * config::chunkSize;

	if (pos.x < floor((double)_prefab.getSize().x/-2.0f) - config::chunkSize || pos.y < floor((double)_prefab.getSize().y/-2.0f) - config::chunkSize || pos.z < floor((double)_prefab.getSize().z/-2.0f) - config::chunkSize || pos.x > ceil((double)_prefab.getSize().x/2.0f) || pos.y > ceil((double)_prefab.getSize().y/2.0f) || pos.z > ceil((double)_prefab.getSize().z/2.0f))
	{
		return terrainChunk();
	}

	pos = pos + _prefab.getSize() / 2;

	terrainChunk chunk(indexedChunkPos);

	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				chunk.setBlock(_prefab.get(pos + point3Di{x,y,z}), point3Di{ x,y,z });
			}
		}
	}
	chunk.clearDirtyFlag();
	chunk.setLoaded();
	return chunk;
}

baseBlock &prefabGenerator::getTopBlock(const point2D &p)
{
	int height = _prefab.getSize().z-1;
	point2D pos = p;
	pos.x += _prefab.getSize().x/2;
	pos.y += _prefab.getSize().y/2;
	while(!_prefab.get({pos.x,pos.y,height}).base->solid && height-- > 0)
	{}
	return *_prefab.get({pos.x,pos.y,height+1}).base;
}

int prefabGenerator::getHeight(const point2D &p)
{
	int height = _prefab.getSize().z-1;
	point2D pos = p;
	pos.x += _prefab.getSize().x/2;
	pos.y += _prefab.getSize().y/2;
	while(!_prefab.get({pos.x,pos.y,height}).base->solid && height-- > 0)
	{}
	return height+1;
}

nlohmann::json prefabGenerator::getJson() const
{
	return json{{"type","prefab"},{"generator",{ {"name", _prefab.getName()} }}};
}

void from_json(const nlohmann::json &j, prefabGenerator &pg)
{
	pg._prefab = prefab(j.at("name").get<std::string>());
}
