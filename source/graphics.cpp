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
	std::filesystem::path fileNameWithoutExt = (HI2::getDataPath() /= "sprites") /= (spriteName);
	std::filesystem::path completeFileName = fileNameWithoutExt.string() + ".png";
	if (texAtlas.find(spriteName) == texAtlas.end()) {
		if (std::filesystem::exists(completeFileName)) {
			texAtlas.insert(make_pair(spriteName, HI2::Texture(completeFileName)));
		}
		else if(std::filesystem::exists(fileNameWithoutExt.string() + "_1.png"))
		{
			int frameCounter = 1;
			std::vector<std::filesystem::path> pathList;
			while(std::filesystem::exists(fileNameWithoutExt.string() +"_" + std::to_string(frameCounter) + ".png"))
			{
				pathList.push_back(fileNameWithoutExt.string() +"_" + std::to_string(frameCounter) + ".png");
				frameCounter++;
			}
			texAtlas.insert(make_pair(spriteName, HI2::Texture(pathList,0.2)));
		}
		else {
			std::cout << "Texture at \"" << completeFileName << "\" not found"
				<< std::endl;
			return nullptr;
		}
	}
	return &(texAtlas.find(spriteName)->second);
}

void graphics::freeTexture(string spriteName) { // frees a texture from texTable[]
	auto it = texAtlas.find(spriteName);
	if (it != texAtlas.end()) {
		it->second.clean();
		texAtlas.erase(it);
	}
}

HI2::Texture* graphics::getTexture(string spriteName) {
	auto it = texAtlas.find(spriteName);
	if (it == texAtlas.end()) {
		std::cout << ("Texture " + spriteName + " not loaded \n") << std::endl;
		return nullptr;
	}
	else
		return &it->second;
}

void graphics::stepAnimations(double s)
{
	for(auto& tex : texAtlas)
	{
		tex.second.step(s);
	}
}

void graphics::freeAllTextures() { // frees all textures
	for (auto it : texAtlas) {
		it.second.clean();
	}
}
