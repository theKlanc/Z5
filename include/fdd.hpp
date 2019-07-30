#pragma once
#include "json.hpp"
using nlohmann::json;

struct fdd {
	double x;
	double y;
	double z;
	double r;

	bool operator==(const fdd& f)const;
	bool operator!=(const fdd& f)const;
	fdd operator+(const fdd& f)const;
	fdd operator-(const fdd& f)const;
	fdd operator+=(const fdd& f)const;
	fdd operator-=(const fdd& f)const;
};

void to_json(json& j, const fdd& f);
void from_json(const json& j, fdd& f);