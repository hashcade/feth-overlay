// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/locale.hpp"

#include <string>
#include <string_view>

namespace feth::core {

std::string ui_text(std::string_view english, Locale locale);

}  // namespace feth::core
