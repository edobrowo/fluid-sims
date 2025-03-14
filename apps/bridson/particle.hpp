#pragma once

#include "math/vector.hpp"
#include "util/common.hpp"

struct Particle {
    Particle(const Vector2D& pos, const Vector2D& vel) : mPos(pos), mVel(vel) {
    }
    Particle() : mPos(0.0f), mVel(0.0f) {
    }

    ~Particle() = default;

    Vector2D mPos;
    Vector2D mVel;
};
