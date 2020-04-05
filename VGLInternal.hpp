#pragma once
#include "GLInternal.hpp"
#include "Math.hpp"
#include <vector>

struct GLContext {
    IntRect bufferRect = IntRect(0, 0, 0, 0);
    std::vector<Color> colorBufferData;
    std::vector<float> depthBufferData;
    GLState state = GLState();
};

extern GLContext *gCurrentContext;