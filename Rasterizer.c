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

void drawTriangleBarycentric(const vglVec2i *vpMin, const vglVec2i *vpMax, const vglVertex *A, const vglVertex *B, const vglVertex *C) {
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

void drawTriangleBarycentricSIMD(const vglVec2i *vpMin, const vglVec2i *vpMax, const vglVertex *A, const vglVertex *B, const vglVertex *C) {
    __m128 BCAr_SIMD = _mm_setr_ps(B->color.r, C->color.r, A->color.r, 0.0f);
    __m128 BCAg_SIMD = _mm_setr_ps(B->color.g, C->color.g, A->color.g, 0.0f);
    __m128 BCAb_SIMD = _mm_setr_ps(B->color.b, C->color.b, A->color.b, 0.0f);
    __m128 BCAa_SIMD = _mm_setr_ps(B->color.a, C->color.a, A->color.a, 0.0f);

    const vglVec2i triMin = VGL_VEC2I_MIN3(A->pos, B->pos, C->pos);
    const vglVec2i triMax = VGL_VEC2I_MAX3(A->pos, B->pos, C->pos);
    const vglVec2i min = VGL_VEC2I_CLAMP(triMin, *vpMin, *vpMax);
    const vglVec2i max = VGL_VEC2I_CLAMP(triMax, *vpMin, *vpMax);

    __m128 invABCz_SIMD = _mm_rcp_ps(_mm_setr_ps(A->pos.z, B->pos.z, C->pos.z, 1.0f));
    __m128 BCAz_SIMD = _mm_setr_ps(B->pos.z, C->pos.z, A->pos.z, 0.0f);

    const bool isDepthTest = gCurrentState->caps & GL_DEPTH_TEST;
    const uint32_t depthFunc = gCurrentState->depthFunc;

    vglVec4f AC, AB;
    VGL_VEC4_SUB(AC, C->pos, A->pos);
    VGL_VEC4_SUB(AB, B->pos, A->pos);

    __m128 bcInvW_SIMD = _mm_rcp_ps(_mm_set_ps1(AB.x*AC.y - AC.x*AB.y));

    for (int y = min.y; y <= max.y; y++) {
        const int PAy = A->pos.y - y;

        for (int x = min.x; x <= max.x; x++) {
            const int PAx = A->pos.x - x;

            const float bcU = AC.x*PAy - PAx*AC.y;
            const float bcV = PAx*AB.y - AB.x*PAy;

            __m128 bcScreen_SIMD = _mm_mul_ps(_mm_setr_ps(bcU, bcV, 0.0f, 0.0f), bcInvW_SIMD);
            vglVec4f bcScreen;
            _mm_store_ps(bcScreen.data, bcScreen_SIMD);
            bcScreen.z = 1.0f - bcScreen.x - bcScreen.y;

            if (bcScreen.x >= 0 && bcScreen.y >= 0 && bcScreen.z >= 0) {
                bcScreen_SIMD = _mm_load_ps(bcScreen.data);

                __m128 bcClip_SIMD = _mm_mul_ps(bcScreen_SIMD, invABCz_SIMD);
                vglVec4f bcClip;
                _mm_store_ps(bcClip.data, bcClip_SIMD);
                bcClip_SIMD = _mm_div_ps(bcClip_SIMD, _mm_set_ps1(bcClip.x + bcClip.y + bcClip.z));
                _mm_store_ps(bcClip.data, bcClip_SIMD);

                const uint32_t idx = x + y*gBufferSize.x;
                if (isDepthTest) {
                    __m128 d = _mm_mul_ps(bcClip_SIMD, BCAz_SIMD);
                    vglVec4f dd;
                    _mm_store_ps(dd.data, d);
                    const float depth = dd.x + dd.y + dd.z;
                    bool depthTestResult = true;
                    COMPARE_FUNC(depthTestResult, depthFunc, depth, gDepthBuffer[idx]);
                    if (!depthTestResult) {
                        continue;
                    }
                    gDepthBuffer[idx] = depth;
                }

                __m128 rgba = _mm_dp_ps(bcScreen_SIMD, BCAr_SIMD, 0x71);
                rgba = _mm_add_ps(rgba, _mm_dp_ps(bcScreen_SIMD, BCAg_SIMD, 0x72));
                rgba = _mm_add_ps(rgba, _mm_dp_ps(bcScreen_SIMD, BCAb_SIMD, 0x74));
                rgba = _mm_add_ps(rgba, _mm_dp_ps(bcScreen_SIMD, BCAa_SIMD, 0x78));

                __m128i y = _mm_cvtps_epi32(rgba);
                y = _mm_packus_epi32(y, y);
                y = _mm_packus_epi16(y, y);
                gColorBuffer[idx].rgba = _mm_cvtsi128_si32(y);
            }
        }
    }
}

void processTriangles() {
    const vglVertex *verts = vglVPGetVertices();
    size_t vertsCount = vglVPGetVerticesCount();

    const vglVec2i vpMin = VGL_VEC2I_CLAMP(gCurrentState->viewport.min, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);
    const vglVec2i vpMax = VGL_VEC2I_CLAMP(gCurrentState->viewport.max, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);

    for (size_t i = 0; i < vertsCount; i += 3) {
        const vglVertex *A = verts + i + 0;
        const vglVertex *B = verts + i + 1;
        const vglVertex *C = verts + i + 2;

        drawTriangleBarycentric(&vpMin, &vpMax, A, B, C);
    }
}

void vglRSProcess() {
    switch (gCurrentState->primType) {
        case GL_TRIANGLES: {
            processTriangles();
            break;
        }
    }
}
