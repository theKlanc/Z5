#include "components/brain.hpp"

void to_json(nlohmann::json &j, const brain &b) {
	j = b.getJson();
}

brain::~brain(){}