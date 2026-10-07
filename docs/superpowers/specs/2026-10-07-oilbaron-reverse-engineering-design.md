# Oil Baron Reverse Engineering & Game Logic Extraction Specification

## 1. Overview & Objectives

**Oil Baron** is a Windows 3.1 strategic business board game (v1.2, 1992–1993, by Al Funk) inspired by classic market manipulation board games such as *Acquire*. Players place tiles on a grid to form oil companies, expand territory, buy and trade stocks, trigger mergers, collect shareholder bonuses, and build their fortune.

The overarching project goal is to develop a modern, client-side web game in pure React and TypeScript (ready for eventual packaging to Steam via Tauri) that preserves the core logic and game loop of Oil Baron while improving UX, visual design, and pacing.

This specification covers **Phase 1: Reverse Engineering & Core Logic Extraction**:
1. Initializing the repository and git configuration.
2. Extracting embedded 16-bit visual assets, dialog templates, menus, and help documentation from `game/OILBARON.EXE` and `game/OILBARON.HLP`.
3. Executing a headless Ghidra decompilation pipeline to generate structured, human-readable C pseudocode and symbol maps in `decompile/`.
4. Tracing and documenting the complete mathematical models, pricing tiers, merger formulas, board state machine, and AI decision heuristics into `docs/game_rules.md`.

---

## 2. Repository Configuration & Environment

### 2.1 Directory Structure
```
oilbaron/
├── .gitignore
├── docs/
│   ├── superpowers/specs/2026-10-07-oilbaron-reverse-engineering-design.md
│   └── game_rules.md            # Comprehensive extracted rulebook and formulas
├── game/                         # Original 16-bit Win3.1 binary & help (ignored in git)
│   ├── FILE_ID.DIZ
│   ├── OILBARON.EXE
│   ├── OILBARON.HLP
│   └── OILBARON.TXT
├── decompile/
│   ├── resources/
│   │   ├── bitmaps/             # Extracted 16-bit company logos and tiles (BMP)
│   │   ├── icons/               # Extracted application and share icons (ICO)
│   │   ├── dialogs.rc           # Dialog templates and control IDs
│   │   └── help_text.txt        # Extracted strings from HLP and TXT
│   ├── symbols/
│   │   ├── symbols.json         # Function names, entry points, and cross-references
│   │   └── strings.json         # Discovered strings and table offsets
│   └── src/
│       ├── main_winproc.c       # Application lifecycle, WinMain, menu handlers
│       ├── board_logic.c        # Grid data structures, placement rules, connectivity
│       ├── company_and_stocks.c # 7 oil companies, pricing curves, share counts
│       ├── mergers.c            # Merger trigger, majority/minority bonuses, stock trades
│       ├── ai.c                 # Computer player evaluation and decision heuristics
│       ├── dialogs.c            # Dialog window procedures and player interactions
│       └── data_tables.c        # Global data tables, constants, and lookup arrays
└── webgame/                     # Target directory for subsequent React/TS development
```

### 2.2 Git Configuration (`.gitignore`)
* Exclude original game binaries (`game/`).
* Exclude Ghidra project files, repositories, and cache locks (`*.gpr`, `*.rep/`).
* Exclude OS/IDE artifacts (`.DS_Store`, `.idea/`, `.vscode/`).
* Exclude future node/web dependencies (`node_modules/`, `dist/`).
* Track all specifications, extracted assets in `decompile/`, and decompiled C sources in `decompile/src/`.

---

## 3. Asset & Resource Extraction Pipeline

Using Linux native tools (`wrestool`, `icotool`, `strings`):
1. **Bitmaps**: Extract 16-bit DIB bitmaps for:
   - 7 oil companies: `AMERICAN`, `ARABIAN`, `IMPERIAL`, `ROYAL`, `SUPRA`, `TOWER`, `WEEMOCO`.
   - Grid chits and markers: `BLANK`, `CHIT`, `DISCARD PIECE`, `DISCARD_SQUARE`, `NOT PLAYABLE`, `PURPLE`.
   - UI status elements: `PLAYER INFO`, `NEXT TURN`, etc.
