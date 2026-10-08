// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

#include <cstdint>

namespace feth::core {
// Match the supported limits in feth-save-editor.
constexpr int MAX_MONEY = 9999999;
constexpr int MAX_RENOWN = 999999;

struct PlayerResources {
  std::uint32_t money;
  std::uint32_t renown;
};
}  // namespace feth::core
