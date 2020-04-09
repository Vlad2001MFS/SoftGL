#pragma once
#include "GL.h"
#include "MathLib.h"
#include <stdbool.h>

#define VGL_MAX_TEXTURES 128

typedef struct vglTexture {
    bool isValid;
    uint32_t target;
    int level, internalFormat, width, height, border;
    uint32_t format, type;
    vglColor *pixels;
} vglTexture;

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
    vglVec2f imTexCoord;
    uint8_t imQuadVertsCounter;
    uint32_t primType;

    vglTexture *texture2d;

    vglTexture textures[VGL_MAX_TEXTURES];
    uint32_t texturesCount;
} vglGLState;

extern vglGLState *gCurrentState;

void vglGLStateSetDefault(vglGLState *state);
