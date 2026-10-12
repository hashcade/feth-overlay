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
  for (int code : {6, 15}) {
    assert(locale_from_switch_language(code) == Locale::SimplifiedChinese);
  }
  for (int code : {11, 16})
    assert(locale_from_switch_language(code) == Locale::TraditionalChinese);
  assert(locale_from_switch_language(7) == Locale::Korean);
  assert(locale_from_nacp_language(2) == Locale::Japanese);
  assert(locale_from_nacp_language(12) == Locale::TraditionalChinese);
  assert(locale_from_nacp_language(13) == Locale::SimplifiedChinese);
  assert(locale_from_nacp_language(14) == Locale::Korean);
  assert(locale_from_nacp_language(0) == Locale::English);
  for (int code : {1, 2, 12, 99}) {
    assert(locale_from_switch_language(code) == Locale::English);
  }
  for (auto locale :
       {Locale::English,
        Locale::SimplifiedChinese,
        Locale::Japanese,
        Locale::TraditionalChinese,
        Locale::Korean}) {
    assert(resolve_locale(LocaleMode::Auto, locale) == locale);
    assert(resolve_locale(LocaleMode::English, locale) == Locale::English);
    assert(
      resolve_locale(LocaleMode::SimplifiedChinese, locale) ==
      Locale::SimplifiedChinese
    );
    assert(resolve_locale(LocaleMode::Japanese, locale) == Locale::Japanese);
  }

  auto mode = LocaleMode::Auto;
  for (int index = 0; index < 6; ++index) {
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
  assert(item_name(16, Locale::TraditionalChinese) == "鐵劍");
  assert(item_name(16, Locale::Korean) == "철의 검");
  assert(class_name(0, Locale::TraditionalChinese) == "貴族");
  assert(class_name(0, Locale::Korean) == "귀족");
  assert(
    resolve_locale(LocaleMode::TraditionalChinese, Locale::English) ==
    Locale::TraditionalChinese
  );
  assert(resolve_locale(LocaleMode::Korean, Locale::English) == Locale::Korean);
  assert(item_name(65535, Locale::Japanese) == "ID 65535");
  assert(character_name("Byleth", Locale::SimplifiedChinese) == "贝雷特");
  assert(character_name("Byleth♀", Locale::SimplifiedChinese) == "贝雷丝♀");
  assert(
    character_name("Unlisted character", Locale::Japanese) ==
    "Unlisted character"
  );

  // All shipped class IDs and item groups need names and translated headings.
  for (const auto& group : NAMED_CLASS_ID_LIST_LIST) {
    for (auto locale :
         {Locale::SimplifiedChinese,
          Locale::Japanese,
          Locale::TraditionalChinese,
          Locale::Korean}) {
      assert(ui_text(group.name, locale) != group.name);
      for (const auto& entry : group.list) {
        assert(!class_name(entry.id, locale).starts_with("ID "));
      }
    }
  }
  for (const auto& group : NAMED_ITEM_ID_SET_LIST) {
    for (auto locale :
         {Locale::SimplifiedChinese,
          Locale::Japanese,
          Locale::TraditionalChinese,
          Locale::Korean}) {
      assert(ui_text(group.name, locale) != group.name);
      if (group.storage == ItemStorage::Convoy) {
        for (auto id : group.itemIdSet)
          assert(!item_name(id, locale).starts_with("ID "));
      }
    }
  }
  assert(ui_text("Item Trainer", Locale::SimplifiedChinese) == "物品编辑");
  assert(ui_text("Support Edit", Locale::Japanese) == "支援編集");
  for (const auto* label :
       {"Resources",
        "Money",
        "Renown",
        "Apply Changes",
        "Class Unlocks",
        "Unlock All",
        "Max All",
        "None",
        "←/→ Rank",
        "Battalion Edit",
        "Fill Missing Types",
        "Refill Endurance",
        "Amount to 99",
        "Refill All Durability"}) {
    assert(ui_text(label, Locale::SimplifiedChinese) != label);
    assert(ui_text(label, Locale::Japanese) != label);
    assert(ui_text(label, Locale::TraditionalChinese) != label);
    assert(ui_text(label, Locale::Korean) != label);
  }
  std::cout << "Localization checks passed\n";
}
