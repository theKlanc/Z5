#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include "json.hpp"

class brain{
public:
	virtual ~brain() = 0;
	virtual void update(double dt) = 0;
	virtual nlohmann::json getJson() const = 0;

	friend void to_json(nlohmann::json &j, const brain &b);
};

void to_json(nlohmann::json &j, const brain &b);