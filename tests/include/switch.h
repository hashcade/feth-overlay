#pragma once

#include <cstddef>
#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using Result = u32;

struct Service;
struct Event;
struct MemoryInfo;

#define R_FAILED(result) ((result) != 0)
#define R_SUCCEEDED(result) ((result) == 0)

constexpr u64 HidNpadButton_X = 1ULL << 2;
constexpr u64 HidNpadButton_Y = 1ULL << 3;
constexpr u64 HidNpadButton_AnyLeft = 1ULL << 4;
constexpr u64 HidNpadButton_AnyRight = 1ULL << 5;
