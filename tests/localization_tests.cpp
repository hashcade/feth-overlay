// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/catalog.hpp"
#include "feth/core/locale.hpp"
#include "feth/core/messages.hpp"
#include "feth/core/names.hpp"

#include <cassert>
#include <iostream>

using namespace feth::core;

int main() {
  // Match MHGU's console-language mapping, including legacy Chinese codes.
  assert(locale_from_switch_language(0) == Locale::Japanese);
  for (int code : {6, 11, 15, 16}) {
    assert(locale_from_switch_language(code) == Locale::SimplifiedChinese);
  }
  for (int code : {1, 2, 12, 99}) {
    assert(locale_from_switch_language(code) == Locale::English);
  }
  for (auto locale :
       {Locale::English, Locale::SimplifiedChinese, Locale::Japanese}) {
    assert(resolve_locale(LocaleMode::Auto, locale) == locale);
    assert(resolve_locale(LocaleMode::English, locale) == Locale::English);
    assert(
      resolve_locale(LocaleMode::SimplifiedChinese, locale) ==
      Locale::SimplifiedChinese
    );
    assert(resolve_locale(LocaleMode::Japanese, locale) == Locale::Japanese);
  }

  auto mode = LocaleMode::Auto;
  for (int index = 0; index < 4; ++index) {
    assert(parse_locale_mode(locale_mode_value(mode)) == mode);
    mode = next_locale_mode(mode);
  }
  assert(mode == LocaleMode::Auto);
  assert(parse_locale_mode("unknown") == LocaleMode::Auto);

  // ID-based lookup uses the editor's catalog rather than overlay list order.
  assert(item_name(16, Locale::English) == "Iron Sword");
  assert(item_name(16, Locale::SimplifiedChinese) == "铁剑");
  assert(item_name(16, Locale::Japanese) == "鉄の剣");
  assert(class_name(0, Locale::English) == "Noble");
  assert(class_name(0, Locale::SimplifiedChinese) == "贵族");
  assert(class_name(0, Locale::Japanese) == "貴族");
  assert(item_name(65535, Locale::Japanese) == "ID 65535");
  assert(character_name("Byleth", Locale::SimplifiedChinese) == "贝雷特");
  assert(character_name("Byleth♀", Locale::SimplifiedChinese) == "贝雷丝♀");
  assert(
    character_name("Unlisted character", Locale::Japanese) ==
    "Unlisted character"
  );

  // All shipped class IDs and item groups need names and translated headings.
  for (const auto& group : NAMED_CLASS_ID_LIST_LIST) {
    for (auto locale : {Locale::SimplifiedChinese, Locale::Japanese}) {
      assert(ui_text(group.name, locale) != group.name);
      for (const auto& entry : group.list) {
        assert(!class_name(entry.id, locale).starts_with("ID "));
      }
    }
  }
  for (const auto& group : NAMED_ITEM_ID_SET_LIST) {
    for (auto locale : {Locale::SimplifiedChinese, Locale::Japanese}) {
      assert(ui_text(group.name, locale) != group.name);
      for (auto id : group.itemIdSet) {
        assert(!item_name(id, locale).starts_with("ID "));
      }
    }
  }
  assert(ui_text("Item Trainer", Locale::SimplifiedChinese) == "物品编辑");
  assert(ui_text("Support Edit", Locale::Japanese) == "支援編集");
  for (const auto* label :
       {"Money & Renown",
        "Money",
        "Renown",
        "Apply Changes",
        "Class Unlocks",
        "Ability Learning",
        "Learned",
        "Not Learned",
        "Battalion Edit",
        "Normal Durability",
        "Refill All Durability"}) {
    assert(ui_text(label, Locale::SimplifiedChinese) != label);
    assert(ui_text(label, Locale::Japanese) != label);
  }
  std::cout << "Localization checks passed\n";
}
