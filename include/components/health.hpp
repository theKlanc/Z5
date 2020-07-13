#pragma once
#include "json.hpp"

struct health
{
	health(unsigned max = 10, unsigned current = 10);
	bool alive();
	bool fullHealth();
	void heal(unsigned healing);
	void damage(unsigned damage);

	unsigned getCurrentHealth() const;
	unsigned getMaxHealth() const;
private:
	unsigned _maxHealth;
	unsigned _currentHealth;

	friend void to_json(nlohmann::json& j, const health& n);
	friend void from_json(const nlohmann::json& j, health& n);
};

void to_json(nlohmann::json& j, const health& n);
void from_json(const nlohmann::json& j, health& n);
