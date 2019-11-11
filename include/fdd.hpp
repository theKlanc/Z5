#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "json.hpp"
using nlohmann::json;

struct fdd { // Four DoF descriptor
	double x;
	double y;
	double z;
	double r;
	fdd(double x=0, double y=0, double z=0, double r=0):x(x),y(y),z(z),r(r){}
	fdd(const point3Di& p):x(p.x),y(p.y),z(p.z),r(0){}
	fdd(rp3d::Vector3 v);
	fdd setMagnitude(double mag);
	fdd normalized() const;
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
	fdd project(const fdd& right)const; //Project right onto this
	double dot(const fdd& right) const;
	point3Di getPoint3Di()const;
	point3Dd getPoint3Dd()const;
	point3Dl getPoint3Dl()const;
	rp3d::Vector3 getVector3() const;

};

void to_json(json& j, const fdd& f);
void from_json(const json& j, fdd& f);