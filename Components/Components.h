#pragma once
#include <iostream>

namespace ecs {
    struct Transform {
        float x;
        float y;
    };

    struct Velocity {
        float vx;
        float vy;
    };

    struct Sprite {
        float radius;
        int r, g, b; // color
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
    struct Pulse {
        float timer = 0.0f;     // current time
        float speed = 1.0f;     // pulse speed
        float minRadius = 3.0f;
        float maxRadius = 12.0f;
    };

    struct Orbit {
        float centerX = 0.0f;
        float centerY = 0.0f;
        float radius  = 80.0f;  // orbit radius
        float angle   = 0.0f;   // current angle
        float speed   = 1.0f;   // angular speed
    };

    struct Fade {
        float timer   = 0.0f;
        float speed   = 1.0f;
        int   alpha   = 255;
    };
}