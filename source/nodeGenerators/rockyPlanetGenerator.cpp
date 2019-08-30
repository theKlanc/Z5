#include "nodeGenerators/rockyPlanetGenerator.hpp"
#include "terrainChunk.hpp"

rockyPlanetGenerator::rockyPlanetGenerator(unsigned s) : nodeGenerator(s)
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetFractalGain(0.4);//0.7
	_noiseGenerator.SetFrequency(0.02f);//0.002
	_noiseGenerator.SetFractalLacunarity(3);//2
	_noiseGenerator.SetFractalOctaves(5);//5?

	_terrainPainter.setEmptyBlock(&block::terrainTable[1]);

	_terrainPainter.addSection(terrainSection(0.2, 60, block::terrainTable[5]));
	_terrainPainter.addSection(terrainSection(0.4, 60, block::terrainTable[3]));
	_terrainPainter.addSection(terrainSection(0.5, 20, block::terrainTable[9]));
	_terrainPainter.addSection(terrainSection(0.6, 60, block::terrainTable[3], &block::terrainTable[4]));
	_terrainPainter.addSection(terrainSection(0.7, 60, block::terrainTable[5]));
	_terrainPainter.addSection(terrainSection(0.75, 40, block::terrainTable[5], &block::terrainTable[12]));
	_terrainPainter.addSection(terrainSection(1, 20, block::terrainTable[12]));


}

terrainChunk rockyPlanetGenerator::getChunk(const point3Di& p)
{
	terrainChunk chunk(p);
	if (p.z < 0)
	{
		return chunk;
	}
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			double noise = ((_noiseGenerator.GetNoise((p.x * config::chunkSize) + x, (p.y * config::chunkSize) + y) + 1) / 2);
			for (int z = 0; z < config::chunkSize; ++z) {
				unsigned int currentHeight = p.z * config::chunkSize + z;
				chunk.setBlock(&_terrainPainter.getBlock(currentHeight, noise), point3Di{ x,y,z });
			}
		}
	}
	fillWater(chunk,p);
	chunk.setLoaded();
	return chunk;
}

void rockyPlanetGenerator::fillWater(terrainChunk& c, const point3Di p)
{
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				unsigned int currentHeight = p.z * config::chunkSize + z;
				if (currentHeight < 140)
				{
					if (c.getBlock({ x,y,z }) == block::terrainTable[1]){
						c.setBlock(&block::terrainTable[2],{x,y,z});
					}
				}

			}
		}
	}
}

