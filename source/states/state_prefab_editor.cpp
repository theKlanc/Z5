#include "states/state_prefab_editor.hpp"
#include "config.hpp"
#include "gameCore.hpp"
#include "services.hpp"
#include <iostream>
#include "icecream-cpp/icecream.hpp"

State::PrefabEditor::PrefabEditor(gameCore& c, std::string name)
{
	_core = &c;
	//load terrain table
	baseBlock::loadTerrainTable();
	_prefab = std::make_unique<prefab>(name);
	load();

	initToolbar();

	_checkpoints.push_back(std::make_unique<prefab>(*_prefab));
	_currentCheckpoint = _checkpoints.begin();
}



State::PrefabEditor::PrefabEditor(gameCore& c, std::string name, point3Di size)
{
	_core = &c;
	//load terrain table
	baseBlock::loadTerrainTable();
	_prefab = std::make_unique<prefab>(name, size);

	initToolbar();
}

State::PrefabEditor::~PrefabEditor()
{
}

void State::PrefabEditor::input(double dt)
{
	checkpoint();
	auto& keysDown = HI2::getKeysDown();
	auto& keysHeld = HI2::getKeysHeld();
	if (keysHeld[HI2::BUTTON::KEY_CONTROL] && keysDown[HI2::BUTTON::KEY_S])
	{
		save();
	}
	if (keysHeld[HI2::BUTTON::KEY_CONTROL] && keysDown[HI2::BUTTON::KEY_Z])
	{
		undo();
	}
	if (keysHeld[HI2::BUTTON::KEY_CONTROL] && keysDown[HI2::BUTTON::KEY_Y])
	{
		redo();
	}
	if(keysDown[HI2::BUTTON::KEY_X] && keysHeld[HI2::BUTTON::KEY_CONTROL]){
		if(selectionL && selectionR)
		{
			point3Di selStart;
			selStart.x = std::min(selectionL->x,selectionR->x);
			selStart.y = std::min(selectionL->y,selectionR->y);
			selStart.z = std::min(selectionL->z,selectionR->z);

			point3Di selEnd;
			selEnd.x = std::max(selectionL->x,selectionR->x);
			selEnd.y = std::max(selectionL->y,selectionR->y);
			selEnd.z = std::max(selectionL->z,selectionR->z);

			point3Di size = selEnd - selStart + point3Di{1,1,1};

			prefab selPfb = _prefab->get(selStart,size);
			_prefab->remove(selStart,size);
			HI2::setClipboard(selPfb.saveSS().str());
			checkpoint();
		}
	}
	if(keysDown[HI2::BUTTON::KEY_C] && keysHeld[HI2::BUTTON::KEY_CONTROL]){
		if(selectionL && selectionR)
		{
			point3Di selStart;
			selStart.x = std::min(selectionL->x,selectionR->x);
			selStart.y = std::min(selectionL->y,selectionR->y);
			selStart.z = std::min(selectionL->z,selectionR->z);

			point3Di selEnd;
			selEnd.x = std::max(selectionL->x,selectionR->x);
			selEnd.y = std::max(selectionL->y,selectionR->y);
			selEnd.z = std::max(selectionL->z,selectionR->z);

			point3Di size = selEnd - selStart + point3Di{1,1,1};

			prefab selPfb = _prefab->get(selStart,size);
			HI2::setClipboard(selPfb.saveSS().str());
		}
	}
	if(keysDown[HI2::BUTTON::KEY_V] && keysHeld[HI2::BUTTON::KEY_CONTROL]){
		prefab selPfb;
		std::stringstream ss(HI2::getClipboard());
		selPfb.loadSS(ss);

		auto mouse = HI2::getTouchPos();
		point3Di blockPos = _camera;
		blockPos.x += (int)(mouse.x / (config::spriteSize * zoom));
		blockPos.y += (int)(mouse.y / (config::spriteSize * zoom));

		_prefab->add(selPfb,blockPos);
		checkpoint();
	}
	if (keysDown[HI2::BUTTON::CANCEL])
	{
		if(!keysHeld[HI2::BUTTON::KEY_SHIFT])
			save();
		_core->popState();
	}
	if (keysDown[HI2::BUTTON::KEY_CONSOLE])
	{
		reloadTerrainTable();
	}

	if (keysDown[HI2::BUTTON::KEY_DASH])
	{
		_camera = _camera + point3Di{(int)(HI2::getScreenWidth()/(config::spriteSize*zoom))/2,(int)(HI2::getScreenHeight()/(config::spriteSize*zoom))/2,0} - point3Di{(int)(HI2::getScreenWidth()/(config::spriteSize*(zoom/2)))/2,(int)(HI2::getScreenHeight()/(config::spriteSize*(zoom/2)))/2,0};
		zoom /= 2;
	}
	if (keysDown[HI2::BUTTON::KEY_PLUS])
	{
		_camera = _camera + point3Di{(int)(HI2::getScreenWidth()/(config::spriteSize*zoom))/2,(int)(HI2::getScreenHeight()/(config::spriteSize*zoom))/2,0} - point3Di{(int)(HI2::getScreenWidth()/(config::spriteSize*(zoom*2)))/2,(int)(HI2::getScreenHeight()/(config::spriteSize*(zoom*2)))/2,0};
		zoom *= 2;
	}
	if (keysDown[HI2::BUTTON::KEY_0])
	{
		_selectedToolbarPos=9;
	}
	if (keysDown[HI2::BUTTON::KEY_1])
	{
		_selectedToolbarPos=0;
	}
	if (keysDown[HI2::BUTTON::KEY_2])
	{
		_selectedToolbarPos=1;
	}
	if (keysDown[HI2::BUTTON::KEY_3])
	{
		_selectedToolbarPos=2;
	}
	if (keysDown[HI2::BUTTON::KEY_4])
	{
		_selectedToolbarPos=3;
	}
	if (keysDown[HI2::BUTTON::KEY_5])
	{
		_selectedToolbarPos=4;
	}
	if (keysDown[HI2::BUTTON::KEY_6])
	{
		_selectedToolbarPos=5;
	}
	if (keysDown[HI2::BUTTON::KEY_7])
	{
		_selectedToolbarPos=6;
	}
	if (keysDown[HI2::BUTTON::KEY_8])
	{
		_selectedToolbarPos=7;
	}
	if (keysDown[HI2::BUTTON::KEY_9])
	{
		_selectedToolbarPos=8;
	}

	if (keysDown[HI2::BUTTON::KEY_MOUSEWHEEL_UP] || keysDown[HI2::BUTTON::KEY_RIGHT])
	{
		_selectedToolbarPos++;
		if (_selectedToolbarPos >= _toolbar.size())
			_selectedToolbarPos = 0;
	}
	if (keysDown[HI2::BUTTON::KEY_MOUSEWHEEL_DOWN] || keysDown[HI2::BUTTON::KEY_LEFT])
	{
		_selectedToolbarPos--;
		if (_selectedToolbarPos < 0)
			_selectedToolbarPos = _toolbar.size() - 1;
	}
	if (keysDown[HI2::BUTTON::KEY_UP])
	{
		if (_toolbar[_selectedToolbarPos]->ID != baseBlock::terrainTable.size() - 1)
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[_toolbar[_selectedToolbarPos]->ID + 1];
		else
		{
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[0];
		}
		if(keysHeld[HI2::BUTTON::KEY_CONTROL]){
			for(int i = _selectedToolbarPos+1;i<_toolbar.size();++i){
				if (_toolbar[i]->ID != baseBlock::terrainTable.size() - 1)
					_toolbar[i] = &baseBlock::terrainTable[_toolbar[i]->ID + 1];
				else
				{
					_toolbar[i] = &baseBlock::terrainTable[0];
				}
			}
		}
	}
	if (keysDown[HI2::BUTTON::KEY_DOWN])
	{
		if (_toolbar[_selectedToolbarPos]->ID != 0)
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[_toolbar[_selectedToolbarPos]->ID - 1];
		else
		{
			_toolbar[_selectedToolbarPos] = &baseBlock::terrainTable[baseBlock::terrainTable.size() - 1];
		}
		if(keysHeld[HI2::BUTTON::KEY_CONTROL]){
			for(int i = _selectedToolbarPos+1;i<_toolbar.size();++i){
				if (_toolbar[i]->ID != 0)
					_toolbar[i] = &baseBlock::terrainTable[_toolbar[i]->ID - 1];
				else
				{
					_toolbar[i] = &baseBlock::terrainTable[baseBlock::terrainTable.size() - 1];
				}
			}
		}
	}
	if (keysDown[HI2::BUTTON::KEY_O])
	{
		_drawStats = !_drawStats;
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
	if (keysDown[HI2::BUTTON::KEY_Q])
	{
		_rotation++;
	}
	if (keysDown[HI2::BUTTON::KEY_E])
	{
		_flip = !_flip;
	}
	if (keysDown[HI2::BUTTON::KEY_T])
	{
		_bgType = (_bgType + 1) % 3;
	}
	if (keysDown[HI2::BUTTON::KEY_X])
	{
		_symmetryMode = (symmetry)(((int)_symmetryMode + 1) % 4);
		std::cout << "symmetry mode: " << (_symmetryMode == symmetry::NONE ? "none" : _symmetryMode == symmetry::V ? "vertical" : _symmetryMode == symmetry::H ? "horizontal" : "both") << std::endl;
	}
	if (keysHeld[HI2::BUTTON::KEY_H]) {
		HI2::startFrame();

		HI2::drawRectangle({}, HI2::getScreenWidth(), HI2::getScreenHeight(), HI2::Color::DarkGrey);
		int comptador = 0;
		HI2::drawText(*Services::fonts.loadFont("lemon"), "wasd: movement", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "rf: move up and down", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "h: show this help", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "q: rotate block", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "e: flip block", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "scroll wheel / left-right: move toolbar selection", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "up-down: change selected block (hold Control to change all)", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "0-9: switch to toolbar position", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "t: switch between backgrounds", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "i: toggle between drawing invisible blocks or not", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "o: toggle between drawing info or not", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "u: toggle between drawing lower layers or not", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "Esc: Save and quit", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "Shift+Esc: Quit without saving", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "Console (º): Reload Spritesheet and terrain table", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "b: Select brush", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "g: Select bucket", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "LClick/RClick: apply current tool", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "Ctrl+{X,C,V}: Cut/Copy/Paste", { 0,comptador++*20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "Ctrl+S: Save", { 0,comptador++*20 }, 20, HI2::Color::White);

		HI2::endFrame();
		_drawingHelp = true;
	}
	if (keysDown[HI2::BUTTON::KEY_B]) {
		_currentTool = tool::PENCIL;
	}
	if (keysDown[HI2::BUTTON::KEY_G]) {
		_currentTool = tool::BUCKET;
	}
	if (keysDown[HI2::BUTTON::KEY_M]) {
		_currentTool = tool::SELECT;
	}

	if (keysDown[HI2::BUTTON::KEY_I]) {
		_drawInvisible = !_drawInvisible;
	}
	if (keysDown[HI2::BUTTON::KEY_U]) {
		if(_cameraDepth==1)
			_cameraDepth=3;
		else
			_cameraDepth=1;
	}
	if(keysDown[HI2::BUTTON::KEY_BACKSPACE]){
		selectionL.reset();
		selectionR.reset();
	}
	if (keysHeld[HI2::BUTTON::KEY_LEFTCLICK])
	{
		auto mouse = HI2::getTouchPos();
		point3Di blockPos = _camera;
		blockPos.x += (int)(mouse.x / (config::spriteSize * zoom));
		blockPos.y += (int)(mouse.y / (config::spriteSize * zoom));
		applyTool(blockPos,false);
		checkpoint();
	}
	if (keysHeld[HI2::BUTTON::KEY_RIGHTCLICK])
	{
		auto mouse = HI2::getTouchPos();
		point3Di blockPos = _camera;
		blockPos.x += (int)(mouse.x / (config::spriteSize * zoom));
		blockPos.y += (int)(mouse.y / (config::spriteSize * zoom));
		applyTool(blockPos,true);
		checkpoint();
	}
}

void State::PrefabEditor::update(double dt)
{
}

void State::PrefabEditor::draw(double dt)
{
	Services::graphics.stepAnimations(dt);
	if (_drawingHelp)
	{
		_drawingHelp = false;
		return;
	}
	HI2::startFrame();
	drawBG();
	for (int depth = _cameraDepth - 1; depth >= 0; --depth) {
		for (int j = 0; j < ceil((double)HI2::getScreenWidth() / (zoom * config::spriteSize)); ++j)
		{
			for (int i = 0; i < ceil((double)HI2::getScreenHeight() / (zoom * config::spriteSize)); ++i)
			{
				point3Di pos = _camera;
				pos.x += j;
				pos.y += i;
				pos.z -= depth;
				if (pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < _prefab->getSize().x && pos.y < _prefab->getSize().y && pos.z < _prefab->getSize().z)
				{
					metaBlock& mb = (*_prefab)[pos];
					if (mb.base->visible)
					{
						HI2::setTextureColorMod(*mb.base->spr->getTexture(), { (unsigned char)(255 - depth * 50),(unsigned char)(255 - depth * 50),(unsigned char)(255 - depth * 50),255 });
						HI2::drawTexture(*mb.base->spr->getTexture(), j * (zoom * config::spriteSize), i * (zoom * config::spriteSize), mb.base->spr->getCurrentFrame().size, mb.base->spr->getCurrentFrame().startPos, zoom, ((double)(int)mb.rotation) * (M_PI / 2), (HI2::FLIP)mb.flip);
					}
					else
					{
						//TODO
						//DRAW INVISIBLE BLOCKS
						if (_drawInvisible) {
							HI2::drawRectangle({ int(j * (zoom * config::spriteSize)), int(i * (zoom * config::spriteSize)) }, int(zoom * config::spriteSize), int(zoom * config::spriteSize), HI2::Color{ (unsigned char)(mb.base->ID == 0 ? 255 : 0),(unsigned char)(mb.base->ID == 1 ? 255 : 0),(unsigned char)(mb.base->ID == 2 ? 255 : 0),127 });
						}
					}
					bool drawnSel = false;
					if(selectionL && selectionL == pos){
						HI2::drawRectangle({ int(j * (zoom * config::spriteSize)), int(i * (zoom * config::spriteSize)) }, int(zoom * config::spriteSize), int(zoom * config::spriteSize), HI2::Color{255,0,0,127});
						drawnSel = true;
					}
					if(selectionR && selectionR == pos){
						HI2::drawRectangle({ int(j * (zoom * config::spriteSize)), int(i * (zoom * config::spriteSize)) }, int(zoom * config::spriteSize), int(zoom * config::spriteSize), HI2::Color{0,0,255,127});
						drawnSel = true;
					}
					if(!drawnSel && isInSelection(pos)){
						HI2::drawRectangle({ int(j * (zoom * config::spriteSize)), int(i * (zoom * config::spriteSize)) }, int(zoom * config::spriteSize), int(zoom * config::spriteSize), HI2::Color{255,255,255,127});
					}
				}
				else
				{
					if (depth == 0)
						HI2::drawRectangle({ int(j * (zoom * config::spriteSize)), int(i * (zoom * config::spriteSize))}, int(zoom * config::spriteSize),int(zoom * config::spriteSize), HI2::Color{ 255,0,0,127 });
				}
			}
		}
	}

	auto mousePos = HI2::getTouchPos();
	//HI2::drawRectangle({((int)(mousePos.x/(config::spriteSize*zoom)))*config::spriteSize*zoom,((int)(mousePos.y/(config::spriteSize*zoom)))*config::spriteSize*zoom},zoom * config::spriteSize,zoom * config::spriteSize,{255,0,0,127});
	if(_currentTool == tool::SELECT){
		HI2::drawRectangle({(int)(((int)(mousePos.x / (config::spriteSize * zoom))) * config::spriteSize * zoom),(int)(((int)(mousePos.y / (config::spriteSize * zoom))) * config::spriteSize * zoom)},int(zoom * config::spriteSize), int(zoom * config::spriteSize), HI2::Color{255,255,255,127});
	}
	else if (_toolbar[_selectedToolbarPos]->visible)
		HI2::drawTexture(*_toolbar[_selectedToolbarPos]->spr->getTexture(), ((int)(mousePos.x / (config::spriteSize * zoom))) * config::spriteSize * zoom, ((int)(mousePos.y / (config::spriteSize * zoom))) * config::spriteSize * zoom, _toolbar[_selectedToolbarPos]->spr->getCurrentFrame().size, _toolbar[_selectedToolbarPos]->spr->getCurrentFrame().startPos, zoom, ((double)(int)_rotation) * (M_PI / 2), _flip ? HI2::FLIP::H : HI2::FLIP::NONE);



	HI2::drawRectangle({ (HI2::getScreenWidth() - (4 + (int)_toolbar.size() * config::spriteSize * 4 + (int)_toolbar.size() * 8)) / 2,HI2::getScreenHeight() - config::spriteSize * 4 - 4 }, 1 + _toolbar.size() * config::spriteSize * 4 + _toolbar.size() * 8, config::spriteSize * 4 + 4, HI2::Color::LightGrey);
	HI2::drawRectangle({ (HI2::getScreenWidth() - (4 + (int)_toolbar.size() * config::spriteSize * 4 + (int)_toolbar.size() * 8)) / 2 + +(int)_selectedToolbarPos * config::spriteSize * 4 + (int)_selectedToolbarPos * 8,HI2::getScreenHeight() - config::spriteSize * 4 - 4 }, config::spriteSize * 4 + 8, config::spriteSize * 4 + 4, HI2::Color::DarkestGrey);

	for (int i = 0; i < _toolbar.size(); ++i)
	{
		if (_toolbar[i]->visible)
		{
			HI2::setTextureColorMod(*_toolbar[i]->spr->getTexture(), HI2::Color::White);
			HI2::drawTexture(*_toolbar[i]->spr->getTexture(), (HI2::getScreenWidth() - (4 + _toolbar.size() * config::spriteSize * 4 + _toolbar.size() * 8)) / 2 + 4 + i * config::spriteSize * 4 + i * 8, HI2::getScreenHeight() - (config::spriteSize * 4), _toolbar[i]->spr->getCurrentFrame().size, _toolbar[i]->spr->getCurrentFrame().startPos, 4, ((double)(int)_rotation) * (M_PI / 2), _flip ? HI2::FLIP::H : HI2::FLIP::NONE);

		}
	}
	if (_drawStats)
	{
		auto mouse = HI2::getTouchPos();
		point3Di blockPos = _camera;
		blockPos.x += (int)(mouse.x / (config::spriteSize * zoom));
		blockPos.y += (int)(mouse.y / (config::spriteSize * zoom));


		HI2::drawText(*Services::fonts.loadFont("lemon"), "name: " + _prefab->getName(), { 0,0 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "size: x:" + std::to_string(_prefab->getSize().x), { 0,20 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "      y:" + std::to_string(_prefab->getSize().y), { 0,40 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "      z:" + std::to_string(_prefab->getSize().z), { 0,60 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "pos   x:" + std::to_string(blockPos.x), { 0,80 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "      y:" + std::to_string(blockPos.y), { 0,100 }, 20, HI2::Color::White);
		HI2::drawText(*Services::fonts.loadFont("lemon"), "      z:" + std::to_string(_camera.z), { 0,120 }, 20, HI2::Color::White);

		HI2::drawText(*Services::fonts.loadFont("lemon"), _toolbar[_selectedToolbarPos]->name, { 0,HI2::getScreenHeight()-20 }, 20, HI2::Color::White);

	}

	HI2::endFrame();
}

void State::PrefabEditor::save()
{
	_prefab->save();
}

void State::PrefabEditor::load()
{
	_prefab->load();
}

void State::PrefabEditor::reloadTerrainTable()
{
	std::vector<int> oldIDs;
	for(auto& b : _toolbar){
		oldIDs.push_back(b->ID);
	}
	std::stringstream pfb = _prefab->saveSS();
	baseBlock::loadTerrainTable();
	Services::graphics.reloadTexture("spritesheet");
	_prefab->loadSS(pfb);
	for (int i = 0; i < _toolbar.size(); ++i)
	{
		if(oldIDs[i]>=baseBlock::terrainTable.size())
			_toolbar[i] = &baseBlock::terrainTable.back();

		_toolbar[i] = &baseBlock::terrainTable[oldIDs[i]];
	}
}

void State::PrefabEditor::initToolbar()
{
	unsigned b = 52;
	for (int i = 0; i < _toolbar.size(); ++i)
	{
		while (!baseBlock::terrainTable[b].visible)
		{
			++b;
		}
		_toolbar[i] = &baseBlock::terrainTable[b++];
	}
}

void State::PrefabEditor::drawBG()
{
	switch (_bgType)
	{
	case 0:
		HI2::drawRectangle({}, HI2::getScreenWidth(), HI2::getScreenHeight(), HI2::Color::Black);
		break;
	case 1:
		HI2::drawRectangle({}, HI2::getScreenWidth(), HI2::getScreenHeight(), HI2::Color::White);
		break;
	case 2:
	default:
		HI2::drawTexture(*Services::graphics.loadTexture("test"), 0, 0);
		break;

	}
}

void State::PrefabEditor::applyTool(point3Di pos, bool rightClick)
{
	switch(_currentTool){
	case tool::PENCIL:
		applyPencil(pos,rightClick);
		break;
	case tool::BUCKET:
		applyBucket(pos);
		break;
	case tool::SELECT:
		applySelect(pos,rightClick);
		break;

	}
}

void State::PrefabEditor::applyPencil(point3Di blockPos, bool rightClick)
{
	baseBlock* targetBlock = rightClick? &baseBlock::terrainTable[0] :_toolbar[_selectedToolbarPos];
	if (blockPos.x >= 0 && blockPos.y >= 0 && blockPos.z >= 0 && blockPos.x < _prefab->getSize().x && blockPos.y < _prefab->getSize().y && blockPos.z < _prefab->getSize().z)
	{
		(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].base = targetBlock;
		(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].rotation = _rotation;
		(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].flip = _flip;
		(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].saveMeta = true;

	}
	if (_symmetryMode == symmetry::V || _symmetryMode == symmetry::BOTH)
	{
		blockPos.y = _prefab->getSize().y - blockPos.y - 1;
		if (blockPos.x >= 0 && blockPos.y >= 0 && blockPos.z >= 0 && blockPos.x < _prefab->getSize().x && blockPos.y < _prefab->getSize().y && blockPos.z < _prefab->getSize().z)
		{
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].base = targetBlock;
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].rotation = blockRotation((2 - (int)_rotation) % 4);
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].flip = !_flip;
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].saveMeta = true;
		}
		blockPos.y = -(blockPos.y - _prefab->getSize().y + 1);
	}
	if (_symmetryMode == symmetry::H || _symmetryMode == symmetry::BOTH)
	{
		blockPos.x = _prefab->getSize().x - blockPos.x - 1;
		if (blockPos.x >= 0 && blockPos.y >= 0 && blockPos.z >= 0 && blockPos.x < _prefab->getSize().x && blockPos.y < _prefab->getSize().y && blockPos.z < _prefab->getSize().z)
		{
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].base = targetBlock;
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].rotation = blockRotation((4 - (int)_rotation) % 4);
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].flip = !_flip;
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].saveMeta = true;
		}
		blockPos.x = -(blockPos.x - _prefab->getSize().x + 1);
	}
	if (_symmetryMode == symmetry::BOTH)
	{
		blockPos.x = _prefab->getSize().x - blockPos.x - 1;
		blockPos.y = _prefab->getSize().y - blockPos.y - 1;
		if (blockPos.x >= 0 && blockPos.y >= 0 && blockPos.z >= 0 && blockPos.x < _prefab->getSize().x && blockPos.y < _prefab->getSize().y && blockPos.z < _prefab->getSize().z)
		{
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].base = targetBlock;
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].rotation = blockRotation((2 + (int)_rotation) % 4);
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].flip = _flip;
			(*_prefab)[blockPos.z * _prefab->getSize().y * _prefab->getSize().x + blockPos.y * _prefab->getSize().x + blockPos.x].saveMeta = true;
		}
	}
}

