#pragma once
#include "HI2.hpp"
#include "json.hpp"
#include "controllable.hpp"

class brain : public controllable {
public:
	virtual ~brain() = 0;
	void update(double dt); //update through AI
	using controllable::update;
	virtual nlohmann::json getJson() const = 0;

	virtual std::string getThoughts() const = 0;
	friend void to_json(nlohmann::json &j, const brain &b);

	void setControlling(controllable* c = nullptr);
protected:
	virtual void _updateInternal(double dt) = 0;
	controllable* _controlling=nullptr;
};

void to_json(nlohmann::json &j, const brain &b);