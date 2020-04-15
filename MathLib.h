#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <memory.h>
#include <float.h>
#include <intrin.h>

// ##################################################################################
// ### Math library
// ##################################################################################

#define VGL_MIN(a, b) ((a) < (b) ? (a) : (b))
#define VGL_MIN3(a, b, c) VGL_MIN(a, VGL_MIN(b, c))
#define VGL_MAX(a, b) ((a) > (b) ? (a) : (b))
#define VGL_MAX3(a, b, c) VGL_MAX(a, VGL_MAX(b, c))
#define VGL_CLAMP(value, minValue, maxValue) (VGL_MAX(minValue, VGL_MIN(value, maxValue)))
#define VGL_RAD(deg) ((deg)*((float)M_PI) / 180)
#define VGL_DEG(rad) ((rad)*180 / ((float)M_PI))
#define VGL_FLT_EQUAL(a, b) (fabsf((a) - (b)) < FLT_EPSILON)
#define VGL_FLT_NOT_EQUAL(a, b) (fabsf((a) - (b)) > FLT_EPSILON)
#define VGL_IN_RANGE(value, min, max) ((value) >= (min) && (value) <= (max))

// Returns the area of triangle multiplied by 2
#define VGL_EDGE_FUNCTION(p, v0, v1) (((p).x - (v0).x)*((v1).y - (v0).y) - ((p).y - (v0).y)*((v1).x - (v0).x))

// ##################################################################################
// ### Color
// ##################################################################################

typedef struct vglColor {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint8_t data[4];
        uint32_t rgba;
    };
} vglColor;

#define VGL_COLOR(r, g, b, a) ((vglColor){ r, g, b, a })
#define VGL_COLOR_SET(out, nr, ng, nb, na) { (out).r = nr; (out).g = ng; (out).b = nb; (out).a = na; }
#define VGL_COLOR_SET_FLOAT4(out, nr, ng, nb, na) VGL_COLOR_SET(out, (nr)*255, (ng)*255, (nb)*255, (na)*255)
#define VGL_COLOR_EQUAL(lhs, rhs) ((lhs).rgba == (rhs).rgba)

// ##################################################################################
// ### Vec2
// ##################################################################################

typedef struct vglVec2i {
    union {
        struct {
            int x, y;
        };
        int data[2];
    };
} vglVec2i;

typedef __declspec(align(16)) struct vglAlignedVec2i {
    union {
        struct {
            int x, y;
        };
        int data[2];
    };
} vglAlignedVec2i;

typedef struct vglVec2f {
    union {
        struct {
            float x, y;
        };
        float data[2];
    };
} vglVec2f;

typedef __declspec(align(16)) struct vglAlignedVec2f {
    union {
        struct {
            float x, y;
        };
        float data[2];
    };
} vglAlignedVec2f;

#define VGL_VEC2I(x, y) ((vglVec2i){ x, y })
#define VGL_VEC2F(x, y) ((vglVec2f){ x, y })
#define VGL_ALIGNED_VEC2I(x, y) ((vglAlignedVec2i){ x, y })
#define VGL_ALIGNED_VEC2F(x, y) ((vglAlignedVec2f){ x, y })

#define VGL_VEC2I_MIN(a, b) ((vglVec2i){ VGL_MIN((a).x, (b).x), VGL_MIN((a).y, (b).y) })
#define VGL_VEC2F_MIN(a, b) ((vglVec2f){ VGL_MIN((a).x, (b).x), VGL_MIN((a).y, (b).y) })
#define VGL_VEC2I_MIN3(a, b, c) VGL_VEC2I_MIN(a, VGL_VEC2I_MIN(b, c))
#define VGL_VEC2F_MIN3(a, b, c) VGL_VEC2F_MIN(a, VGL_VEC2F_MIN(b, c))

#define VGL_VEC2I_MAX(a, b) ((vglVec2i){ VGL_MAX((a).x, (b).x), VGL_MAX((a).y, (b).y) })
#define VGL_VEC2F_MAX(a, b) ((vglVec2f){ VGL_MAX((a).x, (b).x), VGL_MAX((a).y, (b).y) })
#define VGL_VEC2I_MAX3(a, b, c) VGL_VEC2I_MAX(a, VGL_VEC2I_MAX(b, c))
#define VGL_VEC2F_MAX3(a, b, c) VGL_VEC2F_MAX(a, VGL_VEC2F_MAX(b, c))

#define VGL_VEC2I_CLAMP(value, minValue, maxValue) VGL_VEC2I_MAX(minValue, VGL_VEC2I_MIN(value, maxValue))
#define VGL_VEC2F_CLAMP(value, minValue, maxValue) VGL_VEC2F_MAX(minValue, VGL_VEC2F_MIN(value, maxValue))

#define VGL_VEC2_IN_RANGE(value, min, max) (VGL_IN_RANGE((value).x, (min).x, (max).x) && VGL_IN_RANGE((value).y, (min).y, (max).y))

