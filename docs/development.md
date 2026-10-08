# Development

## Layout

- `include/feth/core` and `source/core`: game data, editing logic, Switch
  memory layout, and the `dmnt:cht` client.
- `include/feth/app` and `source/app`: shared editing state and operation status.
- `source/ui/main.cpp`: the overlay entry point.
- `source/ui/components`: main menu, feature menus, and shared menu controls.

The project targets Nintendo Switch. UI components are assembled into one
translation unit because libtesla defines runtime symbols in its header.

## Build

With devkitA64, libnx, and switch-tools installed:

```sh
git submodule update --init --recursive
make -j2
```

The result is `feth-overlay.ovl`. `VERSION` supplies the version shown in the
menu and the overlay metadata.

Without a local devkitPro installation:

```sh
docker run --rm -v "$PWD:/work" -w /work devkitpro/devkita64:latest make -j2
```

## Checks

```sh
make -f Makefile.host test
```

Host checks cover item editing and normal durability, class flag lookup,
ability flags, battalion refill, and support grouping. Layout
assertions check the character size and edited field offsets. A successful
build does not verify menu behavior or memory edits on hardware.

## Localization

Auto prefers the console language, falls back to the game's NACP language,
and uses English for unsupported languages. Chinese console variants map to
Simplified Chinese, as in MHGU Overlay. Manual modes cycle through Auto,
English, Simplified Chinese, and Japanese. The selection is saved in
`sdmc:/config/feth-overlay/config.ini` under `[overlay]` as `language`.

Menu text lives in `source/core/messages.cpp`. Game names are exported through
the save editor's existing CLI, preserving its ID mappings and translations:

```sh
python3 scripts/export_names.py ../feth-save-editor
python3 scripts/export_item_data.py ../feth-save-editor
python3 scripts/export_battalions.py ../feth-save-editor
python3 scripts/export_abilities.py ../feth-save-editor
clang-format -i source/generated/*.cpp
make -f Makefile.host test
```

The export requires the .NET 10 SDK. `assets/localization/names.json` records
the source revision and exported catalogs; `source/generated/names.cpp`
embeds the names so installation still needs only one OVL. Game text retains
its original authorship and is not covered by the MIT license for new code.
Host checks cover language mapping, overrides, persistent mode values, and
name lookup for every class and item category used by the overlay.

## Game data

The memory profile targets only the v1.2.0 build ID. Item durability comes from
the save editor's `fixed_data.bin.gz`; obtainable battalion templates come
from its `ObtainableBattalions.cs`. Re-export these catalogs when updating
that data rather than editing generated C++ files.

Ability learning flags occupy 30 bytes at character offset `0x61`, with one
bit per ID from 0 to 239. The UI excludes placeholder names. DLC abilities
outside that mapped range need their learning flag locations confirmed
before they can be added. No equipment editing is exposed.

The battalion inventory is 200 eight-byte records. Its offset is derived
from the existing support offset: the inventory and four uint32 fields
immediately precede support values in `Player_V23`. Battalion edits also
update a character's equipped copy when its character ID and type match.

Money and renown are separate uint32 fields, at offsets `0x24274` and
`0x250D4` relative to the Convoy. These were checked using the editor's
`Player_V23` and `Activities_V23` layouts with `Marshal.OffsetOf` and
`Marshal.SizeOf`, anchored to the existing support offset. The offsets also
match [Gamerjin's v1.2.0 codes](https://gbatemp.net/threads/fire-emblem-three-houses-general-hacking.544144/page-139#post-8946442).
Only the two resource fields are written. Their input limits match the editor:
9,999,999 money and 999,999 renown.

## Releases

Tags matching `v<VERSION>` build and publish the single `feth-overlay.ovl`.
Record the hardware-testing status in the release notes.

<!-- SPDX-License-Identifier: MIT; Copyright (c) 2026 Jing Haihan -->
