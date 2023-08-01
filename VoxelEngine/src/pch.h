// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// add headers that you want to pre-compile here
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <unordered_map>
#include <optional>
#include <ostream>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <functional>
#include <map>
#include <set>
#include <limits>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <concepts>
#include <thread>
#include <mutex>

#endif //PCH_H
