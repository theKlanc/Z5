#pragma once
#include <fdd.hpp>
#include <universeNode.hpp>

struct position {
	fdd pos;
	universeNode* parent;
	unsigned int parentID;
};

void to_json(nlohmann::json& j, const position& n);
void from_json(const nlohmann::json& j, position& n);