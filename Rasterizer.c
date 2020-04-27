#include "Rasterizer.h"
#include "VGLInternal.h"
#include "Common.h"
#include <intrin.h>
#include <stdbool.h>

static vglIntRect gBufferRect = { 0, 0, 0, 0 };
static vglVec2i gBufferSize = { 0, 0 };
static vglColor *gColorBuffer = NULL;
static float *gDepthBuffer = NULL;

static VGL_VECTOR(vglColor) gColorClearData;
static VGL_VECTOR(float) gDepthClearData;
static bool gIsColorClearDataInitialized = false;
static bool gIsDepthClearDataInitialized = false;

void vglRSInitialize() {
    VGL_VECTOR_CREATE(gColorClearData);
    VGL_VECTOR_CREATE(gDepthClearData);
}

void vglRSShutdown() {
    VGL_VECTOR_DESTROY(gDepthClearData);
    VGL_VECTOR_DESTROY(gColorClearData);
}

void vglRSSetFramebuffer(const vglIntRect *rect, vglColor *colorBuffer, float *depthBuffer) {
    gBufferRect = *rect;
    VGL_RECT_GET_SIZE(gBufferSize, *rect);
    VGL_VECTOR_RESIZE(gColorClearData, gBufferSize.x*gBufferSize.y);
    VGL_VECTOR_RESIZE(gDepthClearData, gBufferSize.x*gBufferSize.y);
    gColorBuffer = colorBuffer;
    gDepthBuffer = depthBuffer;
}

const vglIntRect *vglRSGetFramebufferRect() {
    return &gBufferRect;
}

void vglRSClearColor(const vglColor *color) {
    if (gColorClearData.data[0].rgba != color->rgba || !gIsColorClearDataInitialized) {
        gIsColorClearDataInitialized = true;
        vglColor data[128];
        for (int i = 0; i < VGL_ARRAYSIZE(data); i++) {
            data[i] = *color;
        }
        for (size_t i = 0; i < gColorClearData.size; i += VGL_ARRAYSIZE(data)) {
            memcpy(gColorClearData.data + i, data, VGL_MIN(sizeof(data), sizeof(vglColor)*(gColorClearData.size - i)));
        }
    }
    memcpy(gColorBuffer, gColorClearData.data, gColorClearData.size*sizeof(vglColor));
}

void vglRSClearDepth(float depth) {
    if (gDepthClearData.data[0] != depth || !gIsDepthClearDataInitialized) {
        gIsDepthClearDataInitialized = true;
        float data[128];
        for (int i = 0; i < VGL_ARRAYSIZE(data); i++) {
            data[i] = depth;
        }
        for (size_t i = 0; i < gDepthClearData.size; i += VGL_ARRAYSIZE(data)) {
            memcpy(gDepthClearData.data + i, data, VGL_MIN(sizeof(data), sizeof(float)*(gDepthClearData.size - i)));
        }
    }
    memcpy(gDepthBuffer, gDepthClearData.data, gDepthClearData.size*sizeof(float));
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