#define VGL_VEC2_DOT(a, b) ((a).x*(b).x + (a).y*(b).y)
#define VGL_VEC2_LEN_SQ(a) VGL_VEC2_DOT(a, a)
#define VGL_VEC2_LEN(a) sqrtf(VGL_VEC2_LEN_SQ(a))

#define VGL_VEC2_SET(out, nx, ny) { (out).x = nx; (out).y = ny; }
#define VGL_VEC2_NEGATIVE(out, v) { (out).x = -(v).x; (out).y = -(v).y; }
#define VGL_VEC2_NORMALIZE(out, v) { float invLen = 1.0f / VGL_VEC2_LEN(v); (out).x = (v).x*invLen; (out).y = (v).y*invLen; }
#define VGL_VEC2_REFLECT(out, i, n) { (out) = (n); VGL_VEC2_MUL_SCALAR(out, out, -2*VGL_VEC2_DOT(n, i)); VGL_VEC2_ADD(out, out, i); }

#define VGL_VEC2_ADD(out, a, b) { (out).x = (a).x + (b).x; (out).y = (a).y + (b).y; }
#define VGL_VEC2_SUB(out, a, b) { (out).x = (a).x - (b).x; (out).y = (a).y - (b).y; }
#define VGL_VEC2_MUL(out, a, b) { (out).x = (a).x * (b).x; (out).y = (a).y * (b).y; }
#define VGL_VEC2_DIV(out, a, b) { (out).x = (a).x / (b).x; (out).y = (a).y / (b).y; }
#define VGL_VEC2_ADD_SCALAR(out, a, b) { (out).x = (a).x + (b); (out).y = (a).y + (b); }
#define VGL_VEC2_SUB_SCALAR(out, a, b) { (out).x = (a).x - (b); (out).y = (a).y - (b); }
#define VGL_VEC2_MUL_SCALAR(out, a, b) { (out).x = (a).x * (b); (out).y = (a).y * (b); }
#define VGL_VEC2_DIV_SCALAR(out, a, b) { float inv = 1.0f / (b); VGL_VEC2_MUL_SCALAR(out, a, inv); }

// ##################################################################################
// ### Vec3
// ##################################################################################

typedef struct vglVec3i {
    union {
        struct {
            int x, y, z;
        };
        int data[3];
    };
} vglVec3i;

typedef __declspec(align(16)) struct vglAlignedVec3i {
    union {
        struct {
            int x, y, z;
        };
        int data[3];
    };
} vglAlignedVec3i;

typedef struct vglVec3f {
    union {
        struct {
            float x, y, z;
        };
        float data[3];
    };
} vglVec3f;

typedef __declspec(align(16)) struct vglAlignedVec3f {
    union {
        struct {
            float x, y, z;
        };
        float data[3];
    };
} vglAlignedVec3f;

#define VGL_VEC3I(x, y, z) ((vglVec3i){ x, y, z })
#define VGL_VEC3F(x, y, z) ((vglVec3f){ x, y, z })
#define VGL_ALIGNED_VEC3I(x, y, z) ((vglAlignedVec3i){ x, y, z })
#define VGL_ALIGNED_VEC3F(x, y, z) ((vglAlignedVec3f){ x, y, z })

#define VGL_VEC3I_MIN(a, b) ((vglVec3i){ VGL_MIN((a).x, (b).x), VGL_MIN((a).y, (b).y), VGL_MIN((a).z, (b).z) })
#define VGL_VEC3F_MIN(a, b) ((vglVec3f){ VGL_MIN((a).x, (b).x), VGL_MIN((a).y, (b).y), VGL_MIN((a).z, (b).z) })
#define VGL_VEC3I_MIN3(a, b, c) VGL_VEC3I_MIN(a, VGL_VEC3I_MIN(b, c))
#define VGL_VEC3F_MIN3(a, b, c) VGL_VEC3F_MIN(a, VGL_VEC3F_MIN(b, c))

#define VGL_VEC3I_MAX(a, b) ((vglVec3i){ VGL_MAX((a).x, (b).x), VGL_MAX((a).y, (b).y), VGL_MAX((a).z, (b).z) })
#define VGL_VEC3F_MAX(a, b) ((vglVec3f){ VGL_MAX((a).x, (b).x), VGL_MAX((a).y, (b).y), VGL_MAX((a).z, (b).z) })
#define VGL_VEC3I_MAX3(a, b, c) VGL_VEC3I_MAX(a, VGL_VEC3I_MAX(b, c))
#define VGL_VEC3F_MAX3(a, b, c) VGL_VEC3F_MAX(a, VGL_VEC3F_MAX(b, c))

#define VGL_VEC3I_CLAMP(value, minValue, maxValue) VGL_VEC3I_MAX(minValue, VGL_VEC3I_MIN(value, maxValue))
#define VGL_VEC3F_CLAMP(value, minValue, maxValue) VGL_VEC3F_MAX(minValue, VGL_VEC3F_MIN(value, maxValue))

#define VGL_VEC3_IN_RANGE(value, min, max) (VGL_IN_RANGE((value).x, (min).x, (max).x) && VGL_IN_RANGE((value).y, (min).y, (max).y) && VGL_IN_RANGE((value).z, (min).z, (max).z))

