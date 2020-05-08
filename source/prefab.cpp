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
	_blocks.resize(size.x * size.y * size.z, metaBlock{ &baseBlock::terrainTable[0],blockRotation::UP,false,true });
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

const point3Di& prefab::getSize() const
{
	return _size;
}

metaBlock& prefab::operator[](int i)
{
	return _blocks[i];
}

metaBlock& prefab::get(point3Di p)
{
	return _blocks[p.z * _size.y * _size.x + p.y * _size.x + p.x];
}

std::string prefab::getName() const
{
	return _name;
}
