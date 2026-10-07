#!/usr/bin/env python3
"""
scripts/split_modules.py - Organize Ghidra-decompiled C code into modular subsystems.

Parses decompile/src/all_decompiled.c and splits 756 functions into logical subsystems:
  - board_logic.c:        Grid structures, tile placement, connectivity, flood-fill
  - company_and_stocks.c: 7 oil companies, pricing curves, stock trading, dividends
  - mergers.c:            Merger detection, survivor selection, shareholder bonuses, stock trades
  - ai.c:                 Computer player AI evaluation, scoring heuristics, move selection
  - dialogs.c:            Dialog window procedures, user prompts, settings configuration
  - main_winproc.c:       WinMain, application lifecycle, turn loop state machine, menus
  - data_tables.c:        DGROUP memory map, struct layouts, constants, Borland runtime

Also creates compatibility aliases (board.c, stocks.c, main.c) and exports
decompile/symbols/strings.json.
"""

import os
import sys
import re
import json
import struct

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIR = os.path.join(ROOT_DIR, "decompile", "src")
SYM_DIR = os.path.join(ROOT_DIR, "decompile", "symbols")
ALL_C = os.path.join(SRC_DIR, "all_decompiled.c")
EXE_PATH = os.path.join(ROOT_DIR, "game", "OILBARON.EXE")