#define VGL_VEC3_DOT(a, b) ((a).x*(b).x + (a).y*(b).y + (a).z*(b).z)
#define VGL_VEC3_LEN_SQ(a) VGL_VEC3_DOT(a, a)
#define VGL_VEC3_LEN(a) sqrtf(VGL_VEC3_LEN_SQ(a))

#define VGL_VEC3_SET(out, nx, ny, nz) { (out).x = nx; (out).y = ny; (out).z = nz; }
#define VGL_VEC3_NEGATIVE(out, v) { (out).x = -(v).x; (out).y = -(v).y; (out).z = -(v).z; }
#define VGL_VEC3_NORMALIZE(out, v) { float invLen = 1.0f / VGL_VEC3_LEN(v); (out).x = (v).x*invLen; (out).y = (v).y*invLen; (out).z = (v).z*invLen; }
#define VGL_VEC3_REFLECT(out, i, n) { (out) = (n); VGL_VEC3_MUL_SCALAR(out, out, -2*VGL_VEC3_DOT(n, i)); VGL_VEC3_ADD(out, out, i); }

#define VGL_VEC3_ADD(out, a, b) { (out).x = (a).x + (b).x; (out).y = (a).y + (b).y; (out).z = (a).z + (b).z; }
#define VGL_VEC3_SUB(out, a, b) { (out).x = (a).x - (b).x; (out).y = (a).y - (b).y; (out).z = (a).z - (b).z; }
#define VGL_VEC3_MUL(out, a, b) { (out).x = (a).x * (b).x; (out).y = (a).y * (b).y; (out).z = (a).z * (b).z; }
#define VGL_VEC3_DIV(out, a, b) { (out).x = (a).x / (b).x; (out).y = (a).y / (b).y; (out).z = (a).z / (b).z; }
#define VGL_VEC3_ADD_SCALAR(out, a, b) { (out).x = (a).x + (b); (out).y = (a).y + (b); (out).z = (a).z + (b); }
#define VGL_VEC3_SUB_SCALAR(out, a, b) { (out).x = (a).x - (b); (out).y = (a).y - (b); (out).z = (a).z - (b); }
#define VGL_VEC3_MUL_SCALAR(out, a, b) { (out).x = (a).x * (b); (out).y = (a).y * (b); (out).z = (a).z * (b); }
#define VGL_VEC3_DIV_SCALAR(out, a, b) { float inv = 1.0f / (b); VGL_VEC3_MUL_SCALAR(out, a, inv); }

// ##################################################################################
// ### Vec4
// ##################################################################################

typedef struct __declspec(align(16)) vglVec4i {
    union {
        struct {
            int x, y, z, w;
        };
        int data[4];
    };
} vglVec4i;

typedef struct __declspec(align(16)) vglVec4f {
    union {
        struct {
            float x, y, z, w;
        };
        float data[4];
    };
} vglVec4f;

#define VGL_VEC4I(x, y, z, w) ((vglVec4i){ x, y, z, w })
#define VGL_VEC4F(x, y, z, w) ((vglVec4f){ x, y, z, w })

#define VGL_VEC4I_MIN(a, b) ((vglVec4i){ VGL_MIN((a).x, (b).x), VGL_MIN((a).y, (b).y), VGL_MIN((a).z, (b).z), VGL_MIN((a).w, (b).w) })
#define VGL_VEC4F_MIN(a, b) ((vglVec4f){ VGL_MIN((a).x, (b).x), VGL_MIN((a).y, (b).y), VGL_MIN((a).z, (b).z), VGL_MIN((a).w, (b).w) })
#define VGL_VEC4I_MIN3(a, b, c) VGL_VEC4I_MIN(a, VGL_VEC4I_MIN(b, c))
#define VGL_VEC4F_MIN3(a, b, c) VGL_VEC4F_MIN(a, VGL_VEC4F_MIN(b, c))

#define VGL_VEC4I_MAX(a, b) ((vglVec4i){ VGL_MAX((a).x, (b).x), VGL_MAX((a).y, (b).y), VGL_MAX((a).z, (b).z), VGL_MAX((a).w, (b).w) })
#define VGL_VEC4F_MAX(a, b) ((vglVec4f){ VGL_MAX((a).x, (b).x), VGL_MAX((a).y, (b).y), VGL_MAX((a).z, (b).z), VGL_MAX((a).w, (b).w) })
#define VGL_VEC4I_MAX3(a, b, c) VGL_VEC4I_MAX(a, VGL_VEC4I_MAX(b, c))
#define VGL_VEC4F_MAX3(a, b, c) VGL_VEC4F_MAX(a, VGL_VEC4F_MAX(b, c))

#define VGL_VEC4I_CLAMP(value, minValue, maxValue) VGL_VEC4I_MAX(minValue, VGL_VEC4I_MIN(value, maxValue))
#define VGL_VEC4F_CLAMP(value, minValue, maxValue) VGL_VEC4F_MAX(minValue, VGL_VEC4F_MIN(value, maxValue))

