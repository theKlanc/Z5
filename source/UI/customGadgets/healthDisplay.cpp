#include "UI/customGadgets/healthDisplay.hpp"
#include "config.hpp"
#include "services.hpp"

healthDisplay::healthDisplay(point2D pos, health *h,unsigned spacing, double size, std::string s):_health(h)
{
	_spacing = spacing;
	_scaling = size;
	if(h){
		unsigned amountOfContainers = h->getMaxHealth()/2;
		_size={(int)(amountOfContainers*config::spriteSize * size + spacing*size*(amountOfContainers-1)),(int)(config::spriteSize * size)};
	}
	init(pos,_size,s);
	_empty = Services::graphics.loadTexture("UI/heartcontainer_empty");
	_half = Services::graphics.loadTexture("UI/heartcontainer_half");
	_full = Services::graphics.loadTexture("UI/heartcontainer_full");

}

void healthDisplay::update(const double &dt)
{

}

void healthDisplay::setHealth(health *h)
{
	_health = h;
	if(h){
		unsigned amountOfContainers = h->getMaxHealth()/2;
		_size={(int)(amountOfContainers*config::spriteSize * _scaling + _spacing*_scaling*(amountOfContainers-1)),(int)(config::spriteSize * _scaling)};
		init(_position,_size,_name);
	}
}

void healthDisplay::_draw_internal()
{
	if(_health){
		int remaining = _health->getCurrentHealth();
		for(int i = 0; i < _health->getMaxHealth()/2;++i){
			int pos = i *config::spriteSize * _scaling + _spacing*_scaling*i;
			if(remaining>=2){
				HI2::drawTexture(*_full,pos,0,_scaling);
				remaining-=2;
			}
			else if(remaining==1){
				HI2::drawTexture(*_half,pos,0,_scaling);
				remaining--;
			}
			else{
				HI2::drawTexture(*_empty,pos,0,_scaling);
			}
		}
	}
}
