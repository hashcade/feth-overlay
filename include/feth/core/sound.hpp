// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace feth::core {

// Decode installed sound packs to 48 kHz stereo PCM; no audio is bundled.
std::vector<std::int16_t>
decode_confirmation_sound(std::span<const std::byte> wav);

}  // namespace feth::core
