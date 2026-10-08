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

Host checks cover item editing, class flag lookup, and support grouping. A
successful build does not verify menu behavior or memory edits on hardware.

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
clang-format -i source/generated/names.cpp
make -f Makefile.host test
```

The export requires the .NET 10 SDK. `assets/localization/names.json` records
the source revision and exported catalogs; `source/generated/names.cpp`
embeds the names so installation still needs only one OVL. Game text retains
its original authorship and is not covered by the MIT license for new code.
Host checks cover language mapping, overrides, persistent mode values, and
name lookup for every class and item category used by the overlay.

## Releases

Tags matching `v<VERSION>` build and publish the single `feth-overlay.ovl`.
Record the hardware-testing status in the release notes.

<!-- SPDX-License-Identifier: MIT; Copyright (c) 2026 Jing Haihan -->
