#include "services.hpp"


entt::registry* Services::enttRegistry;
rp3d::CollisionWorld* Services::collisionWorld;
std::mutex Services::physicsMutex;

graphicsManager Services::graphics;
fontManager Services::fonts;
audioManager Services::audio;

std::minstd_rand Services::lcg;
