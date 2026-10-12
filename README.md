# FETH Overlay

> [!NOTE]
> Based on [feth-overlays](https://github.com/3096/feth-overlays) by
> [3096](https://github.com/3096), with contributions from Jacien.

[![build](https://github.com/jinghaihan/feth-overlay/actions/workflows/build.yml/badge.svg)](https://github.com/jinghaihan/feth-overlay/actions/workflows/build.yml)

An in-game overlay for Fire Emblem: Three Houses v1.2.0. Edit money, renown, items,
classes, abilities, support points, and battalions from a single menu.

English, Simplified Chinese, and Japanese are supported. The language follows
your console by default; choose **Language** in the main menu to override it.
Your language choice is remembered between sessions.

## Features

| Menu | Features |
| --- | --- |
| Resources | Edit money and renown. |
| Item Trainer | Restore normal durability, top up owned supplies, or fill a supply category to 99. |
| Class Unlocks | Unlock individual classes or all eligible classes for a character, filtered by starting class, gender, and exclusive roles. |
| Ability Unlocks | Learn or forget abilities for each roster character. |
| Support Edit | Edit support points between characters. |
| Battalion Edit | Edit type, level, and current endurance for owned battalions. |

**Refill All Durability** restores Convoy and character-held items to each
item's normal maximum durability.

**Amount to 99** tops up owned supplies in the categories below. Missing items
stay missing, and zero quantities stay zero. Weapons, equipment, lost items,
and other quest items are unchanged.

Each category button sets every included type to 99, adding missing supplies.
Repeated clicks keep the quantity at 99:

- Potions, exam seals, keys, stat boosters, forge materials, and gold bars.
- Gifts (including flowers), tea, bait, seeds, and ingredients.
- Trade Secret (Anna's quest item).

**Class Unlocks** shows each character's eligible classes, including DLC
classes. **Unlock All** unlocks the displayed classes immediately. Existing
unlock flags are preserved; change classes from the game's own menus.

Changing a battalion's type assigns its default gambit and restores its full
endurance. Level can be adjusted from 1 to 5. Changing only type or endurance
preserves experience progress. Battalion edits also update the equipped copy.
**Refill All Battalions** restores known types in the barracks and on characters.

Toggle **Learned / Not Learned** to learn or forget an ability. Equip abilities
from the game's own menus. Some DLC abilities are not included yet.

Changes affect the running game; save in-game to keep them.

## Requirements

- Fire Emblem: Three Houses v1.2.0
- Atmosphère with `dmnt:cht`
- Tesla Menu or Ultrahand with an overlay loader

## Install

1. Download `feth-overlay.ovl` from the
   [releases](https://github.com/jinghaihan/feth-overlay/releases).
2. Copy it to `sdmc:/switch/.overlays/feth-overlay.ovl`.
3. Remove the old Item Trainer, Class Edit, and Support Edit `.ovl` files if
   they are installed.
4. Start the game, open your overlay menu, and select **FETH Overlay**.

## Controls

- **A** opens a submenu, toggles a setting, or applies an edit. Item actions
  take effect when pressed.
- **B** returns to the previous menu.
- **Left/Right** adjusts a numeric value by 1.
- **L/R** adjusts support points by 100 or battalion endurance by 10.
  Money changes by 10,000 and renown by 1,000.
- **L + Down** hides the overlay and keeps the current menu open for next time.

## Documentation

- [Development guide](docs/development.md): building and contributing.
- [Item ID list](docs/feth_item_ids.txt): Convoy item reference.

## Credits

- [3096/feth-overlays](https://github.com/3096/feth-overlays): the original
  Item Trainer, Class Edit, and Support Edit overlays. This project retains
  and adapts their game data, memory layouts, and editing logic.
- **Jacien**: supported the original project and provided game information
  and testing.
- [Falo's Three Houses save editor work](https://gbatemp.net/threads/fire-emblem-three-houses-general-hacking.544144/post-8948080):
  game-data research credited by the original overlays.
- [jinghaihan/mhgu-overlay](https://github.com/jinghaihan/mhgu-overlay):
  project organization, shared menu styling, numeric controls, and automatic
  language detection.
- [jinghaihan/feth-save-editor](https://github.com/jinghaihan/feth-save-editor):
  the English, Simplified Chinese, and Japanese game names
  exported from its game database, originally provided by imouto1994 and Falo,
  and the per-character class eligibility rules.
- [WerWolv/libtesla](https://github.com/WerWolv/libtesla): the original Tesla
  UI runtime.
- [minazuki19/libtesla](https://github.com/minazuki19/libtesla): the libtesla
  fork used by this overlay, including its layout, theme, and font support.
- [Atmosphere-NX/Atmosphere-libs](https://github.com/Atmosphere-NX/Atmosphere-libs):
  the official `dmnt:cht` client used to access the running game.

Fire Emblem and related names are trademarks of Nintendo. This unofficial
project is not affiliated with or endorsed by Nintendo or Intelligent Systems.

## License

Original FETH code and its derivatives retain [GPL-2.0](LICENSE). Original
contributions by [jinghaihan](https://github.com/jinghaihan), marked with
`SPDX-License-Identifier: MIT`, are covered by [MIT](LICENSE-MIT). Dependencies
and upstream assets retain their respective licenses and authorship.

The combined overlay is distributed under GPL-2.0; the MIT license does not
relicense upstream code or assets.

<!-- SPDX-License-Identifier: MIT; Copyright (c) 2026 Jing Haihan -->
