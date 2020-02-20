#include "states/state_prefab_editor.hpp"
#include "config.hpp"
#include "gameCore.hpp"
#include "services.hpp"

State::PrefabEditor::PrefabEditor(gameCore& c, std::string name)
{
	_core = &c;
	//load terrain table
	baseBlock::loadTerrainTable();
	_prefab = prefab(name);
	load();

	initToolbar();
}



State::PrefabEditor::PrefabEditor(gameCore& c, std::string name, point3Di size)
{
	_core = &c;
	//load terrain table
	baseBlock::loadTerrainTable();
	_prefab = prefab(name,size);
	
	initToolbar();
}

State::PrefabEditor::~PrefabEditor()
{
}

void State::PrefabEditor::input(double dt)
{
	auto& keysDown = HI2::getKeysDown();
	auto& keysHeld = HI2::getKeysHeld();
	if (keysDown[HI2::BUTTON::KEY_ESCAPE])
	{
		save();
		_core->popState();
	}
	if (keysDown[HI2::BUTTON::KEY_DASH])
	{
		zoom /= 2;
		if (zoom < 1)
			zoom = 1;
	}
	if (keysDown[HI2::BUTTON::KEY_PLUS])
	{
		zoom *= 2;
	}
	if (keysDown[HI2::BUTTON::KEY_MOUSEWHEEL_DOWN] || keysDown[HI2::BUTTON::BUTTON_DLEFT])
	{
		_selectedToolbarPos--;
		if (_selectedToolbarPos < 0)
			_selectedToolbarPos = _toolbar.size() - 1;
	}
	if (keysDown[HI2::BUTTON::KEY_MOUSEWHEEL_UP] || keysDown[HI2::BUTTON::BUTTON_DRIGHT])
	{
		_selectedToolbarPos++;
		if (_selectedToolbarPos >= _toolbar.size())
			_selectedToolbarPos = 0;
	}
	if (keysDown[HI2::BUTTON::BUTTON_DUP])
	{
		if (_toolbar[_selectedToolbarPos]->ID != baseBlock::terrainTable.size() - 1)
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[_toolbar[_selectedToolbarPos]->ID + 1];
		else
		{
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[0];
		}
	}
	if (keysDown[HI2::BUTTON::BUTTON_DDOWN])
	{
		if (_toolbar[_selectedToolbarPos]->ID != 0)
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[_toolbar[_selectedToolbarPos]->ID - 1];
		else
		{
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[baseBlock::terrainTable.size() - 1];
		}
	}
	if (keysDown[HI2::BUTTON::KEY_W])
	{
		_camera.y--;
	}
	if (keysDown[HI2::BUTTON::KEY_A])
	{
		_camera.x--;
	}
	if (keysDown[HI2::BUTTON::KEY_S])
	{
		_camera.y++;
	}
	if (keysDown[HI2::BUTTON::KEY_D])
	{
		_camera.x++;
	}
	if (keysDown[HI2::BUTTON::KEY_R])
	{
		_camera.z++;
	}
	if (keysDown[HI2::BUTTON::KEY_F])
	{
		_camera.z--;
	}
	if (keysDown[HI2::BUTTON::KEY_H]) {
		//print help
	}
	if (keysDown[HI2::BUTTON::KEY_I]) {
		_drawInvisible != _drawInvisible;
	}
	if (keysHeld[HI2::BUTTON::KEY_LEFTCLICK])
	{
		auto mouse = HI2::getTouchPos();
		point3Di blockPos = _camera;
		blockPos.x += (int)(mouse.x / (config::spriteSize * zoom));
		blockPos.y += (int)(mouse.y / (config::spriteSize * zoom));
		if (blockPos.x >= 0 && blockPos.y >= 0 && blockPos.z >= 0 && blockPos.x < _prefab.getSize().x && blockPos.y < _prefab.getSize().y && blockPos.z < _prefab.getSize().z)
			_prefab[blockPos.z * _prefab.getSize().y * _prefab.getSize().x + blockPos.y * _prefab.getSize().x + blockPos.x].base = _toolbar[_selectedToolbarPos];
	}
	if (keysHeld[HI2::BUTTON::KEY_RIGHTCLICK])
	{
		auto mouse = HI2::getTouchPos();
		point3Di blockPos = _camera;
		blockPos.x += (int)(mouse.x / (config::spriteSize * zoom));
		blockPos.y += (int)(mouse.y / (config::spriteSize * zoom));
		if (blockPos.x >= 0 && blockPos.y >= 0 && blockPos.z >= 0 && blockPos.x < _prefab.getSize().x && blockPos.y < _prefab.getSize().y && blockPos.z < _prefab.getSize().z)
			_prefab[blockPos.z * _prefab.getSize().y * _prefab.getSize().x + blockPos.y * _prefab.getSize().x + blockPos.x].base = &baseBlock::terrainTable[0];
	}
}

void State::PrefabEditor::update(double dt)
{
}

