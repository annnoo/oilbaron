# Oil Baron Modern React Web Game Implementation Plan (Phase 3)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a complete, responsive, and fully playable client-side React browser game for Oil Baron in `webgame/`, representing every feature, dialog, menu, statistic, and mechanic of the original 1993 Windows game.

**Architecture:** React 18 + Vite + TypeScript frontend powered by the headless engine in `webgame/src/engine/`, with modular components for the top menu bar, 10x12 interactive board, player hand, stock ticker, procedural chiptune audio, and dedicated accessible modal dialogs for all original Windows 3.1 screens.

**Tech Stack:** React 18, Vite 5, TypeScript 5, Lucide React (icons), Web Audio API.

**Spec:** [docs/superpowers/specs/2026-10-09-oilbaron-ui-design.md](file:///home/anno/Code/gamedev/oilbaron/docs/superpowers/specs/2026-10-09-oilbaron-ui-design.md)

## Global Constraints
- Target directory is `webgame/`.
- All original 1993 Oil Baron features must be represented.
- Clean build: `npm run build` must succeed with 0 TypeScript/build errors.
- Existing engine tests must continue to pass 100% on `npm test`.
- Strict git discipline: NEVER commit `game/` binaries or `node_modules/`.

---

### Task 1: React & UI Scaffolding Setup

**Files:**
- Modify: `webgame/package.json`
- Modify: `webgame/vite.config.ts`
- Modify: `webgame/tsconfig.json`
- Create: `webgame/index.html`
- Create: `webgame/src/index.css`
- Create: `webgame/src/assets/audio.ts`
- Create: `webgame/src/main.tsx`
- Test: Build verification via `npm run build`

- [ ] **Step 1: Install React & UI dependencies**
Install `react`, `react-dom`, `@types/react`, `@types/react-dom`, `@vitejs/plugin-react`, `lucide-react`.

- [ ] **Step 2: Configure Vite & TypeScript**
Configure `@vitejs/plugin-react` in `vite.config.ts` and set `"jsx": "react-jsx"` in `tsconfig.json`.

- [ ] **Step 3: Setup `index.html` & `src/index.css`**
Add rich styling, dark industrial oil-tycoon theme, CSS variables for company colors, and responsive grid layout.

- [ ] **Step 4: Implement Procedural Audio Synthesizer (`src/assets/audio.ts`)**
Implement Web Audio API synthesizer for: `click`, `foundCompany`, `mergerAlert`, `cashChime`, `victoryFanfare`.

- [ ] **Step 5: Verify build**
Run `npm run build` to confirm setup works.

- [ ] **Step 6: Commit Task 1**
Commit with: `chore(ui): scaffold react frontend, audio synthesizer, and styling tokens`

---

### Task 2: Top Menu Bar & Company Ticker

**Files:**
- Create: `webgame/src/components/MenuBar.tsx`
- Create: `webgame/src/components/CompanyTicker.tsx`
- Test: Component rendering in `App.tsx`

- [ ] **Step 1: Implement `MenuBar.tsx`**
Replicate original menus:
- Game: New Game (Same Options), New Game (New Options), Save Game, Load Game, Sound Toggle.
- Actions: Buy/Sell Shares (`Alt+B`), Discard Chit (`Alt+D`), Place Chit (`Alt+P`), End Turn (`Alt+E`).
- Views: Wealth Bar Chart (`Alt+N`), Share Graph (`Alt+S`), Company Stats (`Alt+C`), War Room (`Alt+W`), Discarded Pieces.
- Settings: Options, Change Active Player to CPU/Human, Slow Computer Turns toggle, Hide Chits toggle.
- Help: Rules, About Oil Baron.

- [ ] **Step 2: Implement `CompanyTicker.tsx`**
Display active oil companies, tile counts, live share prices, bank share inventory, and Safe badges ($\ge 11$ tiles).

- [ ] **Step 3: Verify & Commit Task 2**
Commit with: `feat(ui): implement classic menu bar with keyboard shortcuts and company ticker`

---

### Task 3: Interactive 10x12 Game Board & Tile Inspector

**Files:**
- Create: `webgame/src/components/GameBoard.tsx`
- Create: `webgame/src/components/TileInspector.tsx`
- Test: Board tile rendering and coordinate mapping

- [ ] **Step 1: Implement `GameBoard.tsx`**
- 10 rows (A–J) $\times$ 12 columns (1–12) responsive grid.
- Render empty grid slots, unincorporated placed chits, and company-controlled tiles with logos/initials.
- Move classification hover previews: color highlights showing whether hovering a chit would result in an isolated placement, company expansion, company founding, merger, or illegal safe merger (dead tile).
- Interactive click to place selected tile.

- [ ] **Step 2: Implement `TileInspector.tsx`**
Quick peek tooltip / inspector modal showing cell coordinates, company owner, chain size, and connectivity.

- [ ] **Step 3: Verify & Commit Task 3**
Commit with: `feat(ui): implement interactive 10x12 game board with move previews and inspector`

---

### Task 4: Player Hand Tray & Sidebar Scoreboard

**Files:**
- Create: `webgame/src/components/PlayerHand.tsx`
- Create: `webgame/src/components/PlayerScoreboard.tsx`
- Test: Hand selection and scoreboard updates

- [ ] **Step 1: Implement `PlayerHand.tsx`**
- Chits tray displaying active player's tiles with coordinate tags (e.g., `C-4`, `F-8`).
- Selected tile highlight; "Place Chit" button.
- "Discard Dead Tile" button enabled when holding dead tiles.
- "Hide Chits" toggle for pass-and-play hotseat privacy.

- [ ] **Step 2: Implement `PlayerScoreboard.tsx`**
- Sidebar listing all 2–6 players with cash reserves, total net worth, active turn badge, and Human vs CPU badge.
- Inline "Change to Human / Change to CPU" button per player (faithful to original Computer menu).

- [ ] **Step 3: Verify & Commit Task 4**
Commit with: `feat(ui): implement player hand tray and interactive scoreboard with CPU toggle`

---

### Task 5: Game Setup, Options & Founding Modals

**Files:**
- Create: `webgame/src/components/modals/NewGameModal.tsx`
- Create: `webgame/src/components/modals/OptionsModal.tsx`
- Create: `webgame/src/components/modals/FoundingCompanyModal.tsx`
- Create: `webgame/src/components/modals/DiscardedTilesModal.tsx`

- [ ] **Step 1: Implement `NewGameModal.tsx`**
Configure 2–6 players, player names, colors, and Human vs CPU toggle.

- [ ] **Step 2: Implement `OptionsModal.tsx`**
Ruleset selection (Classic 10 founder shares vs Modern 1 founder share), hand size (4–10), safe threshold (11), starting cash ($6,000), slow computer turns toggle.

- [ ] **Step 3: Implement `FoundingCompanyModal.tsx`**
Dialog triggered when a move forms a company, allowing player to pick which available company to found and awarding the founder bonus.

- [ ] **Step 4: Implement `DiscardedTilesModal.tsx`**
Modal displaying all permanently dead and discarded tiles throughout match history.

- [ ] **Step 5: Verify & Commit Task 5**
Commit with: `feat(ui): implement game setup, options, company founding, and discard modals`

---

### Task 6: Stock Market & Company Statistics Modals

**Files:**
- Create: `webgame/src/components/modals/StockMarketModal.tsx`
- Create: `webgame/src/components/modals/CompanyStatsModal.tsx`

- [ ] **Step 1: Implement `StockMarketModal.tsx`**
- Modal/drawer opened during `buy_shares` phase or via `Alt+B`.
- Buy up to 3 shares per turn across active companies with remaining stock.
- Live cost tally, cash budget check, and "Confirm Purchase" or "Skip" buttons.

- [ ] **Step 2: Implement `CompanyStatsModal.tsx`**
- Overview of all 7 companies: current size, share price, available bank shares, Safe status.
- Shareholder rankings table showing top shareholders for each company.

- [ ] **Step 3: Verify & Commit Task 6**
Commit with: `feat(ui): implement stock market purchasing drawer and company statistics modal`

---

### Task 7: Merger Modals (Survivor, Bonuses & Share Disposition)

**Files:**
- Create: `webgame/src/components/modals/MergerSurvivorModal.tsx`
- Create: `webgame/src/components/modals/MergerStatsModal.tsx`
- Create: `webgame/src/components/modals/ShareDispositionModal.tsx`

- [ ] **Step 1: Implement `MergerSurvivorModal.tsx`**
Picker dialog when 2 or more merging companies tie for largest size.

- [ ] **Step 2: Implement `MergerStatsModal.tsx`**
Visual breakdown showing 1st place majority bonus ($10 \times P$) and 2nd place minority bonus ($5 \times P$), and cash credited to players.

- [ ] **Step 3: Implement `ShareDispositionModal.tsx`**
Interactive step-by-step modal for each shareholder of the defunct company:
- Sell for cash at pre-merger price.
- Trade 2-for-1 for surviving company shares (if available).
- Hold shares.
- Real-time calculation of shares remaining and cash gained.

- [ ] **Step 4: Verify & Commit Task 7**
Commit with: `feat(ui): implement complete merger workflow modals for survivor, payouts, and share disposition`

---

### Task 8: Data Visualizations, Save/Load & Game Over

**Files:**
- Create: `webgame/src/components/modals/WealthChartModal.tsx`
- Create: `webgame/src/components/modals/ShareGraphModal.tsx`
- Create: `webgame/src/components/modals/WarRoomModal.tsx`
- Create: `webgame/src/components/modals/SaveLoadModal.tsx`
- Create: `webgame/src/components/modals/HelpModal.tsx`
- Create: `webgame/src/components/modals/GameOverModal.tsx`

- [ ] **Step 1: Implement `WealthChartModal.tsx`**
Interactive SVG/CSS bar graph comparing players' Cash vs Stock Value.

- [ ] **Step 2: Implement `ShareGraphModal.tsx`**
Matrix chart showing share distribution per company across all players, with toggle for **Percentages** vs **Totals**!

- [ ] **Step 3: Implement `WarRoomModal.tsx`**
Strategic overview dashboard displaying board territory control, safe company dominance, and leader rankings.

- [ ] **Step 4: Implement `SaveLoadModal.tsx` & `HelpModal.tsx`**
Save to / load from `localStorage`, export/import JSON files, and comprehensive in-game rulebook.

- [ ] **Step 5: Implement `GameOverModal.tsx`**
Final leaderboard, portfolio liquidation breakdown, winner celebration, and "New Game" button.

- [ ] **Step 6: Verify & Commit Task 8**
Commit with: `feat(ui): implement wealth chart, share graph, war room, save/load, and game over screens`

---

### Task 9: App Integration, CPU Loop & Full Verification

**Files:**
- Modify: `webgame/src/App.tsx`
- Test: Full build, lint, and browser verification

- [ ] **Step 1: Wire up `App.tsx` state orchestrator**
- Connect `useReducer` to `gameReducer`.
- Integrate AI turn execution with adjustable delay when active player is a CPU.
- Bind global keyboard shortcuts (`Alt+P`, `Alt+B`, `Alt+D`, `Alt+E`, `Alt+N`, `Alt+S`, `Alt+C`, `Alt+W`, `Space`).
- Integrate sound effects triggers on placements, mergers, purchases, and game over.

- [ ] **Step 2: Run test suite & build**
Run:
```bash
cd webgame
npm test
npm run build
```
Expected: 100% tests pass, build completes with 0 errors.

- [ ] **Step 3: Commit Task 9**
Commit with: `feat(ui): wire main application orchestrator, CPU turn loop, and keyboard shortcuts`
