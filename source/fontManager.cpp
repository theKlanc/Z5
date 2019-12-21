#include "fontManager.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include <iostream>
#include "config.hpp"

using namespace std;

fontManager::~fontManager() {
	freeAllFonts();
	std::cout
		<< "Closing an fontManager object and emptying the atlas, if unintended, "
		"check for unnecessary copies of the fontManager object"
		<< std::endl;
}

bool fontManager::isFontLoaded(string fontFile)
const { // tells if an font with said name is present on the Atlas
	return fontAtlas.find(fontFile) != fontAtlas.end();
}

HI2::Font* fontManager::loadFont(string fontName) { // load a texture from a file into the first free space inside texTable[]
	std::filesystem::path fileNameWithoutExt = HI2::getDataPath().append("fonts").append(fontName);
	std::filesystem::path completeFileName = fileNameWithoutExt.concat(config::fontExtension);
	if (fontAtlas.find(fontName) == fontAtlas.end()) {
		if (std::filesystem::exists(completeFileName)) {
			fontAtlas.insert(make_pair(fontName, HI2::Font(completeFileName)));
		}
		else {
			std::cout << "Font at " << completeFileName << " not found"
				<< std::endl;
			return nullptr;
		}
	}
	return &(fontAtlas.find(fontName)->second);
}

void fontManager::freeFont(string fontName) { // frees a texture from texTable[]
	auto it = fontAtlas.find(fontName);
	if (it != fontAtlas.end()) {
		it->second.clean();
		fontAtlas.erase(it);
	}
}

HI2::Font* fontManager::getFont(string fontName) {
	auto it = fontAtlas.find(fontName);
	if (it == fontAtlas.end()) {
		std::cout << ("Font " + fontName + " not loaded \n") << std::endl;
		return nullptr;
	}
	else
		return &it->second;
}

void fontManager::freeAllFonts() { // frees all fonts
	for (auto it : fontAtlas) {
		it.second.clean();
	}
}
