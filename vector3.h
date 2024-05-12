#pragma once

#include <math.h>
#include <iostream>

struct Vector3
{
	float x, y, z;

	inline void operator+=(const Vector3& a) { x += a.x; y += a.y; z += a.z; }
	inline void operator-=(const Vector3& a) { x -= a.x; y -= a.y; z -= a.z; }
	inline void operator*=(const Vector3& a) { x *= a.x; y *= a.y; z *= a.z; }
	inline void operator/=(const Vector3& a) { x /= a.x; y /= a.y; z /= a.z; }
	inline void operator*=(const float f) { x *= f; y *= f; z *= f; }
	inline void operator/=(const float f) { x /= f; y /= f; z /= f; }
};

inline Vector3 operator+(const Vector3& a, const Vector3& b) { return Vector3{ a.x + b.x, a.y + b.y, a.z + b.z }; }
inline Vector3 operator-(const Vector3& a, const Vector3& b) { return Vector3{ a.x - b.x, a.y - b.y, a.z - b.z }; }
inline Vector3 operator*(const Vector3& a, const Vector3& b) { return Vector3{ a.x * b.x, a.y * b.y, a.z * b.z }; }
inline Vector3 operator/(const Vector3& a, const Vector3& b) { return Vector3{ a.x / b.x, a.y / b.y, a.z / b.z }; }
inline Vector3 operator*(const Vector3& a, const float f) { return Vector3{ a.x * f, a.y * f, a.z * f }; }
inline Vector3 operator/(const Vector3& a, const float f) { return Vector3{ a.x / f, a.y / f, a.z / f }; }

inline float operator^(const Vector3& a, const Vector3& b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
inline Vector3 operator%(const Vector3& a, const Vector3& b) { return Vector3{ (a.y* b.z) - (a.z * b.y), (a.z* b.x) - (a.x * b.z), (a.x* b.y) - (a.y * b.x) }; }

inline float sq_mag(const Vector3& a) { return a ^ a; }
inline float mag(const Vector3& a) { return sqrt(sq_mag(a)); }
inline Vector3 norm(const Vector3& a) { return mag(a) > 0 ? a / mag(a) : Vector3{ 0,0,0 }; }

inline std::ostream& operator<<(std::ostream& stream, const Vector3& v) { return stream << '(' << v.x << ", " << v.y << ", " << v.z << ')'; }

inline float min(float a, float b, float c)
{
    return std::min(std::min(a, b), c);
}

inline float max(float a, float b, float c)
{
    return std::max(std::max(a, b), c);
}

inline Vector3 rgb_to_hsv(const Vector3& rgb)
{
    Vector3 hsv{ 0,0,0 };
    float mx = max(rgb.x, rgb.y, rgb.z);
    float mn = min(rgb.x, rgb.y, rgb.z);

    hsv.z = mx;
    if (mx != 0) hsv.y = (mx - mn) / mx;
    if (hsv.z == 0) return hsv;

    float d = mx - mn;
    if (rgb.x == mx) hsv.x = (rgb.y - rgb.z) / d;
    else if (rgb.y == mx) hsv.x = 2 + ((rgb.z - rgb.x) / d);
    else if (rgb.z == mx) hsv.x = 4 + ((rgb.x - rgb.y) / d);
    hsv.x /= 6.0f;

    if (hsv.x < 0) hsv.x += 1.0f;
    if (hsv.x > 1) hsv.x -= 1.0f;

    return hsv;
}

inline Vector3 hsv_to_rgb(const Vector3& hsv)
{
    if (hsv.y == 0) return Vector3{ hsv.z,hsv.z,hsv.z };
    uint8_t sextant = (uint8_t)(hsv.x * 6.0f);
    float fraction = (hsv.x * 6.0f) - sextant;
    float p = hsv.z * (1.0f - hsv.y);
    float q = hsv.z * (1.0f - (hsv.y * fraction));
    float t = hsv.z * (1.0f - (hsv.y * (1.0f - fraction)));
    switch (sextant)
    {
    case 0: return { hsv.z,t,p };
    case 1: return { q,hsv.z,p };
    case 2: return { p,hsv.z,t };
    case 3: return { p,q,hsv.z };
    case 4: return { t,p,hsv.z };
    case 5: return { hsv.z,p,q };
    }
    return Vector3{ 0,0,0 };
}