def categorize_function(fn: str, addr: str, code: str) -> str:
    """Categorize a decompiled function by name, address segment, and code analysis."""
    seg = addr.split(":")[0]

    # 1. AI subsystem
    if fn in (
        "FUN_1008_a397",  # Main AI evaluation & move decision
        "FUN_1008_ba6a",  # AI turn execution coordinator
        "FUN_1008_bbb8",  # AI stock purchase heuristics
        "FUN_1008_bd1a",  # AI state/timer initialization
        "FUN_1008_a13c",  # AI company valuation helper
        "FUN_1008_a009",  # AI move ranking helper
        "FUN_1008_9f94",  # AI evaluation math
    ) or "s_Computer_Thinking" in code or "s_SlowComputer" in code:
        return "ai.c"

    # 2. Merger subsystem
    if (
        fn == "FUN_1008_2448"  # Core merger detection & resolution engine
        or seg == "1060"       # Merger dialogs (MERGER STATISTICS, ShareOptionsDialog)
        or "s_MERGER_STATISTICS" in code
        or "s_ShareOptionsDialog" in code
        or "1328" in code      # Dialog ID for MERGER STATISTICS
    ):
        return "mergers.c"

    # 3. Board Logic subsystem
    if (
        fn in (
            "FUN_1008_1abd",  # Grid coordinate lookup & cell indexer
            "FUN_1008_7021",  # Grid drawing / coordinate conversion
            "FUN_1008_80b8",  # 4-neighbor adjacency & connectivity
            "FUN_1008_813e",  # Flood-fill / territory expansion
            "FUN_1008_826e",  # Tile placement validity & placement evaluation
            "FUN_1008_8f0c",  # Board mouse click handler (WM_LBUTTONDOWN)
            "FUN_1008_92fd",  # Board cell state update
            "FUN_1008_93e6",  # Grid tile counting & company size recalculation
            "FUN_1008_9c3e",  # Board click dispatch & chit play
            "FUN_1008_a307",  # Grid cell reset / clear
            "FUN_1008_5b20",  # Grid rendering / border drawing
            "FUN_1008_6752",  # Board coordinate validation
        )
        or seg in ("1048", "1050")  # Graphics rendering for tiles, chits, and board grid
    ):
        return "board_logic.c"

    # 4. Companies & Stocks subsystem
    if (
        fn in (
            "FUN_1008_35ac",  # Player stock portfolios & dividend distribution
            "FUN_1008_38c8",  # Buy/Sell stock transaction dialog handler
            "FUN_1008_422f",  # Company statistics & share stats display
            "FUN_1008_4c3f",  # Company creation / founding bonus shares
            "FUN_1008_50e3",  # Majority and minority bonus payouts
            "FUN_1008_5d6b",  # Company open/active status query
            "FUN_1008_5dc4",  # Company trading eligibility
            "FUN_1008_5e10",  # Share transaction availability
            "FUN_1008_5e91",  # Active company count query
            "FUN_1008_67c5",  # Purchase limit enforcement
            "FUN_1008_687f",  # Stock buying constraints & cash verification
            "FUN_1008_6a95",  # Company share limit query
            "FUN_1008_71fc",  # Share buy/sell execution & cash transfer
            "FUN_1008_7a91",  # Dividend payment calculation
            "FUN_1008_7bd8",  # Share balance update & ledger sync
            "FUN_1008_7e1e",  # Dividend notification & payout
            "FUN_1008_88d9",  # Company stock valuation curve & tier pricing
            "FUN_1008_9121",  # Stock value query for company
            "FUN_1008_a1da",  # Company territory size update & price adjustment
            "FUN_1008_0002",  # Check active companies
            "FUN_1008_0057",  # Company stock lookup helper
            "FUN_1008_1936",  # Company formation notification
            "FUN_1008_19f6",  # Share limit error check
            "FUN_1008_1aa7",  # Stock availability query
            "FUN_1008_1e95",  # Stock ledger initialization
            "FUN_1008_1fa6",  # Company index mapping
            "FUN_1008_1fc6",  # Share certificate verification
            "FUN_1008_38a0",  # Share quantity adjustment
            "FUN_1008_3d8d",  # Company status query
            "FUN_1008_3e0e",  # Company logo bitblt display
            "FUN_1008_3f81",  # Company ID validation
            "FUN_1008_3fa3",  # Company color palette lookup
            "FUN_1008_5484",  # Stock transaction dialog prompt
            "FUN_1008_57a7",  # Company banner painting
            "FUN_1008_5ebb",  # Share count display
            "FUN_1008_61d8",  # Player stock holding table display
            "FUN_1008_65bd",  # Stock price format helper
            "FUN_1008_65f9",  # Player cash update
            "FUN_1008_6cc8",  # Company stock limit check
            "FUN_1008_6e18",  # Share trade ratio helper
            "FUN_1008_6e82",  # Stock purchase validation
            "FUN_1008_6ead",  # Share holding query
            "FUN_1008_6f2e",  # Player portfolio query
            "FUN_1008_6f68",  # Company active flag check
            "FUN_1008_6f8f",  # Stock price calculation helper
            "FUN_1008_7000",  # Stock buy cap check
            "FUN_1008_7101",  # Company formation check
            "FUN_1008_7122",  # Company share remaining query
            "FUN_1008_719a",  # Share transaction verification
            "FUN_1008_79a2",  # Share buy/sell limit check
            "FUN_1008_79c7",  # Stock transaction check
            "FUN_1008_79ec",  # Stock trade validation
            "FUN_1008_7a28",  # Company pricing baseline check
            "FUN_1008_7a49",  # Company statistics prompt
            "FUN_1008_7b09",  # INI stock options load
            "FUN_1008_7f62",  # Share balance validation
            "FUN_1008_7f83",  # Company lookup helper
            "FUN_1008_7fa7",  # Stock balance helper
            "FUN_1008_7fde",  # INI settings sync
            "FUN_1008_807d",  # Company safe threshold check
            "FUN_1008_8826",  # Stock price curve indexer
            "FUN_1008_91bd",  # Company market cap calculator
            "FUN_1008_963a",  # Player portfolio evaluation
            "FUN_1008_7591",  # Company status report
            "FUN_1008_51fd",  # Company formation from chits
            "FUN_1008_3423",  # Stock certificate display
        )
        or "s_SHARES" in code
        or "s_Buy_or_Sell" in code
        or "s_Cannot_Buy" in code
        or "s_Cannot_Sell" in code
        or "s_American_Natural_Gas" in code
        or "s_Weem_Oil_Refining" in code
    ):
        return "company_and_stocks.c"

    # 5. Dialogs subsystem
    if (
        seg in ("1070", "1080", "1068", "1058")
        or "s_OPTIONS" in code
        or "s_NEW_GAME" in code
        or "s_DISCARDPIECE" in code
        or "s_CHOOSE_COMPANY" in code
        or "s_ABOUT" in code
        or "s_REGISTRATION" in code
        or "s_QUICK_PEEK" in code
        or "s_KEEP_SHARES" in code
    ):
        return "dialogs.c"

    # 6. Main window procedure & lifecycle
    if (
        seg in ("1000", "1078", "1010", "1018", "1020", "1028", "1030", "1038", "1040", "1088", "1090", "1098")
        or fn in (
            "FUN_1008_0084",  # Game file load / New game initialization state machine
            "FUN_1008_0ed2",  # Turn progression / Player transition loop
            "FUN_1008_2168",  # Turn advance coordinator
            "FUN_1008_21eb",  # Game over evaluation
            "FUN_1008_229d",  # Final score & wealth window display
            "FUN_1008_23d1",  # Game reset & cleanup
            "FUN_1008_6e62",  # Application title / window setup
        )
    ):
        return "main_winproc.c"

    # 7. Fallback -> data_tables.c (Runtime, imports, tables)
    return "data_tables.c"


