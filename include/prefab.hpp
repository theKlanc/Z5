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

	std::stringstream saveSS() const;
	void loadSS(std::stringstream& ss);

	const point3Di& getSize()const ;
	metaBlock& operator[](int);
	metaBlock& operator[](point3Di);

	prefab& add(prefab& pfb, point3Di pos);
	prefab& remove(point3Di pos, point3Di size);
	prefab get(point3Di pos, point3Di size);

	metaBlock& get(point3Di p);
	std::string getName() const;
private:
	std::string _name;
	std::filesystem::path _folder;
	point3Di _size;
	std::vector<metaBlock> _blocks;
};
