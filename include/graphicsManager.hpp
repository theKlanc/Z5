#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include <unordered_map>
#include "json.hpp"

using nlohmann::json;

struct frame{
	point2D startPos;
	point2D size;
};

class sprite{
public:
	sprite(){}
	sprite(HI2::Texture* tex, std::vector<frame> frames, std::string textureName):_texture(tex),_frames(frames),_currentFrame(&_frames[_currentIndex]),_textureName(textureName){}

	std::string_view getTextureName();
	HI2::Texture* getTexture();
	frame& getCurrentFrame();
	void step(double s);
	std::vector<frame>& getAllFrames();

private:
	HI2::Texture* _texture;
	std::string _textureName;
	unsigned _currentIndex = 0;
	double _timeAccumulator = 0;
	double _timeStep = 0.2;
	std::vector<frame> _frames;
	frame* _currentFrame;
};

class graphicsManager {
  public:
	graphicsManager() = default;
	~graphicsManager();
	sprite* loadSprite(std::string name);
	sprite* loadSprite(std::string name, std::string textureName);
	sprite* loadSprite(std::string name, std::string textureName, std::vector<frame> frames);
	bool isSpriteLoaded(std::string spriteFile) const;
	void freeSprite(std::string spriteName);
	sprite* getSprite(std::string spriteName);
	void freeAllSprites();

	void stepAnimations(double s);

  private:
	bool isTextureLoaded(std::string textureFile) const;
	HI2::Texture* loadTexture(std::string fileName);
	void freeTexture(std::string textureName);
	HI2::Texture* getTexture(std::string textureName);
	void freeAllTextures();
	std::unordered_map<std::string, HI2::Texture> _texAtlas;

	std::unordered_map<std::string, sprite> _spriteAtlas;
};

void to_json(json& j, const frame& b);
void from_json(const json& j, frame& b);