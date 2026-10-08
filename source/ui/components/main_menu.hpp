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
    language_item_ = new tsl::elm::ListItem(text(model_, "Language"));
    language_item_->setValue(language_value(model_));
    language_item_->setClickListener([this](u64 keys) {
      if ((keys & HidNpadButton_A) == 0)
        return false;
      model_.cycle_language();
      save_language(model_);
      refresh_labels();
      return true;
    });
    list->addItem(language_item_);

    item_menu_ =
      submenu_item<ItemTrainerGui>(model_, text(model_, "Item Trainer"));
    class_menu_ =
      submenu_item<ClassEditGui>(model_, text(model_, "Class Edit"));
    support_menu_ =
      submenu_item<SupportGui>(model_, text(model_, "Support Edit"));
    battalion_menu_ =
      submenu_item<BattalionEditGui>(model_, text(model_, "Battalion Edit"));
    list->addItem(item_menu_);
    list->addItem(class_menu_);
    list->addItem(support_menu_);
    list->addItem(battalion_menu_);
  }

private:
  void refresh_labels() {
    language_item_->setText(text(model_, "Language"));
    language_item_->setValue(language_value(model_));
    item_menu_->setText(text(model_, "Item Trainer"));
    class_menu_->setText(text(model_, "Class Edit"));
    support_menu_->setText(text(model_, "Support Edit"));
    battalion_menu_->setText(text(model_, "Battalion Edit"));
  }

  tsl::elm::ListItem* language_item_{};
  tsl::elm::ListItem* item_menu_{};
  tsl::elm::ListItem* class_menu_{};
  tsl::elm::ListItem* support_menu_{};
  tsl::elm::ListItem* battalion_menu_{};
  std::chrono::steady_clock::time_point last_refresh_{};
};
