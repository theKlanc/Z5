#include "graphicsManager.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include <iostream>

graphicsManager::~graphicsManager() {
	freeAllTextures();
	std::cout
		<< "Closing a graphicsManager object and emptying the atlas, if unintended, "
		"check for unnecessary copies of the graphicsManager object"
		<< std::endl;
}
sprite *graphicsManager::loadSprite(std::string name)
{
	if(_spriteAtlas.find(name) != _spriteAtlas.end()){
		return &_spriteAtlas.find(name)->second;
	}
	else{
		HI2::Texture* tex = loadTexture(name);
		frame f;
		f.size=HI2::getTextureSize(*tex);
		f.startPos={0,0};
		std::vector<frame> frameVector;
		frameVector.push_back(f);
		return &(_spriteAtlas[name] = sprite(tex,frameVector));
	}
}
sprite *graphicsManager::loadSprite(std::string name, std::string textureName)
{
	if(_spriteAtlas.find(name) != _spriteAtlas.end()){
		return &_spriteAtlas.find(name)->second;
	}
	else{
		HI2::Texture* tex = loadTexture(textureName);
		frame f;
		f.size=HI2::getTextureSize(*tex);
		f.startPos={0,0};
		std::vector<frame> frameVector;
		frameVector.push_back(f);
		return &(_spriteAtlas[name] = sprite(tex,frameVector));
	}
}
sprite *graphicsManager::loadSprite(std::string name, std::string textureName, std::vector<frame> frames)
{
	if(_spriteAtlas.find(name) != _spriteAtlas.end()){
		return &_spriteAtlas.find(name)->second;
	}
	else{
		return &(_spriteAtlas[name] = sprite(loadTexture(textureName),frames));
	}
}

bool graphicsManager::isSpriteLoaded(std::string spriteFile) const
{
	return _spriteAtlas.find(spriteFile)!=_spriteAtlas.end();
}

void graphicsManager::freeSprite(std::string spriteName)
{
	_spriteAtlas.erase(spriteName);
}

sprite *graphicsManager::getSprite(std::string spriteName)
{
	auto it = _spriteAtlas.find(spriteName);
	if(it==_spriteAtlas.end())
		return nullptr;
	return &it->second;
}

void graphicsManager::freeAllSprites()
{
	freeAllTextures();
}

bool graphicsManager::isTextureLoaded(std::string textureFile)
const { // tells if a texture with said name is present on texTable
	return _texAtlas.find(textureFile) != _texAtlas.end();
}

HI2::Texture* graphicsManager::loadTexture(std::string spriteName) { // load a texture from a file into the first free space inside texTable[]
	std::filesystem::path fileNameWithoutExt = (HI2::getDataPath() /= "sprites") /= (spriteName);
	std::filesystem::path completeFileName = fileNameWithoutExt.string() + ".png";
	if (_texAtlas.find(spriteName) == _texAtlas.end()) {
		if (std::filesystem::exists(completeFileName)) {
			_texAtlas.insert(make_pair(spriteName, HI2::Texture(completeFileName)));
		}
		else {
			std::cout << "Texture at " << completeFileName << " not found"
				<< std::endl;
			return nullptr;
		}
	}
	return &(_texAtlas.find(spriteName)->second);
}

void graphicsManager::freeTexture(std::string spriteName) { // frees a texture from texTable[]
	auto it = _texAtlas.find(spriteName);
	if (it != _texAtlas.end()) {
		it->second.clean();
		_texAtlas.erase(it);
	}
}

HI2::Texture* graphicsManager::getTexture(std::string spriteName) {
	auto it = _texAtlas.find(spriteName);
	if (it == _texAtlas.end()) {
		std::cout << ("Texture " + spriteName + " not loaded \n") << std::endl;
		return nullptr;
	}
	else
		return &it->second;
}

void graphicsManager::stepAnimations(double s)
{
	for(auto& sprite : _spriteAtlas)
	{
		sprite.second.step(s);
	}
}

void graphicsManager::freeAllTextures() { // frees all textures
	for (auto it : _texAtlas) {
		it.second.clean();
	}
}

HI2::Texture *sprite::getTexture()
{
	return _texture;
}

frame &sprite::getCurrentFrame()
{
	return *_currentFrame;
}

void sprite::step(double s)
{
	_timeAccumulator+=s;
	while(_timeAccumulator>_timeStep){
		_currentIndex++;
		_timeAccumulator-=_timeStep;
	}
	_currentIndex%=_frames.size();
	_currentFrame=&_frames[_currentIndex];
}
