#include "fdd.hpp"
#include "json.hpp"
#include <cmath>
#include <math.h>

void to_json(nlohmann::json &j, const fdd &f)
{
	j = json{{"x",f.x},{"y",f.y},{"z",f.z},{"r",f.r}};
}

void from_json(const nlohmann::json &j, fdd &f)
{
	j.at("x").get_to(f.x);
	j.at("y").get_to(f.y);
	j.at("z").get_to(f.z);
	j.at("r").get_to(f.r);
}
double fdd::distance(const fdd &r) const
{
//((x2 - x1)2 + (y2 - y1)2 + (z2 - z1)2)1/2
	return sqrt(pow(r.x-x,2)+pow(r.y-y,2)+pow(r.z-z,2));
}

double fdd::distance2D(const fdd &r) const
{
	return sqrt(pow(r.x-x,2)+pow(r.y-y,2));
}

bool fdd::operator==(const fdd &f)const
{
	return x==f.x&&y==f.y&&z==f.z&&r==f.r;
}

bool fdd::operator!=(const fdd &f)const
{
	return x!=f.x||y!=f.y||z!=f.z||r!=f.r;
}

fdd fdd::operator+(const fdd& f)const
{
	return fdd{x+f.x,y+f.y,z+f.z,r+f.r};
}

fdd fdd::operator-(const fdd& f)const
{
	return fdd{x-f.x,y-f.y,z-f.z,r-f.r};
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

fdd& fdd::operator+=(const fdd &f)
{
	x+=f.x;
	y+=f.y;
	z+=f.z;
	r+=f.r;
	return *this;
}

fdd& fdd::operator-=(const fdd &f)
{
	x-=f.x;
	y-=f.y;
	z-=f.z;
	r-=f.r;
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
