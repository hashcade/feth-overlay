// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/locale.hpp"
#include "feth/core/types.hpp"

#include <functional>
#include <string>

namespace feth::app {

struct ItemSettings {
  bool addMissing{};
  bool maxDurability{};
  bool maxAmount{true};
  int id{};
  int durability{core::MAX_ITEM_DURABILITY};
  int amount{core::MAX_ITEM_AMOUNT};
};

class Model {
public:
  void start();
  void stop();
  bool refresh();
  bool apply(const std::function<void()>& action);
  void showError(const std::string& message);

  std::string status() const;
  ItemSettings& items();
  core::Locale display_locale() const;
  core::LocaleMode language_mode() const;
  void set_language(core::LocaleMode mode);
  void cycle_language();

private:
  bool initialized_{};
  std::string status_;
  std::string error_;
  ItemSettings items_;
  core::Locale detected_locale_{core::Locale::English};
  core::LocaleMode language_mode_{core::LocaleMode::Auto};
};

}  // namespace feth::app
