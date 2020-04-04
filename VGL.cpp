#include "VGL.hpp"
#include "VGLInternal.hpp"
#include "GLInternal.hpp"
#include "Rasterizer.hpp"

GLContext *gCurrentContext = nullptr;

GLContext::GLContext() {
    this->bufferRect.set(0, 0, 0, 0);
    this->state = createGLState();
}

GLContext::~GLContext() {
    destroyGLState(this->state);
}

GLContext *vglContextCreate(int w, int h) {
    auto ctx = new GLContext();
    vglContextResizeBuffers(ctx, w, h);
    return ctx;
}

void vglContextDestroy(GLContext *ctx) {
    if (gCurrentContext == ctx) {
        vglContextMakeCurrent(nullptr);
    }
    delete ctx;
}

void vglContextMakeCurrent(GLContext *ctx) {
    if (ctx) {
        gCurrentContext = ctx;
        gCurrentState = ctx->state;
        rsSetFramebuffer(ctx->bufferRect, ctx->colorBufferData.data(), ctx->depthBufferData.data());
    }
    else {
        gCurrentContext = nullptr;
        gCurrentState = nullptr;
        rsSetFramebuffer(IntRect(0, 0, 0, 0), nullptr, nullptr);
    }
}

void vglContextResizeBuffers(GLContext *ctx, int w, int h) {
    auto size = ctx->bufferRect.getSize();
    if (size.x != w || size.y != h) {
        ctx->bufferRect.setSized(0, 0, w, h);
        ctx->colorBufferData.resize(w*h);
        ctx->depthBufferData.resize(w*h);
    }
}

void vglContextGetColorBuffer(GLContext *ctx, void *&colorBuffer, int &pitch) {
    colorBuffer = ctx->colorBufferData.data();
    pitch = ctx->bufferRect.getSize().x*sizeof(ctx->colorBufferData[0]);
}
