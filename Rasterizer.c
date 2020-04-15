#include "Rasterizer.h"
#include "VGLInternal.h"
#include "Common.h"
#include <intrin.h>
#include <stdbool.h>

static vglIntRect gBufferRect = { 0, 0, 0, 0 };
static vglVec2i gBufferSize = { 0, 0 };
static vglColor *gColorBuffer = NULL;
static float *gDepthBuffer = NULL;

static vglColor gColorClearData[VGL_MAX_BUFFER_WIDTH*VGL_MAX_BUFFER_HEIGHT];
static float gDepthClearData[VGL_MAX_BUFFER_WIDTH*VGL_MAX_BUFFER_HEIGHT];
static bool gIsColorClearDataInitialized = false;
static bool gIsDepthClearDataInitialized = false;

void vglRSSetFramebuffer(const vglIntRect *rect, vglColor *colorBuffer, float *depthBuffer) {
    gBufferRect = *rect;
    VGL_RECT_GET_SIZE(gBufferSize, *rect);
    gColorBuffer = colorBuffer;
    gDepthBuffer = depthBuffer;
}

const vglIntRect *vglRSGetFramebufferRect() {
    return &gBufferRect;
}

void vglRSClearColor(const vglColor *color) {
    if (gColorClearData[0].rgba != color->rgba || !gIsColorClearDataInitialized) {
        gIsColorClearDataInitialized = true;
        vglColor data[128];
        for (int i = 0; i < VGL_ARRAYSIZE(data); i++) {
            data[i] = *color;
        }
        for (size_t i = 0; i < VGL_MAX_BUFFER_WIDTH*VGL_MAX_BUFFER_HEIGHT; i += VGL_ARRAYSIZE(data)) {
            memcpy(gColorClearData + i, data, sizeof(data));
        }
    }
    memcpy(gColorBuffer, gColorClearData, gBufferSize.x*gBufferSize.y*sizeof(vglColor));
}

void vglRSClearDepth(float depth) {
    if (gDepthClearData[0] != depth || !gIsDepthClearDataInitialized) {
        gIsDepthClearDataInitialized = true;
        float data[128];
        for (int i = 0; i < VGL_ARRAYSIZE(data); i++) {
            data[i] = depth;
        }
        for (size_t i = 0; i < VGL_MAX_BUFFER_WIDTH*VGL_MAX_BUFFER_HEIGHT; i += VGL_ARRAYSIZE(data)) {
            memcpy(gDepthClearData + i, data, sizeof(data));
        }
    }
    memcpy(gDepthBuffer, gDepthClearData, gBufferSize.x*gBufferSize.y*sizeof(float));
}

#define COMPARE_FUNC(out, func, lhs, rhs) { \
    switch (func) { \
        case GL_NEVER: { \
            (out) = false; \
            break; \
        } \
        case GL_LESS: { \
            (out) = (lhs) < (rhs); \
            break; \
        } \
        case GL_EQUAL: { \
            (out) = VGL_FLT_EQUAL(lhs, rhs); \
            break; \
        } \
        case GL_LEQUAL: { \
            (out) = (lhs) <= (rhs); \
            break; \
        } \
        case GL_GREATER: { \
            (out) = (lhs) > (rhs); \
            break; \
        } \
        case GL_NOTEQUAL: { \
            (out) = VGL_FLT_NOT_EQUAL(lhs, rhs); \
            break; \
        } \
        case GL_GEQUAL: { \
            (out) = (lhs) >= (rhs); \
            break; \
        } \
        case GL_ALWAYS: { \
            (out) = true; \
            break; \
        } \
    } \
}

#define RS_TRI_SHADEMODEL_FLAT 1
#define RS_TRI_SHADEMODEL_SMOOTH 0
#define RS_TRI_TEXTURED 0
#define RS_TRI_NAME FlatColored
#include "RasterizeTriangle.inl"

#define RS_TRI_SHADEMODEL_FLAT 1
#define RS_TRI_SHADEMODEL_SMOOTH 0
#define RS_TRI_TEXTURED 1
#define RS_TRI_NAME FlatTextured
#include "RasterizeTriangle.inl"

#define RS_TRI_SHADEMODEL_FLAT 0
#define RS_TRI_SHADEMODEL_SMOOTH 1
#define RS_TRI_TEXTURED 0
#define RS_TRI_NAME SmoothColored
#include "RasterizeTriangle.inl"

#define RS_TRI_SHADEMODEL_FLAT 0
#define RS_TRI_SHADEMODEL_SMOOTH 1
#define RS_TRI_TEXTURED 1
#define RS_TRI_NAME SmoothTextured
#include "RasterizeTriangle.inl"

void processTriangles(size_t verticesCount) {
    const vglVertex *verts = vglVPGetVertices();

    const vglVec2i vpMin = VGL_VEC2I_CLAMP(gCurrentState->viewport.min, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);
    const vglVec2i vpMax = VGL_VEC2I_CLAMP(gCurrentState->viewport.max, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);

    if (gCurrentState->shadeModel == GL_SMOOTH) {
        if (gCurrentState->isTexture2D) {
            for (size_t i = 0; i < verticesCount; i += 3) {
                const vglVertex *A = verts + i + 0;
                const vglVertex *B = verts + i + 1;
                const vglVertex *C = verts + i + 2;

                drawTriangleHalfPlane_SmoothTextured(&vpMin, &vpMax, A, B, C);
            }
        }
        else {
            for (size_t i = 0; i < verticesCount; i += 3) {
                const vglVertex *A = verts + i + 0;
                const vglVertex *B = verts + i + 1;
                const vglVertex *C = verts + i + 2;

                drawTriangleHalfPlane_SmoothColored(&vpMin, &vpMax, A, B, C);
            }
        }
    }
    else if (gCurrentState->shadeModel == GL_FLAT) {
        if (gCurrentState->isTexture2D) {
            for (size_t i = 0; i < verticesCount; i += 3) {
                const vglVertex *A = verts + i + 0;
                const vglVertex *B = verts + i + 1;
                const vglVertex *C = verts + i + 2;

                drawTriangleHalfPlane_FlatTextured(&vpMin, &vpMax, A, B, C);
            }
        }
        else {
            for (size_t i = 0; i < verticesCount; i += 3) {
                const vglVertex *A = verts + i + 0;
                const vglVertex *B = verts + i + 1;
                const vglVertex *C = verts + i + 2;

                drawTriangleHalfPlane_FlatColored(&vpMin, &vpMax, A, B, C);
            }
        }
    }
}

void vglRSProcess(size_t verticesCount) {
    switch (gCurrentState->primType) {
        case GL_TRIANGLES: {
            processTriangles(verticesCount);
            break;
        }
    }
}
