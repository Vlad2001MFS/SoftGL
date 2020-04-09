#pragma once
#include "GL.h"
#include "MathLib.h"

typedef struct vglGLState {
    vglColor clearColor;
    float clearDepth;

    vglIntRect viewport;
    uint32_t caps;
    uint32_t shadeModel;
    uint32_t depthFunc;

    uint32_t matrixMode;
    vglMat4f *currentMat;
    vglMat4f projMat;
    vglMat4f modelViewMat;

    vglColor imColor;
    uint8_t imQuadVertsCounter;
    uint32_t primType;
} vglGLState;

extern vglGLState *gCurrentState;

void vglGLStateSetDefault(vglGLState *state);
