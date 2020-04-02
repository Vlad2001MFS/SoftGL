#pragma once
#include <stdint.h>

struct Color {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t data;
    };
};

struct Vec2i {
    Vec2i() = default;
    Vec2i(int x, int y) : x(x), y(y) {}

    int x, y;
};