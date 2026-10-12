// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/types.hpp"

#include <span>
#include <string_view>
#include <vector>

namespace feth::core {

struct SupportRank {
  std::string_view name;
  SupportPoint points;
};

std::span<const SupportRank> support_rank_presets();
std::uint8_t support_rank_mask(std::size_t index);
std::vector<SupportRank> available_support_ranks(std::size_t index);
SupportRank support_rank(std::size_t index, SupportPoint points);
SupportPoint max_support_points(std::size_t index);
SupportPoint
step_support_rank(std::size_t index, SupportPoint points, int step);

}  // namespace feth::core
