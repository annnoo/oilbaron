# Oil Baron Web Game Modern UX Redesign Specification

## 1. Executive Summary & Goals

This specification details the comprehensive UX and visual overhaul of the *Oil Baron* web game (`webgame/`).
The goal is to transition the interface from feeling like a dense, modal-heavy enterprise/business application (with Windows 3.1 menus and developer-dashboard dark slate tables) into a **clean, modern strategy board game** (inspired by modern digital tabletop games like *Dorfromantik*, *Acquire*, and *Brass: Birmingham*) while preserving 100% of the underlying 1993 game engine rules and strategic depth.

### Core Objectives
1. **Crystal-Clear Turn Flow ("What to do next")**:
   - Introduce a prominent 2-step turn progression stepper (**Step 1: Drill Well** $\rightarrow$ **Step 2: Invest in Stocks** $\rightarrow$ **End Turn**).
   - Add a dynamic, conversational **Advisor Banner** above the board that clearly explains what each tile will do upon hover or selection (e.g., *Isolated*, *Expands American Oil (+1)*, *Founds new company*, *Merger*, or *Dead tile*).
2. **Zero-Modal Routine Gameplay**:
   - Eliminate full-screen modals for everyday turn actions.
   - Embed the **Stock Market** directly into an **Inline Action Dock** at the bottom of the board during Step 2 with quick `+`/`-` buttons, budget calculations, and an intuitive "Finish Turn" CTA.
   - Keep interactive dialogs strictly for true game events (Founding Company Selection, Merger Survivor Voting, Merger Payout Announcements, and Share Disposition).
3. **On-Demand Depth ("Ledger & Analytics Drawer")**:
   - Replace scattered menu items with a single, sleek slide-out **Ledger Drawer** accessible via a header button.
   - Consolidate Wealth Charts, Share Ownership Matrices, Company Stats, and War Room dominance analytics into clean, tabbed drawer views.
4. **Modern Strategy Tabletop Aesthetic**:
   - Replace the retro Windows 3.1 desktop menu bar (`File`, `Actions`, `Views`, etc.) with a sleek, minimalist modern game header with quick-access controls (Turn badge, Audio toggle, Rules, Ledger, New Game).
   - Board styling with warm charcoal tabletop tones, glowing safe chain borders (11+ tiles), and tactile chit rack.

---

## 2. Layout & Information Architecture

```
+---------------------------------------------------------------------------------------+
|  [Logo: OIL BARON]    |  Turn: Player 1 (Human) [Step 1: Drill]  |  [Audio] [Ledger] [?] [⚙]  |
+---------------------------------------------------------------------------------------+
|  MAIN PLAY AREA (approx 72%)                        | SIDEBAR (approx 28%)             |
|                                                     |                                 |
|  [Advisor Banner: "Select an oil parcel to drill"]  | [Player Standings Leaderboard]  |
|                                                     | - Player 1 ($6,400) [Active]    |
|  +-----------------------------------------------+  | - Computer 1 ($5,200)           |
|  |                                               |  |                                 |
|  |           10x12 BOARD GRID                    |  | [Market Snapshot]               |
|  |   - Coordinate labels A-J, 1-12               |  | - American: $500 (12 shares)    |
|  |   - Company colored chains with logos         |  | - Royal:    $300 (8 shares)     |
|  |   - Safe chain badges (11+ tiles)             |  | - Supra:    $400 (5 shares)     |
|  |   - Hover move preview ghosting               |  |                                 |
|  |                                               |  | [Recent Event Feed]             |
|  +-----------------------------------------------+  | "Player 1 expanded American"    |
|                                                     |                                 |
|  +-----------------------------------------------+  +---------------------------------+
|  | UNIFIED ACTION DOCK (Bottom)                  |  | (On Ledger Toggle)              |
|  | Mode A (Step 1): Chit Rack [A-1] [B-4] ...    |  | SLIDE-OUT LEDGER DRAWER:        |
|  | Mode B (Step 2): Inline Stock Quick-Buy Shelf |  | [Wealth] [Shares] [Stats] [Log] |
|  |                  [+ American -] [+ Royal -]   |  |                                 |
|  |                  [Confirm & Finish Turn (Alt+E)] |                                 |
|  +-----------------------------------------------+------------------------------------+
```