MODULE_DESCRIPTIONS = {
    "board_logic.c": """/*
 * board_logic.c - Oil Baron Board & Grid Logic Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Grid data structure: array of cells at DAT_10a8_22d2
 *   - Row stride: 0x8c bytes, Cell size: 5 bytes
 *   - Cell values:
 *       0 = NONE (Empty)
 *       1 = IMPERIAL, 2 = ROYAL, 3 = AMERICAN, 4 = ARABIAN,
 *       5 = TOWER,    6 = SUPRA, 7 = WEEMOCO
 *       8 = PURPLE (Unformed placed chit)
 *       9 = BLANK (Unplayable / Dead tile)
 *      10 = DISCARD_SQUARE
 *   - Functions include:
 *       * FUN_1008_80b8: 4-neighbor adjacency & connectivity check
 *       * FUN_1008_813e: Flood-fill chain expansion across adjacent tiles
 *       * FUN_1008_826e: Tile placement validation & candidate scoring
 *       * FUN_1008_8f0c: Board mouse click handling (WM_LBUTTONDOWN)
 *       * FUN_1008_93e6: Company territory tile counting
 *       * FUN_1008_9c3e: Tile placement execution & dispatch
 *       * Segments 1048 / 1050: Bitmapped tile and chit rendering
 */
""",

    "company_and_stocks.c": """/*
 * company_and_stocks.c - Oil Baron Companies & Stock Market Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - 7 Oil Companies categorized into 3 pricing tiers:
 *       * Low Tier  (Base $200): Tower, Royal (Colored Cube)
 *       * Mid Tier  (Base $300): American Natural Gas, Oil Arabia
 *       * High Tier (Base $400): Imperial, SupraCo, Weem Oil Refining
 *   - Stock Pricing Curve (FUN_1008_88d9 / FUN_1008_8826):
 *       Share price increases with company territory tile count.
 *   - Shareholder Payouts (FUN_1008_50e3 / FUN_1008_7a91):
 *       * Majority Bonus: 10x current share price
 *       * Minority Bonus:  5x current share price
 *   - Company Formation & Founding (FUN_1008_4c3f, FUN_1008_51fd):
 *       Founder receives 1 free bonus share when establishing a company.
 *   - Stock Trading & Limits (FUN_1008_38c8, FUN_1008_71fc):
 *       Players can purchase up to 3 shares per turn; enforce share limits.
 */
""",

    "mergers.c": """/*
 * mergers.c - Oil Baron Company Merger Resolution Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Trigger: Placement of a tile connecting 2 or more active companies.
 *   - Survivor Determination (FUN_1008_2448):
 *       * Company with strictly greater tile count acquires the smaller.
 *       * In case of a tie in size, player or tie-breaking priority decides.
 *   - Shareholder Bonus Distribution:
 *       * Majority shareholder bonus: 10 * share price
 *       * Minority shareholder bonus:  5 * share price
 *       * Tied holdings divide bonuses equally among tied shareholders.
 *   - Share Disposition Options (FUN_1060_02a7..0397):
 *       Each holder of defunct company shares chooses in turn:
 *       1. KEEP:  Hold shares in anticipation of company rebirth.
 *       2. SELL:  Liquidate at pre-merger share price.
 *       3. TRADE: Exchange 2 defunct shares for 1 acquirer share (if stock remains).
 *   - UI Dialogs:
 *       * MERGER STATISTICS dialog (FUN_1060_0002 / FUN_1060_0045)
 *       * ShareOptionsDialog (FUN_1060_02a7 / FUN_1060_02f8)
 */
""",

    "ai.c": """/*
 * ai.c - Oil Baron Computer Player Artificial Intelligence Subsystem
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - AI Turn Controller (FUN_1008_ba6a):
 *       Executes when current player is flagged as Computer (DAT_10a8_39a4).
 *       Configurable thinking delay (s_SlowComputer).
 *   - Move Evaluation & Scoring (FUN_1008_a397):
 *       Evaluates each tile in the AI hand against candidate grid positions:
 *       * Forming a new company: evaluates available high-tier companies.
 *       * Expanding existing company: weighs immediate share value gain.
 *       * Triggering mergers: calculates majority/minority bonus payouts
 *         and net equity change before initiating a takeover.
 *       * Defending majority holdings.
 *   - Stock Purchase Decision (FUN_1008_bbb8):
 *       Evaluates cash reserves, target company growth rates, and majority
 *       contest status to purchase up to 3 shares per turn.
 */
""",

    "dialogs.c": """/*
 * dialogs.c - Oil Baron User Interface & Dialog Window Procedures
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Win16 / Borland ObjectWindows (OWL) dialog implementations:
 *       * OPTIONS dialog (FUN_1070_0002): grid size, start cash, share limits
 *       * NEW GAME dialog (FUN_1070_02d3): player names, human/computer types
 *       * ABOUT dialog (FUN_1070_0bb2): copyright, version, author credits
 *       * REGISTRATION dialog: shareware registration code validation
 *       * CHOOSE COMPANY dialog: company selection on tie/formation
 *       * DISCARD PIECE dialog: dead/unplayable tile discard
 *       * QUICK PEEK / GRAPH dialogs: portfolio and wealth graphs
 *   - Segments 1080 / 1068 / 1058: Dialog event handlers and widget controls
 */
""",

    "main_winproc.c": """/*
 * main_winproc.c - Application Lifecycle, Main WindowProc & Game State Machine
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - Win16 Entry Point (FUN_1000_0002 / FUN_1000_002f):
 *       Initializes task, registers TurboWindow window class, creates main frame.
 *   - Message Loop & Command Dispatch (Segment 1078, FUN_1078_0c59):
 *       Dispatches WM_COMMAND for menus (MAIN_MENU, COMPUTER, POPUP_MENU).
 *       Handles accelerator shortcuts (Alt+E, Alt+N, Alt+S, Alt+B, Alt+I, Alt+D, Alt+P).
 *   - Turn Loop State Machine (FUN_1008_0084, FUN_1008_0ed2):
 *       Phase 1: Place or Discard tile (Must place before purchasing)
 *       Phase 2: Resolve Company Formation or Merger
 *       Phase 3: Buy / Sell Stocks (up to 3 shares)
 *       Phase 4: Draw replacement tile
 *       Phase 5: Advance to next player ("Press <Space> For next Player")
 *   - End Game Evaluation (FUN_1008_21eb, FUN_1008_229d):
 *       Triggered when all active companies >= 11 safe tiles or any company >= 41 tiles.
 *       Displays Players' Wealth window with final liquidation totals.
 */
""",

    "data_tables.c": """/*
 * data_tables.c - Oil Baron Global Data Tables, Memory Layout & Runtime Stubs
 *
 * Reverse-Engineered from OILBARON.EXE (16-bit Windows 3.1 NE)
 *
 * Subsystem Overview:
 *   - DGROUP (Segment 10a8) memory layout and data structures.
 *   - Company definitions, baseline tiers, pricing curves.
 *   - Board grid layout constants and coordinate conversions.
 *   - Player structure fields (Cash, Stock Portfolio, Hand tiles).
 *   - Borland Pascal runtime stubs (Segment 10a0) and Windows API import stubs (Segment 1230).
 */

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================= */
/* OIL BARON REVERSE-ENGINEERED CONSTANTS & DATA DEFINITIONS                 */
/* ========================================================================= */

/* Company Identifiers */
typedef enum {
    COMPANY_NONE     = 0,
    COMPANY_IMPERIAL = 1,  /* High tier: Imperial Oil and Gas                */
    COMPANY_ROYAL    = 2,  /* Low tier:  Colored Cube Oil Co.                */
    COMPANY_AMERICAN = 3,  /* Mid tier:  American Natural Gas Co.            */
    COMPANY_ARABIAN  = 4,  /* Mid tier:  Oil Arabia, Inc.                    */
    COMPANY_TOWER    = 5,  /* Low tier:  Pyramid Oil Co.                     */
    COMPANY_SUPRA    = 6,  /* High tier: SupraCo Fuel Resources              */
    COMPANY_WEEMOCO  = 7,  /* High tier: Weem Oil Refining                   */
    TILE_PURPLE      = 8,  /* Single placed chit (unformed company)          */
    TILE_BLANK       = 9,  /* Unplayable dead tile                           */
    TILE_DISCARD     = 10  /* Discard square marker                          */
} CompanyId;

/* Company Pricing Tiers */
typedef enum {
    TIER_LOW  = 0,  /* Tower, Royal ($200 base price)                        */
    TIER_MID  = 1,  /* American, Arabian ($300 base price)                   */
    TIER_HIGH = 2   /* Imperial, Supra, Weemoco ($400 base price)            */
} CompanyTier;

/* Company Static Descriptor (36 bytes = 0x24 in DGROUP) */
typedef struct {
    char        name[32];          /* Full company name                      */
    CompanyTier tier;              /* Low, Mid, High                         */
    uint16_t    base_price;        /* Starting price when formed             */
    uint16_t    total_shares;      /* Total share limit (default 25)         */
    uint16_t    safe_size;         /* Immune from takeover threshold (11)    */
} CompanyInfo;

static const CompanyInfo OIL_COMPANIES[8] = {
    { "None",                     TIER_LOW,    0,    0,  0 },
    { "Imperial Oil and Gas",     TIER_HIGH, 400,   25, 11 },
    { "Colored Cube Oil Co.",     TIER_LOW,  200,   25, 11 },
    { "American Natural Gas Co.", TIER_MID,  300,   25, 11 },
    { "Oil Arabia, Inc.",         TIER_MID,  300,   25, 11 },
    { "Pyramid Oil Co.",          TIER_LOW,  200,   25, 11 },
    { "SupraCo Fuel Resources",   TIER_HIGH, 400,   25, 11 },
    { "Weem Oil Refining",        TIER_HIGH, 400,   25, 11 },
};

/* Board Dimensions & Memory Geometry */
#define BOARD_MIN_ROWS       12
#define BOARD_MIN_COLS       12
#define BOARD_DEFAULT_ROWS   12
#define BOARD_DEFAULT_COLS   12
#define BOARD_ROW_STRIDE     0x8c  /* 140 bytes per row in memory             */
#define BOARD_CELL_SIZE      5     /* 5 bytes per board grid square           */
#define BOARD_MAX_HAND_TILES 14    /* Max hand chits (default 6)              */

/* Player Struct Layout (252 bytes = 0xfc in DGROUP at DAT_10a8_30aa) */
typedef struct {
    char     name[32];             /* Player name (e.g. "Player1")           */
    bool     is_computer;          /* 0 = Human, 1 = Computer AI             */
    int32_t  cash;                 /* Player cash balance                    */
    uint16_t shares[8];            /* Shares held per company (1..7)         */
    uint16_t hand_tiles[14];       /* Chits currently in player hand         */
    uint16_t hand_count;           /* Active tiles in hand                   */
} PlayerData;

/* Stock Pricing Curve Matrix: Price($) based on Territory Tile Count */
/* [Tier 0: Low, Tier 1: Mid, Tier 2: High] */
typedef struct {
    uint16_t min_tiles;
    uint16_t max_tiles;
    uint16_t price_low;
    uint16_t price_mid;
    uint16_t price_high;
} StockPriceBracket;

static const StockPriceBracket STOCK_PRICE_CURVE[] = {
    {  2,  2, 200, 300, 400 },
    {  3,  3, 300, 400, 500 },
    {  4,  4, 400, 500, 600 },
    {  5,  5, 500, 600, 700 },
    {  6, 10, 600, 700, 800 },
    { 11, 20, 700, 800, 900 },
    { 21, 30, 800, 900, 1000 },
    { 31, 40, 900, 1000, 1100 },
    { 41, 999, 1000, 1100, 1200 }
};

/* End Game Conditions:
 * 1. All active companies have size >= 11 (Safe)
 * 2. Any single active company reaches size >= 41
 */
#define SAFE_COMPANY_THRESHOLD 11
#define END_GAME_TILE_COUNT    41

/* ========================================================================= */
/* DECOMPILED RUNTIME AND IMPORT STUBS                                      */
/* ========================================================================= */
"""
}


