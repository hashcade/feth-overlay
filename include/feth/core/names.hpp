// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/locale.hpp"
#include "feth/core/types.hpp"

#include <string>
#include <string_view>

namespace feth::core {

std::string item_name(ItemId id, Locale locale);
std::string class_name(ClassId id, Locale locale);
std::string character_name(std::string_view english, Locale locale);

}  // namespace feth::core
