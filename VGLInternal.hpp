#pragma once
#include "GLInternal.hpp"
#include "Math.hpp"
#include <vector>

struct GLContext {
    GLContext();
    ~GLContext();

    IntRect bufferRect;
    std::vector<Color> colorBufferData;
    std::vector<float> depthBufferData;
    GLState *state;
};

extern GLContext *gCurrentContext;