#include "services.hpp"


entt::registry* Services::enttRegistry;

rp3d::DynamicsWorld* Services::dynamicsWorld;
std::mutex Services::physicsMutex;