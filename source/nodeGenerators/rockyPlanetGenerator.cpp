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

	_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[2]);

	_terrainPainter.addSection(terrainSection(0.0001, 60, baseBlock::terrainTable[6])); // fixed stone
	_terrainPainter.addSection(terrainSection(0.2, 60, baseBlock::terrainTable[6])); // stone
	_terrainPainter.addSection(terrainSection(0.4, 60, baseBlock::terrainTable[4])); //dirt
	_terrainPainter.addSection(terrainSection(0.48, 60, baseBlock::terrainTable[10])); // underwater sand
	_terrainPainter.addSection(terrainSection(0.5, 5, baseBlock::terrainTable[10])); // surface sand
	_terrainPainter.addSection(terrainSection(0.6, 40, baseBlock::terrainTable[4], &baseBlock::terrainTable[5])); //grassdirt
	_terrainPainter.addSection(terrainSection(0.65, 10, baseBlock::terrainTable[14])); //deep grass
	_terrainPainter.addSection(terrainSection(0.7, 60, baseBlock::terrainTable[6])); //stone
	_terrainPainter.addSection(terrainSection(0.75, 40, baseBlock::terrainTable[6], &baseBlock::terrainTable[13])); //snowstone
	_terrainPainter.addSection(terrainSection(0.8, 40, baseBlock::terrainTable[13])); //snow
	_terrainPainter.addSection(terrainSection(1, 10, baseBlock::terrainTable[13])); //snow


}

terrainChunk rockyPlanetGenerator::getChunk(const point3Di& p)const
{
	if (p.z < 0 || fdd{ 0,0,0,0 }.distance2D(fdd{ (double)(p.x) * config::chunkSize,(double)(p.y) * config::chunkSize,0,0 }) > (_diameter / 2)*config::chunkSize)
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
					chunk.setBlock({&_terrainPainter.getBlock(currentHeight, noise),(blockRotation)(rand()%4)}, point3Di{ x,y,z });
				}
			}
		}
	}
	fillWater(chunk, p, 240);
	populateTrees(chunk, p, 240 );
	chunk.setLoaded();
	chunk.clearDirtyFlag();
	return chunk;
}

void rockyPlanetGenerator::fillWater(terrainChunk& c, const point3Di p, int waterLevel)const
{
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				int currentHeight = p.z * config::chunkSize + z;
				if (currentHeight < waterLevel)
				{
					if (*c.getBlock({ x,y,z }).base == baseBlock::terrainTable[2]) {
						c.setBlock({&baseBlock::terrainTable[3],(blockRotation)(rand()%4)}, { x,y,z });
					}
				}
			}
		}
	}
}

void rockyPlanetGenerator::populateTrees(terrainChunk& c, const point3Di p, int waterLevel)const
{
	for (int x = -2; x <= config::chunkSize + 2; ++x) {
		for (int y = -2; y <= config::chunkSize + 2; ++y) {
			if (fdd{ 0,0,0,0 }.distance2D(fdd{ (double)(p.x * config::chunkSize) + x,(double)(p.y * config::chunkSize) + y,0,0 }) <= _diameter / 2) {
				if ((p.x * config::chunkSize + x) % 9 == 0 && (p.y * config::chunkSize + y) % 9 == 0) {
					int floorHeight = _terrainPainter.getHeight(getNoise({ p.x * config::chunkSize + x,p.y * config::chunkSize + y }));
					if (floorHeight > waterLevel) {
						if (_terrainPainter.getBlock(floorHeight).ID == 14 || _terrainPainter.getBlock(floorHeight).ID == 4) {
							if (floorHeight + 1 - (p.z * config::chunkSize) > - 6 && floorHeight + 1 - (p.z * config::chunkSize) < config::chunkSize + 6)
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

void rockyPlanetGenerator::placeTree(terrainChunk& c, const point3Di p)const
{
	for (int x = p.x - 2; x <= p.x + 2; x++) // crear copa
	{
		for (int y = p.y - 2; y <= p.y + 2; y++)
		{
			for (int z = p.z + 2; z <= p.z + 5; z++)
			{
				if (x >= 0 && x < config::chunkSize && y >= 0 && y < config::chunkSize && z >= 0 && z < config::chunkSize)
				{
					c.setBlock({&baseBlock::terrainTable[9],(blockRotation)(rand()%4)}, { x,y,z });
				}
			}
		}
	}

	for (int z = p.z - 2; z < p.z + 5; z++) // crear troncasso
	{
		if (p.x >= 0 && p.x < config::chunkSize && p.y >= 0 && p.y < config::chunkSize && z >= 0 && z < config::chunkSize)
		{
			c.setBlock({&baseBlock::terrainTable[8],UP}, { p.x,p.y,z });
		}
	}
}

