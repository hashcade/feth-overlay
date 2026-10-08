// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#include "feth/app/model.hpp"
#include "feth/core/game_session.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace {
bool running{};
bool stopped{};
}  // namespace

namespace feth::core {
bool initialize() {
  return true;
}
void shutdown() {
  stopped = true;
}
bool gameIsRunning() {
  return running;
}
Locale detect_locale() {
  return Locale::SimplifiedChinese;
}
}  // namespace feth::core

int main() {
  feth::app::Model model;
  model.start();
  assert(model.display_locale() == feth::core::Locale::SimplifiedChinese);
  assert(model.status() == "请启动风花雪月 v1.2.0");
  bool called{};
  assert(!model.apply([&] { called = true; }));
  assert(!called);

  running = true;
  assert(model.apply([&] { called = true; }));
  assert(called && model.status() == "修改已应用");
  model.cycle_language();
  assert(model.display_locale() == feth::core::Locale::English);
  assert(model.status() == "Changes applied");
  model.set_language(feth::core::LocaleMode::Japanese);
  assert(!model.apply([] { throw std::runtime_error("0x1234"); }));
  assert(model.status() == "失敗: 0x1234");
  model.set_language(feth::core::LocaleMode::Auto);
  assert(model.status() == "失败: 0x1234");
  assert(model.refresh());
  assert(model.status() == "风花雪月 v1.2.0 已就绪");
  model.stop();
  assert(stopped);
  std::cout << "Model localization checks passed\n";
}
