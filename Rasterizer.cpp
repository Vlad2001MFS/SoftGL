#include "Rasterizer.hpp"
#include "VGLInternal.hpp"
#include <intrin.h>

static IntRect gBufferRect = IntRect(0, 0, 0, 0);
static Color *gColorBuffer = nullptr;
static float *gDepthBuffer = nullptr;

static std::vector<Color> gColorClearData = { Color(0, 0, 0, 0) };
static std::vector<float> gDepthClearData = { 1.0f };

void rsSetFramebuffer(const IntRect &rect, Color *colorBuffer, float *depthBuffer) {
    gBufferRect = rect;
    gColorBuffer = colorBuffer;
    gDepthBuffer = depthBuffer;

    int area = rect.getArea();
    gColorClearData.resize(area, gColorClearData.front());
    gDepthClearData.resize(area, gDepthClearData.front());
}

const IntRect &rsGetFramebufferRect() {
    return gBufferRect;
}

void rsClearColor(const Color &color) {
    if (gColorClearData.front() != color) {
        std::fill(gColorClearData.begin(), gColorClearData.end(), color);
    }
    memcpy(gColorBuffer, gColorClearData.data(), gColorClearData.size()*sizeof(Color));
}

void rsClearDepth(float depth) {
    if (gDepthClearData.front() != depth) {
        std::fill(gDepthClearData.begin(), gDepthClearData.end(), depth);
    }
    memcpy(gDepthBuffer, gDepthClearData.data(), gDepthClearData.size()*sizeof(float));
}

template<typename T>
__forceinline bool __fastcall compareFunc(uint32_t func, T lhs, T rhs) {
    switch (func) {
        case GL_NEVER: {
            return false;
        }
        case GL_LESS: {
            return lhs < rhs;
        }
        case GL_EQUAL: {
            return Math::isEqual(lhs, rhs);
        }
        case GL_LEQUAL: {
            return lhs <= rhs;
        }
        case GL_GREATER: {
            return lhs > rhs;
        }
        case GL_NOTEQUAL: {
            return Math::isNotEqual(lhs, rhs);
        }
        case GL_GEQUAL: {
            return lhs >= rhs;
        }
        case GL_ALWAYS: {
            return true;
        }
    }
}

void drawTriangleBarycentric(const Vec2i &bufferSize, bool isDepthTest, uint32_t depthFunc, const Vec2i &vpMin, const Vec2i &vpMax,
                             const Vertex &A, const Vertex &B, const Vertex &C) {
    const Vec4f colorBCA[] = {
        Vec4f(B.color.r, C.color.r, A.color.r, 0.0f),
        Vec4f(B.color.g, C.color.g, A.color.g, 0.0f),
        Vec4f(B.color.b, C.color.b, A.color.b, 0.0f),
        Vec4f(B.color.a, C.color.a, A.color.a, 0.0f),
    };

    const auto triMin = Vec2i(Vec4f::min(A.pos, B.pos, C.pos));
    const auto triMax = Vec2i(Vec4f::max(A.pos, B.pos, C.pos));
    const auto min = Vec2i::clamp(triMin, vpMin, vpMax);
    const auto max = Vec2i::clamp(triMax, vpMin, vpMax);

    const auto invABCz = Vec4f(1.0f / A.pos.z, 1.0f / B.pos.z, 1.0f / C.pos.z, 0.0f);

    const Vec4f AC = Vec4f(C.pos - A.pos);
    const Vec4f AB = Vec4f(B.pos - A.pos);

    const float bcW = AB.x*AC.y - AC.x*AB.y;
    const float bcInvW = 1.0f / bcW;

    for (int y = min.y; y <= max.y; y++) {
        const int PAy = A.pos.y - y;

        for (int x = min.x; x <= max.x; x++) {
            const int PAx = A.pos.x - x;

            const float bcU = AC.x*PAy - PAx*AC.y;
            const float bcV = PAx*AB.y - AB.x*PAy;

            auto bcScreen = Vec4f(bcU, bcV, 0.0f, 0.0f);
            bcScreen *= bcInvW;
            bcScreen.z = 1.0f - bcScreen.x - bcScreen.y;

            if (bcScreen.x >= 0 && bcScreen.y >= 0 && bcScreen.z >= 0) {
                Vec4f bcClip = bcScreen*invABCz;
                bcClip /= bcClip.x + bcClip.y + bcClip.z; // perspective-correction

                const uint32_t idx = x + y*bufferSize.x;
                if (isDepthTest) {
                    const float depth = bcClip.x*B.pos.z + bcClip.y*C.pos.z + bcClip.z*A.pos.z;
                    if (!compareFunc(depthFunc, depth, gDepthBuffer[idx])) {
                        continue;
                    }
                    gDepthBuffer[idx] = depth;
                }

                gColorBuffer[idx][0] = bcScreen.dot(colorBCA[0]);
                gColorBuffer[idx][1] = bcScreen.dot(colorBCA[1]);
                gColorBuffer[idx][2] = bcScreen.dot(colorBCA[2]);
                gColorBuffer[idx][3] = bcScreen.dot(colorBCA[3]);
            }
        }
    }
}