---

## 3. Detailed Component Specifications

### 3.1 Modern Game Header (`GameHeader.tsx`)
* Replaces the multi-level Windows dropdown `MenuBar.tsx` as the primary top bar.
* **Left**: Modernized brand mark with typography and clean oil drop icon.
* **Center**: Dynamic Turn Pill:
  * Shows active player name, color avatar, human/CPU indicator, and a mini 2-step stepper (`1. Drill` $\rightarrow$ `2. Buy Shares`).
  * If AI is executing, displays a smooth pulse animation ("Computer 1 is thinking...").
* **Right**: Quick action buttons:
  * Sound toggle (Muted / Active retro Web Audio).
  * Ledger toggle (opens on-demand analytics slide-out drawer).
  * Quick Rules modal toggle (`?`).
  * New Game / Settings modal toggle.

### 3.2 Dynamic Turn Advisor (`TurnAdvisor.tsx`)
* Fixed banner directly above the board grid.
* Provides real-time guidance based on `gameState.phase` and hovered/selected tiles:
  * `place_tile` (No selection): *"Select a parcel from your hand tray or click a highlighted cell on the board."*
  * `place_tile` (Hovered tile):
    * **Isolated**: *"Well {coord} is isolated. No oil chain formed yet."*
    * **Expansion**: *"Well {coord} expands {Company} to {size} wells. Share price increases to ${price}."*
    * **Founding**: *"Well {coord} forms a new oil company! You will choose a company and receive 1 founder share."*
    * **Merger**: *"Well {coord} triggers a merger! {Defunct} will be acquired by {Survivor}. Majority bonuses will be paid."*
    * **Dead Tile**: *"Well {coord} is an unplayable tile between two safe companies. Click Discard on your chit to draw a fresh parcel."*
  * `buy_shares`: *"Oil well drilled! You may now purchase up to 3 available company shares, or click Finish Turn."*
  * `choose_founding_company`: *"Select which oil company to incorporate."*
  * `choose_merger_survivor`: *"Tied chain size! Choose which company survives the merger."*
  * `dispose_merger_shares`: *"Merger buyout in progress. Choose whether to hold, sell, or trade defunct shares."*

### 3.3 Enhanced Game Board (`GameBoard.tsx`)
* Polished 10×12 grid with modern tabletop styling:
  * Subtle coordinate guides (A–J, 1–12) with crisp, non-distracting typography.
  * Tactile cell borders with company color indicators.
  * **Safe Company Badges**: Chains with $\ge 11$ tiles display a shield icon / badge indicating the chain is permanent and safe from takeover.
  * Interactive ghosting: When hovering a tile from hand, the board displays glowing target indicators with color-coded classification outlines.

### 3.4 Unified Action Dock & Inline Stock Market (`ActionDock.tsx` & `InlineStockMarket.tsx`)
* Docked directly below the board, providing seamless transition between turn steps without popups.
* **During Step 1 (Place Tile)**:
  * Displays the player's 6 tiles as tactile, physical-feeling cards.
  * Each chit shows coordinate (e.g. `B-7`) and a subtle classification pill:
    * Green: *Found*
    * Yellow: *Expand*
    * Orange: *Merger*
    * Blue: *Open*
    * Red/Gray: *Dead* (includes an immediate "Discard" button on the chit).
  * Clicking a tile highlights its location on the board; clicking again or clicking the highlighted board cell confirms placement.
* **During Step 2 (Buy Shares)**:
  * Smoothly expands into the **Inline Stock Market**:
    * Clean horizontal shelf listing all active companies with bank shares remaining.
    * For each company: company logo/color, share price, available bank shares, and simple `+`/`-` quantity selectors.
    * Real-time budget pill: *"Purchased: 2/3 shares | Total: $800 | Remaining Cash: $4,200"*.
    * Prominent primary CTA: **"Finish Turn (Alt+E)"** (or "Pass & Finish Turn" if 0 shares bought).

