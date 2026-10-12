// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/supports.hpp"

namespace feth::core {

std::vector<SupportRank> available_support_ranks(std::size_t index) {
  const auto presets = support_rank_presets();
  std::vector<SupportRank> result{presets.front()};
  const auto mask = support_rank_mask(index);
  for (std::size_t rank = 1; rank < presets.size(); ++rank)
    if (mask & (1 << (rank - 1)))
      result.push_back(presets[rank]);
  return result;
}

SupportRank support_rank(std::size_t index, SupportPoint points) {
  const auto ranks = available_support_ranks(index);
  auto current = ranks.front();
  for (const auto& rank : ranks)
    if (points >= rank.points)
      current = rank;
  return current;
}

SupportPoint max_support_points(std::size_t index) {
  return available_support_ranks(index).back().points;
}

SupportPoint
step_support_rank(std::size_t index, SupportPoint points, int step) {
  const auto ranks = available_support_ranks(index);
  int current{};
  for (std::size_t rank = 1; rank < ranks.size(); ++rank)
    if (points >= ranks[rank].points)
      current = rank;
  const int next = current + step;
  if (next < 0 || next >= static_cast<int>(ranks.size()))
    return points;
  return ranks[next].points;
}

}  // namespace feth::core
