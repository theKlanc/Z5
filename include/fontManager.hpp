#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include <unordered_map>
class fontManager {
  public:
	fontManager() = default;
	~fontManager();
	bool isFontLoaded(std::string fontFile) const;
	HI2::Font* loadFont(std::string fileName);
	void freeFont(std::string fontName);
	HI2::Font* getFont(std::string fontName);
	void freeAllFonts();

  private:
	std::unordered_map<std::string, HI2::Font> fontAtlas;
};