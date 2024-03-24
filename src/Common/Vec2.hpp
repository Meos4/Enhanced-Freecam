#pragma once

#include "TemplateTypes.hpp"

template <Arithmetic T>
struct Vec2
{
	T x, y;

	friend constexpr Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	friend constexpr Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	friend constexpr Vec2 operator*(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x * rhs.x, lhs.y * rhs.y);
	}

	friend constexpr Vec2 operator*(const Vec2& vec, T scalar)
	{
		return Vec2(vec.x * scalar, vec.y * scalar);
	}

	constexpr Vec2& operator*=(T scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	constexpr Vec2& operator*=(const Vec2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	constexpr Vec2& operator+=(const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	constexpr Vec2& operator-=(const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
};