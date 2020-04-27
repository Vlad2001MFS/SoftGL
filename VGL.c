#include "VGL.h"
#include "VGLInternal.h"
#include "Rasterizer.h"
#include "VertexProcessor.h"
#include "Common.h"
#include <stdlib.h>

vglGLContext *gCurrentContext = NULL;

vglGLContext *vglContextCreate(int w, int h) {
    vglGLContext *ctx = calloc(1, sizeof(vglGLContext));
    VGL_VECTOR_CREATE(ctx->colorBufferData);
    VGL_VECTOR_CREATE(ctx->depthBufferData);
    vglGLStateCreate(&ctx->state);
    vglContextResizeBuffers(ctx, w, h);
    vglVPInitialize();
    vglRSInitialize();
    return ctx;
}

void vglContextDestroy(vglGLContext *ctx) {
    if (gCurrentContext == ctx) {
        vglContextMakeCurrent(NULL);
    }
    vglRSShutdown();
    vglVPShutdown();
    vglGLStateDestroy(&ctx->state);
    VGL_VECTOR_DESTROY(ctx->depthBufferData);
    VGL_VECTOR_DESTROY(ctx->colorBufferData);
    free(ctx);
}

void vglContextMakeCurrent(vglGLContext *ctx) {
    if (ctx) {
        gCurrentContext = ctx;
        gCurrentState = &ctx->state;
        vglRSSetFramebuffer(&ctx->bufferRect, ctx->colorBufferData.data, ctx->depthBufferData.data);
    }
    else {
        gCurrentContext = NULL;
        gCurrentState = NULL;
        const vglIntRect rect = { 0, 0, 0, 0 };
        vglRSSetFramebuffer(&rect, NULL, NULL);
    }
}

void vglContextResizeBuffers(vglGLContext *ctx, int w, int h) {
    vglVec2i bufferSize;
    VGL_RECT_GET_SIZE(bufferSize, ctx->bufferRect);
    if (bufferSize.x != w || bufferSize.y != h) {
        VGL_VECTOR_RESIZE(ctx->colorBufferData, w*h);
        VGL_VECTOR_RESIZE(ctx->depthBufferData, w*h);
        VGL_RECT_SET4_SIZED(ctx->bufferRect, 0, 0, w, h);
    }
}

void vglContextGetColorBuffer(vglGLContext *ctx, void **colorBuffer, int *pitch) {
    *colorBuffer = ctx->colorBufferData.data;
    vglVec2i bufferSize;
    VGL_RECT_GET_SIZE(bufferSize, ctx->bufferRect);
    *pitch = bufferSize.x*sizeof(ctx->colorBufferData.data[0]);
}