void State::PrefabEditor::applyBucket(point3Di pos)
{
	std::deque<point3Di> pp{pos};
	std::set<point3Di> v;
	auto blocAntic = (*_prefab)[pos].base;
	while(!pp.empty())
		floodFill(pp,v,blocAntic,_toolbar[_selectedToolbarPos]);
}

void State::PrefabEditor::applySelect(point3Di pos, bool rightClick)
{
	(rightClick?selectionR:selectionL) = pos;
}

void State::PrefabEditor::floodFill(std::deque<point3Di> &pendingPositions, std::set<point3Di> &visited, baseBlock *targetBlock, baseBlock *newBlock)
{
	point3Di pos = pendingPositions.front();
	pendingPositions.pop_front();
	if(pos.x>=0 && pos.y >= 0 && pos.z>= 0 && pos.x < _prefab->getSize().x&& pos.y < _prefab->getSize().y&& pos.z < _prefab->getSize().z){
		if(visited.find(pos) == visited.end()){
			visited.insert(pos);
			if((*_prefab)[pos].base == targetBlock){
				(*_prefab)[pos].base = newBlock;
				(*_prefab)[pos].rotation = _rotation;
				(*_prefab)[pos].flip = _flip;
				(*_prefab)[pos].saveMeta = true;

				pendingPositions.push_back(pos+point3Di{0,0,-1});
				pendingPositions.push_back(pos+point3Di{0,0,+1});
				pendingPositions.push_back(pos+point3Di{0,-1,0});
				pendingPositions.push_back(pos+point3Di{0,+1,0});
				pendingPositions.push_back(pos+point3Di{-1,0,0});
				pendingPositions.push_back(pos+point3Di{+1,0,0});
			}
		}
	}
}

