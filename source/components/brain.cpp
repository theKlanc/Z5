#include "components/brain.hpp"

void to_json(nlohmann::json &j, const brain &b) {
	j = b.getJson();
}

brain::~brain(){}
void brain::update(double dt)
{
	_updateInternal(dt);
}

void brain::setControlling(controllable *c)
{
	_controlling = c;
}
