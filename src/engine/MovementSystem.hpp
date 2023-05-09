#pragma once

#include "Entity.hpp"

#define PI 3.14159265

class MovementSystem {
public:
    static void sMovement(entityVector& entities);

private:
    static void recalculate(std::shared_ptr<Entity>& entity);
};


void MovementSystem::recalculate(std::shared_ptr<Entity>& entity) {
    entity->cPosition->x += entity->cVelocity->speed * std::cos(entity->cVelocity->angle * PI / 180);
    entity->cPosition->y += entity->cVelocity->speed * std::sin(entity->cVelocity->angle * PI / 180);

    if (entity->cCollision) {
        entity->cCollision->xCenter = entity->cPosition->x + entity->cTexture->width / 2;
        entity->cCollision->yCenter = entity->cPosition->y + entity->cTexture->height / 2;
    }
}

void MovementSystem::sMovement(entityVector& entities) {
    for (auto& entity : entities) {
        if (entity->cInput) {
            if (entity->cInput->up) {
                entity->cVelocity->angle = 270;
                recalculate(entity);
                entity->cInput->up = false;
            }
            if (entity->cInput->down) {
                entity->cVelocity->angle = 90;
                recalculate(entity);
                entity->cInput->down = false;
            }
            if (entity->cInput->left) {
                entity->cVelocity->angle = 180;
                recalculate(entity);
                entity->cInput->left = false;
            }
            if (entity->cInput->right) {
                entity->cVelocity->angle = 0;
                recalculate(entity);
                entity->cInput->right = false;
            }
        } else {
            recalculate(entity);
        }
    }
}
