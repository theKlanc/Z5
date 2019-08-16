#include "graphics.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include <iostream>

using namespace std;

graphics::graphics() {}

graphics::~graphics() {
	freeAllTextures();
	std::cout
		<< "Closing a graphics object and emptying the atlas, if unintended, "
		"check for unnecessary copies of the graphics object"
		<< std::endl;
}

bool graphics::isTextureLoaded(string textureFile)
const { // tells if a texture with said name is present on texTable
	return texAtlas.find(textureFile) != texAtlas.end();
}

HI2::Texture* graphics::loadTexture(string spriteName) { // load a texture from a file into the first free space inside texTable[]
	std::filesystem::path completeFileName = (HI2::getDataPath() /= "sprites") /= (spriteName) += ".webp";
	if (texAtlas.find(spriteName) == texAtlas.end()) {
		if (std::filesystem::exists(completeFileName)) {
			texAtlas.insert(make_pair(spriteName, HI2::Texture(completeFileName)));
		}
		else {
			std::cout << "Texture at \"" << completeFileName << "\" not found"
				<< std::endl;
			return nullptr;
		}
	}
	return &(texAtlas.find(spriteName)->second);
}

void graphics::freeTexture(string fileName) { // frees a texture from texTable[]
	auto it = texAtlas.find(fileName);
	if (it != texAtlas.end()) {
		it->second.clean();
		texAtlas.erase(it);
	}
}

HI2::Texture* graphics::getTexture(string fileName) {
	auto it = texAtlas.find(fileName);
	if (it == texAtlas.end()) {
		std::cout << ("Texture " + fileName + " not loaded \n") << std::endl;
		return nullptr;
	}
	else
		return &it->second;
}

void graphics::freeAllTextures() { // frees all textures
	for (auto it : texAtlas) {
		it.second.clean();
	}
}