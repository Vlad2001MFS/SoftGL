#pragma once
#include "GL.h"
#include "MathLib.h"

typedef struct vglGLState {
    vglColor clearColor;
    float clearDepth;

    uint32_t matrixMode;
    vglMat4f *currentMat;
    vglMat4f projMat;
    vglMat4f modelViewMat;

    vglIntRect viewport;
    uint32_t depthFunc;
    uint32_t caps;

    vglColor imColor;
    uint32_t imQuadVertsCounter;
    uint32_t primType;
} vglGLState;

extern vglGLState *gCurrentState;

void vglGLStateSetDefault(vglGLState *state);
