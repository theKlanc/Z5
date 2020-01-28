#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include <unordered_map>
class audioManager {
  public:
	audioManager() = default;
	~audioManager();
	bool isAudioLoaded(std::string audioFile) const;
	HI2::Audio* loadAudio(std::string fileName);
	void freeAudio(std::string audioName);
	HI2::Audio* getAudio(std::string audioName);

	void freeAllAudio();
  private:
	std::unordered_map<std::string, HI2::Audio> audioAtlas;
};