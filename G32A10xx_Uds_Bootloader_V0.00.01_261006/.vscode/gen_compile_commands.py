#!/usr/bin/env python3
"""Generate compile_commands.json for clangd (Keil project, editor indexing only)."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

INCLUDES = [
    "Application/include",
    "Driver/include",
    "Hal/include",
    "Libraries/CMSIS/Include",
    "Libraries/Device/include",
    "Libraries/G32A10xx_StdPeriphDriver/inc",
    "Uds/auto_lib/inc",
    "Uds/FIFO/inc",
    "Uds/Flash/inc",
    "Uds/Port/inc",
    "Uds/TP/inc",
    "Uds/UDS/inc",
    "Uds/Verify/inc",
]

COMMON_DEFS = [
    "G32A1085",
    "G32A10xx",
    "G32A10x_SERIES",
    "START_FROM_FLASH",
    "TURN_ON_CPU0",
    "__GNUC__",
    "__ARM_ARCH_6M__=1",
    "ARM_MATH_CM0PLUS",
    "UDS_PROJECT_FOR_BOOTLOADER",
]

SKIP_PARTS = {
    ".git",
    "Objects",
    "Listings",
    "IAR",
    "UDS_G32A1085_App",
    "UDS_G32A1085_App_NC",
    "UDS_G32A1065_App",
    "UDS_G32A1065_Bootloader",
}


def flags_for(src):
    cmd = ["clang", "-c", "-std=c99", "-ferror-limit=0", "-Wno-everything"]
    cmd += [f"-D{d}" for d in COMMON_DEFS]
    cmd += [f"-I{ROOT.as_posix()}/{inc}" for inc in INCLUDES]
    cmd += [src.as_posix()]
    return cmd


def main():
    entries = []
    for src in ROOT.rglob("*.c"):
        if any(p in SKIP_PARTS for p in src.parts):
            continue
        entries.append(
            {
                "directory": ROOT.as_posix(),
                "file": src.as_posix(),
                "arguments": flags_for(src),
            }
        )
    out = ROOT / "compile_commands.json"
    out.write_text(json.dumps(entries, indent=2), encoding="utf-8")
    print(f"Wrote {len(entries)} entries to {out}")


if __name__ == "__main__":
    main()
