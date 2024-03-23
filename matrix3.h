#pragma once

#include "vector3.h"

struct Matrix3;

inline float det(const Matrix3& a);
inline Matrix3 cofact(const Matrix3& a);
inline Matrix3 adj(const Matrix3& a);
inline Matrix3 operator/(const Matrix3& a, const float b);

/**
    * structure which represents a 3x3 matrix
    * with components:
    * [ x_0, y_0, z_0 ]
    * [ x_1, y_1, z_1 ]
    * [ x_2, y_2, z_2 ]
    *
    * elements are indexed left to right, top to bottom
    *
    * the `-` operator can be applied to find the transpose of the matrix
    * the `~` operator can be applied to find the inverse of the matrix
    * **/
struct Matrix3
{
    float x_0, y_0, z_0;
    float x_1, y_1, z_1;
    float x_2, y_2, z_2;

    inline Vector3 col0() const { return Vector3{ x_0, x_1, x_2 }; }
    inline Vector3 col1() const { return Vector3{ y_0, y_1, y_2 }; }
    inline Vector3 col2() const { return Vector3{ z_0, z_1, z_2 }; }
    inline Vector3 row0() const { return Vector3{ x_0, y_0, z_0 }; }
    inline Vector3 row1() const { return Vector3{ x_1, y_1, z_1 }; }
    inline Vector3 row2() const { return Vector3{ x_2, y_2, z_2 }; }

    inline void operator=(const Matrix3& a) { x_0 = a.x_0; y_0 = a.y_0; z_0 = a.z_0; x_1 = a.x_1; y_1 = a.y_1; z_1 = a.z_1; x_2 = a.x_2; y_2 = a.y_2; z_2 = a.z_2; }
    inline void operator+=(const Matrix3& a) { x_0 += a.x_0; y_0 += a.y_0; z_0 += a.z_0; x_1 += a.x_1; y_1 += a.y_1; z_1 += a.z_1; x_2 += a.x_2; y_2 += a.y_2; z_2 += a.z_2; }
    inline void operator-=(const Matrix3& a) { x_0 -= a.x_0; y_0 -= a.y_0; z_0 -= a.z_0; x_1 -= a.x_1; y_1 -= a.y_1; z_1 -= a.z_1; x_2 -= a.x_2; y_2 -= a.y_2; z_2 -= a.z_2; }
    inline void operator*=(const Matrix3& a)
    {
        Vector3 r0 = row0();
        Vector3 r1 = row1();
        Vector3 r2 = row2();
        Vector3 c0 = a.col0();
        Vector3 c1 = a.col1();
        Vector3 c2 = a.col2();
        x_0 = r0 ^ c0; y_0 = r0 ^ c1; z_0 = r0 ^ c2;
        x_1 = r1 ^ c0; y_1 = r1 ^ c1; z_1 = r1 ^ c2;
        x_2 = r2 ^ c0; y_2 = r2 ^ c1; z_2 = r2 ^ c2;
    }
    inline void operator*=(const float a) { x_0 *= a; y_0 *= a; z_0 *= a; x_1 *= a; y_1 *= a; z_1 *= a; x_2 *= a; y_2 *= a; z_2 *= a; }
    inline void operator/=(const float a) { x_0 /= a; y_0 /= a; z_0 /= a; x_1 /= a; y_1 /= a; z_1 /= a; x_2 /= a; y_2 /= a; z_2 /= a; }
    inline Matrix3 operator-() const { return Matrix3{ x_0, x_1, x_2, y_0, y_1, y_2, z_0, z_1, z_2 }; }
    inline Matrix3 operator~() const { return adj(*this) / det(*this); }
};

inline Matrix3 operator+(const Matrix3& a, const Matrix3& b)
{
    return Matrix3
    {
        a.x_0 + b.x_0, a.y_0 + b.y_0, a.z_0 + b.z_0,
            a.x_1 + b.x_1, a.y_1 + b.y_1, a.z_1 + b.z_1,
            a.x_2 + b.x_2, a.y_2 + b.y_2, a.z_2 + b.z_2
    };
}

