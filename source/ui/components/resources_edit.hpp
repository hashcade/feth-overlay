// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

class ResourcesEditGui final : public MenuGui {
public:
  explicit ResourcesEditGui(Model& model)
    : MenuGui(model, "Money & Renown") {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto resources = game::getPlayerResources();
    money_ = resources.money;
    renown_ = resources.renown;

    list->addItem(
      numeric_item(text(model_, "Money"), money_, {0, core::MAX_MONEY, 10000})
    );
    list->addItem(
      numeric_item(text(model_, "Renown"), renown_, {0, core::MAX_RENOWN, 1000})
    );

    list->addItem(action_item(model_, text(model_, "Apply Changes"), [this] {
      game::setPlayerResources(
        {static_cast<std::uint32_t>(money_),
         static_cast<std::uint32_t>(renown_)}
      );
    }));
  }

private:
  int money_{};
  int renown_{};
};
