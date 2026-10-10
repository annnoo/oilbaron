# Oil Baron Phaser 3 Game Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a complete, authentic canvas video game implementation of *Oil Baron* in `phasergame/` using Phaser 3.88, TypeScript, and Vite, featuring classical Title Screen, dedicated Game Scene, and an in-game Pause Menu opened on `Escape`.

**Architecture:**
- Create `phasergame/` with Vite, TypeScript, and Phaser 3.88.
- Share `engine/` logic from `webgame/src/engine/` to preserve 100% mathematical rules, AI, and save file structure.
- Implement Phaser Scenes: `BootScene` (assets/fonts), `TitleScene` (retro main menu), `NewGameScene` (player setup), `GameScene` (canvas 10×12 board + lease dock + HUD), `PauseScene` (classical `Escape` pause overlay with Save/Load/Quit), and `StockScene` (canvas equity market).

**Tech Stack:** Phaser 3.88.2, Vite 5, TypeScript 5, Vitest.

**Spec:** `docs/superpowers/specs/2026-10-10-oilbaron-phaser-game.md`

## Global Constraints
- Canvas-first: All menus, boards, and game UI rendered inside the Phaser canvas.
- Engine Invariance: 100% preservation of all engine logic and mathematical determinism.
- Escape Key Flow: Pressing `Escape` in `GameScene` launches `PauseScene` with Resume, Save, Load, New Game, and Title options.
- Quality: Clean TypeScript build (`npm run build`) and clean test suite (`npm test`).

---

### Task 1: Project Scaffolding & Engine Integration (`phasergame/`)

**Files:**
- Create: `phasergame/package.json`
- Create: `phasergame/tsconfig.json`
- Create: `phasergame/vite.config.ts`
- Create: `phasergame/index.html`
- Create: `phasergame/src/main.ts`
- Symlink or copy: `phasergame/src/engine/` $\leftarrow$ `webgame/src/engine/`
- Test: `phasergame/src/engine/__tests__/board.test.ts`

- [ ] **Step 1: Create package.json and project configuration**
  Initialize `phasergame/package.json` with dependencies (`phaser: ^3.88.2`, `vite`, `typescript`, `vitest`).
- [ ] **Step 2: Copy engine files and verify tests**
  Copy `webgame/src/engine/` into `phasergame/src/engine/` and run `npm test`.
- [ ] **Step 3: Setup Phaser entrypoint in `src/main.ts` and `index.html`**
  Configure Phaser.Game with `scale.mode: Phaser.Scale.FIT`, `autoCenter: Phaser.Scale.CENTER_BOTH`, 1280×720 virtual resolution.
- [ ] **Step 4: Verify build and test**
  Run: `cd phasergame && npm install && npm run build && npm test`
- [ ] **Step 5: Commit Task 1**
  ```bash
  git add phasergame
  git commit -m "feat(phaser): scaffold phasergame project with Phaser 3.88 and shared engine"
  ```

---

### Task 2: Boot & Classical Title Scene (`BootScene.ts`, `TitleScene.ts`)

**Files:**
- Create: `phasergame/src/scenes/BootScene.ts`
- Create: `phasergame/src/scenes/TitleScene.ts`
- Create: `phasergame/src/ui/Button.ts`
- Modify: `phasergame/src/main.ts`

- [ ] **Step 1: Implement `ui/Button.ts`**
  Interactive Phaser text/container button with hover highlight, scale animation, and sound callback.
- [ ] **Step 2: Implement `BootScene.ts`**
  Preloads retro SFX, classic sprites (`classic_assets`), and custom Google Fonts (`Chakra Petch`).
- [ ] **Step 3: Implement `TitleScene.ts`**
  Classical retro video game title screen with Oil Baron logo, animated background wellheads, and menu buttons: `NEW GAME`, `LOAD GAME`, `HOW TO PLAY`.
- [ ] **Step 4: Verify build and launch**
  Run: `cd phasergame && npm run build`
- [ ] **Step 5: Commit Task 2**
  ```bash
  git add phasergame/src/scenes/BootScene.ts phasergame/src/scenes/TitleScene.ts phasergame/src/ui/Button.ts phasergame/src/main.ts
  git commit -m "feat(phaser): implement BootScene and classical TitleScene"
  ```

---

### Task 3: In-Game Pause Menu Scene (`PauseScene.ts` on `Escape`)

