#pragma once
#include "HardwareInterface/HardwareInterface.hpp"

class gadget{
	public:
		virtual ~gadget() = 0;
		virtual void draw(point2D offset);
		virtual void drawOverlay(point2D offset);
		virtual void update(const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt);
		virtual void update(const double& dt);
		void setPosition(point2D pos);
		void setSize(point2D size);
		point2D getPosition();
		point2D getSize();
		std::string_view getName();
		void setActive(bool b = true);
		void setVisible(bool b = true);
		void setSelectable(bool b = true);
		bool isVisible(); // is the _visible flag set?
		bool isActive(); // is the _active flag set?
		bool isRenderable(point2D offset, point2D parentSize); // will it be drawn inside the screen?
		bool isCompletelyRenderable(point2D offset, point2D parentSize); // will it be drawn completely inside the screen?
		bool isSelectable();

		void setRight(gadget* g);
		void setUp(gadget* g);
		void setLeft(gadget* g);
		void setDown(gadget* g);
		virtual gadget* getRight();
		virtual gadget* getUp();
		virtual gadget* getLeft();
		virtual gadget* getDown();

		bool touched(point2D touchPosition);

	protected:
		point2D _position;
		point2D _size;
		std::string _name;

		gadget* _right = nullptr;
		gadget* _up = nullptr;
		gadget* _left = nullptr;
		gadget* _down = nullptr;

		bool _active = true;
		bool _visible = true;

		bool _selectable = false;
};
