#pragma once

#include <math.h>
#include <iostream>

struct vector2
{
	float x, y;

	inline void operator+=(const vector2& a) { x += a.x; y += a.y; }
	inline void operator-=(const vector2& a) { x -= a.x; y -= a.y; }
	inline void operator*=(const vector2& a) { x *= a.x; y *= a.y; }
	inline void operator/=(const vector2& a) { x /= a.x; y /= a.y; }
	inline void operator*=(const float f) { x *= f; y *= f; }
	inline void operator/=(const float f) { x /= f; y /= f; }
};

inline vector2 operator+(const vector2& a, const vector2& b) { return vector2{ a.x + b.x, a.y + b.y }; }
inline vector2 operator-(const vector2& a, const vector2& b) { return vector2{ a.x - b.x, a.y - b.y }; }
inline vector2 operator*(const vector2& a, const vector2& b) { return vector2{ a.x * b.x, a.y * b.y }; }
inline vector2 operator/(const vector2& a, const vector2& b) { return vector2{ a.x / b.x, a.y / b.y }; }
inline vector2 operator*(const vector2& a, const float f) { return vector2{ a.x * f, a.y * f }; }
inline vector2 operator/(const vector2& a, const float f) { return vector2{ a.x / f, a.y / f }; }

inline float operator^(const vector2& a, const vector2& b) { return (a.x * b.x) + (a.y * b.y); }

inline float sq_mag(const vector2& a) { return a ^ a; }
inline float mag(const vector2& a) { return sqrt(sq_mag(a)); }
inline vector2 norm(const vector2& a) { return a / mag(a); }

inline std::ostream& operator<<(std::ostream& stream, const vector2& v) { return stream << '(' << v.x << ", " << v.y << ')'; }