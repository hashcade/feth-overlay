// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include <cstdint>
#include <string_view>

namespace feth::core {

enum class Locale : std::uint8_t { English, SimplifiedChinese, Japanese };
enum class LocaleMode : std::uint8_t {
  Auto,
  English,
  SimplifiedChinese,
  Japanese
};

Locale resolve_locale(LocaleMode mode, Locale detected);
Locale locale_from_switch_language(std::int32_t language);
LocaleMode next_locale_mode(LocaleMode mode);
LocaleMode parse_locale_mode(std::string_view value);
const char* locale_mode_value(LocaleMode mode);

}  // namespace feth::core
