# All Authentic 1993 Oil Baron Features Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement all remaining gameplay features from the reverse-engineered 1993 Windows 3.1 `OILBARON.EXE` binary: Wildcard Blank pieces (`BLANK_MOVE`), On-grid Discarded markers (`'X'`), Save/Load persistence (localStorage & JSON file import/export), Dumb vs. Smart AI difficulty heuristics, Dynamic board sizing (up to 26x26), Economic variations (randomized prices & dividends), and the 8th corporation (Purple Oil Co.).

**Architecture:** Extend the modular Redux-style engine (`reducer.ts`, `board.ts`, `ai.ts`, `market.ts`, `setup.ts`, `persistence.ts`) and modern/classic UI layers with strict TDD. All features are data-driven via `GameConfig` and options toggles to preserve backward compatibility and ensure clean deterministic tests.

**Tech Stack:** TypeScript, React, Vitest, Lucide React, HTML5 LocalStorage & File API, Canvas/Pixelated retro CSS.

**Spec:** `docs/game_rules.md`, `decompile/resources/help_text.txt`, and `decompile/resources/dialogs.rc`.

---

## Global Constraints
- Every engine change must be covered with unit tests adhering to strict TDD (tests written and confirmed red before implementation).
- All existing 23 test suites (228 tests) must remain 100% passing.
- Zero TypeScript errors (`tsc && vite build`).
- Work strictly inside the isolated worktree `.worktrees/feat-all-classic-gameplay` on branch `feat/all-classic-gameplay`.

---

### Task 1: Save & Load Game Persistence (`persistence.ts` & Modals)

**Files:**
- Create: `webgame/src/engine/persistence.ts`
- Create: `webgame/src/engine/__tests__/persistence.test.ts`
- Create: `webgame/src/components/modals/SaveLoadModal.tsx`
- Create: `webgame/src/components/modals/__tests__/SaveLoadModal.test.tsx`
- Modify: `webgame/src/App.tsx`
- Modify: `webgame/src/components/MenuBar.tsx`

**Interfaces:**
- `serializeGameState(state: GameState): string`
- `deserializeGameState(json: string): GameState`
- `saveToLocalStorage(state: GameState, slot?: string): boolean`
- `loadFromLocalStorage(slot?: string): GameState | null`
- `listLocalStorageSaveSlots(): Array<{ slot: string; date: string; players: string[] }>`

- [x] **Step 1: Write failing tests for serialization, validation, and localStorage save/load**
- [x] **Step 2: Run tests to verify failure**
- [x] **Step 3: Implement `persistence.ts` with schema validation and export/import helpers**
- [x] **Step 4: Run tests to verify they pass**
- [x] **Step 5: Create `SaveLoadModal.tsx` with slots, download JSON file, and upload JSON file**
- [x] **Step 6: Write unit tests in `SaveLoadModal.test.tsx` and verify passing**
- [x] **Step 7: Wire `SaveLoadModal` into `MenuBar.tsx` (`save_game`, `load_game`) and `App.tsx`**
- [x] **Step 8: Run all tests and commit**

---

### Task 2: On-Grid Discarded Markers (`ShowDiscard = TRUE` / `'X'` Tiles)

**Files:**
- Modify: `webgame/src/engine/types.ts`
- Modify: `webgame/src/engine/reducer.ts`
- Modify: `webgame/src/engine/__tests__/reducer.test.ts`
- Modify: `webgame/src/components/GameBoard.tsx`
- Modify: `webgame/src/components/classic/ClassicGameView.tsx`
- Modify: `webgame/src/components/modals/OptionsModal.tsx`

**Interfaces:**
- `CellState.type`: `'empty' | 'unincorporated' | 'company' | 'discarded'` (or `isDiscarded?: boolean`)
- `GameConfig.showDiscard?: boolean`
- `DISCARD_DEAD_TILE` action marks the board cell as discarded when `showDiscard` is true.

- [ ] **Step 1: Write failing tests in `reducer.test.ts` for marking discarded grid cell**
- [ ] **Step 2: Run tests to verify failure**
- [ ] **Step 3: Update `types.ts` and `reducer.ts` to set cell state on discard**
- [ ] **Step 4: Run tests to verify they pass**
- [ ] **Step 5: Update `GameBoard.tsx` and `ClassicGameView.tsx` to render `'X'` / discard insignia on discarded cells**
- [ ] **Step 6: Add `Show Discarded Markers ('X')` toggle to `OptionsModal.tsx`**
- [ ] **Step 7: Run tests and commit**

---

### Task 3: Wildcard "Blank Pieces" Mechanic (`Blanks = TRUE` & `BlankPieceModal`)

**Files:**
- Modify: `webgame/src/engine/types.ts`
- Modify: `webgame/src/engine/config.ts`
- Modify: `webgame/src/engine/board.ts`
- Modify: `webgame/src/engine/reducer.ts`
- Modify: `webgame/src/engine/__tests__/reducer.test.ts`
- Create: `webgame/src/components/modals/BlankPieceModal.tsx`
- Create: `webgame/src/components/modals/__tests__/BlankPieceModal.test.tsx`
- Modify: `webgame/src/App.tsx`
- Modify: `webgame/src/components/PlayerHand.tsx`
- Modify: `webgame/src/components/classic/ClassicGameView.tsx`

