#include "prefab.hpp"
#include "icecream.hpp"

prefab::prefab()
{
}

prefab::prefab(std::string name)
{
	_name = name;
	_folder = HI2::getDataPath().append("prefabs").append(_name);

	load();
}

prefab::prefab(std::string name, point3Di size)
{
	_name = name;
	_folder = HI2::getDataPath().append("prefabs").append(_name);
	_size = size;
	_blocks.resize(size.x * size.y * size.z, metaBlock::nullBlock);
}

void prefab::save() const
{
	std::filesystem::create_directories(_folder);
	std::filesystem::path outputFileName = _folder;
	outputFileName.append(_name + ".z5p");
	std::ofstream output(outputFileName);
	output << _size.x << ' ' << _size.y << ' ' << _size.z << std::endl;
	output << _blocks;
}

void prefab::load()
{
	std::filesystem::path inputFilename = _folder;
	inputFilename.append(_name + ".z5p");
	if (std::filesystem::exists(inputFilename))
	{
		std::ifstream input(inputFilename);
		if (input.is_open())
		{
			input >> _size.x >> _size.y >> _size.z;
			input >> _blocks;
		}
	}
	else{
		IC(_name+".z5p not found");
	}
}


std::stringstream prefab::saveSS() const
{
	std::stringstream output;
	output << _size.x << ' ' << _size.y << ' ' << _size.z << std::endl;
	output << _blocks;
	return output;
}

void prefab::loadSS(std::stringstream& ss)
{
	ss >> _size.x >> _size.y >> _size.z;
	ss >> _blocks;
}

const point3Di& prefab::getSize() const
{
	return _size;
}

metaBlock& prefab::operator[](int i)
{
	return _blocks[i];
}

metaBlock &prefab::operator[](point3Di p)
{
	return _blocks[p.z * _size.y * _size.x + p.y * _size.x + p.x];
}

prefab &prefab::add(prefab &pfb, point3Di pos)
{
	for(int x = 0; x < pfb._size.x; ++x){
		for(int y = 0; y < pfb._size.y; ++y){
			for(int z = 0; z < pfb._size.z; ++z){
				point3Di src{x,y,z};
				point3Di targetPos = pos + src;
				if(pfb[src] != metaBlock::nullBlock && targetPos.x >= 0 &&targetPos.y >= 0 &&targetPos.z >= 0 &&targetPos.x < _size.x &&targetPos.y < _size.y &&targetPos.z < _size.z){
					(*this)[targetPos] = pfb[src];
				}
			}
		}
	}
	return *this;
}

prefab &prefab::remove(point3Di pos,point3Di size)
{
	for(int x = 0; x < size.x; ++x){
		for(int y = 0; y < size.y; ++y){
			for(int z = 0; z < size.z; ++z){
				point3Di p{x,y,z};
				point3Di targetPos = pos + p;
				if(targetPos.x >= 0 &&targetPos.y >= 0 &&targetPos.z >= 0 &&targetPos.x < _size.x &&targetPos.y < _size.y &&targetPos.z < _size.z){
					(*this)[targetPos] = metaBlock::nullBlock;
				}
			}
		}
	}
	return *this;
}

prefab prefab::get(point3Di pos, point3Di size)
{
	prefab result("tmp",size);
	for(int x = 0; x < size.x; ++x){
		for(int y = 0; y < size.y; ++y){
			for(int z = 0; z < size.z; ++z){
				point3Di p{x,y,z};
				point3Di targetPos = pos + p;
				if(targetPos.x >= 0 &&targetPos.y >= 0 &&targetPos.z >= 0 &&targetPos.x < _size.x &&targetPos.y < _size.y &&targetPos.z < _size.z){
					result[p] = (*this)[targetPos];
				}
			}
		}
	}
	return result;
}

metaBlock& prefab::get(point3Di p)
{
	if(p.x < _size.x && p.y < _size.y && p.z < _size.z)
		return _blocks[p.z * _size.y * _size.x + p.y * _size.x + p.x];
	return metaBlock::nullBlock;
}

std::string prefab::getName() const
{
	return _name;
}
