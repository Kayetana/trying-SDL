#pragma once

#include "SDL.h"
#include "SDL_image.h"

class CPosition {
public:
    CPosition() : x(0.f), y(0.f) {}
    CPosition(float initX, float initY) : x(initX), y(initY) {}

    float x;
    float y;
};

class CTexture {
public:
    CTexture(SDL_Texture* text, int w, int h) : texture(text), width(w), height(h) {}

    SDL_Texture* texture;
    int width;
    int height;
};

class CCollision {
public:
    CCollision(float x, float y, float r) : xCenter(x), yCenter(y), radius(r) {}

    float xCenter;
    float yCenter;
    float radius;
};

class CVelocity {
public:
    CVelocity(int s) : speed(s), angle(0) {}
    CVelocity(int s, int a) : speed(s), angle(a) {}
    int speed;
    int angle;
};

class CInput {
public:
    CInput() : up(false), down(false), left(false), right(false) {}

    bool up;
    bool down;
    bool left;
    bool right;
};
