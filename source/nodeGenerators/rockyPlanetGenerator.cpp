#include "nodeGenerators/rockyPlanetGenerator.hpp"
#include "terrainChunk.hpp"
#include "fdd.hpp"

rockyPlanetGenerator::rockyPlanetGenerator(unsigned s, unsigned diameter) : nodeGenerator(s), _diameter(diameter)
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetFractalGain(0.4);//0.7
	_noiseGenerator.SetFrequency(0.0004f);//0.002
	_noiseGenerator.SetFractalLacunarity(3);//2
	_noiseGenerator.SetFractalOctaves(5);//5?

	_terrainPainter.setEmptyBlock(&block::terrainTable[1]);


	_terrainPainter.addSection(terrainSection(0.0001, 60, block::terrainTable[5])); // fixed stone
	_terrainPainter.addSection(terrainSection(0.2, 60, block::terrainTable[5])); // stone
	_terrainPainter.addSection(terrainSection(0.4, 60, block::terrainTable[3])); //dirt
	_terrainPainter.addSection(terrainSection(0.48, 60, block::terrainTable[9])); // underwater sand
	_terrainPainter.addSection(terrainSection(0.5, 5, block::terrainTable[9])); // surface sand
	_terrainPainter.addSection(terrainSection(0.6, 40, block::terrainTable[3], &block::terrainTable[4])); //grassdirt
	_terrainPainter.addSection(terrainSection(0.65, 10, block::terrainTable[13])); //deep grass
	_terrainPainter.addSection(terrainSection(0.7, 60, block::terrainTable[5])); //stone
	_terrainPainter.addSection(terrainSection(0.75, 40, block::terrainTable[5], &block::terrainTable[12])); //snowstone
	_terrainPainter.addSection(terrainSection(0.8, 40, block::terrainTable[12])); //snow
	_terrainPainter.addSection(terrainSection(1, 10, block::terrainTable[12])); //snow


}

terrainChunk rockyPlanetGenerator::getChunk(const point3Di& p)
{
	if (p.z < 0 || fdd{ 0,0,0,0 }.distance2D(fdd{ (double)(p.x) * config::chunkSize,(double)(p.y) * config::chunkSize,0,0 }) > _diameter / 2)
	{
		return terrainChunk();
	}

	terrainChunk chunk(p);

	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			if (fdd{ 0,0,0,0 }.distance2D(fdd{ (double)(p.x * config::chunkSize) + x,(double)(p.y * config::chunkSize) + y,0,0 }) <= _diameter / 2)
			{
				double noise = getNoise({ (p.x * config::chunkSize) + x, (p.y * config::chunkSize) + y });
				for (int z = 0; z < config::chunkSize; ++z) {
					unsigned int currentHeight = p.z * config::chunkSize + z;
					chunk.setBlock(&_terrainPainter.getBlock(currentHeight, noise), point3Di{ x,y,z });
				}
			}
		}
	}
	fillWater(chunk, p, 240);
	populateTrees(chunk, p, 240 );
	chunk.setLoaded();
	return chunk;
}

void rockyPlanetGenerator::fillWater(terrainChunk& c, const point3Di p, int waterLevel)
{
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				int currentHeight = p.z * config::chunkSize + z;
				if (currentHeight < waterLevel)
				{
					if (c.getBlock({ x,y,z }) == block::terrainTable[1]) {
						c.setBlock(&block::terrainTable[2], { x,y,z });
					}
				}
			}
		}
	}
}

void rockyPlanetGenerator::populateTrees(terrainChunk& c, const point3Di p, int waterLevel)
{
	for (int x = -2; x <= config::chunkSize + 2; ++x) {
		for (int y = -2; y <= config::chunkSize + 2; ++y) {
			if (fdd{ 0,0,0,0 }.distance2D(fdd{ (double)(p.x * config::chunkSize) + x,(double)(p.y * config::chunkSize) + y,0,0 }) <= _diameter / 2) {
				if ((p.x * config::chunkSize + x) % 9 == 0 && (p.y * config::chunkSize + y) % 9 == 0) {
					int floorHeight = _terrainPainter.getHeight(getNoise({ p.x * config::chunkSize + x,p.y * config::chunkSize + y }));
					if (floorHeight > waterLevel) {
						if (_terrainPainter.getBlock(floorHeight).ID == 13 || _terrainPainter.getBlock(floorHeight).ID == 3) {
							if (floorHeight + 1 - (p.z * config::chunkSize) > -6 && floorHeight + 1 - (p.z * config::chunkSize) < config::chunkSize + 6)
							{
								placeTree(c, { x,y,floorHeight + 1 - (p.z * config::chunkSize) });
							}
						}
					}
				}
			}
		}
	}
}

void rockyPlanetGenerator::placeTree(terrainChunk& c, const point3Di p)
{
	for (int x = p.x - 2; x <= p.x + 2; x++) // crear copa
	{
		for (int y = p.y - 2; y <= p.y + 2; y++)
		{
			for (int z = p.z + 2; z <= p.z + 5; z++)
			{
				if (x >= 0 && x < config::chunkSize && y >= 0 && y < config::chunkSize && z >= 0 && z < config::chunkSize)
				{
					c.setBlock(&block::terrainTable[8], { x,y,z });
				}
			}
		}
	}

	for (int z = p.z - 2; z < p.z + 5; z++) // crear troncasso
	{
		if (p.x >= 0 && p.x < config::chunkSize && p.y >= 0 && p.y < config::chunkSize && z >= 0 && z < config::chunkSize)
		{
			c.setBlock(&block::terrainTable[7], { p.x,p.y,z });
		}
	}
}

