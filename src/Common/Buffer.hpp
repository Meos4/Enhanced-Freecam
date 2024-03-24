#pragma once

#include "Types.hpp"

#include <array>
#include <vector>

using Buffer = std::vector<u8>;

template <std::size_t Size>
using SBuffer = std::array<u8, Size>;