### 3.5 Slide-Out Ledger Drawer (`LedgerDrawer.tsx`)
* Slides out smoothly from the right side of the screen over the sidebar when toggled via the header's "Ledger & Analytics" button.
* Contains clean tabs for deep strategic information:
  * **Tab 1: Wealth & Portfolios**: Visual bar chart of all players' Cash vs Total Stock Net Worth.
  * **Tab 2: Share Ownership Matrix**: Comprehensive grid showing each player's share counts and percentage ownership per company.
  * **Tab 3: Company Overview**: Tier ladders, safe thresholds, and historical market values.
  * **Tab 4: Game Log**: Full chronological history of tile placements, mergers, payouts, and share purchases.

### 3.6 Event Dialogs (Reserved for Key Milestones)
* Modals are retained only for high-stakes, interactive decision events:
  * **Founding Company Selection**: Clean card picker with company logos and starting prices.
  * **Merger Survivor Selection**: Clean picker when tied companies merge.
  * **Merger Payouts & Share Disposition**: Clear step-by-step resolution screen for minority/majority shareholder payouts and 2-for-1 trades.
  * **Game Over Screen**: Final liquidation tally, podium, and victory trophy.

---

## 4. Component Hierarchy & File Architecture

```
webgame/src/
├── main.tsx
├── App.tsx                     # Main orchestrator (refactored to wire new layout)
├── index.css                   # Updated modern tabletop styling tokens & animations
├── assets/
│   ├── logos/                  # SVG company logos
│   └── audio.ts                # Web Audio chiptune synthesizer
├── components/
│   ├── GameHeader.tsx          # NEW: Modern header replacing desktop MenuBar
│   ├── TurnAdvisor.tsx         # NEW: Dynamic conversational turn guidance banner
│   ├── GameBoard.tsx           # Polished 10x12 tabletop board with safe shields
│   ├── ActionDock.tsx          # NEW: Unified bottom container for Hand + Inline Market
│   ├── PlayerHand.tsx          # Tactile chit rack with inline discard actions
│   ├── InlineStockMarket.tsx   # NEW: Inline stock purchasing shelf (replaces modal for Step 2)
│   ├── GameSidebar.tsx         # NEW: Consolidated sidebar (Standings + Market snapshot)
│   ├── LedgerDrawer.tsx        # NEW: Smooth slide-out drawer for Wealth, Shares & Stats
│   └── modals/                 # Key decision dialogs (Founding, Merger, Rules, New Game)
```

---

## 5. State Management & Data Flow

* Zero breaking changes to `engine/reducer.ts`, `engine/types.ts`, or `engine/board.ts`.
* The state machine flow remains identical:
  1. `place_tile` $\rightarrow$ dispatch `PLACE_TILE`.
  2. If founding needed $\rightarrow$ `choose_founding_company` $\rightarrow$ dispatch `CHOOSE_FOUNDING_COMPANY`.
  3. If merger needed $\rightarrow$ `choose_merger_survivor` / `dispose_merger_shares` $\rightarrow$ dispatch merger actions.
  4. Advance to `buy_shares` $\rightarrow$ active player adjusts quantities in `InlineStockMarket` $\rightarrow$ clicks "Finish Turn" $\rightarrow$ dispatches `BUY_SHARES` (if any) and `END_TURN`.
* Keyboard shortcuts are maintained:
  * `Alt+E` / `Enter`: Finish Turn / End Turn.
  * `Alt+B`: Focus / toggle Stock Market.
  * `Alt+L`: Toggle Ledger Drawer.
  * `Alt+D`: Discard selected dead chit.

---

## 6. Verification & Acceptance Criteria

1. **Clean Strategy Game Aesthetic**:
   - The top desktop menu bar is gone, replaced with a modern game header.
   - The UI looks like a clean digital strategy tabletop game, avoiding enterprise/spreadsheet density.
2. **Turn Progression & Guidance**:
   - The player always knows what to do via the TurnAdvisor banner and 2-step stepper.
   - Hovering over hand chits provides descriptive, plain-English explanations of move consequences.
3. **Zero Routine Modals**:
   - Buying shares in Step 2 is performed entirely within the bottom action dock with zero modal popups.
   - Discarding dead tiles can be done directly from the chit badge.
4. **On-Demand Depth**:
   - Clicking "Ledger" smoothly reveals Wealth charts, Share matrices, and Company stats without leaving the board view.
5. **Quality & Test Coverage**:
   - All existing engine and component tests pass (`npm test`).
   - Clean production build with zero TypeScript errors (`npm run build`).
