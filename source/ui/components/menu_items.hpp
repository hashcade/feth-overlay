// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jing Haihan

#pragma once

// Included by source/ui/main.cpp inside its private UI namespace.

std::string text(const Model& model, std::string_view english) {
  return core::ui_text(english, model.display_locale());
}

std::string language_value(const Model& model) {
  switch (model.language_mode()) {
    case core::LocaleMode::English:
      return "English";
    case core::LocaleMode::SimplifiedChinese:
      return "简体中文";
    case core::LocaleMode::Japanese:
      return "日本語";
    default:
      return text(model, "Automatic");
  }
}

void save_language(const Model& model) {
  mkdir("sdmc:/config", 0777);
  mkdir("sdmc:/config/feth-overlay", 0777);
  setIniFileValue(
    kSettingsPath,
    "overlay",
    "language",
    core::locale_mode_value(model.language_mode())
  );
}

class LocalizedOverlayFrame final : public tsl::elm::OverlayFrame {
public:
  LocalizedOverlayFrame(Model& model, std::string title)
    : OverlayFrame(std::move(title), kVersion),
      model_(model) {}

  void setTitle(std::string title) {
    m_title = std::move(title);
  }

  void draw(tsl::gfx::Renderer* renderer) override {
    const bool hide_footer = deactivateOriginalFooter;
    deactivateOriginalFooter = true;
    OverlayFrame::draw(renderer);
    deactivateOriginalFooter = hide_footer;

    if (!hide_footer) {
      const auto footer = "\uE0E1  " + text(model_, "Back") + "     \uE0E0  " +
                          text(model_, "OK");
      renderer->drawString(
        footer.c_str(), false, 30, 693, 23, renderer->a(defaultTextColor)
      );
    }
  }

private:
  Model& model_;
};

class MenuGui : public tsl::Gui {
public:
  MenuGui(Model& model, std::string title, bool requiresGame = true)
    : model_(model),
      title_(std::move(title)),
      requires_game_(requiresGame) {}

  tsl::elm::Element* createUI() override {
    frame_ = new LocalizedOverlayFrame(model_, text(model_, title_));
    auto* list = new tsl::elm::List(6);

    list->addItem(
      new tsl::elm::CustomDrawer(
        [this](tsl::gfx::Renderer* renderer, u16 x, u16 y, u16, u16) {
          renderer->drawString(
            model_.status().c_str(),
            false,
            x + 8,
            y + 30,
            19,
            renderer->a({0xC, 0xC, 0xC, 0xF})
          );
        }
      ),
      55
    );

    const bool ready = model_.refresh();
    if (ready || !requires_game_) {
      try {
        populate(list);
      } catch (const std::exception& error) {
        model_.showError(error.what());
      }
    }

    frame_->setContent(list);
    return frame_;
  }

  bool handleInput(
    u64 keysDown,
    u64 keysHeld,
    const HidTouchState&,
    JoystickPosition,
    JoystickPosition
  ) override {
    // Ignore the opening chord briefly, as in MHGU Overlay.
    if (
      std::chrono::steady_clock::now() - g_shown_at >=
        std::chrono::milliseconds(300) &&
      (keysDown & HidNpadButton_Down) && (keysHeld & HidNpadButton_L)
    ) {
      tsl::Overlay::get()->hide();
      return true;
    }
    if (keysDown & HidNpadButton_B) {
      tsl::goBack();
      return true;
    }
    return false;
  }

protected:
  virtual void populate(tsl::elm::List* list) = 0;
  Model& model_;
  LocalizedOverlayFrame* frame_{};

private:
  std::string title_;
  bool requires_game_;
};

template <typename Gui, typename... Args>
tsl::elm::ListItem*
submenu_item(Model& model, const std::string& label, Args... args) {
  auto* item = new tsl::elm::ListItem(label);
  item->setClickListener([model_ptr = &model, args...](u64 keys) {
    if ((keys & HidNpadButton_A) == 0) {
      return false;
    }
    tsl::changeTo<Gui>(*model_ptr, args...);
    return true;
  });
  return item;
}

struct NumericOptions {
  int minimum{};
  int maximum{};
  int largeStep{};
};

class NumericListItem final : public tsl::elm::ListItem {
public:
  NumericListItem(std::string label, int& value, NumericOptions options)
    : ListItem(std::move(label)),
      value_(value),
      options_(options) {
    setMaximum(options.maximum);
  }

  bool onClick(u64 keys) override {
    int delta{};
    if (keys & HidNpadButton_AnyLeft)
      delta = -1;
    else if (keys & HidNpadButton_AnyRight)
      delta = 1;
    else if (keys & HidNpadButton_L)
      delta = -options_.largeStep;
    else if (keys & HidNpadButton_R)
      delta = options_.largeStep;
    else
      return false;
    value_ = std::clamp(value_ + delta, options_.minimum, options_.maximum);
    setValue(std::to_string(value_));
    return true;
  }

  void setMaximum(int maximum) {
    options_.maximum = maximum;
    value_ = std::clamp(value_, options_.minimum, options_.maximum);
    setValue(std::to_string(value_));
  }

private:
  int& value_;
  NumericOptions options_;
};

NumericListItem*
numeric_item(const std::string& label, int& value, NumericOptions options) {
  return new NumericListItem(label, value, options);
}

tsl::elm::ListItem* action_item(
  Model& model, const std::string& label, std::function<void()> action
) {
  auto* item = new tsl::elm::ListItem(label);
  item->setClickListener([model_ptr = &model,
                          action = std::move(action)](u64 keys) {
    if ((keys & HidNpadButton_A) == 0) {
      return false;
    }
    model_ptr->apply(action);
    return true;
  });
  return item;
}

void category_header(tsl::elm::List* list, std::string title) {
  list->addItem(
    new tsl::elm::CustomDrawer(
      [title = std::move(title)](
        tsl::gfx::Renderer* renderer, u16 x, u16 y, u16, u16
      ) {
        renderer->drawString(
          title.c_str(),
          false,
          x + 8,
          y + 30,
          19,
          renderer->a({0xC, 0xC, 0xC, 0xF})
        );
      }
    ),
    45
  );
}
