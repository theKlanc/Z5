#include "fdd.hpp"
#include "json.hpp"
#include <cmath>
#include <math.h>
#include "HI2.hpp"
#include "reactphysics3d.h"

void to_json(nlohmann::json& j, const fdd& f)
{
	j = nlohmann::json{ {"x",f.x},{"y",f.y},{"z",f.z},{"r",f.r} };
}

void from_json(const nlohmann::json& j, fdd& f)
{
	j.at("x").get_to(f.x);
	j.at("y").get_to(f.y);
	j.at("z").get_to(f.z);
	j.at("r").get_to(f.r);
}

fdd::fdd(rp3d::Vector3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	r = 0;
}

double fdd::angle(const fdd &r) const
{
	double d = dot(r);
	return std::acos(d/(magnitude()*r.magnitude()));
}

fdd fdd::setMagnitude(double mag)
{
	if(magnitude() != 0.0)
	*this *= (mag / magnitude());
	return *this;
}

fdd fdd::normalized() const
{
	fdd copy = *this;
	return copy / copy.magnitude();
}

double fdd::magnitude() const
{
	return distance({ 0,0,0,0 });
}

double fdd::distance(const fdd& r) const
{
	//((x2 - x1)2 + (y2 - y1)2 + (z2 - z1)2)1/2
	return sqrt(pow(r.x - x, 2) + pow(r.y - y, 2) + pow(r.z - z, 2));
}

double fdd::distance2D(const fdd& r) const
{
	return sqrt(pow(r.x - x, 2) + pow(r.y - y, 2));
}

bool fdd::sameDirection(const fdd& r, double slackAngle) const
{
	fdd rNorm = r;
	rNorm.setMagnitude(1);

	fdd copy(*this);
	copy.setMagnitude(1);
	fdd result = copy - rNorm;

	return angle(r) < slackAngle;
}

bool fdd::operator==(const fdd& f)const
{
	return x == f.x && y == f.y && z == f.z && r == f.r;
}

bool fdd::operator!=(const fdd& f)const
{
	return x != f.x || y != f.y || z != f.z || r != f.r;
}

fdd fdd::operator+(const fdd& f)const
{
	return fdd{ x + f.x,y + f.y,z + f.z,r + f.r };
}

fdd fdd::operator-(const fdd& f)const
{
	return fdd{ x - f.x,y - f.y,z - f.z,r - f.r };
}

fdd fdd::operator+(const point3Dd &f) const
{
	return { x + f.x,y + f.y,z + f.z, r};
}

fdd fdd::operator-(const point3Dd &f) const
{
	return { x - f.x,y - f.y,z - f.z, r};
}

fdd fdd::operator+(const point3Di& f) const
{
	return { x + f.x,y + f.y,z + f.z, r};
}

fdd fdd::operator-(const point3Di& f) const
{
	return { x - f.x,y - f.y,z - f.z, r};
}

fdd fdd::operator*(const fdd& f) const
{
	return { x * f.x,y * f.y,z * f.z,r * f.r };
}

fdd fdd::operator/(const fdd& f) const
{
	return { x / f.x,y / f.y,z / f.z,r / f.r };
}

fdd fdd::operator*(const double& f) const
{
	return { x * f,y * f,z * f,r * f };
}

fdd fdd::operator/(const double& f) const
{
	return { x / f,y / f,z / f,r / f };
}

fdd& fdd::operator+=(const fdd& f)
{
	x += f.x;
	y += f.y;
	z += f.z;
	r += f.r;
	return *this;
}

fdd& fdd::operator-=(const fdd& f)
{
	x -= f.x;
	y -= f.y;
	z -= f.z;
	r -= f.r;
	return *this;
}

fdd &fdd::operator+=(const point3Dd &f)
{
	x += f.x;
	y += f.y;
	z += f.z;
	return *this;
}

fdd &fdd::operator-=(const point3Dd &f)
{
	x -= f.x;
	y -= f.y;
	z -= f.z;
	return *this;
}

fdd& fdd::operator*=(const fdd& f)
{
	x *= f.x;
	y *= f.y;
	z *= f.z;
	r *= f.r;
	return *this;
}

fdd& fdd::operator/=(const fdd& f)
{
	x /= f.x;
	y /= f.y;
	z /= f.z;
	r /= f.r;
	return *this;
}

fdd& fdd::operator*=(const double& f)
{
	x *= f;
	y *= f;
	z *= f;
	r *= f;
	return *this;
}

fdd& fdd::operator/=(const double& f)
{
	x /= f;
	y /= f;
	z /= f;
	r /= f;
	return *this;
}

fdd fdd::operator-()
{
	return {-x,-y,-z,-r};
}

fdd fdd::project(const fdd& right) const
{
	fdd n = normalized();
	return n * (right.dot(n));
}

std::string fdd::to_string() const
{
	return "x: " + std::to_string(x) + " y: " + std::to_string(y) + " z: " + std::to_string(z) + " r: " + std::to_string(r);
}

double fdd::dot(const fdd& right) const
{
	return x * right.x + y * right.y + z * right.z;
}

point2D fdd::getPoint2D() const
{
	point2D p;
	p.x = floor(x);
	p.y = floor(y);
	return p;
}

point3Di fdd::getPoint3Di()const
{
	return { (int)floor(x),(int)floor(y),(int)floor(z) };
}

point3Dd fdd::getPoint3Dd()const
{
	return { x,y,z };
}

point3Dl fdd::getPoint3Dl()const
{
	return { (long)x,(long)y,(long)z };
}

reactphysics3d::Vector3 fdd::getVector3() const
{
	return rp3d::Vector3{ (rp3d::decimal)x,(rp3d::decimal)y,(rp3d::decimal)z };
}

std::ostream& operator<<(std::ostream& os, const fdd& f)
{
	return os << "x: " << f.x << " y: " << f.y << " z: " << f.z << " r: " << f.r;
}
