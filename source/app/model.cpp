// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/app/model.hpp"
#include "feth/core/game_session.hpp"
#include "feth/core/messages.hpp"

#include <exception>

namespace feth::app {

void Model::start() {
  initialized_ = core::initialize();
  detected_locale_ = core::detect_locale();
  refresh();
}

void Model::stop() {
  if (initialized_) {
    core::shutdown();
    initialized_ = false;
  }
}

bool Model::refresh() {
  error_.clear();
  if (!initialized_) {
    status_ = "Cheat service unavailable";
    return false;
  }

  if (!core::gameIsRunning()) {
    status_ = "Start Three Houses v1.2.0";
    return false;
  }

  status_ = "Three Houses v1.2.0 ready";
  return true;
}

bool Model::apply(const std::function<void()>& action) {
  if (!refresh()) {
    return false;
  }

  try {
    action();
    status_ = "Changes applied";
    return true;
  } catch (const std::exception& error) {
    showError(error.what());
    return false;
  }
}

void Model::showError(const std::string& message) {
  status_ = "Failed";
  error_ = message;
}

std::string Model::status() const {
  auto result = core::ui_text(status_, display_locale());
  if (!error_.empty()) {
    result += ": " + error_;
  }
  return result;
}

ItemSettings& Model::items() {
  return items_;
}

core::Locale Model::display_locale() const {
  return core::resolve_locale(language_mode_, detected_locale_);
}

core::LocaleMode Model::language_mode() const {
  return language_mode_;
}

void Model::set_language(core::LocaleMode mode) {
  language_mode_ = mode;
}

void Model::cycle_language() {
  language_mode_ = core::next_locale_mode(language_mode_);
}

}  // namespace feth::app
