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

    vglVec4f pos[3];

    const vglVec3f ndcMin = { -1, -1, -1 };
    const vglVec3f ndcMax = {  1,  1,  1 };

    size_t verticesCount = 0;
    vglVertex *v = gVertices;
    for (size_t i = 0; i < gVerticesCount; i += 3) {
        vglVertex *tri = gVertices + i;

        VGL_MAT4_MUL_VEC4(pos[0], mvp, tri[0].pos);
        VGL_VEC4_DIV_SCALAR(pos[0], pos[0], pos[0].w);

        VGL_MAT4_MUL_VEC4(pos[1], mvp, tri[1].pos);
        VGL_VEC4_DIV_SCALAR(pos[1], pos[1], pos[1].w);

        VGL_MAT4_MUL_VEC4(pos[2], mvp, tri[2].pos);
        VGL_VEC4_DIV_SCALAR(pos[2], pos[2], pos[2].w);

        if (VGL_VEC3_IN_RANGE(pos[0], ndcMin, ndcMax)
            || VGL_VEC3_IN_RANGE(pos[1], ndcMin, ndcMax)
            || VGL_VEC3_IN_RANGE(pos[2], ndcMin, ndcMax)) {

            VGL_MAT4_MUL_VEC4(tri[0].pos, vpMat, pos[0]);
            VGL_MAT4_MUL_VEC4(tri[1].pos, vpMat, pos[1]);
            VGL_MAT4_MUL_VEC4(tri[2].pos, vpMat, pos[2]);
            if (v != tri) {
                memcpy(v, tri, sizeof(vglVertex)*3);
            }
            v += 3;
            verticesCount += 3;
        }
    }
    gVerticesCount = 0;

    vglRSProcess(verticesCount);
}

const vglVertex *vglVPGetVertices() {
    return gVertices;
}

size_t vglVPGetVerticesCount() {
    return gVerticesCount;
}
