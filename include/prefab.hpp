#pragma once
#include <string>
#include "HI2.hpp"
#include "block.hpp"

class prefab
{
public:
	prefab();
	prefab(std::string name);
	prefab(std::string name, point3Di size);

	void save() const;
	void load();

	const point3Di& getSize()const ;
	metaBlock& operator[](int);
	metaBlock& get(point3Di p);
	std::string getName() const;
private:
	std::string _name;
	std::filesystem::path _folder;
	point3Di _size;
	std::vector<metaBlock> _blocks;
};
