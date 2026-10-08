// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/locale.hpp"

namespace feth::core {

Locale resolve_locale(LocaleMode mode, Locale detected) {
  switch (mode) {
    case LocaleMode::English:
      return Locale::English;
    case LocaleMode::SimplifiedChinese:
      return Locale::SimplifiedChinese;
    case LocaleMode::Japanese:
      return Locale::Japanese;
    default:
      return detected;
  }
}

Locale locale_from_switch_language(std::int32_t language) {
  switch (language) {
    case 0:
      return Locale::Japanese;
    case 6:
    case 11:
    case 15:
    case 16:
      return Locale::SimplifiedChinese;
    default:
      return Locale::English;
  }
}

LocaleMode next_locale_mode(LocaleMode mode) {
  switch (mode) {
    case LocaleMode::Auto:
      return LocaleMode::English;
    case LocaleMode::English:
      return LocaleMode::SimplifiedChinese;
    case LocaleMode::SimplifiedChinese:
      return LocaleMode::Japanese;
    default:
      return LocaleMode::Auto;
  }
}

LocaleMode parse_locale_mode(std::string_view value) {
  if (value == "en")
    return LocaleMode::English;
  if (value == "zh-Hans")
    return LocaleMode::SimplifiedChinese;
  if (value == "ja")
    return LocaleMode::Japanese;
  return LocaleMode::Auto;
}

const char* locale_mode_value(LocaleMode mode) {
  switch (mode) {
    case LocaleMode::English:
      return "en";
    case LocaleMode::SimplifiedChinese:
      return "zh-Hans";
    case LocaleMode::Japanese:
      return "ja";
    default:
      return "auto";
  }
}

}  // namespace feth::core
