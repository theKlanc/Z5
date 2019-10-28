#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include "json.hpp"
using nlohmann::json;

struct fdd { // Four DoF descriptor
	double x;
	double y;
	double z;
	double r;

	fdd setMagnitude(double mag);
	double magnitude()const;
	double distance(const fdd& r)const;
	double distance2D(const fdd& r)const;
	bool sameDirection(const fdd& r)const;
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
	const point3Di getPoint3Di()const;
	const point3Dd getPoint3Dd()const;
	const point3Dl getPoint3Dl()const;
};

void to_json(json& j, const fdd& f);
void from_json(const json& j, fdd& f);