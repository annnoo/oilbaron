# Oil Baron Tactical Game UX Polish & Share Liquidation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Transform the webgame into the exact aesthetic of the tactical simulation mockup (`geological-simulation.html`) with Chakra Petch fonts, chunky tactile lease cards, clean map without business toolbars, and add full share selling & liquidation support in the inline dock.

**Architecture:**
- Update `index.css` to import Google Font `Chakra Petch`, define `--font-tactical`, restyle the `.ops-bar`, `.baron-profile`, `.turn-directive` pill, `.concessions-dock`, `.lease-slip`, and `.drill-actuator` to match `geological-simulation.html`.
- Upgrade `GameBoard.tsx` to add `hideToolbar?: boolean` (defaulting to `true` when in `.survey-map-container` / modern mode), removing the desktop dashboard toolbar.
- Upgrade `InlineStockMarket.tsx` to support both `buy` and `sell` tabs: allow players to sell shares (`onConfirmSales`, `onSellAll`), showing current holdings, price per share, and liquid cash return.
- Wire `ConcessionsDock.tsx` and `App.tsx` with `playerShares`, `onConfirmSales`, and `onSellAll`.

**Tech Stack:** React 18, TypeScript, Vitest, CSS3 (`Chakra Petch` Google Font, CSS variables, keyframe animations).

**Spec:** `docs/superpowers/specs/2026-10-10-oilbaron-tactical-simulation-polish.md`

## Global Constraints
- Engine Invariance: 100% preservation of all underlying game engine rules, math, and state machines in `src/engine/`.
- Zero Routine Modals: Share purchases and sales happen entirely inline in the bottom dock.
- Viewport Constraints: Locked to `100vw × 100vh` without page-level scrollbars.
- Zero Regressions: All 371+ tests must pass cleanly and production build (`npm run build`) must succeed with 0 errors.

---

### Task 1: Tactical Visual & Typographic Styling Overhaul (`index.css`)

**Files:**
- Modify: `webgame/src/index.css`
- Test: Vitest regression suite (`npm test`)

**Interfaces:**
- Produces: Google Font `Chakra Petch` import, `--font-tactical`, tactile `54px × 64px` lease slips with scan chips, glowing pill turn-directive, and emerald/gold actuators matching `geological-simulation.html`.

- [ ] **Step 1: Update `index.css` with mockup fonts and styling**
  - Add `@import url('https://fonts.googleapis.com/css2?family=Chakra+Petch:wght@500;700;900&family=Inter:wght@400;600;700&display=swap');`
  - Set `--font-tactical: 'Chakra Petch', monospace;`
  - Apply `--font-tactical` to `.baron-meta .funds`, `.turn-directive .phase`, `.lease-slip .lease-coord`, `.drill-actuator`, `.well`.
  - Refine `.baron-profile`: `border-radius: 12px; border: 1px solid rgba(56, 189, 248, 0.3); backdrop-filter: blur(12px); box-shadow: 0 4px 20px rgba(0,0,0,0.5);`.
  - Refine `.badge-icon`: `width: 38px; height: 38px; background: linear-gradient(135deg, #0284c7, #0369a1); border-radius: 8px; border: 1px solid #38bdf8; font-family: var(--font-tactical); font-weight: 900; color: white;`.
  - Refine `.turn-directive`: `border-radius: 24px; padding: 6px 20px; border: 1px solid rgba(34, 197, 94, 0.4); box-shadow: 0 0 15px rgba(34, 197, 94, 0.2);`.
  - Refine `.lease-slip`: `width: 54px; height: 64px; border-radius: 6px; background: #1e293b; border: 1px solid #475569;`.
  - Refine `.lease-slip.selected`: `transform: translateY(-8px); background: #0f2744; border: 2px solid #38bdf8; box-shadow: 0 10px 20px rgba(56, 189, 248, 0.4);`.
  - Refine `.drill-actuator`: `height: 64px; border-radius: 10px; background: linear-gradient(135deg, #16a34a, #15803d); border: 1px solid #4ade80; color: white; font-family: var(--font-tactical); font-weight: 900; box-shadow: 0 8px 25px rgba(22, 163, 74, 0.5);`.
- [ ] **Step 2: Run test suite to verify no regressions**
  Run: `cd webgame && npm test`
  Expected: PASS (371/371 tests).
- [ ] **Step 3: Commit Task 1**
  ```bash
  git add webgame/src/index.css
  git commit -m "feat(ui): update styling to match tactical simulation mockup with Chakra Petch fonts"
  ```

---

### Task 2: Clean Geological Map Without Business Toolbars (`GameBoard.tsx`)

**Files:**
- Modify: `webgame/src/components/GameBoard.tsx`
- Test: `webgame/src/components/__tests__/GameBoard.test.tsx`

**Interfaces:**
- Consumes: `hideToolbar?: boolean`
- Produces: Clean 10×12 map without `board-toolbar` (`Inspect Mode`, `Legend`, `Tooltip ON`) in modern view mode.

- [ ] **Step 1: Write test for `hideToolbar` prop in `GameBoard.test.tsx`**
  Assert that when `hideToolbar={true}`, `.board-toolbar` is not rendered.