2. **Icons**: Extract application icons (`A_ICON`, `SHARES`).
3. **Dialog Templates**: Dump all dialog templates (`ABOUT`, `BLANK_MOVE`, `CHOOSE COMPANY`, `COMPANY_STATS`, `DISCARDPIECE`, `GAME_OVER`, `KEEP_SHARES`, `MERGER STATISTICS`, `NEW GAME`, `OPTIONS`, `SHARE_STATS`, `SHAREOPTIONSDIALOG`, `SHARES DIALOG`).
4. **Help & Rule Text**: Extract readable text blocks from `OILBARON.HLP` and `OILBARON.TXT` to capture original scoring rules, player limits, and share constraints.

---

## 4. Headless Ghidra Decompilation Pipeline

### 4.1 Analyzer Setup
* **Binary Target**: `game/OILBARON.EXE`
* **Architecture**: Intel x86 16-bit Protected Mode (NE / New Executable format).
* **Ghidra Executable**: `/opt/ghidra/support/analyzeHeadless`
* **Calling Conventions**: Win16 standard (`__pascal` for exported APIs and WindowProcs, `__cdecl` for internal utilities).

### 4.2 Decompiler Exporter Script (`ExportDecompiledC.java`)
A custom Ghidra headless post-script will automate the extraction:
1. Initialize Ghidra's `DecompInterface` with default compiler options.
2. Enumerate all functions registered by the auto-analysis passes.
3. For each function:
   - Run the decompiler to generate high-level C pseudocode.
   - Categorize each function by its memory segment, imported API calls (e.g. `GDI`, `USER`, `KERNEL`), or symbol pattern.
   - Write out structured C files into `decompile/src/`.
4. Export symbol mapping:
   - Function name, virtual address / segment:offset, caller/callee xrefs.
   - Global variables, string constants, and lookup tables.

---

## 5. Reverse Engineering Analysis & Rule Synthesis

Once C pseudocode is generated, we will trace the execution flow and write `docs/game_rules.md`, covering:

### 5.1 Board & Grid Mechanics
* Board dimensions (rows $\times$ columns).
* Cell representation (unplayed, dead/unplayable, single tile, company territory).
* Tile drawing, player hand sizes, and discarding dead tiles.

### 5.2 The 7 Oil Companies
* Complete attributes of each company:
  - Name, color/identity, tier category (low, medium, high tier).
  - Total share count limit (typically 25 shares per company).
  - Safe size threshold (tile count $\ge 11$ making a company immune from takeover).
  - Founding conditions and founder share bonus.

### 5.3 Stock Market & Pricing System
* Stock price determination formula: lookup table mapping (company size, tier) $\rightarrow$ share price.
* Share transaction limits per turn (e.g., maximum 3 shares per turn).
* Cash management and bankrupt prevention.

### 5.4 Merger Resolution Algorithm
* Trigger: Placement of a tile connecting two or more active companies.
* Acquirer selection: Highest tile count wins; tie-breaking rules.
* Shareholder payouts:
  - Majority shareholder bonus formula.
  - Minority shareholder bonus formula.
  - Split payouts for ties in shareholding.
* Share disposition options for the defunct company:
  - Keep (hold defunct shares in anticipation of rebirth).
  - Sell (liquidate at pre-merger share price).
  - Trade (2 defunct shares for 1 acquirer share, while acquirer stock remains).
* Cascade mergers: Order of operations when 3 or 4 companies are connected in a single move.

### 5.5 Player Turn State Machine & End Game
* Turn sequence:
  1. Place Tile
  2. Resolve Company Formation / Merger (if triggered)
  3. Buy Stocks (up to 3 shares)
  4. Draw replacement tile
  5. End turn / pass to next player
* Game end trigger (e.g., all active companies are safe, or any single company reaches $\ge 41$ tiles).
* Final score calculation and wealth ranking.

### 5.6 AI Decision Heuristics
* Tile placement evaluation (prioritizing founding companies, growing owned stocks, defending against hostile takeovers).
* Stock purchasing preferences (building majorities, buying undervalued growing companies).
* Merger trading decision logic (holding vs selling vs trading 2-for-1).

---

## 6. Success & Verification Criteria

1. **Repository Setup**: Git repo active, `game/` and Ghidra caches ignored, clean git status.
2. **Asset Extraction**: All 16-bit BMPs, icons, and dialog templates saved to `decompile/resources/`.
3. **Complete Decompilation**: All functions in `OILBARON.EXE` decompiled into readable C sources in `decompile/src/` with a symbol reference table in `decompile/symbols/`.
4. **Comprehensive Rules Doc**: `docs/game_rules.md` completed with exact formulas, board dimensions, price matrices, merger payouts, and AI weights.
