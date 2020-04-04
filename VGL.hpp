#pragma once

struct GLContext;

GLContext *vglContextCreate(int w, int h);
void vglContextDestroy(GLContext *ctx);
void vglContextMakeCurrent(GLContext *ctx);
void vglContextResizeBuffers(GLContext *ctx, int w, int h);
void vglContextGetColorBuffer(GLContext *ctx, void *&colorBuffer, int &pitch);
