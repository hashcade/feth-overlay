// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/messages.hpp"

namespace feth::core {
namespace {

constexpr const char* kMessages[][3] = {
  {"Language", "语言", "言語"},
  {"Automatic", "自动", "自動"},
  {"Item Trainer", "物品编辑", "アイテム編集"},
  {"Class Unlocks", "兵种解锁", "兵種解放"},
  {"Ability Edit", "特性编辑", "スキル編集"},
  {"Learned", "已学会", "習得済み"},
  {"Not Learned", "未学会", "未習得"},
  {"Learned Abilities", "已学会特性", "習得スキル"},
  {"Battalion Edit", "骑士团编辑", "騎士団編集"},
  {"Battalion Type", "骑士团类型", "騎士団の種類"},
  {"Owned Battalions", "已持有骑士团", "所持騎士団"},
  {"Experience", "经验", "経験値"},
  {"Endurance", "兵力", "戦力"},
  {"Refill Endurance", "补满兵力", "戦力を補充"},
  {"Apply Battalion", "应用骑士团设置", "騎士団設定を適用"},
  {"Refill All Battalions", "一键补满骑士团兵力", "全騎士団の戦力を補充"},
  {"Add Missing Battalions", "补齐缺少的骑士团", "未所持騎士団を追加"},
  {"Support Edit", "支援编辑", "支援編集"},
  {"Classes", "职业", "兵種"},
  {"Support", "支援", "支援"},
  {"Set Item", "设置物品", "アイテム設定"},
  {"Set Specific Item", "设置指定物品", "指定アイテムを設定"},
  {"Item ID", "物品 ID", "アイテム ID"},
  {"Durability", "耐久", "耐久"},
  {"Amount", "数量", "個数"},
  {"Apply Item", "应用物品设置", "アイテム設定を適用"},
  {"Quick Edit", "批量编辑", "一括編集"},
  {"Add Missing Items", "添加未持有物品", "未所持アイテムを追加"},
  {"Normal Durability", "补满正常耐久", "通常の耐久に回復"},
  {"Refill All Durability", "一键补满全部耐久", "全アイテムの耐久を回復"},
  {"Amount to 99", "数量设为 99", "個数を99に設定"},
  {"Owned Items", "已持有物品", "所持アイテム"},
  {"Potions", "回复药", "回復薬"},
  {"Exam Seals", "考试准考证", "資格試験パス"},
  {"Keys", "钥匙", "鍵"},
  {"Gold Bars", "金块", "金塊"},
  {"Stat Boosters", "能力提升道具", "能力アップアイテム"},
  {"Anna Quest Item", "安娜任务物品", "アンナのクエストアイテム"},
  {"Unique", "特殊职业", "固有兵種"},
  {"Beginner", "初级职业", "初級職"},
  {"Intermediate", "中级职业", "中級職"},
  {"Advanced", "高级职业", "上級職"},
  {"Special", "特级职业", "特級職"},
  {"Master", "最高级职业", "最上級職"},
  {"Enemy", "敌方职业", "敵専用兵種"},
  {"Support Points", "支援点数", "支援値"},
  {"Apply Support", "应用支援点数", "支援値を適用"},
  {"Cheat service unavailable",
   "金手指服务不可用",
   "チートサービスを利用できません"},
  {"Start Three Houses v1.2.0",
   "请启动风花雪月 v1.2.0",
   "風花雪月 v1.2.0 を起動してください"},
  {"Three Houses v1.2.0 ready",
   "风花雪月 v1.2.0 已就绪",
   "風花雪月 v1.2.0 準備完了"},
  {"Changes applied", "修改已应用", "変更を適用しました"},
  {"Failed", "失败", "失敗"},
  {"On", "开启", "オン"},
  {"Off", "关闭", "オフ"},
  {"Back", "返回", "戻る"},
  {"OK", "确定", "決定"},
};

}  // namespace

std::string ui_text(std::string_view english, Locale locale) {
  for (const auto& message : kMessages) {
    if (message[0] == english) {
      return message[static_cast<unsigned>(locale)];
    }
  }
  return std::string(english);
}

}  // namespace feth::core
