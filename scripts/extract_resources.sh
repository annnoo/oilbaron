#!/usr/bin/env bash
set -euo pipefail

mkdir -p decompile/resources/bitmaps
mkdir -p decompile/resources/icons

echo "Extracting bitmaps from OILBARON.EXE..."
wrestool -x -t 2 game/OILBARON.EXE -o decompile/resources/bitmaps/

echo "Extracting icons from OILBARON.EXE..."
wrestool -x -t 14 game/OILBARON.EXE -o decompile/resources/icons/ || true
wrestool -x -t 3 game/OILBARON.EXE -o decompile/resources/icons/ || true

echo "Extracting dialogs and menus..."
wrestool -x -t 5 game/OILBARON.EXE > decompile/resources/dialogs_raw.bin || true
wrestool -x -R -t 5 game/OILBARON.EXE > decompile/resources/dialogs_raw.bin || true
wrestool -l -v game/OILBARON.EXE > decompile/resources/resources_list.txt

echo "Decompiling dialog templates to dialogs.rc..."
python3 scripts/parse_dialogs.py

echo "Extracting strings and help text..."
strings -n 4 game/OILBARON.HLP > decompile/resources/help_text.txt || true
strings -n 4 game/OILBARON.EXE > decompile/resources/exe_strings.txt

echo "Resource extraction complete."
