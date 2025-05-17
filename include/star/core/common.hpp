#ifndef ENGINE_COMMON_HPP
#define ENGINE_COMMON_HPP

// clang-format off

#ifdef _WIN32
    #define NOMINMAX
    // Try to include the system sdkddkver.h first, fallback to our custom one
    #if __has_include(<sdkddkver.h>)
        #include <sdkddkver.h>
    #else
        #include "sdkddkver.h"
    #endif
    #include <winsock2.h>
    #include <windows.h>
    #define STAR_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #define STAR_PLATFORM_APPLE
    #if TARGET_OS_IPHONE
        #define STAR_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define STAR_PLATFORM_MACOS
    #endif
#elif defined(__linux__)
    #define STAR_PLATFORM_LINUX
#elif defined(__ANDROID__)
    #define STAR_PLATFORM_ANDROID
#endif

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <memory>
#include <new>

#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <list>
#include <forward_list>

#include <algorithm>
#include <functional>
#include <utility>
#include <type_traits>
#include <initializer_list>
#include <tuple>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <limits>
#include <random>
#include <numeric>

// BGFX
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/embedded_shader.h>
#include <bx/timer.h>

// SDL3
#include <SDL3/SDL.h>

namespace star {
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using f32 = float;
    using f64 = double;
    using byte = u8;
}

using namespace star;

#define STAR_VERSION_MAJOR 0
#define STAR_VERSION_MINOR 1
#define STAR_VERSION_PATCH 0

#if defined(_DEBUG) || defined(DEBUG)
    #define STAR_DEBUG
    #define STAR_ASSERT(x, msg) assert((x) && (msg))
#else
    #define STAR_ASSERT(x, msg) ((void)0)
#endif

// clang-format on

#endif
