#include "nodeGenerators/terrainPainterGenerator.hpp"
#include "terrainChunk.hpp"
#include "fdd.hpp"
#include <iostream>

terrainPainterGenerator::terrainPainterGenerator()
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetFractalGain(0.4);//0.7
	_noiseGenerator.SetFrequency(0.0004f);//0.002
	_noiseGenerator.SetFractalLacunarity(3);//2
	_noiseGenerator.SetFractalOctaves(5);//5?
}

terrainPainterGenerator::terrainPainterGenerator(unsigned seed, unsigned diameter) : nodeGenerator(seed), _diameter(diameter)
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetFractalGain(0.4);//0.7
	_noiseGenerator.SetFrequency(0.0004f);//0.002
	_noiseGenerator.SetFractalLacunarity(3);//2
	_noiseGenerator.SetFractalOctaves(5);//5?
	if(_seed == 1){ // Sun
		_emptyBlockID = 1;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 400, baseBlock::terrainTable[104])); // fixed solid
		_terrainPainter.addSection(terrainSection(0.0002, 100, baseBlock::terrainTable[102])); // fixed stone
		_terrainPainter.addSection(terrainSection(1, 1, baseBlock::terrainTable[0])); // fixed stone

	}
	else if(_seed == 2){ // Mercury
		_emptyBlockID = 1;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[97])); // fixed solid
		_terrainPainter.addSection(terrainSection(1, 100, baseBlock::terrainTable[97],&baseBlock::terrainTable[7])); // fixed stone
	}
	else if(_seed == 3){ // Venus
		_emptyBlockID = 103;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[98])); // fixed solid
		_terrainPainter.addSection(terrainSection(1, 100, baseBlock::terrainTable[98],&baseBlock::terrainTable[99])); // fixed stone
	}
	else if(_seed == 4){ // Gaia
		_emptyBlockID = 2;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 60, baseBlock::terrainTable[6])); // fixed stone
		_terrainPainter.addSection(terrainSection(0.2, 60, baseBlock::terrainTable[6])); // stone
		_terrainPainter.addSection(terrainSection(0.4, 60, baseBlock::terrainTable[4])); //dirt
		_terrainPainter.addSection(terrainSection(0.48, 60, baseBlock::terrainTable[10])); // underwater sand
		_terrainPainter.addSection(terrainSection(0.5, 5, baseBlock::terrainTable[29],&baseBlock::terrainTable[10])); // surface sand
		_terrainPainter.addSection(terrainSection(0.6, 40, baseBlock::terrainTable[4], &baseBlock::terrainTable[5])); //grassdirt
		_terrainPainter.addSection(terrainSection(0.65, 10, baseBlock::terrainTable[4], &baseBlock::terrainTable[17])); //deep grass
		_terrainPainter.addSection(terrainSection(0.7, 60, baseBlock::terrainTable[98], &baseBlock::terrainTable[6])); //stone
		_terrainPainter.addSection(terrainSection(0.75, 40, baseBlock::terrainTable[6], &baseBlock::terrainTable[16])); //snowstone
		_terrainPainter.addSection(terrainSection(0.8, 40, baseBlock::terrainTable[107], &baseBlock::terrainTable[16])); //snow
		_terrainPainter.addSection(terrainSection(1, 10, baseBlock::terrainTable[107], &baseBlock::terrainTable[16])); //snow

		_liquidLevel=240;
		_liquidID = 3;
	}
	else if(_seed == 5){ // Mars
		_emptyBlockID = 2;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[98])); // fixed solid
		_terrainPainter.addSection(terrainSection(1, 100, baseBlock::terrainTable[98],&baseBlock::terrainTable[99])); // fixed stone
	}
	else if(_seed == 6){ // Jupiter
		_emptyBlockID = 102;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[102])); // fixed solid
		_terrainPainter.addSection(terrainSection(0.0002, 100, baseBlock::terrainTable[100])); // fixed solid
		_terrainPainter.addSection(terrainSection(1, 1, baseBlock::terrainTable[102])); // fixed stone
	}
	else if(_seed == 7){ // Saturn
		_emptyBlockID = 102;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[102])); // fixed solid
		_terrainPainter.addSection(terrainSection(0.0002, 100, baseBlock::terrainTable[100])); // fixed solid
		_terrainPainter.addSection(terrainSection(1, 1, baseBlock::terrainTable[102])); // fixed stone
	}
	else if(_seed == 8){ // Uranus
		_emptyBlockID = 101;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[106])); // fixed ice
		_terrainPainter.addSection(terrainSection(0.0002, 100, baseBlock::terrainTable[101])); // fixed atmo
		_terrainPainter.addSection(terrainSection(1, 50, baseBlock::terrainTable[101])); // atmo
	}
	else if(_seed == 9){ // Neptune
		_emptyBlockID = 101;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[106])); // fixed ice
		_terrainPainter.addSection(terrainSection(0.0002, 100, baseBlock::terrainTable[101])); // fixed atmo
		_terrainPainter.addSection(terrainSection(1, 50, baseBlock::terrainTable[101])); // atmo
	}
	else if(_seed == 10){ // Selene
		_emptyBlockID = 1;

		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);

		_terrainPainter.addSection(terrainSection(0.0001, 200, baseBlock::terrainTable[97])); // fixed solid
		_terrainPainter.addSection(terrainSection(1, 100, baseBlock::terrainTable[97],&baseBlock::terrainTable[7])); // fixed stone
	}
	else{
		_emptyBlockID = 0;
		_terrainPainter.setEmptyBlock(&baseBlock::terrainTable[_emptyBlockID]);
		_terrainPainter.addSection(terrainSection(0.0001, 5, baseBlock::terrainTable[106])); // fixed errorinium
		_terrainPainter.addSection(terrainSection(1, 1, baseBlock::terrainTable[0])); // fixed errorinium

	}
}

