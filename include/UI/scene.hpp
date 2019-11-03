#pragma once
#include "UI/gadget.hpp"

class scene{
	public:
		scene(std::string name = "");
		std::string_view getName();
		void setName(std::string s);
		void draw();
		void update(const unsigned long long& down,const unsigned long long& up,const unsigned long long& held, const point2D& mouse, double dt);
		void addGadget(std::shared_ptr<gadget> g);
		void select(std::shared_ptr<gadget> g);
	private:
		std::string _name;
		gadget* _selected = nullptr;
		std::vector<std::shared_ptr<gadget>> _gadgets;
};
