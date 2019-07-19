#pragma once
#include "universeNode.hpp"

class star : virtual public universeNode{
	public:
	star(const json& j, universeNode* parent = nullptr);
	private:

};
