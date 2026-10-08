// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

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

  const std::string& status() const;
  ItemSettings& items();

private:
  bool initialized_{};
  std::string status_;
  ItemSettings items_;
};

}  // namespace feth::app
