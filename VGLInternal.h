#pragma once
#include "GLInternal.h"
#include "MathLib.h"

#define VGL_MAX_BUFFER_WIDTH 1920
#define VGL_MAX_BUFFER_HEIGHT 1080

typedef struct vglGLContext {
    vglIntRect bufferRect;
    vglColor colorBufferData[VGL_MAX_BUFFER_WIDTH*VGL_MAX_BUFFER_HEIGHT];
    float depthBufferData[VGL_MAX_BUFFER_WIDTH*VGL_MAX_BUFFER_HEIGHT];
    vglGLState state;
} vglGLContext;

extern vglGLContext *gCurrentContext;