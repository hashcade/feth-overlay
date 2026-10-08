// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

class MainGui final : public MenuGui {
public:
  explicit MainGui(Model& model)
    : MenuGui(model, "FETH Overlay", false) {}

  void update() override {
    const auto now = std::chrono::steady_clock::now();
    if (now - last_refresh_ >= std::chrono::milliseconds(500)) {
      model_.refresh();
      last_refresh_ = now;
    }
  }

protected:
  void populate(tsl::elm::List* list) override {
    list->addItem(submenu_item<ItemTrainerGui>(model_, "Item Trainer"));
    list->addItem(submenu_item<ClassEditGui>(model_, "Class Edit"));
    list->addItem(submenu_item<SupportGui>(model_, "Support Edit"));
  }

private:
  std::chrono::steady_clock::time_point last_refresh_{};
};
