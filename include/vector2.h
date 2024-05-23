#pragma once

#include <math.h>
#include <iostream>

struct Vector2
{
	float x, y;

	inline void operator+=(const Vector2& a) { x += a.x; y += a.y; }
	inline void operator-=(const Vector2& a) { x -= a.x; y -= a.y; }
	inline void operator*=(const Vector2& a) { x *= a.x; y *= a.y; }
	inline void operator/=(const Vector2& a) { x /= a.x; y /= a.y; }
	inline void operator*=(const float f) { x *= f; y *= f; }
	inline void operator/=(const float f) { x /= f; y /= f; }
};

inline Vector2 operator+(const Vector2& a, const Vector2& b) { return Vector2{ a.x + b.x, a.y + b.y }; }
inline Vector2 operator-(const Vector2& a, const Vector2& b) { return Vector2{ a.x - b.x, a.y - b.y }; }
inline Vector2 operator*(const Vector2& a, const Vector2& b) { return Vector2{ a.x * b.x, a.y * b.y }; }
inline Vector2 operator/(const Vector2& a, const Vector2& b) { return Vector2{ a.x / b.x, a.y / b.y }; }
inline Vector2 operator*(const Vector2& a, const float f) { return Vector2{ a.x * f, a.y * f }; }
inline Vector2 operator/(const Vector2& a, const float f) { return Vector2{ a.x / f, a.y / f }; }

inline float operator^(const Vector2& a, const Vector2& b) { return (a.x * b.x) + (a.y * b.y); }

inline float sq_mag(const Vector2& a) { return a ^ a; }
inline float mag(const Vector2& a) { return sqrt(sq_mag(a)); }
inline Vector2 norm(const Vector2& a) { return a / mag(a); }

inline std::ostream& operator<<(std::ostream& stream, const Vector2& v) { return stream << '(' << v.x << ", " << v.y << ')'; }