#define VGL_VEC4_IN_RANGE(value, min, max) (VGL_IN_RANGE((value).x, (min).x, (max).x) && VGL_IN_RANGE((value).y, (min).y, (max).y) && VGL_IN_RANGE((value).z, (min).z, (max).z) && VGL_IN_RANGE((value).w, (min).w, (max).w))

#define VGL_VEC4_DOT(a, b) ((a).x*(b).x + (a).y*(b).y + (a).z*(b).z + (a).w*(b).w)
#define VGL_VEC4_LEN_SQ(a) VGL_VEC4_DOT(a, a)
#define VGL_VEC4_LEN(a) sqrtf(VGL_VEC4_LEN_SQ(a))

#define VGL_VEC4_SET(out, nx, ny, nz, nw) { (out).x = nx; (out).y = ny; (out).z = nz; (out).w = nw; }
#define VGL_VEC4_NEGATIVE(out, v) { (out).x = -(v).x; (out).y = -(v).y; (out).z = -(v).z; (out).w = -(v).w; }
#define VGL_VEC4_NORMALIZE(out, v) { float invLen = 1.0f / VGL_VEC4_LEN(v); (out).x = (v).x*invLen; (out).y = (v).y*invLen; (out).z = (v).z*invLen; (out).w = (v).w*invLen; }
#define VGL_VEC4_REFLECT(out, i, n) { (out) = (n); VGL_VEC4_MUL_SCALAR(out, out, -2*VGL_VEC4_DOT(n, i)); VGL_VEC4_ADD(out, out, i); }

#define VGL_VEC4_ADD(out, a, b) { (out).x = (a).x + (b).x; (out).y = (a).y + (b).y; (out).z = (a).z + (b).z; (out).w = (a).w + (b).w; }
#define VGL_VEC4_SUB(out, a, b) { (out).x = (a).x - (b).x; (out).y = (a).y - (b).y; (out).z = (a).z - (b).z; (out).w = (a).w - (b).w; }
#define VGL_VEC4_MUL(out, a, b) { (out).x = (a).x * (b).x; (out).y = (a).y * (b).y; (out).z = (a).z * (b).z; (out).w = (a).w * (b).w; }
#define VGL_VEC4_DIV(out, a, b) { (out).x = (a).x / (b).x; (out).y = (a).y / (b).y; (out).z = (a).z / (b).z; (out).w = (a).w / (b).w; }
#define VGL_VEC4_ADD_SCALAR(out, a, b) { (out).x = (a).x + (b); (out).y = (a).y + (b); (out).z = (a).z + (b); (out).w = (a).w + (b); }
#define VGL_VEC4_SUB_SCALAR(out, a, b) { (out).x = (a).x - (b); (out).y = (a).y - (b); (out).z = (a).z - (b); (out).w = (a).w - (b); }
#define VGL_VEC4_MUL_SCALAR(out, a, b) { (out).x = (a).x * (b); (out).y = (a).y * (b); (out).z = (a).z * (b); (out).w = (a).w * (b); }
#define VGL_VEC4_DIV_SCALAR(out, a, b) { float inv = 1.0f / (b); VGL_VEC4_MUL_SCALAR(out, a, inv); }

// ##################################################################################
// ### Rect
// ##################################################################################

typedef struct vglIntRect {
    vglVec2i min, max;
} vglIntRect;

typedef struct vglFloatRect {
    vglVec2f min, max;
} vglFloatRect;

#define VGL_INT_RECT(minX, minY, maxX, maxY) ((vglIntRect){ VGL_VEC2I(minX, minY), VGL_VEC2I(maxX, maxY) })
#define VGL_FLOAT_RECT(minX, minY, maxX, maxY) ((vglFloatRect){ VGL_VEC2F(minX, minY), VGL_VEC2F(maxX, maxY) })

#define VGL_RECT_SET4(out, minX, minY, maxX, maxY) { (out).min.x = minX; (out).min.y = minY; (out).max.x = maxX; (out).max.y = maxY; }
#define VGL_RECT_SET2(out, nmin, nmax) { (out).min = nmin; (out).max = nmax; }

#define VGL_RECT_SET4_SIZED(out, nx, ny, nw, nh) { (out).min.x = nx; (out).min.y = ny; (out).max.x = (out).min.x + nw - 1; (out).max.y = (out).min.y + nh - 1; }
#define VGL_RECT_SET2_SIZED(out, npos, nsize) { (out).min.x = npos; (out).max.x = (out).min.x + (nsize).x - 1; (out).max.y = (out).min.y + (nsize).y - 1; }
#define VGL_RECT_GET_SIZE(out, a) { VGL_VEC2_SUB(out, (a).max, (a).min); VGL_VEC2_ADD_SCALAR(out, out, 1) }
#define VGL_RECT_GET_AREA(a) ((a).max.x - (a).min.x + 1)*((a).max.y - (a).min.y + 1)

// ##################################################################################
// ### Mat4
// ##################################################################################

