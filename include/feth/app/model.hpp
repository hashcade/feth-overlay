// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

#include "feth/core/locale.hpp"

#include <functional>
#include <string>

namespace feth::app {

class Model {
public:
  void start();
  void stop();
  bool refresh();
  bool apply(const std::function<void()>& action);
  void showError(const std::string& message);

  std::string status() const;
  std::string game_version() const;
  core::Locale display_locale() const;
  core::LocaleMode language_mode() const;
  void set_language(core::LocaleMode mode);
  void cycle_language();

private:
  bool initialized_{};
  std::string status_;
  std::string game_version_;
  std::string error_;
  core::Locale detected_locale_{core::Locale::English};
  core::LocaleMode language_mode_{core::LocaleMode::Auto};
};

}  // namespace feth::app
