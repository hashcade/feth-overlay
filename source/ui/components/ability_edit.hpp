// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan
#pragma once

class LearnedAbilitiesGui final : public MenuGui {
public:
  LearnedAbilitiesGui(Model& model, core::RosterEntry entry)
    : MenuGui(
        model,
        core::character_name(entry.name, model.display_locale()) + " - " +
          text(model, "Ability Learning")
      ),
      entry_(std::move(entry)) {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto abilities = game::getCharacterAbilities(entry_.index);
    for (const auto id : core::ability_ids()) {
      auto* item = new tsl::elm::ToggleListItem(
        core::ability_name(id, model_.display_locale()),
        core::ability_is_learned(abilities, id),
        text(model_, "Learned"),
        text(model_, "Not Learned")
      );
      item->setStateChangedListener([this, id, item](bool learned) {
        if (!model_.apply([this, id, learned] {
              game::setCharacterAbilityLearned(entry_.index, id, learned);
            }))
          item->setState(!learned);
      });
      list->addItem(item);
    }
  }

private:
  core::RosterEntry entry_;
};

class AbilityEditGui final : public MenuGui {
public:
  explicit AbilityEditGui(Model& model)
    : MenuGui(model, "Ability Learning") {}

protected:
  void populate(tsl::elm::List* list) override {
    for (const auto& entry : game::getRosterEntries()) {
      list->addItem(
        submenu_item<LearnedAbilitiesGui>(
          model_,
          core::character_name(entry.name, model_.display_locale()),
          entry
        )
      );
    }
  }
};