typedef struct __declspec(align(16)) vglMat4f {
    union {
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        // 0 4  8 12
        // 1 5  9 13
        // 2 6 10 14
        // 3 7 11 15
        vglVec4f cols[4];
        float data[16];
    };
} vglMat4f;

#define VGL_MAT4(nm11, nm12, nm13, nm14,   nm21, nm22, nm23, nm24,   nm31, nm32, nm33, nm34,   nm41, nm42, nm43, nm44) \
    ((vglMat4f) { \
        .m11 = nm11, .m12 = nm12, .m13 = nm13, .m14 = nm14, \
        .m21 = nm21, .m22 = nm22, .m23 = nm23, .m24 = nm24, \
        .m31 = nm31, .m32 = nm32, .m33 = nm33, .m34 = nm34, \
        .m41 = nm41, .m42 = nm42, .m43 = nm43, .m44 = nm44, \
    })

#define VGL_MAT4_IDENTITY() VGL_MAT4(\
    1, 0, 0, 0, \
    0, 1, 0, 0, \
    0, 0, 1, 0, \
    0, 0, 0, 1  \
)

#define VGL_MAT4_SET(out,   nm11, nm12, nm13, nm14,   nm21, nm22, nm23, nm24,   nm31, nm32, nm33, nm34,   nm41, nm42, nm43, nm44) { \
    (out) = VGL_MAT4(nm11, nm12, nm13, nm14,  \
                     nm21, nm22, nm23, nm24,  \
                     nm31, nm32, nm33, nm34,  \
                     nm41, nm42, nm43, nm44); \
}

#define VGL_MAT4_SETV(out, ptr) memcpy(&(out), ptr, sizeof(out))

#define VGL_MAT4_SET_IDENTITY(out) VGL_MAT4_SET(out, \
    1, 0, 0, 0, \
    0, 1, 0, 0, \
    0, 0, 1, 0, \
    0, 0, 0, 1  \
)

#define VGL_MAT4_SET_TRANSLATE(out, nx, ny, nz) VGL_MAT4_SET(out, \
    1, 0, 0, nx, \
    0, 1, 0, ny, \
    0, 0, 1, nz, \
    0, 0, 0, 1   \
)

#define VGL_MAT4_SET_ROTATE(out, angle, nx, ny, nz) { \
    const float c = cosf(angle); \
    const float s = sinf(angle); \
\
    vglVec3f axis = { nx, ny, nz }; \
    VGL_VEC3_NORMALIZE(axis, axis); \
\
    vglVec3f temp;                 \
    VGL_VEC3_MUL_SCALAR(temp, axis, 1 - c); \
\
    VGL_MAT4_SET_IDENTITY(out); \
\
    (out).m11 = c + temp.x * axis.x; \
    (out).m12 = temp.x * axis.y + s * axis.z; \
    (out).m13 = temp.x * axis.z - s * axis.y; \
\
    (out).m21 = temp.y * axis.x - s * axis.z; \
    (out).m22 = c + temp.y * axis.y; \
    (out).m23 = temp.y * axis.z + s * axis.x; \
\
    (out).m31 = temp.z * axis.x + s * axis.y; \
    (out).m32 = temp.z * axis.y - s * axis.x; \
    (out).m33 = c + temp.z * axis.z; \
}

#define VGL_MAT4_SET_SCALE(out, nx, ny, nz) VGL_MAT4_SET(out, \
    nx, 0,  0,  0, \
    0,  ny, 0,  0, \
    0,  0,  nz, 0, \
    0,  0,  0,  1  \
)

#define VGL_MAT4_SET_PERSPECTIVE_RH(out, fovy, aspect, zNear, zFar) { \
    const float tanHalfFovy = tanf((fovy) / 2.0f); \
\
    VGL_MAT4_SET_IDENTITY(out); \
\
    (out).m11 = 1.0f / ((aspect) * tanHalfFovy); \
    (out).m22 = 1.0f / (tanHalfFovy); \
    (out).m33 = -((zFar) + (zNear)) / ((zFar) - (zNear)); \
    (out).m43 = -1.0f; \
    (out).m34 = -(2.0f * (zFar) * (zNear)) / ((zFar) - (zNear)); \
}

#define VGL_MAT4_SET_VIEWPORT(out, nx, ny, nw, nh) { VGL_MAT4_SET(out, \
        0.5f*(nw),  0.0f,      0.0f, 0.5f*(nw) + (nx), \
        0.0f,      -0.5f*(nh), 0.0f, 0.5f*(nh) + (ny), \
        0.0f,       0.0f,      0.5f, 0.5f,             \
        0.0f,       0.0f,      0.0f, 0.5f              \
    ); \
}

#define VGL_MAT4_GET(mat, r, c) ((mat).cols[c].data[r])

