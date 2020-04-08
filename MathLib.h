#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <memory.h>
#include <float.h>

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
#define VGL_VEC2_NORMALIZE(out, v) { float invLen = 1.0f / VGL_VEC2_LEN(v); (out).x = (v).x*invLen; (out).y = (v).y*invLen; }
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
#define VGL_VEC3_NORMALIZE(out, v) { float invLen = 1.0f / VGL_VEC3_LEN(v); (out).x = (v).x*invLen; (out).y = (v).y*invLen; (out).z = (v).z*invLen; }
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

typedef struct vglVec4i {
    union {
        struct {
            int x, y, z, w;
        };
        int data[4];
    };
} vglVec4i;

typedef __declspec(align(16)) struct vglAlignedVec4i {
    union {
        struct {
            int x, y, z, w;
        };
        int data[4];
    };
} vglAlignedVec4i;

typedef struct vglVec4f {
    union {
        struct {
            float x, y, z, w;
        };
        float data[4];
    };
} vglVec4f;

typedef __declspec(align(16)) struct vglAlignedVec4f {
    union {
        struct {
            float x, y, z, w;
        };
        float data[4];
    };
} vglAlignedVec4f;

#define VGL_VEC4I(x, y, z, w) ((vglVec4i){ x, y, z, w })
#define VGL_VEC4F(x, y, z, w) ((vglVec4f){ x, y, z, w })
#define VGL_ALIGNED_VEC4I(x, y, z, w) ((vglAlignedVec4i){ x, y, z, w })
#define VGL_ALIGNED_VEC4F(x, y, z, w) ((vglAlignedVec4f){ x, y, z, w })

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
#define VGL_VEC4_NORMALIZE(out, v) { float invLen = 1.0f / VGL_VEC4_LEN(v); (out).x = (v).x*invLen; (out).y = (v).y*invLen; (out).z = (v).z*invLen; (out).w = (v).w*invLen; }
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

typedef struct vglMat4f {
    union {
        struct {
            float m11, m21, m31, m41;
            float m12, m22, m32, m42;
            float m13, m23, m33, m43;
            float m14, m24, m34, m44;
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
        nm11, nm12, nm13, nm14, \
        nm21, nm22, nm23, nm24, \
        nm31, nm32, nm33, nm34, \
        nm41, nm42, nm43, nm44  \
    })

#define VGL_MAT4_IDENTITY() VGL_MAT4(\
    1, 0, 0, 0, \
    0, 1, 0, 0, \
    0, 0, 1, 0, \
    0, 0, 0, 1  \
)

#define VGL_MAT4_SET(out,   nm11, nm12, nm13, nm14,   nm21, nm22, nm23, nm24,   nm31, nm32, nm33, nm34,   nm41, nm42, nm43, nm44) { \
    (out).m11 = nm11; (out).m21 = nm21; (out).m31 = nm31; (out).m41 = nm41; \
    (out).m12 = nm12; (out).m22 = nm22; (out).m32 = nm32; (out).m42 = nm42; \
    (out).m13 = nm13; (out).m23 = nm23; (out).m33 = nm33; (out).m43 = nm43; \
    (out).m14 = nm14; (out).m24 = nm24; (out).m34 = nm34; (out).m44 = nm44; \
}

#define VGL_MAT4_SETV(out, ptr) memcpy((out).data, ptr, sizeof((out).data))

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
    (out).m33 = - ((zFar) + (zNear)) / ((zFar) - (zNear)); \
    (out).m43 = - 1.0f; \
    (out).m34 = - (2.0f * (zFar) * (zNear)) / ((zFar) - (zNear)); \
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
            VGL_MAT4_GET(out, r, c) = 0; \
            for (int i = 0; i < 4; i++) { \
                VGL_MAT4_GET(out, r, c) += VGL_MAT4_GET(a, r, i)*VGL_MAT4_GET(b, i, c); \
            } \
        } \
    } \
}

#define VGL_MAT4_MUL_VEC4(out, a, b) { \
    for (int r = 0; r < 4; r++) { \
        (out).data[r] = 0; \
        for (int c = 0; c < 4; c++) { \
            (out).data[r] += VGL_MAT4_GET(a, r, c)*(b).data[c]; \
        } \
    } \
}
