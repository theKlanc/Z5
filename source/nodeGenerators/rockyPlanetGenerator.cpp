#include "nodeGenerators/rockyPlanetGenerator.hpp"

terrainChunk rockyPlanetGenerator::getChunk(const point3Di& p)
{
	terrainChunk chunk(p);
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			int terrainHeight = ((_noiseGenerator.GetNoise((p.x * config::chunkSize) + x, (p.y * config::chunkSize) + y) + 1) / 2)*_maxHeight;
			for (int z = 0; z < config::chunkSize; ++z) {
				unsigned int currentHeight = p.z * config::chunkSize + z;
				if (currentHeight > terrainHeight) {
					chunk.setBlock(&block::terrainTable.at(1), point3Di{ x,y,z });
				}
				else if (currentHeight == terrainHeight) {
					chunk.setBlock(&block::terrainTable.at(4), point3Di{ x,y,z });
				}
				else {
					chunk.setBlock(&block::terrainTable.at(3), point3Di{ x,y,z });
				}
			}
		}
	}

	return chunk;
}
