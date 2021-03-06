#pragma once
#include "HI2.hpp"
#include "reactphysics3d.h"
#include "json.hpp"
using nlohmann::json;

struct fdd { // Four DoF descriptor
	double x = 0;
	double y = 0;
	double z = 0;
	double r = 0;
	fdd(double x=0, double y=0, double z=0, double r=0):x(x),y(y),z(z),r(r){}
	fdd(const point3Di& p):x(p.x),y(p.y),z(p.z),r(0){}
	fdd(rp3d::Vector3 v);
	double angle(const fdd& r) const;
	fdd setMagnitude(double mag);
	fdd normalized() const;
	double magnitude()const;
	double distance(const fdd& r)const;
	double distance2D(const fdd& r)const;
	bool sameDirection(const fdd& r, double slackAngle = std::numeric_limits<double>::epsilon())const;
	bool operator==(const fdd& f)const;
	bool operator!=(const fdd& f)const;
	fdd operator+(const fdd& f)const;
	fdd operator-(const fdd& f)const;
	fdd operator+(const point3Dd& f)const;
	fdd operator-(const point3Dd& f)const;
	fdd operator+(const point3Di& f)const;
	fdd operator-(const point3Di& f)const;
	fdd operator*(const fdd& f)const;
	fdd operator/(const fdd& f)const;
	fdd operator*(const double& f)const;
	fdd operator/(const double& f)const;
	fdd& operator+=(const fdd& f);
	fdd& operator-=(const fdd& f);
	fdd& operator+=(const point3Dd& f);
	fdd& operator-=(const point3Dd& f);
	fdd& operator*=(const fdd& f);
	fdd& operator/=(const fdd& f);
	fdd& operator*=(const double& f);
	fdd& operator/=(const double& f);
	fdd operator-();
	fdd project(const fdd& right)const; //Project right onto this
	std::string to_string() const;
	double dot(const fdd& right) const;
	point2D getPoint2D() const;
	point3Di getPoint3Di()const;
	point3Dd getPoint3Dd()const;
	point3Dl getPoint3Dl()const;
	operator const point3Di(){return getPoint3Di();}
	rp3d::Vector3 getVector3() const;
};


std::ostream& operator<<(std::ostream &os, const fdd& f);

void to_json(json& j, const fdd& f);
void from_json(const json& j, fdd& f);