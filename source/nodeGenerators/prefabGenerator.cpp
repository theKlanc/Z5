#include "nodeGenerators/prefabGenerator.hpp"

terrainChunk prefabGenerator::getChunk(const point3Di& p)const
{
	if (p.x != 1 || p.y != 1 || p.z != 1)
	{
		return terrainChunk();
	}

	terrainChunk chunk(p);

	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				if (z == 0)
					chunk.setBlock({&baseBlock::terrainTable.at(12),UP, true}, point3Di{ x,y,z });
				else if (x == 0 && y == 0)
					chunk.setBlock({&baseBlock::terrainTable.at(12),UP, true}, point3Di{ x,y,z });
				else if (z == 3)
					chunk.setBlock({&baseBlock::terrainTable.at(11),UP, true}, point3Di{ x,y,z });
				else
					chunk.setBlock({&baseBlock::terrainTable.at(1),UP, true}, point3Di{ x,y,z });
			}
		}
	}
	chunk.setLoaded();
	return chunk;
}

baseBlock &prefabGenerator::getTopBlock(const point2D &p) const
{
	return baseBlock::terrainTable[0];
}

int prefabGenerator::getHeight(const point2D &p) const
{
	return 0;
}

nlohmann::json prefabGenerator::getJson() const
{
	return json{{"type","prefab"},{"generator",{ {"stub", "stub"} }}};
}

void from_json(const nlohmann::json &j, prefabGenerator &pg)
{
	pg = prefabGenerator();
}
