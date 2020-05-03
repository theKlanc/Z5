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

	std::vector<fdd> getPositions() const;
	void setPositions(std::vector<fdd> f);

	void setParent(universeNode* p);

protected:
	std::vector<fdd> _positions;
	universeNode* _parent;
};