void State::PrefabEditor::draw(double dt)
{
	HI2::startFrame();
	for (int depth = _cameraDepth - 1; depth >= 0; --depth) {
		for (int j = 0; j < HI2::getScreenWidth() / (zoom * config::spriteSize); ++j)
		{
			for (int i = 0; i < HI2::getScreenHeight() / (zoom * config::spriteSize); ++i)
			{
				point3Di pos = _camera;
				pos.x += j;
				pos.y += i;
				pos.z -= depth;
				if (pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < _prefab.getSize().x && pos.y < _prefab.getSize().y && pos.z < _prefab.getSize().z)
				{
					metaBlock& mb = _prefab[pos.z * _prefab.getSize().y * _prefab.getSize().x + pos.y * _prefab.getSize().x + pos.x];
					if (mb.base->visible)
					{
						HI2::setTextureColorMod(*mb.base->spr->getTexture(), { unsigned char(255 - depth * 50),unsigned char(255 - depth * 50),unsigned char(255 - depth * 50),255 });
						HI2::drawTexture(*mb.base->spr->getTexture(), j * (zoom * config::spriteSize), i * (zoom * config::spriteSize), mb.base->spr->getCurrentFrame().size, mb.base->spr->getCurrentFrame().startPos, zoom, ((double)(int)mb.rotation) * (M_PI / 2));
					}
					else
					{
						//TODO
						//DRAW INVISIBLE BLOCKS
						if (_drawInvisible) {
							HI2::drawRectangle({ j * (zoom * config::spriteSize), i * (zoom * config::spriteSize) }, zoom * config::spriteSize, zoom * config::spriteSize, HI2::Color{ (unsigned char)(mb.base->ID == 0 ? 255 : 0),(unsigned char)(mb.base->ID == 1 ? 255 : 0),(unsigned char)(mb.base->ID == 2 ? 255 : 0),127 });
						}
					}
				}
				else
				{
					if (depth == 0)
						HI2::drawRectangle({ j * (zoom * config::spriteSize), i * (zoom * config::spriteSize) }, zoom * config::spriteSize, zoom * config::spriteSize, HI2::Color{ 255,0,0,127 });
				}
			}
		}
	}

	auto mousePos = HI2::getTouchPos();
	//HI2::drawRectangle({((int)(mousePos.x/(config::spriteSize*zoom)))*config::spriteSize*zoom,((int)(mousePos.y/(config::spriteSize*zoom)))*config::spriteSize*zoom},zoom * config::spriteSize,zoom * config::spriteSize,{255,0,0,127});
	HI2::drawTexture(*_toolbar[_selectedToolbarPos]->spr->getTexture(), ((int)(mousePos.x / (config::spriteSize * zoom))) * config::spriteSize * zoom, ((int)(mousePos.y / (config::spriteSize * zoom))) * config::spriteSize * zoom, _toolbar[_selectedToolbarPos]->spr->getCurrentFrame().size, _toolbar[_selectedToolbarPos]->spr->getCurrentFrame().startPos, zoom, ((double)(int)_rotation) * (M_PI / 2));



	HI2::drawRectangle({ (HI2::getScreenWidth() - (4 + (int)_toolbar.size() * config::spriteSize * 4 + (int)_toolbar.size() * 8)) / 2,HI2::getScreenHeight() - config::spriteSize * 4 - 4 }, 1 + _toolbar.size() * config::spriteSize * 4 + _toolbar.size() * 8, config::spriteSize * 4 + 4, HI2::Color::LightGrey);
	HI2::drawRectangle({ (HI2::getScreenWidth() - (4 + (int)_toolbar.size() * config::spriteSize * 4 + (int)_toolbar.size() * 8)) / 2 + +(int)_selectedToolbarPos * config::spriteSize * 4 + (int)_selectedToolbarPos * 8,HI2::getScreenHeight() - config::spriteSize * 4 - 4 }, config::spriteSize * 4 + 8, config::spriteSize * 4 + 4, HI2::Color::DarkestGrey);

	for (int i = 0; i < _toolbar.size(); ++i)
	{
		if (_toolbar[i]->visible)
		{
			HI2::setTextureColorMod(*_toolbar[i]->spr->getTexture(), HI2::Color::White);
			HI2::drawTexture(*_toolbar[i]->spr->getTexture(), (HI2::getScreenWidth() - (4 + _toolbar.size() * config::spriteSize * 4 + _toolbar.size() * 8)) / 2 + 4 + i * config::spriteSize * 4 + i * 8, HI2::getScreenHeight() - (config::spriteSize * 4), _toolbar[i]->spr->getCurrentFrame().size, _toolbar[i]->spr->getCurrentFrame().startPos, 4, ((double)(int)_rotation) * (M_PI / 2));

		}
	}

	HI2::drawText(*Services::fonts.loadFont("test"), "name: " + _prefab.getName(), { 0,0 }, 20, HI2::Color::White);
	HI2::drawText(*Services::fonts.loadFont("test"), "size: x:" + std::to_string(_prefab.getSize().x), { 0,20 }, 20, HI2::Color::White);
	HI2::drawText(*Services::fonts.loadFont("test"), "      y:" + std::to_string(_prefab.getSize().y), { 0,40 }, 20, HI2::Color::White);
	HI2::drawText(*Services::fonts.loadFont("test"), "      z:" + std::to_string(_prefab.getSize().z), { 0,60 }, 20, HI2::Color::White);


	HI2::endFrame();
}

void State::PrefabEditor::save()
{
	_prefab.save();
}

void State::PrefabEditor::load()
{
	_prefab.load();
}

void State::PrefabEditor::initToolbar()
{
	unsigned b = 0;
	for (int i = 0; i < _toolbar.size(); ++i)
	{
		while (!baseBlock::terrainTable[b].visible)
		{
			++b;
		}
		_toolbar[i] = &baseBlock::terrainTable[b++];
	}
}