#!/usr/bin/env python3
"""Package all three overlays in the SD card directory layout."""

from pathlib import Path
from shutil import copyfile
from zipfile import ZIP_DEFLATED, ZipFile


root = Path(__file__).resolve().parents[1]
output = root / "output"
directory = output / "switch/.overlays"
directory.mkdir(parents=True, exist_ok=True)

with ZipFile(output / "feth-overlays.zip", "w", ZIP_DEFLATED) as archive:
    for project, name, filename in (
        ("item-trainer", "feth-item-trainer", "item-trainer.ovl"),
        ("class-edit", "feth-class-edit", "feth-class-edit.ovl"),
        ("support-viewer", "feth-support-viewer", "feth-support-viewer.ovl"),
    ):
        source = root / project / f"{name}.ovl"
        data = source.read_bytes()
        if data[0x10:0x14] != b"NRO0":
            raise ValueError(f"Invalid overlay: {source.name}")
        destination = directory / filename
        copyfile(source, destination)
        archive.write(destination, destination.relative_to(output))

print(output / "feth-overlays.zip")
