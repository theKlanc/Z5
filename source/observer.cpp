#include "observer.hpp"

std::array<std::unordered_map<void*,std::function<void(eventArgs args)>>,(unsigned)eventType::_SIZE> observer::_subscribers;

void observer::registerObserver(eventType t, std::function<void(eventArgs args)> f, void *owner)
{
	if(!_subscribers[(unsigned)t].contains(owner))
		_subscribers[(unsigned)t].emplace(owner,f);
}

void observer::deleteObserver(eventType t, void *owner)
{
	if(_subscribers[(unsigned)t].contains(owner))
		_subscribers[(unsigned)t].erase(owner);
}
