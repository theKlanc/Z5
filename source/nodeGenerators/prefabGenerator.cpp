#include "nodeGenerators/prefabGenerator.hpp"

prefabGenerator::prefabGenerator(std::string s)
{
	_prefab = prefab(s);
}

terrainChunk prefabGenerator::getChunk(const point3Di& indexedChunkPos)
{
	point3Di pos = indexedChunkPos * config::chunkSize;
	if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x+config::chunkSize-1 >= _prefab.getSize().x || pos.y+config::chunkSize-1 >= _prefab.getSize().y || pos.z+config::chunkSize-1 >= _prefab.getSize().z)
	{
		return terrainChunk();
	}

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
	int height = _prefab.getSize().z;
	while(!_prefab.get({p.x,p.y,height}).base->solid && height > 0)
	{}
	return *_prefab.get({p.x,p.y,height}).base;
}

int prefabGenerator::getHeight(const point2D &p)
{
	int height = _prefab.getSize().z-1;
	while(!_prefab.get({p.x,p.y,height}).base->solid && height-- > 0)
	{}
	return height;
}

nlohmann::json prefabGenerator::getJson() const
{
	return json{{"type","prefab"},{"generator",{ {"name", _prefab.getName()} }}};
}

void from_json(const nlohmann::json &j, prefabGenerator &pg)
{
	pg._prefab = prefab(j.at("name").get<std::string>());
}
