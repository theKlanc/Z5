#pragma once
#include "UI/gadget.hpp"

class scene{
	public:
		scene(std::string name = "");
		std::string_view getName();
		void setName(std::string s);
		void draw();
		void update(const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, double dt);
		void addGadget(std::shared_ptr<gadget> g);
		void removeGadget(std::shared_ptr<gadget> g);
		void select(std::shared_ptr<gadget> g);
		void clear();
	private:
		std::string _name;
		gadget* _selected = nullptr;
		std::vector<std::shared_ptr<gadget>> _gadgets;
};
