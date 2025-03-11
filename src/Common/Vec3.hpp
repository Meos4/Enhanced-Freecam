#pragma once

#include "TemplateTypes.hpp"

template <Arithmetic T>
struct Vec3
{
	T x, y, z;

	template <typename U>
	operator Vec3<U>() const
	{
		return Vec3<U>{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) };
	}

	friend constexpr Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	friend constexpr Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	friend constexpr Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	friend constexpr Vec3 operator*(const Vec3& vec, T scalar)
	{
		return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	constexpr Vec3& operator*=(T scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	constexpr Vec3& operator*=(const Vec3& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	constexpr Vec3& operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	constexpr Vec3& operator-=(const Vec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
};