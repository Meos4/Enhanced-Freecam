#pragma once

#include "TemplateTypes.hpp"

#include <algorithm>
#include <limits>
#include <numbers>

namespace Math
{
	inline constexpr double
		// PI / 180
		radPerDeg{ 0.01745329251994329576923690768489 },
		// 180 / PI
		degPerRad{ 57.295779513082320876798154814105 };

	inline constexpr auto pi{ std::numbers::pi_v<float> };

	template <FloatingPoint T>
	[[nodiscard]] constexpr T toRadians(T degrees)
	{
		return degrees * static_cast<T>(radPerDeg);
	}

	template <FloatingPoint T>
	[[nodiscard]] constexpr T toDegrees(T radians)
	{
		return radians * static_cast<T>(degPerRad);
	}

	template <Arithmetic T>
	[[nodiscard]] constexpr T wrap(T val, T min, T max)
	{
		if (val > max)
		{
			return min + (val - max);
		}
		else if (val < min)
		{
			return max + (val - min);
		}
		return val;
	}

	template <FloatingPoint T>
	[[nodiscard]] constexpr T wrapPi(T val)
	{
		static constexpr auto piT{ std::numbers::pi_v<T> };
		return Math::wrap(val, -piT, piT);
	}

	template <FloatingPoint FP, Integral Int>
	[[nodiscard]] constexpr FP normalizeInt(Int val)
	{
		return val / static_cast<FP>(std::numeric_limits<Int>::max());
	}
}