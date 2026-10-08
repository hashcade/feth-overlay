// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/app/model.hpp"
#include "feth/core/game_session.hpp"

#include <exception>

namespace feth::app {

void Model::start() {
  initialized_ = core::initialize();
  refresh();
}

void Model::stop() {
  if (initialized_) {
    core::shutdown();
    initialized_ = false;
  }
}

bool Model::refresh() {
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
  status_ = "Failed: " + message;
}

const std::string& Model::status() const {
  return status_;
}

ItemSettings& Model::items() {
  return items_;
}

}  // namespace feth::app
