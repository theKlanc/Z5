#include "components/health.hpp"

health::health(unsigned max, unsigned current){
	_maxHealth = max;
	_currentHealth = current;
	if(_currentHealth > _maxHealth)
		_currentHealth = _maxHealth;
}

bool health::alive(){
	return _currentHealth > 0;
}

bool health::fullHealth(){
	return _currentHealth == _maxHealth;
}

void health::heal(unsigned healing){
	_currentHealth+=healing;
	if(_currentHealth>_maxHealth)
		_currentHealth=_maxHealth;
}

void health::damage(unsigned damage){
	if(damage >= _currentHealth)
		_currentHealth=0;
	else
		_currentHealth-=damage;
}

void to_json(nlohmann::json &j, const health &n){
	j=nlohmann::json{{"max",n._maxHealth},{"current",n._currentHealth}};
}

void from_json(const nlohmann::json &j, health &n){
	n._maxHealth = j.at("max").get<unsigned>();
	n._currentHealth = j.at("current").get<unsigned>();
}
