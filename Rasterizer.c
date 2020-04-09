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

void drawTriangleBarycentricSmooth(const vglVec2i *vpMin, const vglVec2i *vpMax, const vglVertex *A, const vglVertex *B, const vglVertex *C) {
    vglVec2f AC, AB;
    VGL_VEC2_SUB(AC, C->pos, A->pos);
    VGL_VEC2_SUB(AB, B->pos, A->pos);

    const float bcW = AC.x*AB.y - AB.x*AC.y;
    if (bcW >= 1.0f) { // back-face culling
        const float bcInvW = 1.0f / bcW;

        const vglVec3f colorBCA[] = {
            VGL_VEC3F(B->color.r, C->color.r, A->color.r),
            VGL_VEC3F(B->color.g, C->color.g, A->color.g),
            VGL_VEC3F(B->color.b, C->color.b, A->color.b),
            VGL_VEC3F(B->color.a, C->color.a, A->color.a),
        };

        const vglVec2i triMin = VGL_VEC2I_MIN3(A->pos, B->pos, C->pos);
        const vglVec2i triMax = VGL_VEC2I_MAX3(A->pos, B->pos, C->pos);
        const vglVec2i min = VGL_VEC2I_CLAMP(triMin, *vpMin, *vpMax);
        const vglVec2i max = VGL_VEC2I_CLAMP(triMax, *vpMin, *vpMax);

        const vglVec3f invABCz = { 1.0f / B->pos.z, 1.0f / C->pos.z, 1.0f / A->pos.z };

        const bool isDepthTest = gCurrentState->caps & GL_DEPTH_TEST;
        const uint32_t depthFunc = gCurrentState->depthFunc;

        vglVec3f bcScreen = { 0, 0, 0 };
        vglVec3f bcClip = { 0, 0, 0 };

        vglVec2f PA;
        for (int y = min.y; y <= max.y; ++y) {
            PA.y = A->pos.y - y;

            const uint32_t idx = y*gBufferSize.x + min.x;
            uint8_t *colorBuffer = gColorBuffer + idx;
            float *depthBuffer = gDepthBuffer + idx;

            for (int x = min.x; x <= max.x; ++x, colorBuffer += 4, ++depthBuffer) {
                PA.x = A->pos.x - x;

                bcScreen.x = (PA.x*AC.y - AC.x*PA.y)*bcInvW;
                if (bcScreen.x >= 0.0f) {

                    bcScreen.y = (AB.x*PA.y - PA.x*AB.y)*bcInvW;
                    if (bcScreen.y >= 0.0f) {

                        bcScreen.z = 1.0f - bcScreen.x - bcScreen.y;
                        if (bcScreen.z >= 0.0f) {
                            VGL_VEC3_MUL(bcClip, bcScreen, invABCz);
                            VGL_VEC3_DIV_SCALAR(bcClip, bcClip, bcClip.x + bcClip.y + bcClip.z); // perspective-correction

                            if (isDepthTest) {
                                const float depth = bcClip.x*B->pos.z + bcClip.y*C->pos.z + bcClip.z*A->pos.z;

                                bool depthTestResult = true;
                                COMPARE_FUNC(depthTestResult, depthFunc, depth, *depthBuffer);
                                if (!depthTestResult) {
                                    continue;
                                }
                                *depthBuffer = depth;
                            }

                            colorBuffer[0] = (uint8_t)VGL_VEC3_DOT(bcScreen, colorBCA[0]);
                            colorBuffer[1] = (uint8_t)VGL_VEC3_DOT(bcScreen, colorBCA[1]);
                            colorBuffer[2] = (uint8_t)VGL_VEC3_DOT(bcScreen, colorBCA[2]);
                            colorBuffer[3] = (uint8_t)VGL_VEC3_DOT(bcScreen, colorBCA[3]);
                        }
                    }
                }
            }
        }
    }
}

