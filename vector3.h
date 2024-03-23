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
inline Vector3 norm(const Vector3& a) { return a / mag(a); }

inline std::ostream& operator<<(std::ostream& stream, const Vector3& v) { return stream << '(' << v.x << ", " << v.y << ", " << v.z << ')'; }