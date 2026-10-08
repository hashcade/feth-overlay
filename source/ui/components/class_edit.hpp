// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays.
// Modifications Copyright (c) 2026 Jing Haihan

#pragma once

class ClassListGui final : public MenuGui {
public:
  ClassListGui(Model& model, core::RosterEntry entry)
    : MenuGui(
        model,
        core::character_name(entry.name, model.display_locale()) + " - " +
          text(model, "Classes")
      ),
      entry_(std::move(entry)) {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto unlocks =
      game::getRosterCharacterClassUnlockAtIndex(entry_.index);
    for (const auto& category : core::NAMED_CLASS_ID_LIST_LIST) {
      category_header(list, text(model_, category.name));
      for (const auto& character_class : category.list) {
        auto name =
          core::class_name(character_class.id, model_.display_locale());
        for (const auto* gender : {"♂", "♀"}) {
          if (character_class.name.ends_with(gender))
            name += gender;
        }
        auto* item = new tsl::elm::ToggleListItem(
          name,
          core::classIsUnlocked(unlocks, character_class.id),
          text(model_, "On"),
          text(model_, "Off")
        );
        item->setStateChangedListener(
          [this, item, id = character_class.id](bool unlocked) {
            if (!model_.apply([this, id, unlocked] {
                  game::setClassUnlockAtIndexOfRosterCharacterAtIndex(
                    entry_.index, id, unlocked
                  );
                })) {
              item->setState(!unlocked);
            }
          }
        );
        list->addItem(item);
      }
    }
  }

private:
  core::RosterEntry entry_;
};

class ClassEditGui final : public MenuGui {
public:
  explicit ClassEditGui(Model& model)
    : MenuGui(model, "Class Edit") {}

protected:
  void populate(tsl::elm::List* list) override {
    for (const auto& entry : game::getRosterEntries()) {
      list->addItem(
        submenu_item<ClassListGui>(
          model_,
          core::character_name(entry.name, model_.display_locale()),
          entry
        )
      );
    }
  }
};
