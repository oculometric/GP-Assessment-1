#pragma once

#include "vector3.h"

struct matrix3;

inline float det(const matrix3& a);
inline matrix3 cofact(const matrix3& a);
inline matrix3 adj(const matrix3& a);
inline matrix3 operator/(const matrix3& a, const float b);

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
struct matrix3
{
    float x_0, y_0, z_0;
    float x_1, y_1, z_1;
    float x_2, y_2, z_2;

    inline vector3 col_0() const { return vector3{ x_0, x_1, x_2 }; }
    inline vector3 col_1() const { return vector3{ y_0, y_1, y_2 }; }
    inline vector3 col_2() const { return vector3{ z_0, z_1, z_2 }; }
    inline vector3 row_0() const { return vector3{ x_0, y_0, z_0 }; }
    inline vector3 row_1() const { return vector3{ x_1, y_1, z_1 }; }
    inline vector3 row_2() const { return vector3{ x_2, y_2, z_2 }; }

    inline void operator=(const matrix3& a) { x_0 = a.x_0; y_0 = a.y_0; z_0 = a.z_0; x_1 = a.x_1; y_1 = a.y_1; z_1 = a.z_1; x_2 = a.x_2; y_2 = a.y_2; z_2 = a.z_2; }
    inline void operator+=(const matrix3& a) { x_0 += a.x_0; y_0 += a.y_0; z_0 += a.z_0; x_1 += a.x_1; y_1 += a.y_1; z_1 += a.z_1; x_2 += a.x_2; y_2 += a.y_2; z_2 += a.z_2; }
    inline void operator-=(const matrix3& a) { x_0 -= a.x_0; y_0 -= a.y_0; z_0 -= a.z_0; x_1 -= a.x_1; y_1 -= a.y_1; z_1 -= a.z_1; x_2 -= a.x_2; y_2 -= a.y_2; z_2 -= a.z_2; }
    inline void operator*=(const matrix3& a)
    {
        vector3 r0 = row_0();
        vector3 r1 = row_1();
        vector3 r2 = row_2();
        vector3 c0 = a.col_0();
        vector3 c1 = a.col_1();
        vector3 c2 = a.col_2();
        x_0 = r0 ^ c0; y_0 = r0 ^ c1; z_0 = r0 ^ c2;
        x_1 = r1 ^ c0; y_1 = r1 ^ c1; z_1 = r1 ^ c2;
        x_2 = r2 ^ c0; y_2 = r2 ^ c1; z_2 = r2 ^ c2;
    }
    inline void operator*=(const float a) { x_0 *= a; y_0 *= a; z_0 *= a; x_1 *= a; y_1 *= a; z_1 *= a; x_2 *= a; y_2 *= a; z_2 *= a; }
    inline void operator/=(const float a) { x_0 /= a; y_0 /= a; z_0 /= a; x_1 /= a; y_1 /= a; z_1 /= a; x_2 /= a; y_2 /= a; z_2 /= a; }
    inline matrix3 operator-() const { return matrix3{ x_0, x_1, x_2, y_0, y_1, y_2, z_0, z_1, z_2 }; }
    inline matrix3 operator~() const { return adj(*this) / det(*this); }
};

inline matrix3 operator+(const matrix3& a, const matrix3& b)
{
    return matrix3
    {
        a.x_0 + b.x_0, a.y_0 + b.y_0, a.z_0 + b.z_0,
            a.x_1 + b.x_1, a.y_1 + b.y_1, a.z_1 + b.z_1,
            a.x_2 + b.x_2, a.y_2 + b.y_2, a.z_2 + b.z_2
    };
}

inline matrix3 operator-(const matrix3& a, const matrix3& b)
{
    return matrix3
    {
        a.x_0 - b.x_0, a.y_0 - b.y_0, a.z_0 - b.z_0,
            a.x_1 - b.x_1, a.y_1 - b.y_1, a.z_1 - b.z_1,
            a.x_2 - b.x_2, a.y_2 - b.y_2, a.z_2 - b.z_2
    };
}

inline matrix3 operator*(const matrix3& a, const matrix3& b)
{
    vector3 r0 = a.row_0();
    vector3 r1 = a.row_1();
    vector3 r2 = a.row_2();
    vector3 c0 = b.col_0();
    vector3 c1 = b.col_1();
    vector3 c2 = b.col_2();
    return matrix3
    {
        r0^ c0, r0^ c1, r0^ c2,
            r1^ c0, r1^ c1, r1^ c2,
            r2^ c0, r2^ c1, r2^ c2
    };
}

inline vector3 operator*(const matrix3& a, const vector3& b)
{
    return vector3{ a.row_0() ^ b, a.row_1() ^ b, a.row_2() ^ b };
}

inline matrix3 operator*(const matrix3& a, const float b)
{
    return matrix3
    {
        a.x_0* b, a.y_0* b, a.z_0* b,
            a.x_1* b, a.y_1* b, a.z_1* b,
            a.x_2* b, a.y_2* b, a.z_2* b
    };
}

inline matrix3 operator/(const matrix3& a, const float b)
{
    return matrix3
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
inline float minor(const matrix3& a, const uint8_t e)
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
inline float det(const matrix3& a) { return ((a.x_0 * minor(a, 0)) - (a.y_0 * minor(a, 1))) + (a.z_0 * minor(a, 2)); }

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
inline matrix3 cofact(const matrix3& a) { return matrix3{ minor(a, 0), -minor(a, 1), minor(a, 2), -minor(a, 3), minor(a, 4), -minor(a, 5), minor(a, 6), -minor(a, 7), minor(a, 8) }; }

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
inline matrix3 adj(const matrix3& a) { return -cofact(a); }
