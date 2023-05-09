#pragma once

#include <cmath>
#include <memory>

#include "Entity.hpp"

class CollisionSystem {
public:
    static void sCollision(EntityManager& manager);

private:
    static bool isCollide(std::shared_ptr<Entity>& lhs, std::shared_ptr<Entity>& rhs);
};

void CollisionSystem::sCollision(EntityManager& manager) {
    if (!manager.hasEntityType("bullet") || !manager.hasEntityType("enemy")) {
        return;
    }
    for (auto& bullet : manager.getEntities("bullet")) {
        for (auto& enemy : manager.getEntities("enemy")) {
            if (isCollide(bullet, enemy)) {
                bullet->destroy();
                enemy->destroy();
            }
        }
    }
    // for each player and each enemy
    // for each player and each bullet
    // for each entity and tile
}

bool CollisionSystem::isCollide(std::shared_ptr<Entity>& lhs, std::shared_ptr<Entity>& rhs) {
    // (x2 - x1)^2 + (y2 - y1)^2 < (r1 + r2)^2
    float leftSideExpr = std::pow((lhs->cCollision->xCenter - rhs->cCollision->xCenter), 2) +
                         std::pow((lhs->cCollision->yCenter - rhs->cCollision->yCenter), 2);

    float rightSideExpr = std::pow((lhs->cCollision->radius + rhs->cCollision->radius), 2);
    return leftSideExpr < rightSideExpr;
}
