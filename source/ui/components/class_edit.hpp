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
    items_.clear();
    list->addItem(action_item(model_, text(model_, "Unlock All"), [this] {
      game::unlockAvailableClasses(entry_.index);
      refresh_items();
    }));

    const auto unlocks =
      game::getRosterCharacterClassUnlockAtIndex(entry_.index);
    for (const auto& category : core::NAMED_CLASS_ID_LIST_LIST) {
      const auto available = [this](const core::NamedClassId& character_class) {
        return core::class_is_available(entry_.id, character_class.id);
      };
      if (std::none_of(category.list.begin(), category.list.end(), available))
        continue;
      category_header(list, text(model_, category.name));
      for (const auto& character_class : category.list) {
        if (!available(character_class))
          continue;
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
        items_.push_back({character_class.id, item});
      }
    }
  }

private:
  void refresh_items() {
    const auto unlocks =
      game::getRosterCharacterClassUnlockAtIndex(entry_.index);
    for (const auto& [id, item] : items_)
      item->setState(core::classIsUnlocked(unlocks, id));
  }

  core::RosterEntry entry_;
  std::vector<std::pair<core::ClassId, tsl::elm::ToggleListItem*>> items_;
};

class ClassEditGui final : public MenuGui {
public:
  explicit ClassEditGui(Model& model)
    : MenuGui(model, "Class Unlocks") {}

protected:
  void populate(tsl::elm::List* list) override {
    for (const auto& entry : game::getRosterEntries()) {
      const auto available = core::available_class_unlocks(entry.id);
      if (
        available.baseClassUnlocks.none() &&
        available.specialClassUnlocks.none()
      )
        continue;
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
