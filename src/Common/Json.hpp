#pragma once

#include "TemplateTypes.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>
#include <filesystem>
#include <limits>
#include <optional>
#include <type_traits>

#define JSON_GET(Read, Var) Json::get(Read, #Var, &Var);
#define JSON_GET_MIN_MAX(Read, Var, Min, Max) Json::get(Read, #Var, &Var, Min, Max);
#define JSON_SET(Write, Var) Json::set(Write, #Var, Var);

namespace Json
{
	using Read = nlohmann::json;
	using Write = nlohmann::ordered_json;
	using Exception = nlohmann::json::exception;

	inline constexpr auto exceptionFormat{ "\"{}\" is invalid, Reason: {}" };

	std::optional<Json::Read> read(const std::filesystem::path& path);
	bool overwrite(const Json::Write& json, const std::filesystem::path& path);

	template <typename Key, Arithmetic Val>
	bool get(const Json::Read& json, const Key& key, Val* val, 
		Val min = std::numeric_limits<Val>::min(), Val max = std::numeric_limits<Val>::max())
	{
		if (!json.contains(key))
		{
			return false;
		}

		const Val jsonVal{ json.at(key) };
		*val = std::clamp(jsonVal, min, max);
		return true;
	}

	template <typename Key, UnscopedEnum Enum>
	bool get(const Json::Read& json, const Key& key, Enum* val, auto min, auto max)
	{
		using EnumType = std::underlying_type_t<Enum>;
		return Json::get(json, key, (EnumType*)val, static_cast<EnumType>(min), static_cast<EnumType>(max));
	}

	template <typename Key, typename Val>
	void set(Json::Write* json, const Key& key, const Val& val)
	{
		(*json)[key] = val;
	}
}