void drawTriangleBarycentricSIMD(const Vec2i &bufferSize, bool isDepthTest, uint32_t depthFunc, const Vec2i &vpMin, const Vec2i &vpMax,
                                 const Vertex &A, const Vertex &B, const Vertex &C) {
    __m128 BCAr_SIMD = _mm_setr_ps(B.color.r, C.color.r, A.color.r, 0.0f);
    __m128 BCAg_SIMD = _mm_setr_ps(B.color.g, C.color.g, A.color.g, 0.0f);
    __m128 BCAb_SIMD = _mm_setr_ps(B.color.b, C.color.b, A.color.b, 0.0f);
    __m128 BCAa_SIMD = _mm_setr_ps(B.color.a, C.color.a, A.color.a, 0.0f);

    const auto triMin = Vec2i(Vec4f::min(A.pos, B.pos, C.pos));
    const auto triMax = Vec2i(Vec4f::max(A.pos, B.pos, C.pos));
    const auto min = Vec2i::clamp(triMin, vpMin, vpMax);
    const auto max = Vec2i::clamp(triMax, vpMin, vpMax);

    __m128 invABCz_SIMD = _mm_rcp_ps(_mm_setr_ps(A.pos.z, B.pos.z, C.pos.z, 1.0f));
    __m128 BCAz_SIMD = _mm_setr_ps(B.pos.z, C.pos.z, A.pos.z, 0.0f);

    const Vec4f AC = C.pos - A.pos;
    const Vec4f AB = B.pos - A.pos;

    __m128 bcInvW_SIMD = _mm_rcp_ps(_mm_set_ps1(AB.x*AC.y - AC.x*AB.y));

    for (int y = min.y; y <= max.y; y++) {
        const int PAy = A.pos.y - y;

        for (int x = min.x; x <= max.x; x++) {
            const int PAx = A.pos.x - x;

            const float bcU = AC.x*PAy - PAx*AC.y;
            const float bcV = PAx*AB.y - AB.x*PAy;

            __m128 bcScreen_SIMD = _mm_mul_ps(_mm_setr_ps(bcU, bcV, 0.0f, 0.0f), bcInvW_SIMD);
            Vec4f bcScreen;
            _mm_store_ps(bcScreen.data, bcScreen_SIMD);
            bcScreen.z = 1.0f - bcScreen.x - bcScreen.y;

            if (bcScreen.x >= 0 && bcScreen.y >= 0 && bcScreen.z >= 0) {
                bcScreen_SIMD = _mm_load_ps(bcScreen.data);
                __m128 bcClip_SIMD = _mm_mul_ps(bcScreen_SIMD, invABCz_SIMD);
                Vec4f bcClip;
                _mm_store_ps(bcClip.data, bcClip_SIMD);
                bcClip_SIMD = _mm_div_ps(bcClip_SIMD, _mm_set_ps1(bcClip.x + bcClip.y + bcClip.z));
                _mm_store_ps(bcClip.data, bcClip_SIMD);

                const uint32_t idx = x + y*bufferSize.x;
                if (isDepthTest) {
                    __m128 d = _mm_mul_ps(bcClip_SIMD, BCAz_SIMD);
                    Vec4f dd;
                    _mm_store_ps(dd.data, d);
                    const float depth = dd.x + dd.y + dd.z;
                    if (!compareFunc(depthFunc, depth, gDepthBuffer[idx])) {
                        continue;
                    }
                    gDepthBuffer[idx] = depth;
                }

                __m128 r = _mm_dp_ps(bcScreen_SIMD, BCAr_SIMD, 0x71);
                __m128 g = _mm_dp_ps(bcScreen_SIMD, BCAg_SIMD, 0x72);
                __m128 b = _mm_dp_ps(bcScreen_SIMD, BCAb_SIMD, 0x74);
                __m128 a = _mm_dp_ps(bcScreen_SIMD, BCAa_SIMD, 0x78);

                __m128 rgba = _mm_add_ps(r, g);
                rgba = _mm_add_ps(rgba, b);
                rgba = _mm_add_ps(rgba, a);

                __m128i y = _mm_cvtps_epi32(rgba); // Convert them to 32-bit ints
                y = _mm_packus_epi32(y, y); // Pack down to 16 bits
                y = _mm_packus_epi16(y, y); // Pack down to 8 bits
                gColorBuffer[idx].rgba = _mm_cvtsi128_si32(y);
            }
        }
    }
}

void processTriangles() {
    const std::vector<Vertex> &verts = vpGetVertices();
    size_t vertsCount = verts.size();

    const Vec2 bufferSize = gCurrentContext->bufferRect.getSize();
    const bool isDepthTest = gCurrentState->caps & GL_DEPTH_TEST;
    const uint32_t depthFunc = gCurrentState->depthFunc;

    const auto vpMin = Vec2i::clamp(gCurrentState->viewport.min, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);
    const auto vpMax = Vec2i::clamp(gCurrentState->viewport.max, gCurrentContext->bufferRect.min, gCurrentContext->bufferRect.max);

    for (size_t i = 0; i < vertsCount; i += 3) {
        const Vertex &A = verts[i + 0];
        const Vertex &B = verts[i + 1];
        const Vertex &C = verts[i + 2];

        drawTriangleBarycentricSIMD(bufferSize, isDepthTest, depthFunc, vpMin, vpMax, A, B, C);
    }
}

void rsProcess() {
    switch (gCurrentState->primType) {
        case GL_TRIANGLES: {
            processTriangles();
            break;
        }
    }
}
