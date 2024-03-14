#pragma once

#include <math.h>
#include <iostream>

struct vector3
{
	float x, y, z;

	inline void operator+=(const vector3& a) { x += a.x; y += a.y; z += a.z; }
	inline void operator-=(const vector3& a) { x -= a.x; y -= a.y; z -= a.z; }
	inline void operator*=(const vector3& a) { x *= a.x; y *= a.y; z *= a.z; }
	inline void operator/=(const vector3& a) { x /= a.x; y /= a.y; z /= a.z; }
	inline void operator*=(const float f) { x *= f; y *= f; z *= f; }
	inline void operator/=(const float f) { x /= f; y /= f; z /= f; }
};

inline vector3 operator+(const vector3& a, const vector3& b) { return vector3{ a.x + b.x, a.y + b.y, a.z + b.z }; }
inline vector3 operator-(const vector3& a, const vector3& b) { return vector3{ a.x - b.x, a.y - b.y, a.z - b.z }; }
inline vector3 operator*(const vector3& a, const vector3& b) { return vector3{ a.x * b.x, a.y * b.y, a.z * b.z }; }
inline vector3 operator/(const vector3& a, const vector3& b) { return vector3{ a.x / b.x, a.y / b.y, a.z / b.z }; }
inline vector3 operator*(const vector3& a, const float f) { return vector3{ a.x * f, a.y * f, a.z * f }; }
inline vector3 operator/(const vector3& a, const float f) { return vector3{ a.x / f, a.y / f, a.z / f }; }

inline float operator^(const vector3& a, const vector3& b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }

inline float sq_mag(const vector3& a) { return a ^ a; }
inline float mag(const vector3& a) { return sqrt(sq_mag(a)); }
inline vector3 norm(const vector3& a) { return a / mag(a); }

inline std::ostream& operator<<(std::ostream& stream, const vector3& v) { return stream << '(' << v.x << ", " << v.y << ", " << v.z << ')'; }