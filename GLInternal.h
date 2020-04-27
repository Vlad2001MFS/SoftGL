#pragma once
#include "GL.h"
#include "MathLib.h"
#include "Common.h"
#include <stdbool.h>

typedef struct vglTexture {
    bool isValid;
    uint32_t target;
    int level, internalFormat, width, height, border;
    uint32_t format, type;
    vglColor *pixels;
} vglTexture;

typedef struct vglLight {
    bool state;
    vglVec4f ambient, diffuse, specular, position;
    vglVec3f spotDirection;
    float spotExponent, spotCutOff, spotCutOffCos;
    float constantAtt, linearAtt, quadraticAtt;
} vglLight;

typedef struct vglGLState {
    vglColor clearColor;
    float clearDepth;

    vglIntRect viewport;
    uint32_t shadeModel;
    bool isDepthTest;
    uint32_t depthFunc;
    bool isTexture2D;
    bool isLighting;
    vglLight light[8];
    vglVec4f lightModelAmbient;

    uint32_t matrixMode;
    vglMat4f *currentMat;
    vglMat4f projMat;
    vglMat4f modelViewMat;

    vglColor imColor;
    vglVec2f imTexCoord;
    vglVec3f imNormal;
    uint8_t imQuadVertsCounter;
    uint32_t primType;

    vglTexture *texture2d;

    VGL_VECTOR(vglTexture) textures;
} vglGLState;

extern vglGLState *gCurrentState;

void vglGLStateCreate(vglGLState *state);
void vglGLStateDestroy(vglGLState *state);
