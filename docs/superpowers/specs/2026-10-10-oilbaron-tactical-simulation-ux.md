# Oil Baron Tactical Exploration Simulation UX Specification

## 1. Executive Summary & Vision

This specification defines the transformation of the *Oil Baron* web game (`webgame/`) into a **Tactile Mobile Exploration Simulation**.
Rather than looking like an enterprise SaaS application (multi-level menus, sidebars, data tables) or a toy wooden board game (dominoes, poker felt, emoji badges), the game is designed as an **immersive, real-feeling geological oil exploration simulation** (inspired by modern tactical mobile games like *Turmoil*, *Plague Inc*, and *Subterfuge*).

The player sits at a tactical field operations terminal surveying a 10×12 oil basin concession map, deploying drill rigs to strike oil, expanding corporate pipeline networks, engineering corporate takeovers, and trading petroleum equities.

---

## 2. Global Constraints & Core Rules
- **Engine Invariance**: 100% preservation of all underlying 1993 game engine rules, math, and state machines in `src/engine/` (`reducer.ts`, `board.ts`, `market.ts`, `ai.ts`, `types.ts`).
- **Full Viewport Immersion**: The game runs locked to `100vw × 100vh` without page-level scrollbars, outer container cards, or webapp wrappers.
- **Zero Routine Modals**: Day-to-day turn actions (drilling parcels, purchasing shares, discarding dead leases) happen directly on the tactical canvas without modal popups. Modals are reserved strictly for high-stakes corporate drama (Founding Incorporation, Merger Takeover Announcements, and Share Disposition).
- **Quality & Tests**: Clean production build (`npm run build`) and all unit/integration tests must pass with 0 errors.

---

## 3. Information Architecture & Screen Anatomy

```
+------------------------------------------------------------------------------------------+
|  [OP] PLAYER 1  $6,400    |  DRILL OPERATION: Target Parcel B-5  |  CPU: $5,200  [📊] [⚙] |
+------------------------------------------------------------------------------------------+
|  [American: $500 · 5w]      [Imperial: SAFE $800 · 12w]      [Supra: $400 · 3w]          |
|                                                                                          |
|                        TACTICAL OIL CONCESSION MAP (10x12)                               |
|        - Deep petroleum slate terrain with seismic grid lines                            |
|        - Quiet empty parcels (no text clutter, clean guide dots)                         |
|        - Active wellheads with industrial pumpjack glyphs (▲ A, ◆ I)                     |
|        - Pipeline network borders linking adjacent company wells                         |
|        - Fortified Refinery Hub borders on safe chains (>= 11 wells)                     |
|        - High-contrast target highlight when a parcel is armed                           |
|                                                                                          |
|  +----------------------------------------------------+   +---------------------------+  |
|  | CONCESSIONS TRAY: Active Leases in Hand            |   | [ DRILL WELL ]            |  |
|  | [B-5: EXPAND] [C-2: FOUND] [F-8: MERGER] ... [E-1] |   |   PARCEL B-5              |  |
|  +----------------------------------------------------+   +---------------------------+  |
+------------------------------------------------------------------------------------------+
```

---

## 4. Component Details & Interactions

### 4.1 Tactical Operations Bar (`TacticalOpsBar.tsx`)
* Replaces standard web navigation with a sleek, field-operations HUD:
  * **Left (Concessionaire Profile)**: Badge icon, active player name, operating cash (`$6,400`), and net worth.
  * **Center (Mission Directive)**: High-contrast directive banner that guides the player through each step:
    * Step 1 (Survey & Drill): *"Target parcel B-5 to expand American Oil field"* (updates dynamically on hover/select).
    * Step 2 (Petroleum Exchange): *"Acquire up to 3 corporation shares or pass turn"*.
    * Special States: CPU thinking indicator, Founding incorporation prompt, Merger takeover status.
  * **Right (Operations Menu & Stats)**:
    * Audio toggle, Tactical Ledger toggle (`📊`), Game Settings (`⚙`), and 1993 Classic Mode switcher.

### 4.2 Corporate Pipeline Ticker
* A streamlined, borderless status strip directly beneath the top HUD showing all active oil conglomerates:
  * Company name with corporate color bar.
  * Current stock price per barrel.
  * Well count and `SAFE` refinery badge if $\ge 11$ wells.

### 4.3 Geological Survey Map (`SurveyMap.tsx` / `GameBoard.tsx`)
* A 10×12 grid rendered as an authentic oil exploration survey map:
  * **Empty Parcels**: Clean, dark seismic survey cells with subtle center dots. No noisy `A-1` text inside empty cells; clean row/column ruler coordinates on perimeter edges only.
  * **Drilled Wellheads**: Solid company color blocks with clean typography and industrial rig markers (`▲` American, `◆` Imperial, etc.).
  * **Refinery Hubs (Safe Chains)**: Chains with $\ge 11$ wells receive a distinctive industrial armor border, communicating total takeover immunity.
  * **Target Reticle**: Arming a parcel in hand paints an animated tactical crosshair reticle on the target cell.

### 4.4 Concessions Dock & Petroleum Exchange (`ConcessionsDock.tsx`)
* Floating command console at the bottom of the screen:
  * **Step 1 (Drilling Leases)**:
    * Displays the player's 6 parcel leases as crisp tactical cards.
    * Real-time seismic scan chips: `EXPAND` (Green), `FOUND` (Cyan), `MERGER` (Orange), `OPEN` (Slate), `DISCARD` (Red with strike-through on dead unplayable parcels).
    * Selecting a lease arms the **"DRILL WELL — PARCEL [COORD]"** actuator button.
    * Tapping the actuator button (or clicking the target cell on the board) drills the well.
  * **Step 2 (Petroleum Exchange)**:
    * The lease tray smoothly transitions into the equity purchasing shelf.
    * Lists active companies with bank equity available, current price, and `+` / `-` selectors (max 3 total shares, cash budget enforced).
    * The main actuator button transitions into **"CONFIRM TRADES & FINISH TURN"** (or **"PASS TURN"**).

### 4.5 Tactical Ledger Sheet (`TacticalLedgerSheet.tsx`)
* Accessible on-demand via the `📊` button or `Alt+L`:
  * Slides up smoothly from the bottom as a tactical field operations sheet.
  * Tab 1: **Portfolios & Net Worth** (Cash vs Stock equity breakdown).
  * Tab 2: **Corporate Shareholdings Matrix** (Share counts per player per company).
  * Tab 3: **Conglomerate Diagnostics** (Well counts, prices, safe thresholds).
  * Tab 4: **Operations History Log** (Chronological record of drill strikes, mergers, and dividends).

---

## 5. Verification & Acceptance Criteria
1. Fullscreen tactical game feel: zero webapp dashboard chrome, zero toy board game elements, 100vw × 100vh locked viewport.
2. Complete turn flow: Step 1 drill actuation $\rightarrow$ Step 2 stock equity purchasing $\rightarrow$ finish turn.
3. All original 1993 game rules remain 100% intact.
4. Clean production build (`npm run build`) and 100% test pass rate (`npm test`).
