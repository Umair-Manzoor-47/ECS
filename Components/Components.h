#pragma once

struct Transform {
    float x;
    float y;
};

struct Velocity {
    float vx;
    float vy;
};

struct LoudTransform {
    float x, y;

    LoudTransform(float x, float y) : x(x), y(y) {
        std::cout << "Constructed (" << x << ", " << y << ")\n";
    }

    LoudTransform(LoudTransform&& other) noexcept : x(other.x), y(other.y) {
        std::cout << "Moved (" << x << ", " << y << ")\n";
    }

    ~LoudTransform() {
        std::cout << "Destroyed (" << x << ", " << y << ")\n";
    }
};