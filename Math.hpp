#pragma once
#include <type_traits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <memory.h>

// ##################################################################################
// ### Math library
// ##################################################################################

struct Math {
    static float deg(float rad) {
        return static_cast<float>(rad*180.0f / M_PI);
    }

    static float rad(float deg) {
        return static_cast<float>(deg / 180.0f*M_PI);
    }

    static float sqrt(float sq) {
        return sqrtf(sq);
    }

    static float sin(float angle) {
        return sinf(angle);
    }

    static float cos(float angle) {
        return cosf(angle);
    }

    static float tan(float angle) {
        return tanf(angle);
    }
};

// ##################################################################################
// ### Color
// ##################################################################################

struct Color {
    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

    void setFloat4(float r, float g, float b, float a) {
        this->r = static_cast<uint8_t>(r*255);
        this->g = static_cast<uint8_t>(g*255);
        this->b = static_cast<uint8_t>(b*255);
        this->a = static_cast<uint8_t>(a*255);
    }

    bool operator==(const Color &rhs) const {
        return this->data == rhs.data;
    }

    bool operator!=(const Color &rhs) const {
        return this->data != rhs.data;
    }

    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t data;
    };
};

// ##################################################################################
// ### Vec2
// ##################################################################################

template<typename T>
struct Vec2 {
    Vec2() = default;
    Vec2(T x, T y) : x(x), y(y) {}
    Vec2(T xy) : x(xy), y(xy) {}

    void set(T x, T y) {
        this->x = x;
        this->y = y;
    }

    float lengthSq() {
        return this->x*this->x + this->y*this->y;
    }

    float length() {
        return Math::sqrt(lengthSq());
    }
    
    void normalize() {
        static_assert(std::is_floating_point_v<T>, "T must be floating point value");

        float invLen = 1.0f / length();
        this->x *= invLen;
        this->y *= invLen;
    }

    Vec2 getNormalized() {
        Vec2 v = *this;
        v.normalize();
        return v;
    }

    T operator[](int i) const {
        return this->data[i];
    }

    T &operator[](int i) {
        return this->data[i];
    }

    Vec2 operator+(const Vec2 &rhs) const {
        return Vec2(this->x + rhs.x, this->y + rhs.y);
    }

    Vec2 operator-(const Vec2 &rhs) const {
        return Vec2(this->x - rhs.x, this->y - rhs.y);
    }

    Vec2 operator*(T rhs) const {
        return Vec2(this->x*rhs, this->y*rhs);
    }

    Vec2 operator/(T rhs) const {
        T inv = static_cast<T>(1) / rhs;
        return Vec2(this->x*inv, this->y*inv);
    }

