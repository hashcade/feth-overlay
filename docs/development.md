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

## Releases

Tags matching `v<VERSION>` build and publish the single `feth-overlay.ovl`.
Record the hardware-testing status in the release notes.

<!-- SPDX-License-Identifier: MIT; Copyright (c) 2026 Jing Haihan -->
