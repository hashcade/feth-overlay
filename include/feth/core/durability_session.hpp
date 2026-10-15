// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace feth::core {

struct ProcessMemory {
  std::function<void(std::uint64_t, std::span<std::byte>)> read;
  std::function<void(std::uint64_t, std::span<const std::byte>)> write;
};

void refill_item_durability(const ProcessMemory& memory, std::uint64_t main);

}  // namespace feth::core
