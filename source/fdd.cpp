#include "fdd.hpp"
#include "json.hpp"

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

fdd fdd::operator+=(const fdd &f)const
{
	return *this + f;
}

fdd fdd::operator-=(const fdd &f)const
{
	return *this - f;
}
