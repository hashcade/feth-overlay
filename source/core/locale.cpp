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
    case LocaleMode::TraditionalChinese:
      return Locale::TraditionalChinese;
    case LocaleMode::Korean:
      return Locale::Korean;
    default:
      return detected;
  }
}

Locale locale_from_switch_language(std::int32_t language) {
  switch (language) {
    case 0:
      return Locale::Japanese;
    case 6:
    case 15:
      return Locale::SimplifiedChinese;
    case 11:
    case 16:
      return Locale::TraditionalChinese;
    case 7:
      return Locale::Korean;
    default:
      return Locale::English;
  }
}

Locale locale_from_nacp_language(std::int32_t language) {
  switch (language) {
    case 2:
      return Locale::Japanese;
    case 12:
      return Locale::TraditionalChinese;
    case 13:
      return Locale::SimplifiedChinese;
    case 14:
      return Locale::Korean;
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
      return LocaleMode::TraditionalChinese;
    case LocaleMode::TraditionalChinese:
      return LocaleMode::Japanese;
    case LocaleMode::Japanese:
      return LocaleMode::Korean;
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
  if (value == "zh-Hant")
    return LocaleMode::TraditionalChinese;
  if (value == "ko")
    return LocaleMode::Korean;
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
    case LocaleMode::TraditionalChinese:
      return "zh-Hant";
    case LocaleMode::Korean:
      return "ko";
    default:
      return "auto";
  }
}

}  // namespace feth::core
