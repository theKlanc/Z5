#pragma once
#include "json.hpp"
using nlohmann::json;

struct fdd { // Four DoF descriptor
	double x;
	double y;
	double z;
	double r;


	double magnitude()const;
	double distance(const fdd& r)const;
	double distance2D(const fdd& r)const;
	bool operator==(const fdd& f)const;
	bool operator!=(const fdd& f)const;
	fdd operator+(const fdd& f)const;
	fdd operator-(const fdd& f)const;
	fdd operator*(const fdd& f)const;
	fdd operator/(const fdd& f)const;
	fdd operator*(const double& f)const;
	fdd operator/(const double& f)const;
	fdd& operator+=(const fdd& f);
	fdd& operator-=(const fdd& f);
	fdd& operator*=(const fdd& f);
	fdd& operator/=(const fdd& f);
	fdd& operator*=(const double& f);
	fdd& operator/=(const double& f);
};

void to_json(json& j, const fdd& f);
void from_json(const json& j, fdd& f);