def extract_strings_from_exe(exe_path: str) -> dict:
    """Extract strings and table offsets from OILBARON.EXE for symbol reference."""
    strings_data = {
        "dgroup_strings": [],
        "dialog_identifiers": [],
        "companies": [
            {"id": 1, "name": "Imperial", "full_name": "Imperial Oil and Gas", "tier": "High", "base_price": 400},
            {"id": 2, "name": "Royal", "full_name": "Colored Cube Oil Co.", "tier": "Low", "base_price": 200},
            {"id": 3, "name": "American", "full_name": "American Natural Gas Co.", "tier": "Mid", "base_price": 300},
            {"id": 4, "name": "Arabian", "full_name": "Oil Arabia, Inc.", "tier": "Mid", "base_price": 300},
            {"id": 5, "name": "Tower", "full_name": "Pyramid Oil Co.", "tier": "Low", "base_price": 200},
            {"id": 6, "name": "Supra", "full_name": "SupraCo Fuel Resources", "tier": "High", "base_price": 400},
            {"id": 7, "name": "Weemoco", "full_name": "Weem Oil Refining", "tier": "High", "base_price": 400},
        ],
        "grid_offsets": {
            "board_cells": "DAT_10a8_22d2",
            "row_stride": 140,
            "cell_size": 5,
            "player_array": "DAT_10a8_30aa",
            "player_stride": 252,
            "active_companies": "DAT_10a8_3790"
        }
    }

    if not os.path.exists(exe_path):
        return strings_data

    with open(exe_path, "rb") as f:
        data = f.read()

    # DGROUP is at sector 477 (offset 122112, len 7402)
    dgroup_offset = 122112
    dgroup_len = 7402
    dgroup = data[dgroup_offset:dgroup_offset + dgroup_len]

    # Scan Pascal and null-terminated strings
    seen = set()
    for i in range(len(dgroup) - 1):
        slen = dgroup[i]
        if 2 <= slen <= 64 and i + 1 + slen <= len(dgroup):
            s_bytes = dgroup[i + 1:i + 1 + slen]
            if all(32 <= b < 127 for b in s_bytes):
                text = s_bytes.decode("ascii")
                if text not in seen and len(text) >= 3:
                    seen.add(text)
                    strings_data["dgroup_strings"].append({
                        "offset": f"0x{i:04x}",
                        "dgroup_symbol": f"DAT_10a8_{i:04x}",
                        "value": text
                    })

    # Dialog IDs
    dialog_names = [
        "ABOUT", "BLANK_MOVE", "CHOOSE COMPANY", "COMPANY_STATS", "DISCARDPIECE",
        "GAME_OVER", "KEEP_SHARES", "MERGER STATISTICS", "NEW GAME", "OPTIONS",
        "SHARE_STATS", "SHAREOPTIONSDIALOG", "SHARES DIALOG", "QUICK_PEEK",
        "OPTIONS_CHOSEN", "INTRODUCTION", "REGISTRATION"
    ]
    for d in dialog_names:
        strings_data["dialog_identifiers"].append(d)

    return strings_data


