#pragma once
#include <fdd.hpp>
#include <universeNode.hpp>

struct position {
	fdd pos;
	universeNode* parent;
	unsigned int parentID;
	fdd getRPos() const;
	void setRPos(fdd p);
private:
	fdd rpos;
};

void to_json(nlohmann::json& j, const position& n);
void from_json(const nlohmann::json& j, position& n);