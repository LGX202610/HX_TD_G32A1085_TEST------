#!/usr/bin/env python3
"""Generate compile_commands.json for clangd (Keil project, editor indexing only)."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

INCLUDES = [
    "Porting/Boot/inc",
    "Porting/Debug/inc",
    "Porting/Hal/Crc_Hal/inc",
    "Porting/Hal/Flash_Hal/inc",
    "Porting/Hal/Timer_Hal/inc",
    "Porting/Hal/Uds_Algorithm_Hal/inc",
    "Porting/Hal/Watchdog_Hal/inc",
    "Porting/Public_Inc",
    "Porting/SDK/G32A10xx_SDK/platform/devices/CMSIS/Include",
    "Porting/SDK/G32A10xx_SDK/platform/devices/Include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Can/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Can_UpLayerDriver/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Flash_UpLayerDriver/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Gpio/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Interrupt/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Rcm/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Tmr/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Usart/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Wdg/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Misc/include",
    "Porting/SDK/G32A10xx_SDK/platform/drivers/Flash/include",
    "UDS_stack/unified_bootloader_stack/Source/auto_lib/inc",
    "UDS_stack/unified_bootloader_stack/Source/Demo/inc",
    "UDS_stack/unified_bootloader_stack/Source/FIFO/inc",
    "UDS_stack/unified_bootloader_stack/Source/Flash_App/inc",
    "UDS_stack/unified_bootloader_stack/Source/UDS_stack/TP/inc",
    "UDS_stack/unified_bootloader_stack/Source/UDS_stack/TP/inc/CAN_TP",
    "UDS_stack/unified_bootloader_stack/Source/UDS_stack/UDS/inc",
    "UDS_stack/unified_bootloader_stack/Source/UDS_stack/Versions",
    "UDS_stack/unified_bootloader_stack/Source/UDS_stack/EUC_Verify/include",
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
    "UDS_G32A1085_Bootloader",
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
