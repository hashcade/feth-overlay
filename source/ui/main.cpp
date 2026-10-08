// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#define TESLA_INIT_IMPL
#include <tesla.hpp>

#include "feth/app/model.hpp"
#include "feth/core/catalog.hpp"
#include "feth/core/engine.hpp"
#include "feth/core/game_session.hpp"
#include "feth/core/items.hpp"
#include "feth/core/messages.hpp"
#include "feth/core/names.hpp"

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>

namespace {

using feth::app::Model;
namespace core = feth::core;
namespace game = feth::core;

constexpr const char* kVersion = "v" FETH_OVERLAY_VERSION;
constexpr const char* kSettingsPath = "sdmc:/config/feth-overlay/config.ini";
std::chrono::steady_clock::time_point g_shown_at;

// clang-format off
#include "components/menu_items.hpp"
#include "components/resources_edit.hpp"
#include "components/item_trainer.hpp"
#include "components/class_edit.hpp"
#include "components/ability_edit.hpp"
#include "components/support_edit.hpp"
#include "components/battalion_edit.hpp"
#include "components/main_menu.hpp"
// clang-format on

class FethOverlay final : public tsl::Overlay {
public:
  void initServices() override {
    model_.set_language(
      core::parse_locale_mode(
        parseValueFromIniSection(kSettingsPath, "overlay", "language")
      )
    );
    model_.start();
  }

  void exitServices() override {
    model_.stop();
  }

  void onShow() override {
    g_shown_at = std::chrono::steady_clock::now();
    model_.refresh();
  }

  std::unique_ptr<tsl::Gui> loadInitialGui() override {
    return initially<MainGui>(model_);
  }

private:
  Model model_;
};

}  // namespace

int main(int argc, char** argv) {
  // Match MHGU's 640px menu without allocating its full-width hunting HUD.
  framebufferWidth = 640;
  framebufferHeight = 720;
  return tsl::loop<FethOverlay>(argc, argv);
}
