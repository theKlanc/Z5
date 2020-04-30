#pragma once
#include "HI2.hpp"
#include "json.hpp"
#include "controllable.hpp"

class brain : public controllable {
public:
	virtual ~brain() = 0;
	virtual void update(double dt) = 0; //update through AI
	using controllable::update;
	virtual nlohmann::json getJson() const = 0;

	virtual std::string getThoughts() const = 0;
	friend void to_json(nlohmann::json &j, const brain &b);
};

void to_json(nlohmann::json &j, const brain &b);