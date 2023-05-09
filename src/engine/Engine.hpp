#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include "Entity.hpp"
#include "Components.hpp"
#include "CollisionSystem.hpp"
#include "MovementSystem.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cmath>

// texture width, texture height, collision radius
struct PlayerConfig {
    std::string texture;
    int twidth, theight;
    float rad;
    int speed;
};

struct EnemyConfig {
    std::string texture;
    int twidth, theight;
    float rad;
    int speed;
};

struct BulletConfig {
    std::string texture;
    int twidth, theight;
    float rad;
    int speed;
};

struct WindowConfig {
    std::string title;
    int width;
    int height;
};

using entityVector = std::vector<std::shared_ptr<Entity>>;

class Engine {
public:
    Engine(const std::string& configPath);
    void run();
    ~Engine();

private:
    void readConfig(const std::string& config);
    void init();
    SDL_Texture* loadTexture(const std::string& file);
    void sInput(entityVector& entities);
    void sRender(const entityVector& entities);

    // должно быть в игре, а не в движке
    void spawnPlayer();
    void spawnEnemy();
    void spawnBullet(std::shared_ptr<Entity>& entity);

private:
    SDL_Window* window = nullptr;
    WindowConfig m_windowConf;
    SDL_Renderer* renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> m_textures;

    PlayerConfig m_playerConf;
    EnemyConfig m_enemyConf;
    BulletConfig m_bulletConf;

    bool m_paused = false;
    bool m_isRunning = false;
    
    EntityManager manager;

    size_t counter = 0;
};

Engine::Engine(const std::string& configPath) {
    readConfig(configPath);
    init();
}

// game
void Engine::readConfig(const std::string& config) {
    std::ifstream file(config);
    if (!file.is_open()) {
        throw std::runtime_error("file not opened");
    }

    file >> m_playerConf.texture >> m_playerConf.twidth >> m_playerConf.theight >> m_playerConf.rad >> m_playerConf.speed;
    file >> m_enemyConf.texture >> m_enemyConf.twidth >> m_enemyConf.theight >> m_enemyConf.rad >> m_enemyConf.speed;
    file >> m_bulletConf.texture >> m_bulletConf.twidth >> m_bulletConf.theight >> m_bulletConf.rad >> m_bulletConf.speed;
    file >> m_windowConf.title >> m_windowConf.width >> m_windowConf.height;
}

void Engine::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::runtime_error("SDL_Init");
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        throw std::runtime_error("IMG_Init");
    }

    window = SDL_CreateWindow(m_windowConf.title.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              m_windowConf.width,
                              m_windowConf.height,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        throw std::runtime_error("CreateRenderer");
    }

    /*
        ресурсный/файловый менеджер
    */
    m_textures.emplace("player", loadTexture(m_playerConf.texture));
    m_textures.emplace("enemy", loadTexture(m_enemyConf.texture));
    m_textures.emplace("bullet", loadTexture(m_bulletConf.texture));

    m_isRunning = true;
}

SDL_Texture* Engine::loadTexture(const std::string& file) {
    SDL_Texture* texture = nullptr;
    texture = IMG_LoadTexture(renderer, file.c_str());
    if (texture == nullptr) {
        throw std::runtime_error("loading texture");
    }
    return texture;
}


// loop
void Engine::run() {
    spawnPlayer();
    spawnEnemy();

    while (m_isRunning) {
        sInput(manager.getEntities());
        
        manager.update();
        MovementSystem::sMovement(manager.getEntities());
        CollisionSystem::sCollision(manager);
        
        if (counter % 100 == 0) {
            spawnEnemy();
        }
        ++counter;

        sRender(manager.getEntities());
    }
}

void Engine::sInput(entityVector& entities) {
    for (auto& entity : entities) {
        if (entity->cInput) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    m_isRunning = false;
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    spawnBullet(entity);
                }
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            m_isRunning = false;
                            break;
                        case SDLK_p:
                            m_paused = !m_paused;
                            break;
                        case SDLK_w:
                            entity->cInput->up = true;
                            break;
                        case SDLK_s:
                            entity->cInput->down = true;
                            break;
                        case SDLK_a:
                            entity->cInput->left = true;
                            break;
                        case SDLK_d:
                            entity->cInput->right = true;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}

void Engine::sRender(const entityVector& entities) {
    SDL_RenderClear(renderer);

    for (const auto& entity : entities) {
        // прямоугольник, который берем от загруженной текстуры
        SDL_Rect src;
        src.x = 0;
        src.y = 0;
        src.w = entity->cTexture->width;
        src.h = entity->cTexture->height;

        // прямоугольник в окне, в который рисуем src
        SDL_Rect dst;
        dst.x = entity->cPosition->x;
        dst.y = entity->cPosition->y;
        dst.w = entity->cTexture->width * 1.5;
        dst.h = entity->cTexture->height * 1.5;

        SDL_RenderCopy(renderer, entity->cTexture->texture, &src, &dst);
    }
    SDL_RenderPresent(renderer);
}

void Engine::spawnPlayer() {
    auto player = manager.addEntity("player");

    float initX = std::rand() % (m_windowConf.width - 100);
    float initY = std::rand() % (m_windowConf.height - 100);

    float xCenter = initX + m_playerConf.twidth / 2;
    float yCenter = initY + m_playerConf.theight / 2;

    player->cPosition = std::make_shared<CPosition>(initX, initY);
    player->cVelocity = std::make_shared<CVelocity>(m_playerConf.speed);
    player->cCollision = std::make_shared<CCollision>(xCenter, yCenter, m_playerConf.rad);
    player->cTexture = std::make_shared<CTexture>(m_textures.at("player"), m_playerConf.twidth, m_playerConf.theight);
    player->cInput = std::make_shared<CInput>();
}

void Engine::spawnEnemy() {
    auto enemy = manager.addEntity("enemy");

    float initX = std::rand() % (m_windowConf.width - 100);
    float initY = std::rand() % (m_windowConf.height - 100);

    float xCenter = initX + m_enemyConf.twidth / 2;
    float yCenter = initY + m_enemyConf.theight / 2;

    enemy->cPosition = std::make_shared<CPosition>(initX, initY);
    enemy->cVelocity = std::make_shared<CVelocity>(m_enemyConf.speed);
    enemy->cCollision = std::make_shared<CCollision>(xCenter, yCenter, m_enemyConf.rad);
    enemy->cTexture = std::make_shared<CTexture>(m_textures.at("enemy"), m_enemyConf.twidth, m_enemyConf.theight);
}

void Engine::spawnBullet(std::shared_ptr<Entity>& entity) {
    auto bullet = manager.addEntity("bullet");

    float initX = entity->cPosition->x;
    float initY = entity->cPosition->y;

    float xCenter = initX + m_bulletConf.twidth / 2;
    float yCenter = initY + m_bulletConf.theight / 2;

    bullet->cPosition = std::make_shared<CPosition>(initX, initY);
    bullet->cVelocity = std::make_shared<CVelocity>(m_bulletConf.speed, entity->cVelocity->angle);
    bullet->cCollision = std::make_shared<CCollision>(xCenter, yCenter, m_bulletConf.speed);
    bullet->cTexture = std::make_shared<CTexture>(m_textures.at("bullet"), m_bulletConf.twidth, m_bulletConf.theight);
}

Engine::~Engine() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    for (auto& [key, texture] : m_textures) {
        SDL_DestroyTexture(texture);
    }
    // IMG_Quit();
    // SDL_Quit();
}
