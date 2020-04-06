#include "VGL.h"
#include "VGLInternal.h"
#include "Rasterizer.h"
#include "Common.h"
#include <stdlib.h>

vglGLContext *gCurrentContext = NULL;

vglGLContext *vglContextCreate(int w, int h) {
    vglGLContext *ctx = calloc(1, sizeof(vglGLContext));
    vglContextResizeBuffers(ctx, w, h);
    return ctx;
}

void vglContextDestroy(vglGLContext *ctx) {
    if (gCurrentContext == ctx) {
        vglContextMakeCurrent(NULL);
    }
    free(ctx);
}

void vglContextMakeCurrent(vglGLContext *ctx) {
    if (ctx) {
        gCurrentContext = ctx;
        gCurrentState = &ctx->state;
        rsSetFramebuffer(ctx->bufferRect, ctx->colorBufferData, ctx->depthBufferData);
    }
    else {
        gCurrentContext = NULL;
        gCurrentState = NULL;
        rsSetFramebuffer(VGL_INT_RECT(0, 0, 0, 0), NULL, NULL);
    }
}

void vglContextResizeBuffers(vglGLContext *ctx, int w, int h) {
    VGL_ASSERT(w <= VGL_MAX_BUFFER_WIDTH && h <= VGL_MAX_BUFFER_HEIGHT);

    vglVec2i bufferSize;
    VGL_RECT_GET_SIZE(bufferSize, ctx->bufferRect);
    if (bufferSize.x != w || bufferSize.y != h) {
        VGL_RECT_SET4_SIZED(ctx->bufferRect, 0, 0, w, h);
    }
}

void vglContextGetColorBuffer(vglGLContext *ctx, void **colorBuffer, int *pitch) {
    colorBuffer = ctx->colorBufferData;
    vglVec2i bufferSize;
    VGL_RECT_GET_SIZE(bufferSize, ctx->bufferRect);
    pitch = bufferSize.x*sizeof(ctx->colorBufferData[0]);
}
