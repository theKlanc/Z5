#include "services.hpp"


rp3d::PhysicsCommon Services::physicsCommon;
entt::registry* Services::enttRegistry;
rp3d::PhysicsWorld* Services::physicsWorld;
std::mutex Services::physicsMutex;

graphicsManager Services::graphics;
fontManager Services::fonts;
audioManager Services::audio;
colliderManager Services::colliders;

std::minstd_rand Services::lcg;
