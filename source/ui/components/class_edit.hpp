// SPDX-License-Identifier: GPL-2.0-only
// Derived from 3096/feth-overlays.
// Modifications Copyright (c) 2026 Jing Haihan

#pragma once

class ClassListGui final : public MenuGui {
public:
  ClassListGui(Model& model, core::RosterEntry entry)
    : MenuGui(model, entry.name + " - Classes"),
      entry_(std::move(entry)) {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto unlocks =
      game::getRosterCharacterClassUnlockAtIndex(entry_.index);
    for (const auto& category : core::NAMED_CLASS_ID_LIST_LIST) {
      category_header(list, category.name);
      for (const auto& character_class : category.list) {
        auto* item = new tsl::elm::ToggleListItem(
          character_class.name,
          core::classIsUnlocked(unlocks, character_class.id)
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
      list->addItem(submenu_item<ClassListGui>(model_, entry.name, entry));
    }
  }
};
