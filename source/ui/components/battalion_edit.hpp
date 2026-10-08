// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

class BattalionTypeGui final : public MenuGui {
public:
  BattalionTypeGui(Model& model, core::Battalion& battalion)
    : MenuGui(model, "Battalion Type"),
      battalion_(battalion) {}

protected:
  void populate(tsl::elm::List* list) override {
    for (const auto& entry : core::battalion_templates()) {
      auto* item = new tsl::elm::ListItem(
        core::battalion_name(entry.type, model_.display_locale())
      );
      item->setClickListener([this, entry](u64 keys) {
        if (!(keys & HidNpadButton_A))
          return false;
        battalion_.type = entry.type;
        battalion_.skill = entry.skill;
        battalion_.stamina = entry.stamina;
        tsl::goBack();
        return true;
      });
      list->addItem(item);
    }
  }

private:
  core::Battalion& battalion_;
};

class BattalionSlotGui final : public MenuGui {
public:
  BattalionSlotGui(Model& model, std::size_t index)
    : MenuGui(model, "Battalion Edit"),
      index_(index) {}

  void update() override {
    if (!type_item_ || battalion_.type == shown_type_)
      return;
    shown_type_ = battalion_.type;
    stamina_ = battalion_.stamina;
    type_item_->setValue(
      core::battalion_name(shown_type_, model_.display_locale())
    );
    stamina_item_->setMaximum(maximum_stamina());
  }

protected:
  void populate(tsl::elm::List* list) override {
    battalion_ = game::getBattalion(index_);
    shown_type_ = battalion_.type;
    exp_ = battalion_.exp;
    stamina_ = battalion_.stamina;
    type_item_ = new tsl::elm::ListItem(text(model_, "Battalion Type"));
    type_item_->setValue(
      core::battalion_name(shown_type_, model_.display_locale())
    );
    type_item_->setClickListener([this](u64 keys) {
      if (!(keys & HidNpadButton_A))
        return false;
      tsl::changeTo<BattalionTypeGui>(model_, battalion_);
      return true;
    });
    list->addItem(type_item_);
    list->addItem(numeric_item(
      text(model_, "Experience"), exp_, {0, core::MAX_BATTALION_EXP, 100}
    ));
    stamina_item_ = numeric_item(
      text(model_, "Endurance"), stamina_, {0, maximum_stamina(), 10}
    );
    list->addItem(stamina_item_);
    list->addItem(action_item(model_, text(model_, "Refill Endurance"), [this] {
      stamina_ = maximum_stamina();
      stamina_item_->setMaximum(maximum_stamina());
      battalion_.stamina = stamina_;
      battalion_.exp = exp_;
      game::setBattalion(index_, battalion_);
    }));
    list->addItem(action_item(model_, text(model_, "Apply Battalion"), [this] {
      battalion_.stamina = stamina_;
      battalion_.exp = exp_;
      game::setBattalion(index_, battalion_);
    }));
  }

private:
  int maximum_stamina() const {
    const auto* entry = core::battalion_template(battalion_.type);
    return entry ? entry->stamina : std::numeric_limits<std::uint16_t>::max();
  }
  std::size_t index_;
  core::Battalion battalion_{};
  int exp_{};
  int stamina_{};
  std::uint8_t shown_type_{};
  tsl::elm::ListItem* type_item_{};
  NumericListItem* stamina_item_{};
};

class BattalionListGui final : public MenuGui {
public:
  explicit BattalionListGui(Model& model)
    : MenuGui(model, "Owned Battalions") {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto battalions = game::getBattalions();
    for (std::size_t index = 0; index < battalions.size(); ++index) {
      const auto& battalion = battalions[index];
      if (battalion.type >= core::EMPTY_BATTALION_TYPE)
        continue;
      const auto label =
        std::to_string(index + 1) + " · " +
        core::battalion_name(battalion.type, model_.display_locale());
      list->addItem(submenu_item<BattalionSlotGui>(model_, label, index));
    }
  }
};

class BattalionEditGui final : public MenuGui {
public:
  explicit BattalionEditGui(Model& model)
    : MenuGui(model, "Battalion Edit") {}

protected:
  void populate(tsl::elm::List* list) override {
    list->addItem(
      action_item(model_, text(model_, "Refill All Battalions"), [] {
        game::refillBattalionStamina();
      })
    );
    list->addItem(
      action_item(model_, text(model_, "Add Missing Battalions"), [] {
        game::addMissingBattalions();
      })
    );
    list->addItem(
      submenu_item<BattalionListGui>(model_, text(model_, "Owned Battalions"))
    );
  }
};
