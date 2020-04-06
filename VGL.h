#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vglGLContext vglGLContext;

vglGLContext *vglContextCreate(int w, int h);
void vglContextDestroy(vglGLContext *ctx);
void vglContextMakeCurrent(vglGLContext *ctx);
void vglContextResizeBuffers(vglGLContext *ctx, int w, int h);
void vglContextGetColorBuffer(vglGLContext *ctx, void **colorBuffer, int *pitch);

#ifdef __cplusplus
}
#endif
