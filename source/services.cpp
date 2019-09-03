#include "services.hpp"


entt::registry* Services::enttRegistry;
rp3d::CollisionWorld* Services::collisionWorld;
std::mutex Services::physicsMutex;