#define VGL_MAT4_MUL(out, a, b) { \
    for (int r = 0; r < 4; r++) { \
        for (int c = 0; c < 4; c++) { \
            VGL_MAT4_GET(out, r, c) = VGL_MAT4_GET(b, r, 0)*VGL_MAT4_GET(a, 0, c)  \
                                    + VGL_MAT4_GET(b, r, 1)*VGL_MAT4_GET(a, 1, c)  \
                                    + VGL_MAT4_GET(b, r, 2)*VGL_MAT4_GET(a, 2, c)  \
                                    + VGL_MAT4_GET(b, r, 3)*VGL_MAT4_GET(a, 3, c); \
        } \
    } \
}

#define VGL_MAT4_MUL_VEC4(out, a, b) { \
    __m128 __v__ = _mm_load_ps((b).data); \
    _mm_store_ss((out).data + 0, _mm_dp_ps(__v__, _mm_load_ps((a).cols + 0), 0xF1)); \
    _mm_store_ss((out).data + 1, _mm_dp_ps(__v__, _mm_load_ps((a).cols + 1), 0xF1)); \
    _mm_store_ss((out).data + 2, _mm_dp_ps(__v__, _mm_load_ps((a).cols + 2), 0xF1)); \
    _mm_store_ss((out).data + 3, _mm_dp_ps(__v__, _mm_load_ps((a).cols + 3), 0xF1)); \
}

#define VGL_MAT3_MUL_VEC3(out, a, b) { \
    for (int c = 0; c < 3; c++) { \
        (out).data[c] = VGL_MAT4_GET(a, 0, c)*(b).data[0]  \
                      + VGL_MAT4_GET(a, 1, c)*(b).data[1]  \
                      + VGL_MAT4_GET(a, 2, c)*(b).data[2]; \
    } \
}

#define VGL_MAT4_TRANSPOSE(out, a) { \
    __m128 col0 = _mm_load_ps((a).cols[0].data); \
    __m128 col1 = _mm_load_ps((a).cols[1].data); \
    __m128 col2 = _mm_load_ps((a).cols[2].data); \
    __m128 col3 = _mm_load_ps((a).cols[3].data); \
    _MM_TRANSPOSE4_PS(col0, col1, col2, col3); \
    _mm_store_ps((out).cols[0].data, col0); \
    _mm_store_ps((out).cols[1].data, col1); \
    _mm_store_ps((out).cols[2].data, col2); \
    _mm_store_ps((out).cols[3].data, col3); \
}

