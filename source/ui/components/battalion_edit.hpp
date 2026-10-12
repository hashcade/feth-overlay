// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

class BattalionEditGui final : public MenuGui {
public:
  explicit BattalionEditGui(Model& model)
    : MenuGui(model, "Battalion Edit") {}

protected:
  void populate(tsl::elm::List* list) override {
    list->addItem(action_item(model_, text(model_, "Refill Endurance"), [] {
      game::refillBattalionStamina();
    }));
    list->addItem(action_item(model_, text(model_, "Max All"), [] {
      game::maxBattalionLevels();
    }));
    list->addItem(action_item(model_, text(model_, "Fill Missing Types"), [] {
      game::fillMissingBattalions();
    }));
  }
};