void drawTriangleBarycentricFlat(const vglVec2i *vpMin, const vglVec2i *vpMax, const vglVertex *A, const vglVertex *B, const vglVertex *C) {
    vglVec2f AC, AB;
    VGL_VEC2_SUB(AC, C->pos, A->pos);
    VGL_VEC2_SUB(AB, B->pos, A->pos);

    const float bcW = AC.x*AB.y - AB.x*AC.y;
    if (bcW >= 1.0f) { // back-face culling
        const float bcInvW = 1.0f / bcW;

        const vglVec2i triMin = VGL_VEC2I_MIN3(A->pos, B->pos, C->pos);
        const vglVec2i triMax = VGL_VEC2I_MAX3(A->pos, B->pos, C->pos);
        const vglVec2i min = VGL_VEC2I_CLAMP(triMin, *vpMin, *vpMax);
        const vglVec2i max = VGL_VEC2I_CLAMP(triMax, *vpMin, *vpMax);

        const vglVec3f invABCz = { 1.0f / B->pos.z, 1.0f / C->pos.z, 1.0f / A->pos.z };

        const bool isDepthTest = gCurrentState->caps & GL_DEPTH_TEST;
        const uint32_t depthFunc = gCurrentState->depthFunc;

        vglVec3f bcScreen = { 0, 0, 0 };
        vglVec3f bcClip = { 0, 0, 0 };

        vglVec2f PA;
        for (int y = min.y; y <= max.y; ++y) {
            PA.y = A->pos.y - y;

            const uint32_t idx = y*gBufferSize.x + min.x;
            uint32_t *colorBuffer = gColorBuffer + idx;
            float *depthBuffer = gDepthBuffer + idx;

            for (int x = min.x; x <= max.x; ++x, colorBuffer++, ++depthBuffer) {
                PA.x = A->pos.x - x;

                bcScreen.x = (PA.x*AC.y - AC.x*PA.y)*bcInvW;
                if (bcScreen.x >= 0.0f) {

                    bcScreen.y = (AB.x*PA.y - PA.x*AB.y)*bcInvW;
                    if (bcScreen.y >= 0.0f) {

                        bcScreen.z = 1.0f - bcScreen.x - bcScreen.y;
                        if (bcScreen.z >= 0.0f) {
                            VGL_VEC3_MUL(bcClip, bcScreen, invABCz);
                            VGL_VEC3_DIV_SCALAR(bcClip, bcClip, bcClip.x + bcClip.y + bcClip.z); // perspective-correction

                            if (isDepthTest) {
                                const float depth = bcClip.x*B->pos.z + bcClip.y*C->pos.z + bcClip.z*A->pos.z;

                                bool depthTestResult = true;
                                COMPARE_FUNC(depthTestResult, depthFunc, depth, *depthBuffer);
                                if (!depthTestResult) {
                                    continue;
                                }
                                *depthBuffer = depth;
                            }

                            *colorBuffer = A->color.rgba;
                        }
                    }
                }
            }
        }
    }
}

void processTriangles(size_t verticesCount) {
    const vglVertex *verts = vglVPGetVertices();

    const vglVec2i vpMin = VGL_VEC2I_CLAMP(gCurrentState->viewport.min, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);
    const vglVec2i vpMax = VGL_VEC2I_CLAMP(gCurrentState->viewport.max, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);

    if (gCurrentState->shadeModel == GL_SMOOTH) {
        for (size_t i = 0; i < verticesCount; i += 3) {
            const vglVertex *A = verts + i + 0;
            const vglVertex *B = verts + i + 1;
            const vglVertex *C = verts + i + 2;

            drawTriangleBarycentricSmooth(&vpMin, &vpMax, A, B, C);
        }
    }
    else if (gCurrentState->shadeModel == GL_FLAT) {
        for (size_t i = 0; i < verticesCount; i += 3) {
            const vglVertex *A = verts + i + 0;
            const vglVertex *B = verts + i + 1;
            const vglVertex *C = verts + i + 2;

            drawTriangleBarycentricFlat(&vpMin, &vpMax, A, B, C);
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
