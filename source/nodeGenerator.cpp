#include "nodeGenerator.hpp"
#include "universeNode.hpp"
#include <time.h>

nodeGenerator::nodeGenerator()
{

}

nodeGenerator::nodeGenerator(int maxHeight)
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetSeed(time(nullptr));
	_maxHeight=maxHeight;
}

terrainChunk nodeGenerator::getChunk(const point3Di &p)
{
	terrainChunk chunk(p);
	for(int x =0;x<config::chunkSize;++x){
		for(int y =0;y<config::chunkSize;++y){
			int height = _noiseGenerator.GetNoise(p.x*config::chunkSize+x,p.y*config::chunkSize+y);
			for(int z =0;x<config::chunkSize;++z){
				if(p.z*config::chunkSize+z > height*_maxHeight){
					chunk.setBlock(&_terrainTable->at(1),point3Di{x,y,z});
				}
				else if(p.z*config::chunkSize+z == height*_maxHeight){
					chunk.setBlock(&_terrainTable->at(4),point3Di{x,y,z});
				}
				else{
					chunk.setBlock(&_terrainTable->at(3),point3Di{x,y,z});
				}
			}
		}
	}
	return chunk;
}
