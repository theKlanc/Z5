#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include <unordered_map>
class graphics {
  public:
	graphics();
	~graphics();
	bool isTextureLoaded(std::string textureFile) const;
	HI2::Texture* loadTexture(std::string fileName);
	void freeTexture(std::string fileName);
	void freeTexture(HI2::Texture& texture);
	HI2::Texture* getTexture(std::string fileName);

  private:
	void freeAllTextures();
	std::unordered_map<std::string, HI2::Texture> texAtlas;
};