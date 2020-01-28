#pragma once
#include "UI/gadget.hpp"

class panel : public gadget {
public:
	panel(point2D pos, point2D size, std::string s = "");
	void draw(point2D offset) override;
	void update(const std::bitset<HI2::BUTTON_SIZE>& down, const std::bitset<HI2::BUTTON_SIZE>& up, const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt) override;
	void update(const double& dt) override;

	virtual gadget* getRight() override;
	virtual gadget* getUp() override;
	virtual gadget* getLeft() override;
	virtual gadget* getDown() override;

	void addGadget(std::shared_ptr<gadget> g);
	void removeGadget(std::shared_ptr<gadget>g);
	void clear();


protected:
	std::vector<std::shared_ptr<gadget>> _gadgets;
	gadget* _selected = nullptr;
};
