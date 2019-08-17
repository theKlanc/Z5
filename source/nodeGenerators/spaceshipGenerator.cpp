#include "nodeGenerators/spaceshipGenerator.hpp"

terrainChunk spaceshipGenerator::getChunk(const point3Di& p)
{
	terrainChunk chunk(p);
	if (p.x != 0 || p.y != 0 || p.z != 0)
	{
		return chunk;
	}
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				if(z==0)
					chunk.setBlock(&block::terrainTable.at(11), point3Di{ x,y,z });
				else if(x==0 && y ==0)
					chunk.setBlock(&block::terrainTable.at(11), point3Di{ x,y,z });
				else
					chunk.setBlock(&block::terrainTable.at(0), point3Di{ x,y,z });
			}
		}
	}
	chunk.setLoaded();
	return chunk;
}