    Vec2 &operator+=(const Vec2 &rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    Vec2 &operator-=(const Vec2 &rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    Vec2 &operator*=(T rhs) {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    Vec2 &operator/=(T rhs) {
        T inv = static_cast<T>(1) / rhs;
        this->x *= inv;
        this->y *= inv;
        return *this;
    }

    union {
        struct {
            T x, y;
        };
        T data[2];
    };
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;

// ##################################################################################
// ### Vec3
// ##################################################################################

template<typename T>
struct Vec3 {
    Vec3() = default;
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    Vec3(T xyz) : x(xyz), y(xyz), z(xyz) {}

    void set(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float lengthSq() {
        return this->x*this->x + this->y*this->y + this->z*this->z;
    }

    float length() {
        return Math::sqrt(lengthSq());
    }

    void normalize() {
        static_assert(std::is_floating_point_v<T>, "T must be floating point value");

        float invLen = 1.0f / length();
        this->x *= invLen;
        this->y *= invLen;
        this->z *= invLen;
    }

    Vec3 getNormalized() {
        Vec3 v = *this;
        v.normalize();
        return v;
    }

    T operator[](int i) const {
        return this->data[i];
    }

    T &operator[](int i) {
        return this->data[i];
    }

    Vec3 operator+(const Vec3 &rhs) const {
        return Vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
    }

    Vec3 operator-(const Vec3 &rhs) const {
        return Vec3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
    }

    Vec3 operator*(T rhs) const {
        return Vec3(this->x*rhs, this->y*rhs, this->z*rhs);
    }

    Vec3 operator/(T rhs) const {
        T inv = static_cast<T>(1) / rhs;
        return Vec3(this->x*inv, this->y*inv, this->z*inv);
    }

    Vec3 &operator+=(const Vec3 &rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    Vec3 &operator-=(const Vec3 &rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    Vec3 &operator*=(T rhs) {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        return *this;
    }

    Vec3 &operator/=(T rhs) {
        T inv = static_cast<T>(1) / rhs;
        this->x *= inv;
        this->y *= inv;
        this->z *= inv;
        return *this;
    }

    union {
        struct {
            T x, y, z;
        };
        T data[3];
    };
};

using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;

// ##################################################################################
// ### Vec4
// ##################################################################################

template<typename T>
struct Vec4 {
    Vec4() = default;
    Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    Vec4(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}

    void set(T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    float lengthSq() {
        return this->x*this->x + this->y*this->y + this->z*this->z + this->w*this->w;
    }

    float length() {
        return Math::sqrt(lengthSq());
    }

    void normalize() {
        static_assert(std::is_floating_point_v<T>, "T must be floating point value");

        float invLen = 1.0f / length();
        this->x *= invLen;
        this->y *= invLen;
        this->z *= invLen;
        this->w *= invLen;
    }

    Vec4 getNormalized() {
        Vec4 v = *this;
        v.normalize();
        return v;
    }

    T operator[](int i) const {
        return this->data[i];
    }

    T &operator[](int i) {
        return this->data[i];
    }

    Vec4 operator+(const Vec4 &rhs) const {
        return Vec4(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
    }

    Vec4 operator-(const Vec4 &rhs) const {
        return Vec4(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
    }

    Vec4 operator*(T rhs) const {
        return Vec4(this->x*rhs, this->y*rhs, this->z*rhs, this->w*rhs);
    }

    Vec4 operator/(T rhs) const {
        T inv = static_cast<T>(1) / rhs;
        return Vec4(this->x*inv, this->y*inv, this->z*inv, this->w*inv);
    }

    Vec4 &operator+=(const Vec4 &rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        this->w += rhs.w;
        return *this;
    }

    Vec4 &operator-=(const Vec4 &rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        this->w -= rhs.w;
        return *this;
    }

    Vec4 &operator*=(T rhs) {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        this->w *= rhs;
        return *this;
    }

    Vec4 &operator/=(T rhs) {
        T inv = static_cast<T>(1) / rhs;
        this->x *= inv;
        this->y *= inv;
        this->z *= inv;
        this->w *= inv;
        return *this;
    }

    union {
        struct {
            T x, y, z, w;
        };
        T data[4];
    };
};

using Vec4i = Vec4<int>;
using Vec4f = Vec4<float>;

// ##################################################################################
// ### Rect
// ##################################################################################

template<typename T>
struct Rect {
    Rect() = default;
    Rect(T minX, T minY, T maxX, T maxY) {
        set(minX, minY, maxX, maxY);
    }
    Rect(const Vec2<T> &min, const Vec2<T> &max) {
        set(min, max);
    }

    void set(T minX, T minY, T maxX, T maxY) {
        this->min.set(minX, minY);
        this->max.set(maxX, maxY);
    }

    void set(const Vec2<T> &min, const Vec2<T> &max) {
        this->min = min;
        this->max = max;
    }

    void setSized(T x, T y, T w, T h) {
        this->min.set(x, y);
        this->max.set(x + w - 1, y + h - 1);
    }

    void setSized(const Vec2<T> &pos, const Vec2<T> &size) {
        this->min = pos;
        this->max = pos + size - Vec2<T>(static_cast<T>(1));
    }

    Vec2<T> getSize() const {
        return this->max - this->min + Vec2<T>(static_cast<T>(1));
    }

    T getArea() const {
        return (this->max.x - this->min.x + static_cast<T>(1))*(this->max.y - this->min.y + static_cast<T>(1));
    }

    Vec2<T> min, max;
};

using IntRect = Rect<int>;
using FloatRect = Rect<float>;

// ##################################################################################
// ### Mat4
// ##################################################################################

template<typename T>
struct Mat4 {
    Mat4() = default;
    Mat4(T m11, T m12, T m13, T m14,
          T m21, T m22, T m23, T m24,
          T m31, T m32, T m33, T m34,
          T m41, T m42, T m43, T m44) {
        set(m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44);
    }

    void translate(T x, T y, T z) {
        (*this) *= createTranslate(x, y, z);
    }

    void rotate(T angle, T x, T y, T z) {
        (*this) *= createRotate(angle, x, y, z);
    }

    void scale(T x, T y, T z) {
        (*this) *= createScale(x, y, z);
    }

    void set(T m11, T m12, T m13, T m14,
             T m21, T m22, T m23, T m24,
             T m31, T m32, T m33, T m34,
             T m41, T m42, T m43, T m44) {
        this->m11 = m11; this->m21 = m21; this->m31 = m31; this->m41 = m41;
        this->m12 = m12; this->m22 = m22; this->m32 = m32; this->m42 = m42;
        this->m13 = m13; this->m23 = m23; this->m33 = m33; this->m43 = m43;
        this->m14 = m14; this->m24 = m24; this->m34 = m34; this->m44 = m44;
    }

    void set(const T *data) {
        memcpy(this->data, data, sizeof(this->data));
    }

    void setIdentity() {
        *this = Identity;
    }

    void setTranslate(T x, T y, T z) {
        set(1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1);
    }

    void setRotate(T angle, T x, T y, T z) {
        const T a = angle;
        const T c = Math::cos(a);
        const T s = Math::sin(a);

        auto axis = Vec3<T>(x, y, z);
        axis.normalize();

        const auto temp = axis*(static_cast<T>(1) - c);

        setIdentity();

        this->m11 = c + temp[0] * axis[0];
        this->m12 = temp[0] * axis[1] + s * axis[2];
        this->m13 = temp[0] * axis[2] - s * axis[1];

        this->m21 = temp[1] * axis[0] - s * axis[2];
        this->m22 = c + temp[1] * axis[1];
        this->m23 = temp[1] * axis[2] + s * axis[0];

        this->m31 = temp[2] * axis[0] + s * axis[1];
        this->m32 = temp[2] * axis[1] - s * axis[0];
        this->m33 = c + temp[2] * axis[2];
    }

    void setScale(T x, T y, T z) {
        set(x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1);
    }

    void setPerspectiveRH(T fovy, T aspect, T zNear, T zFar) {
        //assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

        const T tanHalfFovy = Math::tan(fovy / static_cast<T>(2));

        setIdentity();

        this->m11 = static_cast<T>(1) / (aspect * tanHalfFovy);
        this->m22 = static_cast<T>(1) / (tanHalfFovy);
        this->m33 = - (zFar + zNear) / (zFar - zNear);
        this->m34 = - static_cast<T>(1); // transpose this if there are problems
        this->m43 = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear); // transpose this if there are problems
    }

    void setViewport(T x, T y, T w, T h) {
        T a = (w - static_cast<T>(1)) / static_cast<T>(2);
        T b = (h - static_cast<T>(1)) / static_cast<T>(2);
        set(a,  0, 0, x,
            0, -b, 0, y,
            0,  0, 1, 0,
            a,  b, 0, 1);
    }

    Mat4 operator*(const Mat4 &rhs) const {
        Mat4 m;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                m(r, c) = 0;
                for (int i = 0; i < 4; i++) {
                    m(r, c) += (*this)(r, i)*rhs(i, c);
                }
            }
        }
        return m;
    }

    Mat4 &operator*=(const Mat4 &rhs) {
        *this = (*this)*rhs;
        return *this;
    }

    T operator()(int r, int c) const {
        return this->data[c + r*4];
    }

    T &operator()(int r, int c) {
        return this->data[c + r*4];
    }

    static Mat4 createTranslate(T x, T y, T z) {
        Mat4 m;
        m.setTranslate(x, y, z);
        return m;
    }

    static Mat4 createRotate(T angle, T x, T y, T z) {
        Mat4 m;
        m.setRotate(angle, x, y, z);
        return m;
    }

    static Mat4 createScale(T x, T y, T z) {
        Mat4 m;
        m.setScale(x, y, z);
        return m;
    }

    static Mat4 createPerspectiveRH(T fovy, T aspect, T zNear, T zFar) {
        Mat4 m;
        m.setPerspectiveRH(fovy, aspect, zNear, zFar);
        return m;
    }

    static const Mat4 Identity;

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
        float data[16];
    };
};

template<typename T>
const Mat4<T> Mat4<T>::Identity = Mat4<T>(1, 0, 0, 0,
                                          0, 1, 0, 0,
                                          0, 0, 1, 0,
                                          0, 0, 0, 1);

using Mat4f = Mat4<float>;
