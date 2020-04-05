#pragma once
#include "GL.hpp"
#include "Math.hpp"

struct GLState {
    Color clearColor = Color(0, 0, 0, 255);
    float clearDepth = 1.0f;

    uint32_t matrixMode = GL_MODELVIEW;
    Mat4f projMat = Mat4f::Identity;
    Mat4f modelViewMat = Mat4f::Identity;

    IntRect viewport = IntRect(0, 0, 0, 0);
    uint32_t depthFunc = GL_LESS;
    uint32_t caps = 0;

    Color imColor = Color(255, 255, 255, 255);
    uint32_t imQuadVertsCounter = 0;
    uint32_t primType = 0;

    Mat4f &currentMat() {
        if (matrixMode == GL_PROJECTION) {
            return projMat;
        }
        else if (matrixMode == GL_MODELVIEW) {
            return modelViewMat;
        }
    }
};

extern GLState *gCurrentState;