def main():
    print(f"Reading decompiled source from {ALL_C}...")
    if not os.path.exists(ALL_C):
        print(f"Error: {ALL_C} does not exist. Run ExportDecompiledC.java first.", file=sys.stderr)
        sys.exit(1)

    with open(ALL_C, "r", encoding="utf-8", errors="replace") as f:
        content = f.read()

    # Parse functions
    func_pattern = re.compile(
        r"/\* ======================================================== \*/\n"
        r"/\* Function: (\S+) @ (\S+) \*/\n"
        r"/\* ======================================================== \*/\n"
        r"(.*?)(?=(?:/\* ======================================================== \*/)|$)",
        re.DOTALL
    )

    matches = func_pattern.findall(content)
    print(f"Parsed {len(matches)} functions from {ALL_C}.")

    # Categorize
    modules = {
        "board_logic.c": [],
        "company_and_stocks.c": [],
        "mergers.c": [],
        "ai.c": [],
        "dialogs.c": [],
        "main_winproc.c": [],
        "data_tables.c": []
    }

    for fn_name, entry_addr, code in matches:
        mod = categorize_function(fn_name, entry_addr, code)
        modules[mod].append((fn_name, entry_addr, code))

    # Write each module
    for mod_name, func_list in modules.items():
        out_path = os.path.join(SRC_DIR, mod_name)
        print(f"Writing {mod_name} ({len(func_list)} functions)...")
        with open(out_path, "w", encoding="utf-8") as out:
            out.write(MODULE_DESCRIPTIONS.get(mod_name, f"/* {mod_name} */\n"))
            out.write("\n/* Function Index:\n")
            for fn, addr, _ in func_list:
                out.write(f" *   {fn:20} @ {addr}\n")
            out.write(" */\n\n")

            for fn, addr, code in func_list:
                out.write("/* ======================================================== */\n")
                out.write(f"/* Function: {fn} @ {addr} */\n")
                out.write("/* ======================================================== */\n")
                out.write(code.rstrip())
                out.write("\n\n")

    # Create aliases
    aliases = {
        "board.c": "board_logic.c",
        "stocks.c": "company_and_stocks.c",
        "main.c": "main_winproc.c"
    }
    for alias, target in aliases.items():
        alias_path = os.path.join(SRC_DIR, alias)
        if os.path.exists(alias_path) or os.path.islink(alias_path):
            os.remove(alias_path)
        os.symlink(target, alias_path)
        print(f"Created symlink {alias} -> {target}")

    # Export strings.json
    os.makedirs(SYM_DIR, exist_ok=True)
    strings_file = os.path.join(SYM_DIR, "strings.json")
    print(f"Exporting strings metadata to {strings_file}...")
    strings_meta = extract_strings_from_exe(EXE_PATH)
    with open(strings_file, "w", encoding="utf-8") as f:
        json.dump(strings_meta, f, indent=2)

    print("\nSummary of categorized modules:")
    for mod_name in sorted(modules.keys()):
        flist = modules[mod_name]
        out_path = os.path.join(SRC_DIR, mod_name)
        lines = sum(1 for _ in open(out_path))
        print(f"  {mod_name:22}: {len(flist):3d} functions, {lines:5d} lines")

    print("\nDecompilation organization complete.")


if __name__ == "__main__":
    main()
