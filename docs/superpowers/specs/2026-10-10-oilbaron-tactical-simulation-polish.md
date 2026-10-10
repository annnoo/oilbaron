# Oil Baron Tactical Game UX Polish & Share Liquidation Specification

## 1. Executive Summary & Goals

This specification refines the Oil Baron Tactical Exploration Simulation to address user feedback:
1. **Full Stock Exchange Mechanics (Selling & Liquidation)**: Enable full share liquidation (`SELL_SHARES`, `SELL_ALL_SHARES`) directly within the modern inline petroleum exchange without forcing players to use classic view or obsolete dialogs.
2. **True Game Aesthetic Matching Mockup**:
   - Eliminate the business app toolbar on the board (`[Grid] 10x12`, `Inspect Mode`, `Legend`, `Tooltip ON/OFF` buttons).
   - Adopt authentic tactical game styling from the agreed mockup (`geological-simulation.html`), including Google Font `Chakra Petch` for funds, coordinates, and wellheads.
   - Restyle the Concessions Dock to match tactile `52px × 62px` lease cards with punchy colored scan chips (`EXPAND`, `FOUND`, `MERGER`, `OPEN`, `DISCARD`) and glowing actuators.
   - Top Operations Bar matching the mockup's badge icon, glowing directive pill, and tactical buttons.

---

## 2. Global Constraints & Core Rules
- **Engine Invariance**: 100% preservation of all underlying game engine rules, math, and state machines in `src/engine/`.
- **Zero Routine Modals**: Share purchases and sales happen entirely inline in the bottom dock.
- **Viewport Constraints**: Locked to `100vw × 100vh` without page-level scrollbars.
- **Zero Regressions**: All 371+ tests must pass cleanly and production build (`npm run build`) must succeed with 0 errors.

---

## 3. Detailed Component Upgrades

### 3.1 Petroleum Equity Exchange with Selling & Liquidation (`InlineStockMarket.tsx`)
- In Step 2 (`buy_shares` phase), provide a prominent mode switcher:
  - `[ ACQUIRE SHARES (BUY) ]` vs `[ LIQUIDATE SHARES (SELL) ]` (or side-by-side buy/sell controls).
- In **Buy Mode**:
  - Keep the existing `+` / `-` counters for active companies (up to 3 total shares, checking cash budget and available bank shares).
  - Actuator button: `"CONFIRM PURCHASES & FINISH TURN"` (or `"PASS & FINISH TURN"` if 0 selected).
- In **Sell Mode**:
  - Show active companies where the player currently owns shares (`player.shares[companyId] > 0`).
  - Display current market price per share and total liquid value.
  - Quick action buttons per company: `[-1 Share]`, `[Sell All]`.
  - Global action button: `"LIQUIDATE ALL HOLDINGS"` (dispatches `SELL_ALL_SHARES`).
  - Instant cash credit with audio cash chime.
  - Actuator button: `"FINISH TURN"`.

### 3.2 Geological Survey Map Component (`GameBoard.tsx`)
- In modern simulation view mode, **hide the desktop dashboard toolbar**:
  - Remove `board-toolbar` containing `Inspect Mode`, `Hover Tooltip`, `Legend Toggle`, and `Grid Size Badge`.
  - The map is a pure 10×12 dark seismic grid with perimeter ruler numbers/letters.
  - Retain inspector popovers on cell click if needed, without cluttering the screen with permanent toggles.

### 3.3 Visual & Typographic Styling Overhaul (`index.css`)
- Import `Chakra Petch` font from Google Fonts:
  `@import url('https://fonts.googleapis.com/css2?family=Chakra+Petch:wght@500;700;900&family=Inter:wght@400;600;700&display=swap');`
- Define `--font-tactical: 'Chakra Petch', monospace;`.
- Apply `--font-tactical` to:
  - Funds (`.baron-meta .funds`, `.conglomerate-badge`, `.well`).
  - Parcel coordinates (`.lease-slip .lease-coord`, `.board-coord-header`, `.drill-actuator`).
  - Directive phase tags (`.turn-directive .phase`).
- **Tactile Lease Cards**:
  - Dimensions: `54px × 64px`, border-radius `6px`, background `rgba(30, 41, 59, 0.9)`.
  - Selected state: `transform: translateY(-8px); border: 2px solid #38bdf8; box-shadow: 0 10px 20px rgba(56, 189, 248, 0.4);`.
- **Top Ops Bar**:
  - Avatar badge icon: `38px × 38px`, gradient `#0284c7` to `#0369a1` with glowing cyan border `#38bdf8`.
  - Pill directive: `border-radius: 24px`, background `rgba(15, 23, 42, 0.95)`, glowing green/cyan box shadow.
- **Drill Actuator Button**:
  - Chunky, tactile, industrial button with glowing emerald gradient `linear-gradient(135deg, #16a34a, #15803d)`.

---

## 4. Verification & Testing
- Unit tests for `InlineStockMarket.tsx` covering:
  - Tab switching between Buy and Sell modes.
  - Selling shares and calling `onConfirmSales` or `onSellAll`.
  - Repurchase allowance reflection.
- Unit tests for `GameBoard.tsx` verifying clean survey grid without toolbar clutter.
- Verification of full test suite (`npm test`) and production build (`npm run build`).
