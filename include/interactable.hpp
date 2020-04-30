#pragma once
#include "entt/entt.hpp"
#include "fdd.hpp"

class universeNode; //yikers

class interactable
{
public:
	interactable();

	virtual void interact(entt::entity e) = 0;

	virtual json getJson() const = 0;

	fdd getPosition() const;
	void setPosition(fdd f);

private:
	fdd _position;
	universeNode* _parent;
};

