#include "nodeGenerators/rockyPlanetGenerator.hpp"

terrainChunk rockyPlanetGenerator::getChunk(const point3Di& p)
{
	int _maxHeight = 40;
	terrainChunk chunk(p);
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			int height = _noiseGenerator.GetNoise(p.x * config::chunkSize + x, p.y * config::chunkSize + y);
			for (int z = 0; z < config::chunkSize; ++z) {
				if (p.z * config::chunkSize + z > height * _maxHeight) {
					chunk.setBlock(&block::terrainTable.at(1), point3Di{ x,y,z });
				}
				else if (p.z * config::chunkSize + z == height * _maxHeight) {
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
