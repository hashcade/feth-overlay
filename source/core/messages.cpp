// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/core/messages.hpp"

namespace feth::core {
namespace {

constexpr const char* kMessages[][3] = {
  {"Language", "语言", "言語"},
  {"Automatic", "自动", "自動"},
  {"Resources", "资源编辑", "リソース編集"},
  {"Money", "金钱", "資金"},
  {"Renown", "名声", "名声"},
  {"Apply Changes", "应用修改", "変更を適用"},
  {"Item Trainer", "物品编辑", "アイテム編集"},
  {"Class Unlocks", "兵种解锁", "兵種解放"},
  {"Unlock All", "解锁全部", "すべて解放"},
  {"Battalion Edit", "骑士团编辑", "騎士団編集"},
  {"Battalion Type", "骑士团类型", "騎士団の種類"},
  {"Owned Battalions", "已持有骑士团", "所持騎士団"},
  {"Level", "等级", "レベル"},
  {"Current Endurance", "当前兵力", "現在の戦力"},
  {"Refill Endurance", "补满兵力", "戦力を補充"},
  {"Apply Battalion", "应用骑士团设置", "騎士団設定を適用"},
  {"Refill All Battalions", "补满骑士团兵力", "全騎士団の戦力を補充"},
  {"Support Edit", "支援编辑", "支援編集"},
  {"Classes", "职业", "兵種"},
  {"Support", "支援", "支援"},
  {"Refill All Durability", "补满全部耐久", "全アイテムの耐久を回復"},
  {"Amount to 99", "数量设为 99", "個数を99に設定"},
  {"Potions", "回复药", "回復薬"},
  {"Exam Seals", "准考证", "資格試験パス"},
  {"Keys", "钥匙", "鍵"},
  {"Gold Bars", "金块", "金塊"},
  {"Stat Boosters", "能力药", "能力アップアイテム"},
  {"Forge Materials", "锻造材料", "錬成素材"},
  {"Gifts", "礼物", "贈り物"},
  {"Tea", "茶叶", "茶葉"},
  {"Bait", "鱼饵", "釣り餌"},
  {"Seeds", "种子", "種"},
  {"Ingredients", "食材", "食材"},
  {"Trade Secret", "秘密道具", "秘密のアイテム"},
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
  {"Three Houses v1.2.0", "风花雪月 v1.2.0", "風花雪月 v1.2.0"},
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
