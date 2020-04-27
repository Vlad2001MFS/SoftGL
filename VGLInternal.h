#pragma once
#include "GLInternal.h"
#include "Common.h"

typedef struct vglGLContext {
    vglIntRect bufferRect;
    VGL_VECTOR(vglColor) colorBufferData;
    VGL_VECTOR(float) depthBufferData;
    vglGLState state;
} vglGLContext;

extern vglGLContext *gCurrentContext;