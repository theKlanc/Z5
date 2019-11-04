#include "UI/scene.hpp"

scene::scene(std::string name)
{
	_name = name;
}

std::string_view scene::getName()
{
	return _name;
}

void scene::setName(std::string s)
{
	_name = s;
}

void scene::draw()
{
	for (std::shared_ptr<gadget> g : _gadgets) {
		if (g->isVisible() && g->isRenderable({ 0,0 },{HI2::getScreenWidth(),HI2::getScreenHeight()}))
			g->draw({ 0,0 });
	}
	if (_selected != nullptr && _selected->isVisible() && _selected->isRenderable({ 0,0 },{HI2::getScreenWidth(),HI2::getScreenHeight()})) {
		HI2::drawEmptyRectangle(_selected->getPosition(), _selected->getSize().x, _selected->getSize().y, HI2::Color::Blue);
	}
}

void scene::update(const unsigned long long& down, const unsigned long long& up, const unsigned long long& held, const point2D& mouse, double dt)
{
	for (std::shared_ptr<gadget> g : _gadgets) {
		if (g->isActive())
			g->update(dt);
		if (down & HI2::BUTTON::TOUCH && g->touched(mouse) && g->isSelectable())
			_selected = g.get();
	}
	if (_selected != nullptr)
	{
		if (down & HI2::BUTTON::BUTTON_RIGHT) {
			gadget* temp = _selected->getRight();
			if (temp != nullptr && temp->isSelectable())
				_selected = temp;
		}
		else if (down & HI2::BUTTON::BUTTON_UP) {
			gadget* temp = _selected->getUp();
			if (temp != nullptr && temp->isSelectable())
				_selected = temp;
		}
		else if (down & HI2::BUTTON::BUTTON_LEFT) {
			gadget* temp = _selected->getLeft();
			if (temp != nullptr && temp->isSelectable())
				_selected = temp;
		}
		else if (down & HI2::BUTTON::BUTTON_DOWN) {
			gadget* temp = _selected->getDown();
			if (temp != nullptr && temp->isSelectable())
				_selected = temp;
		}
	}



	if (_selected != nullptr && _selected->isActive()) {
		_selected->update(down, up, held, mouse, dt);
	}

}

void scene::addGadget(std::shared_ptr<gadget> g)
{
	_gadgets.push_back(g);
	if (_selected == nullptr && g->isSelectable()) {
		_selected = g.get();
	}
}

void scene::select(std::shared_ptr<gadget> g)
{
	_selected = g.get();
}
