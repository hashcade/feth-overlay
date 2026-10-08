// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

#include "feth/core/locale.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <string>

namespace feth::core {
struct Battalion {
  std::int16_t characterId;
  std::uint16_t exp;
  std::uint16_t stamina;
  std::uint8_t type;
  std::uint8_t skill;
  bool operator==(const Battalion&) const = default;
};
static_assert(sizeof(Battalion) == 8);
using BattalionArray = std::array<Battalion, 200>;
constexpr std::uint16_t MAX_BATTALION_EXP = 400;
constexpr std::uint8_t EMPTY_BATTALION_TYPE = 200;

struct BattalionTemplate {
  std::uint8_t type;
  std::uint16_t stamina;
  std::uint8_t skill;
};
std::span<const BattalionTemplate> battalion_templates();
const BattalionTemplate* battalion_template(std::uint8_t type);
std::string battalion_name(std::uint8_t type, Locale locale);
std::size_t refill_battalions(std::span<Battalion> battalions);
std::size_t add_missing_battalions(std::span<Battalion> battalions);
}  // namespace feth::core
