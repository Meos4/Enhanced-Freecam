#pragma once

#if _WIN32
#include "Windows/Process.hpp"
using Process = Windows::Process;
#endif