bool State::PrefabEditor::isInSelection(point3Di pos)
{
	if(!selectionL || !selectionR)
		return false;

	point3Di selStart;
	selStart.x = std::min(selectionL->x,selectionR->x);
	selStart.y = std::min(selectionL->y,selectionR->y);
	selStart.z = std::min(selectionL->z,selectionR->z);

	point3Di selEnd;
	selEnd.x = std::max(selectionL->x,selectionR->x);
	selEnd.y = std::max(selectionL->y,selectionR->y);
	selEnd.z = std::max(selectionL->z,selectionR->z);

	return pos.x>=selStart.x && pos.x<=selEnd.x &&pos.y>=selStart.y && pos.y<=selEnd.y &&pos.z>=selStart.z && pos.z<=selEnd.z;
}

void State::PrefabEditor::checkpoint()
{
	return;
	if(!*_currentCheckpoint || **_currentCheckpoint != *_prefab){
		auto it = _currentCheckpoint;
		it++;
		_checkpoints.erase(it,_checkpoints.end());
		_checkpoints.push_back(std::make_unique<prefab>(*_prefab));
		_currentCheckpoint++;
		while(_checkpoints.size() > config::pfbEditorMaxCheckpoints){
			if(_currentCheckpoint != _checkpoints.begin())
				_checkpoints.pop_front();
			else
				_checkpoints.pop_back();
		}
	}
}

void State::PrefabEditor::undo()
{
	if(_currentCheckpoint != _checkpoints.begin())
	{
		_currentCheckpoint--;
		_prefab = std::make_unique<prefab>(**_currentCheckpoint);
	}
}

void State::PrefabEditor::redo()
{
	if(_currentCheckpoint != --_checkpoints.end()){
		_currentCheckpoint++;
		_prefab = std::make_unique<prefab>(**_currentCheckpoint);
	}
}
