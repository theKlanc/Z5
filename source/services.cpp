#include "services.hpp"


entt::registry* Services::enttRegistry;
rp3d::CollisionWorld* Services::collisionWorld;
std::mutex Services::physicsMutex;

graphicsManager Services::graphics;
fontManager Services::fonts;
audioManager Services::audio;
colliderManager Services::colliders;

std::minstd_rand Services::lcg;

observer Services::obsvr;
