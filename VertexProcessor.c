#include "VertexProcessor.h"
#include "GLInternal.h"
#include "Rasterizer.h"
#include "Common.h"

#define VGL_VP_MAX_VERTICES 40000

static vglVertex gVertices[VGL_VP_MAX_VERTICES];
static size_t gVerticesCount = 0;

vglVertex *vglVPNewVertex() {
    VGL_ASSERT(gVerticesCount < VGL_VP_MAX_VERTICES);

    return gVertices + gVerticesCount++;
}

void vglVPCopyVertex(size_t idx) {
    VGL_ASSERT(gVerticesCount < VGL_VP_MAX_VERTICES);
    VGL_ASSERT(idx < gVerticesCount);

    gVertices[gVerticesCount++] = gVertices[idx];
}

void vglVPProcess() {
    vglVec2i vpPos = gCurrentState->viewport.min;
    vglVec2i vpSize;
    VGL_RECT_GET_SIZE(vpSize, gCurrentState->viewport);

    vglMat4f vpMat;
    VGL_MAT4_SET_VIEWPORT(vpMat, vpPos.x, vpPos.y, vpSize.x, vpSize.y);

    vglMat4f mvp;
    VGL_MAT4_MUL(mvp, gCurrentState->projMat, gCurrentState->modelViewMat);

    vglVec4f pos;
    for (vglVertex *v = gVertices; v != gVertices + gVerticesCount; v++) {
        VGL_MAT4_MUL_VEC4(pos, mvp, v->pos);
        VGL_VEC4_DIV_SCALAR(pos, pos, pos.w);
        VGL_MAT4_MUL_VEC4(v->pos, vpMat, pos);
    }

    vglRSProcess();
    gVerticesCount = 0;
}

const vglVertex *vglVPGetVertices() {
    return gVertices;
}

size_t vglVPGetVerticesCount() {
    return gVerticesCount;
}
