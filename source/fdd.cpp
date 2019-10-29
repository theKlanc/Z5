#include "fdd.hpp"
#include "json.hpp"
#include <cmath>
#include <math.h>

void to_json(nlohmann::json& j, const fdd& f)
{
	j = json{ {"x",f.x},{"y",f.y},{"z",f.z},{"r",f.r} };
}

void from_json(const nlohmann::json& j, fdd& f)
{
	j.at("x").get_to(f.x);
	j.at("y").get_to(f.y);
	j.at("z").get_to(f.z);
	j.at("r").get_to(f.r);
}

fdd fdd::setMagnitude(double mag)
{
	*this *= (mag / magnitude());
	return *this;
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

bool fdd::sameDirection(const fdd& r) const
{
	fdd rNorm = r;
	rNorm.setMagnitude(1);

	fdd copy(*this);
	copy.setMagnitude(1);
	fdd result = copy - rNorm;

	return result.x < std::numeric_limits<double>::epsilon() && result.x > -std::numeric_limits<double>::epsilon() && result.y < std::numeric_limits<double>::epsilon() && result.y > -std::numeric_limits<double>::epsilon() && result.z < std::numeric_limits<double>::epsilon() && result.z > -std::numeric_limits<double>::epsilon();
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

point3Di fdd::getPoint3Di()const
{
	return { (int)x,(int)y,(int)z };
}

point3Dd fdd::getPoint3Dd()const
{
	return { x,y,z };
}

point3Dl fdd::getPoint3Dl()const
{
	return { (long)x,(long)y,(long)z };
}