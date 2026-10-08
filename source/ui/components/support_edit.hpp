// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

class SupportEditGui final : public MenuGui {
public:
  SupportEditGui(Model& model, core::SupportEntry entry)
    : MenuGui(model, entry.p_pair->first + " x " + entry.p_pair->second),
      entry_(entry) {}

protected:
  void populate(tsl::elm::List* list) override {
    points_ = game::getSupportPointAtIndex(entry_.index);
    list->addItem(numeric_item("Support Points", points_, 0, 9999, 100));
    list->addItem(action_item(model_, "Apply Support", [this] {
      game::setSupportPointAtIndex(
        entry_.index, static_cast<core::SupportPoint>(points_)
      );
    }));
  }

private:
  core::SupportEntry entry_;
  int points_{};
};

class SupportListGui final : public MenuGui {
public:
  SupportListGui(Model& model, core::SupportList entries)
    : MenuGui(model, entries.displayName + " - Support"),
      entries_(std::move(entries)) {}

  void update() override {
    if (pending_item_ == nullptr) {
      return;
    }
    if (model_.refresh()) {
      try {
        pending_item_->setValue(
          std::to_string(game::getSupportPointAtIndex(pending_index_))
        );
      } catch (const std::exception& error) {
        model_.showError(error.what());
      }
    }
    pending_item_ = nullptr;
  }

protected:
  void populate(tsl::elm::List* list) override {
    for (const auto& entry : entries_.list) {
      const auto& pair = *entry->p_pair;
      auto* item = new tsl::elm::ListItem(
        entries_.displayName == pair.first ? pair.second : pair.first
      );
      item->setValue(
        std::to_string(game::getSupportPointAtIndex(entry->index))
      );
      item->setClickListener([this, item, entry](u64 keys) {
        if ((keys & HidNpadButton_A) == 0) {
          return false;
        }
        pending_item_ = item;
        pending_index_ = entry->index;
        tsl::changeTo<SupportEditGui>(model_, *entry);
        return true;
      });
      list->addItem(item);
    }
  }

private:
  core::SupportList entries_;
  tsl::elm::ListItem* pending_item_{};
  std::size_t pending_index_{};
};

class SupportGui final : public MenuGui {
public:
  explicit SupportGui(Model& model)
    : MenuGui(model, "Support Edit") {}

protected:
  void populate(tsl::elm::List* list) override {
    const auto collection = core::getSupportCollection();
    for (const auto& character : collection.supportListList) {
      list->addItem(
        submenu_item<SupportListGui>(model_, character.displayName, character)
      );
    }
  }
};