**Files:**
- Create: `phasergame/src/scenes/PauseScene.ts`
- Create: `phasergame/src/scenes/SaveLoadScene.ts`
- Test: `phasergame/src/scenes/__tests__/PauseScene.test.ts`

- [ ] **Step 1: Implement `PauseScene.ts`**
  Darkened backdrop overlay displaying:
  - `RESUME GAME` (returns to GameScene)
  - `NEW GAME` (opens roster setup)
  - `SAVE MISSION` (opens save slots)
  - `LOAD MISSION` (opens load slots)
  - `QUIT TO TITLE` (stops match and opens TitleScene)
  Listens for `Escape` key to resume.
- [ ] **Step 2: Implement `SaveLoadScene.ts`**
  Slot-based save/load manager utilizing `src/engine/persistence.ts`.
- [ ] **Step 3: Test keyboard and state delegation**
  Verify `Escape` key toggle logic.
- [ ] **Step 4: Commit Task 3**
  ```bash
  git add phasergame/src/scenes/PauseScene.ts phasergame/src/scenes/SaveLoadScene.ts phasergame/src/scenes/__tests__/PauseScene.test.ts
  git commit -m "feat(phaser): implement PauseScene on Escape key and SaveLoadScene"
  ```

---

### Task 4: Tactical Oil Exploration Game Scene (`GameScene.ts`)

**Files:**
- Create: `phasergame/src/scenes/GameScene.ts`
- Create: `phasergame/src/ui/ConcessionsTray.ts`
- Create: `phasergame/src/ui/HudBar.ts`
- Modify: `phasergame/src/main.ts`

- [ ] **Step 1: Implement `HudBar.ts`**
  Renders active concessionaire name, cash reserves, net worth, turn directive banner, and Pause button (`[ESC] MENU`).
- [ ] **Step 2: Implement `GameScene.ts` Board Grid**
  Renders 10×12 seismic survey grid with coordinates A–J and 1–12. Interactive parcel cells supporting hover crosshair and drill placement.
- [ ] **Step 3: Implement `ConcessionsTray.ts`**
  Tactile 6-lease rack at bottom of canvas with status chips (`EXPAND`, `FOUND`, `MERGER`, `OPEN`, `DISCARD`) and emerald **[ DRILL WELL ]** actuator button.
- [ ] **Step 4: Wire `Escape` Key in `GameScene.ts`**
  Pressing `Escape` launches `PauseScene` in overlay mode while keeping `GameScene` paused.
- [ ] **Step 5: Run tests and build**
  Run: `cd phasergame && npm run build && npm test`
- [ ] **Step 6: Commit Task 4**
  ```bash
  git add phasergame/src/scenes/GameScene.ts phasergame/src/ui/ConcessionsTray.ts phasergame/src/ui/HudBar.ts phasergame/src/main.ts
  git commit -m "feat(phaser): implement GameScene with tactical board, concessions dock, and Escape pause wiring"
  ```

---

### Task 5: Petroleum Equity Exchange & Modal Overlays (`StockScene.ts`, `FoundingModal.ts`, `MergerModal.ts`)

**Files:**
- Create: `phasergame/src/scenes/StockScene.ts`
- Create: `phasergame/src/ui/FoundingDialog.ts`
- Create: `phasergame/src/ui/MergerDialog.ts`
- Modify: `phasergame/src/scenes/GameScene.ts`

- [ ] **Step 1: Implement `StockScene.ts`**
  Canvas-based stock market supporting both Buy (quota 3 shares) and Sell/Liquidate modes.
- [ ] **Step 2: Implement Founding & Merger canvas dialogs**
  Interactive selection for new company incorporation and takeover survivor selection.
- [ ] **Step 3: Verify end-to-end match play**
  Run `npm test` and `npm run build`.
- [ ] **Step 4: Commit Task 5**
  ```bash
  git add phasergame/src/scenes/StockScene.ts phasergame/src/ui/FoundingDialog.ts phasergame/src/ui/MergerDialog.ts phasergame/src/scenes/GameScene.ts
  git commit -m "feat(phaser): implement canvas StockScene, Founding, and Merger dialogs"
  ```

---

### Task 6: Final Verification & Integration

**Files:**
- Full test suite & build verification

- [ ] **Step 1: Run comprehensive tests (`npm test`)**
- [ ] **Step 2: Run production build (`npm run build`)**
- [ ] **Step 3: Conduct whole-branch review**
