#pragma once
#include "observer.hpp"
#include "services.hpp"

namespace systems{
	void projectileDamage(eventArgs args);
	void projectileBounce(eventArgs args);
}