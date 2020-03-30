#include "audioManager.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include <iostream>
#include "config.hpp"

using namespace std;

audioManager::~audioManager() {
	freeAllAudio();
	std::cout
		<< "Closing an audioManager object and emptying the atlas, if unintended, "
		"check for unnecessary copies of the audioManager object"
		<< std::endl;
}

bool audioManager::isAudioLoaded(string audioFile)
const { // tells if an audio with said name is present on the Atlas
	return audioAtlas.find(audioFile) != audioAtlas.end();
}

HI2::Audio* audioManager::loadAudio(string audioName) { // load a audio from a file into the first free space inside texTable[]
	std::filesystem::path fileNameWithoutExt = HI2::getDataPath().append("sounds").append(audioName);
	std::filesystem::path completeFileName = fileNameWithoutExt.concat(config::audioExtension);
	if (audioAtlas.find(audioName) == audioAtlas.end()) {
		if (std::filesystem::exists(completeFileName)) {
			audioAtlas.insert(make_pair(audioName, HI2::Audio(completeFileName)));
		}
		else {
			std::cout << "Audio at " << completeFileName << " not found"
				<< std::endl;
			return nullptr;
		}
	}
	return &(audioAtlas.find(audioName)->second);
}

void audioManager::freeAudio(string audioName) { // frees a texture from texTable[]
	auto it = audioAtlas.find(audioName);
	if (it != audioAtlas.end()) {
		it->second.clean();
		audioAtlas.erase(it);
	}
}

HI2::Audio* audioManager::getAudio(string audioName) {
	auto it = audioAtlas.find(audioName);
	if (it == audioAtlas.end()) {
		std::cout << ("Audio " + audioName + " not loaded \n") << std::endl;
		return nullptr;
	}
	else
		return &it->second;
}

void audioManager::freeAllAudio() { // frees all audio
	for (auto it : audioAtlas) {
		it.second.clean();
	}
	audioAtlas.clear();
}