terrainChunk terrainPainterGenerator::getChunk(const point3Di& p)
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
	if(_liquidLevel>0)
		fillLiquid(chunk, p, _liquidLevel);
	//populateTrees(chunk, p, _liquidLevel );
	chunk.setLoaded();
	chunk.clearDirtyFlag();
	return chunk;
}

int terrainPainterGenerator::getHeight(const point2D &p)
{
	int result = _terrainPainter.getHeight(getNoise(p));
	if(result < _liquidLevel)
		result=_liquidLevel;
	return result;
}

void terrainPainterGenerator::fillLiquid(terrainChunk& c, const point3Di p, int liquidLevel) const
{
	for (int x = 0; x < config::chunkSize; ++x) {
		for (int y = 0; y < config::chunkSize; ++y) {
			for (int z = 0; z < config::chunkSize; ++z) {
				int currentHeight = p.z * config::chunkSize + z;
				if (currentHeight < liquidLevel)
				{
					if (*c.getBlock({ x,y,z }).base == baseBlock::terrainTable[_emptyBlockID]) {
						c.setBlock({&baseBlock::terrainTable[_liquidID],(blockRotation)(rand()%4)}, { x,y,z });
					}
				}
			}
		}
	}
}

void terrainPainterGenerator::populateTrees(terrainChunk& c, const point3Di p, int waterLevel)const
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

void terrainPainterGenerator::placeTree(terrainChunk& c, const point3Di p)const
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



terrainSection::terrainSection(double noise, int sectionWidth, baseBlock& b, baseBlock* surfaceBlock) : _block(&b)
{
	_noiseCeiling = noise;
	_sectionWidth = sectionWidth;
	_surfaceBlock = surfaceBlock;
}

double terrainSection::getNoiseCeiling() const
{
	return _noiseCeiling;
}

double terrainSection::getSectionWidth() const
{
	return _sectionWidth;
}

baseBlock& terrainSection::getBlock() const
{
	return *_block;
}

baseBlock* terrainSection::getSurfaceBlock() const
{
	return _surfaceBlock;
}

baseBlock& terrainPainter::getBlock(int height, double noise) const
{
	auto it = _terrainList.begin();
	int accumulatedHeight = 0;
	double lastNoise = 0;
	while (true) // while we haven't found the baseBlock
	{
		if (accumulatedHeight + it->getSectionWidth() < height) // if this section is too low
		{
			accumulatedHeight += it->getSectionWidth();
			lastNoise = it->getNoiseCeiling();
			if (++it == _terrainList.end())//advance to next section, if end then exit
			{
				break;
			}
		}
		else
		{// we're in this section, calculate if we're below noise
			int floorHeight = (noise - lastNoise) / (it->getNoiseCeiling() - lastNoise) * it->getSectionWidth() + accumulatedHeight;
			if (floorHeight > height)
			{
				return it->getBlock();
			}
			if (floorHeight == height)
			{
				if (it->getSurfaceBlock() != nullptr)
				{
					return *it->getSurfaceBlock();
				}
				else
					return it->getBlock();
			}
			else
			{
				return *_emptyBlock;
			}
		}
	}
	return *_emptyBlock;
}