**Interfaces:**
- Tile in hand can have `isBlank?: boolean`
- `GameAction`: `| { type: 'PLACE_BLANK_TILE'; blankTileCoord: TileCoord; targetCoord: TileCoord }`
- When `config.allowBlanks` is enabled, blank tiles (`isBlank: true`) are generated in the tile pool.
- Playing a blank tile triggers the `BlankPieceModal` coordinate selector to choose any unoccupied cell.

- [ ] **Step 1: Write failing tests for blank tile pool injection and `PLACE_BLANK_TILE` reducer logic**
- [ ] **Step 2: Run tests to verify failure**
- [ ] **Step 3: Implement blank tile support in `reducer.ts`, `board.ts`, and `config.ts`**
- [ ] **Step 4: Run tests to verify they pass**
- [ ] **Step 5: Create `BlankPieceModal.tsx` enabling players to pick any vacant square on the board**
- [ ] **Step 6: Write unit tests in `BlankPieceModal.test.tsx` and verify passing**
- [ ] **Step 7: Wire blank piece interaction into `PlayerHand.tsx`, `ClassicGameView.tsx`, and `App.tsx`**
- [ ] **Step 8: Run all tests and commit**

---

### Task 4: AI Difficulty Heuristics (Dumb AI vs Smart AI `ai.c`)

**Files:**
- Modify: `webgame/src/engine/types.ts`
- Modify: `webgame/src/engine/ai.ts`
- Modify: `webgame/src/engine/__tests__/ai.test.ts`
- Modify: `webgame/src/components/modals/NewGameModal.tsx`
- Modify: `webgame/src/components/modals/__tests__/Task5Modals.test.tsx`

**Interfaces:**
- `PlayerState.aiDifficulty?: 'dumb' | 'smart'`
- `PlayerConfig.aiDifficulty?: 'dumb' | 'smart'`
- `simulateAiTurn(state, difficulty)`:
  - If `'dumb'`: introduces random noise (`Math.random()`), suboptimal tile plays, random share buys.
  - If `'smart'`: strict majority threshold defense, growth rate optimization, and merger trade arbitrage.

- [ ] **Step 1: Write failing tests in `ai.test.ts` testing Dumb AI randomness vs Smart AI deterministic optimality**
- [ ] **Step 2: Run tests to verify failure**
- [ ] **Step 3: Implement Dumb AI noise generator and Smart AI heuristics in `ai.ts`**
- [ ] **Step 4: Run tests to verify they pass**
- [ ] **Step 5: Add AI difficulty selector (Dumb vs Smart) per CPU player in `NewGameModal.tsx`**
- [ ] **Step 6: Run tests and commit**

---

### Task 5: 8th Corporation ("Purple Oil Company") & Economy Randomizers

**Files:**
- Modify: `webgame/src/engine/types.ts`
- Modify: `webgame/src/engine/config.ts`
- Modify: `webgame/src/engine/setup.ts`
- Modify: `webgame/src/engine/__tests__/config.test.ts`
- Modify: `webgame/src/engine/__tests__/setup.test.ts`
- Modify: `webgame/src/assets/classicAssets.ts`
- Modify: `webgame/src/components/modals/OptionsModal.tsx`

**Interfaces:**
- Add `'purple'` company definition to `CLASSIC_COMPANIES` (ID: `purple`, Name: `Purple Oil Company, Int.`, Tier: `low`, Base: \$7.50 / \$200, Color: `#8b5cf6`, Dividend: 6.7%).
- `GameConfig.randomSharePrices?: boolean`
- `GameConfig.randomDividends?: boolean`
- `GameConfig.enableEighthCompany?: boolean`

- [ ] **Step 1: Write failing tests for 8th company definition and price/dividend randomizers**
- [ ] **Step 2: Run tests to verify failure**
- [ ] **Step 3: Implement `purple` company in `config.ts` and randomizer options in `setup.ts`**
- [ ] **Step 4: Run tests to verify they pass**
- [ ] **Step 5: Update `OptionsModal.tsx` with toggles for 8th company and random economy**
- [ ] **Step 6: Run all tests and commit**

---

### Task 6: Dynamic Board Sizing (Up to $26 \times 26$ Grid)

**Files:**
- Modify: `webgame/src/engine/board.ts`
- Modify: `webgame/src/engine/__tests__/board.test.ts`
- Modify: `webgame/src/components/modals/OptionsModal.tsx`
- Modify: `webgame/src/components/modals/NewGameModal.tsx`
- Modify: `webgame/src/components/GameBoard.tsx`
- Modify: `webgame/src/components/classic/ClassicGameView.tsx`

**Interfaces:**
- `ROW_LABELS`: A through Z (26 rows)
- `boardRows`: 10 to 26 (default 10 or 12)
- `boardCols`: 12 to 26 (default 12)
- UI handles responsive scaling and scrollable containment for boards larger than 12x12.

- [ ] **Step 1: Write failing tests in `board.test.ts` for A-Z row labels and up to 26x26 board setups**
- [ ] **Step 2: Run tests to verify failure**
- [ ] **Step 3: Implement full 26-row and 26-col coordinates support in `board.ts`**
- [ ] **Step 4: Run tests to verify they pass**
- [ ] **Step 5: Add row/col sliders to `OptionsModal.tsx` and `NewGameModal.tsx`**
- [ ] **Step 6: Verify board rendering in `GameBoard.tsx` and `ClassicGameView.tsx` with dynamic grid styles**
- [ ] **Step 7: Run all tests and build verification**
- [ ] **Step 8: Final commit**
