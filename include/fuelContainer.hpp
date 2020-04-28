#pragma once
#include "fuel.hpp"


class fuelContainer
{
public:
	fuelContainer(){}
	double fill(double mass);//fills the container with mass and returns the remainder
	double request(double mass);//tries to retrieve mass contents and returns the remainder (as in leftover request)
	double getContent();//returns the amount of mass contained within
	bool isEmpty() const;
	bool isFull() const;
private:
	const fuel *_fuelType = nullptr;
	double _capacity; //fuel capacity in kg
	double _content; //current fuel in kg

	friend void to_json(nlohmann::json& j, const fuelContainer& c);
	friend void from_json(const nlohmann::json& j, fuelContainer& c);
};

void to_json(nlohmann::json& j, const fuelContainer& c);
void from_json(const nlohmann::json& j, fuelContainer& c);