#define VGL_MAT4_INVERSE(out, a) {                                            \
    __m128 in0 = _mm_load_ps((a).cols[0].data);                               \
    __m128 in1 = _mm_load_ps((a).cols[1].data);                               \
    __m128 in2 = _mm_load_ps((a).cols[2].data);                               \
    __m128 in3 = _mm_load_ps((a).cols[3].data);                               \
    __m128 Fac0;                                                              \
    {                                                                         \
	    /*	valType SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3]; */   \
	    /*	valType SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3]; */   \
	    /*	valType SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3]; */   \
	    /*	valType SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3]; */   \
                                                                              \
	    __m128 Swp0a = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(3, 3, 3, 3));     \
	    __m128 Swp0b = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(2, 2, 2, 2));     \
                                                                              \
	    __m128 Swp00 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(2, 2, 2, 2));     \
	    __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp03 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(3, 3, 3, 3));     \
                                                                              \
	    __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);                              \
	    __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);                              \
	    Fac0 = _mm_sub_ps(Mul00, Mul01);                                      \
    }                                                                         \
                                                                              \
    __m128 Fac1;                                                              \
    {                                                                         \
	    /*	valType SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3]; */   \
	    /*	valType SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3]; */   \
	    /*	valType SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3]; */   \
	    /*	valType SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3]; */   \
                                                                              \
	    __m128 Swp0a = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(3, 3, 3, 3));     \
	    __m128 Swp0b = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(1, 1, 1, 1));     \
                                                                              \
	    __m128 Swp00 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(1, 1, 1, 1));     \
	    __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp03 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(3, 3, 3, 3));     \
                                                                              \
	    __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);                              \
	    __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);                              \
	    Fac1 = _mm_sub_ps(Mul00, Mul01);                                      \
    }                                                                         \
                                                                              \
                                                                              \
    __m128 Fac2;                                                              \
    {                                                                         \
	    /*	valType SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2]; */   \
	    /*	valType SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2]; */   \
	    /*	valType SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2]; */   \
	    /*	valType SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2]; */   \
                                                                              \
	    __m128 Swp0a = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(2, 2, 2, 2));     \
	    __m128 Swp0b = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(1, 1, 1, 1));     \
                                                                              \
	    __m128 Swp00 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(1, 1, 1, 1));     \
	    __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp03 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(2, 2, 2, 2));     \
                                                                              \
	    __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);                              \
	    __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);                              \
	    Fac2 = _mm_sub_ps(Mul00, Mul01);                                      \
    }                                                                         \
                                                                              \
    __m128 Fac3;                                                              \
    {                                                                         \
	    /*	valType SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3]; */   \
	    /*	valType SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3]; */   \
	    /*	valType SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3]; */   \
	    /*	valType SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3]; */   \
                                                                              \
	    __m128 Swp0a = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(3, 3, 3, 3));     \
	    __m128 Swp0b = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(0, 0, 0, 0));     \
                                                                              \
	    __m128 Swp00 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(0, 0, 0, 0));     \
	    __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp03 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(3, 3, 3, 3));     \
                                                                              \
	    __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);                              \
	    __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);                              \
	    Fac3 = _mm_sub_ps(Mul00, Mul01);                                      \
    }                                                                         \
                                                                              \
    __m128 Fac4;                                                              \
    {                                                                         \
	    /*	valType SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2]; */   \
	    /*	valType SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2]; */   \
	    /*	valType SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2]; */   \
	    /*	valType SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2]; */   \
                                                                              \
	    __m128 Swp0a = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(2, 2, 2, 2));     \
	    __m128 Swp0b = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(0, 0, 0, 0));     \
                                                                              \
	    __m128 Swp00 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(0, 0, 0, 0));     \
	    __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp03 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(2, 2, 2, 2));     \
                                                                              \
	    __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);                              \
	    __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);                              \
	    Fac4 = _mm_sub_ps(Mul00, Mul01);                                      \
    }                                                                         \
                                                                              \
    __m128 Fac5;                                                              \
    {                                                                         \
	    /*	valType SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1]; */   \
	    /*	valType SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1]; */   \
	    /*	valType SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1]; */   \
	    /*	valType SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1]; */   \
                                                                              \
	    __m128 Swp0a = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(1, 1, 1, 1));     \
	    __m128 Swp0b = _mm_shuffle_ps(in3, in2, _MM_SHUFFLE(0, 0, 0, 0));     \
                                                                              \
	    __m128 Swp00 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(0, 0, 0, 0));     \
	    __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0)); \
	    __m128 Swp03 = _mm_shuffle_ps(in2, in1, _MM_SHUFFLE(1, 1, 1, 1));     \
                                                                              \
	    __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);                              \
	    __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);                              \
	    Fac5 = _mm_sub_ps(Mul00, Mul01);                                      \
    }                                                                         \
                                                                              \
    __m128 SignA = _mm_set_ps( 1.0f,-1.0f, 1.0f,-1.0f);                       \
    __m128 SignB = _mm_set_ps(-1.0f, 1.0f,-1.0f, 1.0f);                       \
                                                                              \
    /* m[1][0] */                                                             \
    /* m[0][0] */                                                             \
    /* m[0][0] */                                                             \
    /* m[0][0] */                                                             \
    __m128 Temp0 = _mm_shuffle_ps(in1, in0, _MM_SHUFFLE(0, 0, 0, 0));         \
    __m128 Vec0 = _mm_shuffle_ps(Temp0, Temp0, _MM_SHUFFLE(2, 2, 2, 0));      \
                                                                              \
    /* m[1][1] */                                                             \
    /* m[0][1] */                                                             \
    /* m[0][1] */                                                             \
    /* m[0][1] */                                                             \
    __m128 Temp1 = _mm_shuffle_ps(in1, in0, _MM_SHUFFLE(1, 1, 1, 1));         \
    __m128 Vec1 = _mm_shuffle_ps(Temp1, Temp1, _MM_SHUFFLE(2, 2, 2, 0));      \
                                                                              \
    /* m[1][2] */                                                             \
    /* m[0][2] */                                                             \
    /* m[0][2] */                                                             \
    /* m[0][2] */                                                             \
    __m128 Temp2 = _mm_shuffle_ps(in1, in0, _MM_SHUFFLE(2, 2, 2, 2));         \
    __m128 Vec2 = _mm_shuffle_ps(Temp2, Temp2, _MM_SHUFFLE(2, 2, 2, 0));      \
                                                                              \
    /* m[1][3] */                                                             \
    /* m[0][3] */                                                             \
    /* m[0][3] */                                                             \
    /* m[0][3] */                                                             \
    __m128 Temp3 = _mm_shuffle_ps(in1, in0, _MM_SHUFFLE(3, 3, 3, 3));         \
    __m128 Vec3 = _mm_shuffle_ps(Temp3, Temp3, _MM_SHUFFLE(2, 2, 2, 0));      \
                                                                              \
    /* col0                                                                   \
       + (Vec1[0] * Fac0[0] - Vec2[0] * Fac1[0] + Vec3[0] * Fac2[0]),         \
       - (Vec1[1] * Fac0[1] - Vec2[1] * Fac1[1] + Vec3[1] * Fac2[1]),         \
       + (Vec1[2] * Fac0[2] - Vec2[2] * Fac1[2] + Vec3[2] * Fac2[2]),         \
       - (Vec1[3] * Fac0[3] - Vec2[3] * Fac1[3] + Vec3[3] * Fac2[3]), */      \
    __m128 Mul00 = _mm_mul_ps(Vec1, Fac0);                                    \
    __m128 Mul01 = _mm_mul_ps(Vec2, Fac1);                                    \
    __m128 Mul02 = _mm_mul_ps(Vec3, Fac2);                                    \
    __m128 Sub00 = _mm_sub_ps(Mul00, Mul01);                                  \
    __m128 Add00 = _mm_add_ps(Sub00, Mul02);                                  \
    __m128 Inv0 = _mm_mul_ps(SignB, Add00);                                   \
                                                                              \
    /* col1                                                                   \
       - (Vec0[0] * Fac0[0] - Vec2[0] * Fac3[0] + Vec3[0] * Fac4[0]),         \
       + (Vec0[0] * Fac0[1] - Vec2[1] * Fac3[1] + Vec3[1] * Fac4[1]),         \
       - (Vec0[0] * Fac0[2] - Vec2[2] * Fac3[2] + Vec3[2] * Fac4[2]),         \
       + (Vec0[0] * Fac0[3] - Vec2[3] * Fac3[3] + Vec3[3] * Fac4[3]), */      \
    __m128 Mul03 = _mm_mul_ps(Vec0, Fac0);                                    \
    __m128 Mul04 = _mm_mul_ps(Vec2, Fac3);                                    \
    __m128 Mul05 = _mm_mul_ps(Vec3, Fac4);                                    \
    __m128 Sub01 = _mm_sub_ps(Mul03, Mul04);                                  \
    __m128 Add01 = _mm_add_ps(Sub01, Mul05);                                  \
    __m128 Inv1 = _mm_mul_ps(SignA, Add01);                                   \
                                                                              \
    /* col2                                                                   \
       + (Vec0[0] * Fac1[0] - Vec1[0] * Fac3[0] + Vec3[0] * Fac5[0]),         \
       - (Vec0[0] * Fac1[1] - Vec1[1] * Fac3[1] + Vec3[1] * Fac5[1]),         \
       + (Vec0[0] * Fac1[2] - Vec1[2] * Fac3[2] + Vec3[2] * Fac5[2]),         \
       - (Vec0[0] * Fac1[3] - Vec1[3] * Fac3[3] + Vec3[3] * Fac5[3]), */      \
    __m128 Mul06 = _mm_mul_ps(Vec0, Fac1);                                    \
    __m128 Mul07 = _mm_mul_ps(Vec1, Fac3);                                    \
    __m128 Mul08 = _mm_mul_ps(Vec3, Fac5);                                    \
    __m128 Sub02 = _mm_sub_ps(Mul06, Mul07);                                  \
    __m128 Add02 = _mm_add_ps(Sub02, Mul08);                                  \
    __m128 Inv2 = _mm_mul_ps(SignB, Add02);                                   \
                                                                              \
    /* col3                                                                   \
       - (Vec1[0] * Fac2[0] - Vec1[0] * Fac4[0] + Vec2[0] * Fac5[0]),         \
       + (Vec1[0] * Fac2[1] - Vec1[1] * Fac4[1] + Vec2[1] * Fac5[1]),         \
       - (Vec1[0] * Fac2[2] - Vec1[2] * Fac4[2] + Vec2[2] * Fac5[2]),         \
       + (Vec1[0] * Fac2[3] - Vec1[3] * Fac4[3] + Vec2[3] * Fac5[3])); */     \
    __m128 Mul09 = _mm_mul_ps(Vec0, Fac2);                                    \
    __m128 Mul10 = _mm_mul_ps(Vec1, Fac4);                                    \
    __m128 Mul11 = _mm_mul_ps(Vec2, Fac5);                                    \
    __m128 Sub03 = _mm_sub_ps(Mul09, Mul10);                                  \
    __m128 Add03 = _mm_add_ps(Sub03, Mul11);                                  \
    __m128 Inv3 = _mm_mul_ps(SignA, Add03);                                   \
                                                                              \
    __m128 Row0 = _mm_shuffle_ps(Inv0, Inv1, _MM_SHUFFLE(0, 0, 0, 0));        \
    __m128 Row1 = _mm_shuffle_ps(Inv2, Inv3, _MM_SHUFFLE(0, 0, 0, 0));        \
    __m128 Row2 = _mm_shuffle_ps(Row0, Row1, _MM_SHUFFLE(2, 0, 2, 0));        \
                                                                              \
    /*	valType Determinant = m[0][0] * Inverse[0][0]                         \
    						+ m[0][1] * Inverse[1][0]                         \
    						+ m[0][2] * Inverse[2][0]                         \
    						+ m[0][3] * Inverse[3][0]; */                     \
    __m128 Det0 = _mm_dp_ps(in0, Row2, 0xff);                                 \
    __m128 Rcp0 = _mm_div_ps(_mm_set1_ps(1.0f), Det0);                        \
    /*__m128 Rcp0 = _mm_rcp_ps(Det0);*/                                       \
                                                                              \
    /*	Inverse /= Determinant; */                                            \
    _mm_store_ps((out).cols[0].data, _mm_mul_ps(Inv0, Rcp0));                 \
    _mm_store_ps((out).cols[1].data, _mm_mul_ps(Inv1, Rcp0));                 \
    _mm_store_ps((out).cols[2].data, _mm_mul_ps(Inv2, Rcp0));                 \
    _mm_store_ps((out).cols[3].data, _mm_mul_ps(Inv3, Rcp0));                 \
}
