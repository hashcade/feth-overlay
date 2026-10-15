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
and uses English for unsupported languages. Simplified Chinese, Traditional
Chinese, and Korean console settings are detected separately. Manual modes cycle through Auto, English, Simplified
Chinese, Traditional Chinese, Japanese, and Korean. The selection is saved in
`sdmc:/config/feth-overlay/config.ini` under `[overlay]` as `language`.

Menu text lives in `source/core/messages.cpp`. Game names are exported through
the save editor's existing CLI, preserving its ID mappings and translations:

```sh
python3 scripts/export_names.py ../feth-save-editor
python3 scripts/export_item_data.py ../feth-save-editor
python3 scripts/export_battalions.py ../feth-save-editor
python3 scripts/export_classes.py ../feth-save-editor
python3 scripts/export_supports.py ../feth-save-editor
clang-format -i source/generated/*.cpp
make -f Makefile.host test
```

The export requires the .NET 10 SDK. `assets/localization/names.json` records
the source revision and exported catalogs; `source/generated/names.cpp`
embeds the names so installation still needs only one OVL. Game text retains
its original authorship and is not covered by the MIT license for new code.
Traditional Chinese and Korean shared fonts are enabled through
`patches/libtesla-fonts.patch`, applied to a build-local header without
modifying the dependency checkout.

Host checks cover language mapping, overrides, persistent mode values, and
name lookup for every class and item category used by the overlay.

One-shot action feedback reuses the active Ultrahand sound pack's `enter.wav`
and `[ultrahand] sound_effects` from `sdmc:/config/ultrahand/config.ini`.
The existing libtesla dependency has no audio helper, so a small optional
`audout` adapter submits that file without changing UI runtimes or bundling
assets. Playback is asynchronous. Hiding/exiting the overlay stops playback
before freeing its aligned buffer. Missing files, disabled audio, unsupported
WAVs, and unavailable audio services fall back to a two-second **Done** value.
The WAV parser accepts short 8/16-bit PCM mono/stereo packs at 8–48 kHz and
converts them to 48 kHz stereo. Failure details reuse the subtitle area.
The shared action-feedback policy matches MHGU Overlay: a result is consumed
once, only success requests confirmation audio, and silent completion expires
after two seconds. Opening a menu never replays a previous success.

## Game data

Class eligibility is exported by calling the save editor's `ClassEligibility`
rules. `assets/class-eligibility.json` records the source revision, actual
unit IDs, journal record IDs, and allowed classes. Unit IDs and journal
records are different: playable Jeritza is unit 1045 / record 43, while
unit 49 is an NPC variant. Pegasus Knight uses playable class 54, not 23;
Death Knight is class 91 and is exclusive to playable Jeritza. The menu and
bulk unlock share the same eligibility table, including Noble/Commoner,
gender restrictions, exclusive classes, and White Heron Cup eligibility.
Re-export after changing those rules in the editor. Bulk unlock only adds
eligible flags; hidden or previously modified flags remain untouched.
Dancer (class 43) also toggles Sword Avo +20 (ability 56). Bulk class unlock
adds it only for characters eligible for Dancer. Disabling Dancer clears the
learned bit and replaces equipped occurrences with the editor's empty ability
ID 240. Equipped abilities occupy five bytes at character offset `0x7F`.

Support-rank presets and per-pair limits are exported by calling the editor's
`SupportRankPresets` and `SupportPairRanks`. `assets/support-ranks.json` records
all 270 entries and their source revision. The exporter checks the original
pair order against the editor's game database. Pairs without conversations
are hidden without renumbering their memory indices. Left/Right skips ranks
that do not exist for the pair and applies the preset immediately. **Max All**
affects only the selected character's pairs and preserves points already above
the maximum-rank preset. It does not change conversation or story flags.

The memory profile targets only the v1.2.0 build ID. Item durability comes from
the save editor's `fixed_data.bin.gz`; obtainable battalion templates come
from its `ObtainableBattalions.cs`. Re-export these catalogs when updating
that data rather than editing generated C++ files.

Durability refill follows the save pointer at `main + 0x1B12190`. During combat,
it also follows the battle manager pointer at `main + 0x1AC3730`, whose 105
unit pointers start at `+8`. These locations were checked against the v1.2.0
NSO's relocations: GOT `0x1AA8078` resolves to `0x1AC3730`; `0x94E68` through
`0x94E74` clears the pointer slots up to manager `+0x350`.
Units share the six-item prefix with roster records. Refill requires an enabled
unit (`+0xD6`, bit 0), matching slot index (`+0xF0`), player faction (`+0x100 == 0`),
and a character ID (`+0x24`) present in the roster. The manager and save pointers
are checked again before writing, each item is rechecked before its edit, and
only its durability byte (`+2`) is written and verified. Host memory-fixture
checks cover combat copies, enemy/NPC exclusions, bounds, stale pointers, and
failed readback. Hardware testing is still required for the running-game path.

Supply categories live in `source/core/catalog.cpp`. Convoy categories use
item IDs; storehouse categories use indices into `Player_V23.MiscItems`
(223 byte counts at `0x13B1`) or `GiftItems` (225 byte counts at `0x1490`).
These offsets were checked with `Marshal.OffsetOf` in the save editor and
anchored to `CharacterSupportValues` at `0x1080`. Storehouse names come from
the editor's v1.2.0 message database. Only ordinary supplies are included;
lost items, quest-only bait, and unused entries are excluded. Anna's Trade
Secret is Convoy item 1161, separate from the quest objects in `GiftItems`.

Item actions are immediate. The owned-quantity action skips zero counts;
category actions set each type to 99 and add missing Convoy types only once.
Host checks cover both rules, unrelated inventory remaining unchanged, and
normal durability for new Convoy items.

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

From a clean, synchronized `main` branch, run `uv run scripts/release.py patch`
(or `minor` / `major`). The script updates `VERSION`, creates the release commit
and annotated tag, and pushes them together.

Tags matching `v<VERSION>` build and publish the single `feth-overlay.ovl`.
Record the hardware-testing status in the release notes.

<!-- SPDX-License-Identifier: MIT; Copyright (c) 2026 Jing Haihan -->
