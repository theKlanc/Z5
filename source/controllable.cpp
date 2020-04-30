#include "controllable.hpp"

controllable::controllable()
{

}

void controllable::setExitCallback(std::function<void ()> f)
{
	_exitCallback = f;
}