baseBlock& terrainPainter::getBlock(int height) const
{
	auto it = _terrainList.begin();
	int accumulatedHeight = 0;
	while (true) // while we haven't found the height
	{
		if (height <= accumulatedHeight + it->getSectionWidth()) // if we belong to this section
		{
			if (height == accumulatedHeight + it->getSectionWidth())
			{
				if (it->getSurfaceBlock() != nullptr)
				{
					return *it->getSurfaceBlock();
				}
			}
			return it->getBlock();
		}
		else //advance
		{
			accumulatedHeight += it->getSectionWidth();
			it++;
		}
	}
}

int terrainPainter::getHeight(const double& noise) const
{
	auto it = _terrainList.begin();
	auto oldSection = *it;
	int accumulatedHeight = 0;
	double lastNoise = 0;
	assert(noise <= 1);
	while (true) // while we haven't found the height
	{
		if (noise <= it->getNoiseCeiling()) // if we belong to this section
		{
			return (noise - lastNoise) / (it->getNoiseCeiling() - lastNoise) * it->getSectionWidth() + accumulatedHeight;
		}
		else //advance
		{
			accumulatedHeight += it->getSectionWidth();
			lastNoise = it->getNoiseCeiling();
			it++;
		}
	}
}

void terrainPainter::addSection(terrainSection s)
{
	_terrainList.push_back(s);
}

void terrainPainter::setEmptyBlock(baseBlock* emptyBlock)
{
	_emptyBlock = emptyBlock;
}

void to_json(nlohmann::json &j, const terrainPainter &tp)
{
	j = json{ {"emptyBlockID", tp._emptyBlock->ID},{"sections", tp._terrainList}};
}

void to_json(nlohmann::json &j, const terrainSection &ts)
{
	j = json{ {"noiseCeiling", ts._noiseCeiling},{"sectionWidth", ts._sectionWidth},{"baseBlockID",ts._block->ID}};
	if(ts._surfaceBlock!=nullptr){
		j.push_back({"surfaceBlock",ts._surfaceBlock->ID});
	}
}

void from_json(const nlohmann::json &j, terrainSection &ts)
{
	ts._noiseCeiling = j.at("noiseCeiling").get<double>();
	ts._sectionWidth = j.at("sectionWidth").get<int>();
	ts._block = &baseBlock::terrainTable[j.at("baseBlockID").get<unsigned>()];
	if (j.contains("surfaceBlock"))
	{
		ts._surfaceBlock = &baseBlock::terrainTable[j.at("surfaceBlock").get<unsigned>()];
	}
}
void from_json(const nlohmann::json &j, terrainPainter &tp)
{
	tp._emptyBlock = &baseBlock::terrainTable[j.at("emptyBlockID").get<unsigned>()];
	for (const nlohmann::json& element : j.at("sections")) {
		tp._terrainList.push_back(element.get<terrainSection>());
	}
}
void from_json(const nlohmann::json &j, terrainPainterGenerator &tpg)
{
	tpg._diameter = j.at("diameter").get<unsigned>();
	tpg._liquidLevel = j.at("liquidLevel").get<unsigned>();
	tpg._liquidID = j.at("liquidID").get<unsigned>();
	tpg._emptyBlockID = j.at("emptyBlockID").get<unsigned>();
	tpg._seed = j.at("seed").get<unsigned>();
	tpg._terrainPainter = j.at("terrainPainter").get<terrainPainter>();
}

baseBlock &terrainPainterGenerator::getTopBlock(const point2D &p)
{
	return _terrainPainter.getBlock(getHeight(p));
}

nlohmann::json terrainPainterGenerator::getJson() const
{
	return json{{"type","terrainPainter"},{"generator",{ {"diameter", _diameter},			{"liquidLevel", _liquidLevel},
			 {"liquidID", _liquidID}, {"emptyBlockID", _emptyBlockID},
			 {"seed", _seed},{"terrainPainter", _terrainPainter} }}};
}