- [ ] **Step 2: Run test to verify it fails**
  Run: `cd webgame && npx vitest run src/components/__tests__/GameBoard.test.tsx`
- [ ] **Step 3: Implement `hideToolbar` in `GameBoard.tsx`**
  - Add `hideToolbar?: boolean` to `GameBoardProps`.
  - In JSX, conditionally render `{(!hideToolbar) && <div className="board-toolbar">...</div>}`.
- [ ] **Step 4: Run test to verify it passes**
  Run: `cd webgame && npx vitest run src/components/__tests__/GameBoard.test.tsx`
- [ ] **Step 5: Commit Task 2**
  ```bash
  git add webgame/src/components/GameBoard.tsx webgame/src/components/__tests__/GameBoard.test.tsx
  git commit -m "feat(ui): allow hiding board toolbar in geological survey map mode"
  ```

---

### Task 3: Petroleum Equity Exchange with Share Selling & Liquidation (`InlineStockMarket.tsx`)

**Files:**
- Modify: `webgame/src/components/InlineStockMarket.tsx`
- Test: `webgame/src/components/__tests__/InlineStockMarket.test.tsx`

**Interfaces:**
- Consumes:
  - `playerShares?: Partial<Record<CompanyId, number>>`
  - `onConfirmSales?: (sales: ShareSale[]) => void`
  - `onSellAll?: () => void`
- Produces: Dual-mode exchange supporting both `[ ACQUIRE SHARES ]` and `[ LIQUIDATE SHARES ]` tabs with instant share selling buttons.

- [ ] **Step 1: Write tests for selling and liquidation in `InlineStockMarket.test.tsx`**
  - Test mode toggle between "buy" and "sell".
  - Test rendering owned shares with market prices in sell mode.
  - Test executing `onConfirmSales` when selling shares.
  - Test executing `onSellAll` when liquidating all holdings.
- [ ] **Step 2: Run test to verify it fails**
  Run: `cd webgame && npx vitest run src/components/__tests__/InlineStockMarket.test.tsx`
- [ ] **Step 3: Implement Sell Mode in `InlineStockMarket.tsx`**
  - Add state `mode: 'buy' | 'sell'`.
  - In Sell Mode: list companies where `playerShares[id] > 0`.
  - Provide `Sell 1` and `Sell All (company)` buttons per company.
  - Provide `"Liquidate All Portfolio Holdings"` button (`onSellAll`).
  - Update turn finished button: `"FINISH TURN"`.
- [ ] **Step 4: Run test to verify it passes**
  Run: `cd webgame && npx vitest run src/components/__tests__/InlineStockMarket.test.tsx`
- [ ] **Step 5: Commit Task 3**
  ```bash
  git add webgame/src/components/InlineStockMarket.tsx webgame/src/components/__tests__/InlineStockMarket.test.tsx
  git commit -m "feat(ui): add share selling and liquidation tabs to InlineStockMarket"
  ```

---

### Task 4: Concessions Dock & App Wiring for Selling Shares (`ConcessionsDock.tsx` & `App.tsx`)

**Files:**
- Modify: `webgame/src/components/ConcessionsDock.tsx`
- Modify: `webgame/src/App.tsx`
- Test: `webgame/src/components/__tests__/ConcessionsDock.test.tsx`
- Test: `webgame/src/__tests__/App.test.tsx`

**Interfaces:**
- Connects `gameState.players[activePlayerIndex].shares` and `handleConfirmSales` / `handleSellAllShares` from `App.tsx` through `ConcessionsDock` to `InlineStockMarket`.
- Passes `hideToolbar={true}` to `GameBoard` in `App.tsx`.

- [ ] **Step 1: Update `ConcessionsDock.tsx` props and tests**
  - Accept `playerShares`, `onConfirmSales`, `onSellAll` in `ConcessionsDockProps`.
  - Pass them down to `InlineStockMarket`.
- [ ] **Step 2: Update `App.tsx`**
  - Pass `playerShares={activePlayer?.shares ?? {}}`, `onConfirmSales={handleConfirmSales}`, and `onSellAll={handleSellAllShares}` to `<ConcessionsDock />`.
  - Pass `hideToolbar={true}` to `<GameBoard />` in the simulation screen.
- [ ] **Step 3: Run full test suite and build**
  Run: `cd webgame && npm test && npm run build`
  Expected: PASS (all tests pass, 0 errors).
- [ ] **Step 4: Commit Task 4**
  ```bash
  git add webgame/src/components/ConcessionsDock.tsx webgame/src/App.tsx webgame/src/components/__tests__/ConcessionsDock.test.tsx webgame/src/__tests__/App.test.tsx
  git commit -m "feat(ui): wire share liquidation and clean survey map in App simulation screen"
  ```

---

### Task 5: End-to-End Verification & Whole-Branch Review

**Files:**
- Full test suite & build check

- [ ] **Step 1: Run comprehensive tests (`npm test`)**
- [ ] **Step 2: Run production build (`npm run build`)**
- [ ] **Step 3: Conduct whole-branch review**
