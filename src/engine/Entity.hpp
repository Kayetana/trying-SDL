#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include "Components.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <memory>

class Entity {
public:
    Entity() {}
    Entity(const std::string& tag, size_t id) : m_tag(tag), m_id(id) {}

    bool isActive() {
        return m_active;
    }
    size_t getId() {
        return m_id;
    }
    std::string getTag() {
        return m_tag;
    }

    void destroy() {
        m_active = false;
    }

    std::shared_ptr<CPosition> cPosition;
    std::shared_ptr<CVelocity> cVelocity;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CTexture> cTexture;
    std::shared_ptr<CInput> cInput;

private:
    const size_t m_id = 0;
    const std::string m_tag = "Default";
    bool m_active = true;
};

using entityVector = std::vector<std::shared_ptr<Entity>>;

class EntityManager {
public:
    EntityManager() {}

    std::shared_ptr<Entity> addEntity(const std::string& tag) {
        auto entity = std::make_shared<Entity>(tag, ++totalEntities);
        m_toAdd.push_back(entity);

        return entity;
    }

    void update() {
        for (auto& entity : m_toAdd) {
            m_entities.push_back(entity);
            m_entityMap[entity->getTag()].push_back(entity);
        }
        m_toAdd.clear();

        m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [](std::shared_ptr<Entity> entity) { return !entity->isActive(); }),
            m_entities.end());

        for (auto& [key, vect] : m_entityMap) {
            vect.erase(std::remove_if(vect.begin(), vect.end(), [](std::shared_ptr<Entity> entity) { return !entity->isActive(); }), vect.end());
        }
    }

    entityVector& getEntities() {
        return m_entities;
    }

    entityVector& getEntities(const std::string& tag) {
        return m_entityMap.at(tag);
    }

    bool hasEntityType(const std::string& tag) {
        return m_entityMap.find(tag) != m_entityMap.end();
    }

private:
    entityVector m_entities;
    entityVector m_toAdd;
    std::map<std::string, entityVector> m_entityMap;
    size_t totalEntities = 0;
};
