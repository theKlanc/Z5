#pragma once
#include "json.hpp"
using nlohmann::json;

struct fdd {
	double x;
	double y;
	double z;
	double r;
};

void to_json(json& j, const fdd& f);
void from_json(const json& j, fdd& f);