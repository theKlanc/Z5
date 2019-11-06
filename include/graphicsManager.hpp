#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include <unordered_map>
class graphicsManager {
  public:
	graphicsManager() = default;
	~graphicsManager();
	bool isTextureLoaded(std::string textureFile) const;
	HI2::Texture* loadTexture(std::string fileName);
	void freeTexture(std::string spriteName);
	HI2::Texture* getTexture(std::string spriteName);
	void stepAnimations(double ms);

  private:
	void freeAllTextures();
	std::unordered_map<std::string, HI2::Texture> texAtlas;
};