inline Matrix3 operator-(const Matrix3& a, const Matrix3& b)
{
    return Matrix3
    {
        a.x_0 - b.x_0, a.y_0 - b.y_0, a.z_0 - b.z_0,
            a.x_1 - b.x_1, a.y_1 - b.y_1, a.z_1 - b.z_1,
            a.x_2 - b.x_2, a.y_2 - b.y_2, a.z_2 - b.z_2
    };
}

inline Matrix3 operator*(const Matrix3& a, const Matrix3& b)
{
    Vector3 r0 = a.row0();
    Vector3 r1 = a.row1();
    Vector3 r2 = a.row2();
    Vector3 c0 = b.col0();
    Vector3 c1 = b.col1();
    Vector3 c2 = b.col2();
    return Matrix3
    {
        r0^ c0, r0^ c1, r0^ c2,
            r1^ c0, r1^ c1, r1^ c2,
            r2^ c0, r2^ c1, r2^ c2
    };
}

inline Vector3 operator*(const Matrix3& a, const Vector3& b)
{
    return Vector3{ a.row0() ^ b, a.row1() ^ b, a.row2() ^ b };
}

inline Matrix3 operator*(const Matrix3& a, const float b)
{
    return Matrix3
    {
        a.x_0* b, a.y_0* b, a.z_0* b,
            a.x_1* b, a.y_1* b, a.z_1* b,
            a.x_2* b, a.y_2* b, a.z_2* b
    };
}

inline Matrix3 operator/(const Matrix3& a, const float b)
{
    return Matrix3
    {
        a.x_0 / b, a.y_0 / b, a.z_0 / b,
            a.x_1 / b, a.y_1 / b, a.z_1 / b,
            a.x_2 / b, a.y_2 / b, a.z_2 / b
    };
}

/**
    * calculate the minor of a matrix
    *
    * - by taking the determinant of the matrix when the row and column
    * containing the specified element are discarded.
    *
    * elements are ordered:
    *
    * [ 0, 1, 2 ]
    * [ 3, 4, 5 ]
    * [ 6, 7, 8 ]
    *
    * @param a matrix on which to operate
    * @param e index of the element for which to find the minor matrix. see above
    *
    * @return determinant of the resulting minor matrix
    * **/
inline float minor(const Matrix3& a, const uint8_t e)
{
    switch (e)
    {
    case 0: return (a.y_1 * a.z_2) - (a.y_2 * a.z_1);
    case 1: return (a.x_1 * a.z_2) - (a.x_2 * a.z_1);
    case 2: return (a.x_1 * a.y_2) - (a.y_1 * a.x_2);
    case 3: return (a.y_0 * a.z_2) - (a.z_0 * a.y_2);
    case 4: return (a.x_0 * a.z_2) - (a.z_0 * a.x_2);
    case 5: return (a.x_0 * a.y_2) - (a.y_0 * a.x_2);
    case 6: return (a.y_0 * a.z_1) - (a.y_1 * a.z_0);
    case 7: return (a.x_0 * a.z_1) - (a.z_0 * a.x_1);
    case 8: return (a.x_0 * a.y_1) - (a.x_1 * a.y_0);
    default: return 0;
    }
}

/**
    * calculate determinant of a matrix
    *
    * - by multiplying three elements by their minors and alternately adding
    * and subtracting them together
    *
    * @param a matrix on which to operate
    *
    * @return determinant of the matrix
    * **/
inline float det(const Matrix3& a) { return ((a.x_0 * minor(a, 0)) - (a.y_0 * minor(a, 1))) + (a.z_0 * minor(a, 2)); }

/**
    * calculate matrix of cofactors from a matrix
    *
    * - by finding the matrix of minors (each element replaced with
    * its minor), then multiplying by a checkered mask of alternating 1, -1
    *
    * @param a matrix on which to operate
    *
    * @return matrix of cofactors
    * **/
inline Matrix3 cofact(const Matrix3& a) { return Matrix3{ minor(a, 0), -minor(a, 1), minor(a, 2), -minor(a, 3), minor(a, 4), -minor(a, 5), minor(a, 6), -minor(a, 7), minor(a, 8) }; }

/**
    * calculate adjoint of a matrix
    *
    * - by finding the transpose of the matrix of cofactors of
    * the input matrix
    *
    * @param a matrix on which to operate
    *
    * @return adjoint matrix of the input
    * **/
inline Matrix3 adj(const Matrix3& a) { return -cofact(a); }
