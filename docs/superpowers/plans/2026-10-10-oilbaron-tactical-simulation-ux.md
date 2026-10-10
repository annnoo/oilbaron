# Oil Baron Tactical Exploration Simulation UX Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Transform the *Oil Baron* web game into an authentic, immersive tactical oil exploration simulation game (full-viewport 100vw × 100vh, geological survey map, tactile drilling lease dock, industrial actuator button, and tactical petroleum exchange) while preserving 100% of the underlying 1993 game engine rules and passing all tests.

**Architecture:** Refactor the UI layer to adopt a full-screen tactical simulation structure: `TacticalOpsBar` (top HUD & pipeline ticker), `GameBoard` (geological survey map with industrial wellheads and safe refinery borders), `ConcessionsDock` (tactical lease slips in hand + drill actuator + inline petroleum exchange), and `TacticalLedgerSheet` (slide-up operations sheet). Zero changes to `src/engine/`.

**Tech Stack:** React 19, TypeScript, Lucide React, Modern CSS (custom properties, flexbox/grid), Vitest.

**Spec:** `docs/superpowers/specs/2026-10-10-oilbaron-tactical-simulation-ux.md`

## Global Constraints
- Zero breaking changes to `src/engine/` algorithms, rulesets, prices, or state machines.
- All game actions (drilling, founding, mergers, buyouts, dividends, AI turns, save/load) must function seamlessly.
- Keyboard shortcuts (`Alt+E`, `Alt+B`, `Alt+P`, `Alt+D`, `Alt+L`) must be preserved or enhanced.
- Viewport locked to `100vw × 100vh` without outer web container cards or page scrollbars.
- Build (`npm run build`) and test suite (`npm test`) must pass with 0 errors.

---

### Task 1: Tactical Simulation Styles & Fullscreen Viewport (`index.css`)

**Files:**
- Modify: `webgame/src/index.css`
- Test: Verify with `npm test` and `npm run build`

**Interfaces:**
- Produces: CSS design tokens, typography, and styling for `.simulation-screen`, `.ops-bar`, `.baron-profile`, `.turn-directive`, `.concessions-dock`, `.leases-tray`, `.lease-slip`, `.drill-actuator`, and geological survey parcel states.

- [ ] **Step 1: Add full-viewport simulation styles to `index.css`**

Add rules for:
- `.simulation-screen` (`width: 100vw; height: 100vh; overflow: hidden; background: radial-gradient(...)`)
- `.ops-bar`, `.baron-profile`, `.badge-icon`, `.baron-meta`, `.turn-directive`, `.ops-controls`
- `.survey-map-container`, `.network-status-bar`, `.conglomerate-badge`
- `.sector-grid`, `.parcel`, `.parcel.target`, `.well`, `.well.american`, `.well.imperial`, `.well.safe`
- `.concessions-dock`, `.leases-tray`, `.lease-slip`, `.lease-slip.selected`, `.drill-actuator`

- [ ] **Step 2: Verify CSS builds cleanly**

Run: `cd webgame && npm run build`
Expected: PASS.

- [ ] **Step 3: Commit Task 1**

```bash
git add webgame/src/index.css
git commit -m "feat(ui): add tactical simulation full-viewport styles and design tokens"
```

---

### Task 2: Tactical Operations Bar & Pipeline Ticker (`TacticalOpsBar.tsx`)

**Files:**
- Create: `webgame/src/components/TacticalOpsBar.tsx`
- Create: `webgame/src/components/__tests__/TacticalOpsBar.test.tsx`

**Interfaces:**
- Consumes:
  - `activePlayerName: string`
  - `activePlayerCash: number`
  - `activePlayerNetWorth?: number`
  - `activePlayerColor?: string`
  - `isAi: boolean`
  - `turnPhase: TurnPhase`
  - `directiveText: string`
  - `directivePhase: string`
  - `companies: Partial<Record<CompanyId, CompanyState>>`
  - `definitions: readonly CompanyDefinition[]`
  - `soundMuted: boolean`
  - `language: 'en' | 'de'`
  - `onToggleSound: () => void`
  - `onToggleLedger: () => void`
  - `onOpenOptions: () => void`
  - `onSwitchClassic?: () => void`
- Produces:
  - `<TacticalOpsBar />` rendering the top HUD and active corporate pipeline ticker chips.

- [ ] **Step 1: Write the failing unit test for `TacticalOpsBar`**

```tsx
// webgame/src/components/__tests__/TacticalOpsBar.test.tsx
import React from 'react';
import { describe, it, expect, vi } from 'vitest';
import { renderToStaticMarkup } from 'react-dom/server';
import { TacticalOpsBar } from '../TacticalOpsBar';
import { CLASSIC_COMPANIES } from '../../engine/config';

describe('TacticalOpsBar', () => {
  it('renders player funds, directive, and active pipeline tickers', () => {
    const html = renderToStaticMarkup(
      <TacticalOpsBar
        activePlayerName="Player 1"
        activePlayerCash={6400}
        activePlayerNetWorth={14200}
        activePlayerColor="#f59e0b"
        isAi={false}
        turnPhase="place_tile"
        directiveText="Drill parcel B-5 to expand American Oil field"
        directivePhase="Survey & Drill Operation"
        companies={{
          american: { id: 'american', isActive: true, isSafe: false, size: 5, stockPrice: 500, availableShares: 10, tiles: [] }
        }}
        definitions={CLASSIC_COMPANIES}
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenOptions={vi.fn()}
      />
    );

    expect(html).toContain('Player 1');
    expect(html).toContain('$6,400');
    expect(html).toContain('Drill parcel B-5');
    expect(html).toContain('American');
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/TacticalOpsBar.test.tsx`
Expected: FAIL with module not found `TacticalOpsBar`.

- [ ] **Step 3: Implement `TacticalOpsBar.tsx`**

```tsx
// webgame/src/components/TacticalOpsBar.tsx
import React from 'react';
import { Volume2, VolumeX, BarChart2, Settings, Monitor } from 'lucide-react';
import type { CompanyDefinition, CompanyId, CompanyState, TurnPhase } from '../engine/types';
import { playClick } from '../assets/audio';

export interface TacticalOpsBarProps {
  activePlayerName: string;
  activePlayerCash: number;
  activePlayerNetWorth?: number;
  activePlayerColor?: string;
  isAi: boolean;
  turnPhase: TurnPhase;
  directiveText: string;
  directivePhase: string;
  companies: Partial<Record<CompanyId, CompanyState>>;
  definitions: readonly CompanyDefinition[];
  soundMuted: boolean;
  language: 'en' | 'de';
  onToggleSound: () => void;
  onToggleLedger: () => void;
  onOpenOptions: () => void;
  onSwitchClassic?: () => void;
}

export const TacticalOpsBar: React.FC<TacticalOpsBarProps> = ({
  activePlayerName,
  activePlayerCash,
  activePlayerNetWorth,
  activePlayerColor = '#38bdf8',
  isAi,
  directiveText,
  directivePhase,
  companies,
  definitions,
  soundMuted,
  onToggleSound,
  onToggleLedger,
  onOpenOptions,
  onSwitchClassic,
}) => {
  const activeCompanies = definitions.filter((def) => {
    const c = companies[def.id];
    return c && c.isActive;
  });

  return (
    <header className="ops-bar-container">
      <div className="ops-bar">
        {/* Concessionaire Profile */}
        <div className="baron-profile">
          <div className="badge-icon" style={{ borderColor: activePlayerColor }}>
            {activePlayerName.slice(0, 2).toUpperCase()}
          </div>
          <div className="baron-meta">
            <div className="title">
              {activePlayerName} {isAi ? '(CPU)' : ''}
            </div>
            <div className="funds">
              ${activePlayerCash.toLocaleString()}
              {activePlayerNetWorth !== undefined && (
                <span className="networth"> / ${activePlayerNetWorth.toLocaleString()}</span>
              )}
            </div>
          </div>
        </div>

        {/* Turn Directive */}
        <div className="turn-directive">
          <div className="phase">{directivePhase}</div>
          <div className="text">{directiveText}</div>
        </div>

        {/* Tactical Controls */}
        <div className="ops-controls">
          <button
            type="button"
            className="btn-hud"
            onClick={() => {
              playClick();
              onToggleLedger();
            }}
            title="Strategic Ledger & Financials (Alt+L)"
            aria-label="ledger"
          >
            <BarChart2 size={16} />
          </button>

          <button
            type="button"
            className="btn-hud"
            onClick={() => {
              playClick();
              onToggleSound();
            }}
            title={soundMuted ? 'Unmute Audio' : 'Mute Audio'}
            aria-label="sound"
          >
            {soundMuted ? <VolumeX size={16} /> : <Volume2 size={16} />}
          </button>

          <button
            type="button"
            className="btn-hud"
            onClick={() => {
              playClick();
              onOpenOptions();
            }}
            title="Operations Settings"
            aria-label="settings"
          >
            <Settings size={16} />
          </button>

          {onSwitchClassic && (
            <button
              type="button"
              className="btn-hud"
              onClick={() => {
                playClick();
                onSwitchClassic();
              }}
              title="Switch to 1993 View"
              aria-label="classic view"
            >
              <Monitor size={16} />
            </button>
          )}
        </div>
      </div>

      {/* Active Pipeline Network Strip */}
      <div className="network-status-bar">
        {activeCompanies.length === 0 ? (
          <div className="network-idle">No active corporate pipeline networks.</div>
        ) : (
          activeCompanies.map((def) => {
            const cState = companies[def.id];
            const isSafe = cState?.isSafe ?? false;
            return (
              <div
                key={def.id}
                className={`conglomerate-badge ${isSafe ? 'safe' : ''}`}
                style={{ borderLeftColor: def.color }}
              >
                <span className="company-name" style={{ color: def.color }}>
                  {def.name}
                </span>
                {isSafe && <span className="safe-pill">REFINERY HUB</span>}
                <span className="company-details">
                  ${cState?.stockPrice ?? 0} · {cState?.size ?? 0} wells
                </span>
              </div>
            );
          })
        )}
      </div>
    </header>
  );
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/TacticalOpsBar.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 2**

```bash
git add webgame/src/components/TacticalOpsBar.tsx webgame/src/components/__tests__/TacticalOpsBar.test.tsx
git commit -m "feat(ui): add TacticalOpsBar component with live pipeline networks and field HUD"
```

---

### Task 3: Geological Survey Map Component Upgrade (`GameBoard.tsx`)

**Files:**
- Modify: `webgame/src/components/GameBoard.tsx`
- Test: `webgame/src/components/__tests__/GameBoard.test.tsx`

**Interfaces:**
- Produces: Clean 10×12 geological survey map without cell coordinate text clutter (`A-1`, `A-2` removed from empty cells; clean perimeter rulers), active industrial wellheads (`▲`, `◆`), and fortified refinery borders on safe chains.

- [ ] **Step 1: Update `GameBoard.tsx` rendering to remove cell text clutter and add wellhead styling**

In `webgame/src/components/GameBoard.tsx`:
- When an empty cell renders, do not print coordinate text strings inside the cell. Render clean survey parcels with subtle center guide dots.
- When an active company cell renders, render the wellhead marker with company initial and glyph (`▲` or `◆`).
- When a company chain is safe ($\ge 11$ wells), add the `.well.safe` class for the reinforced refinery hub border.
- Highlight the target cell with the tactical crosshair reticle when hovered or selected.

- [ ] **Step 2: Run GameBoard tests to verify**

Run: `cd webgame && npx vitest run src/components/__tests__/GameBoard.test.tsx`
Expected: PASS.

- [ ] **Step 3: Commit Task 3**

```bash
git add webgame/src/components/GameBoard.tsx webgame/src/components/__tests__/GameBoard.test.tsx
git commit -m "feat(ui): upgrade GameBoard into clean geological survey map with industrial wellheads"
```

---

### Task 4: Concessions Dock & Drill Actuator Component (`ConcessionsDock.tsx`)

**Files:**
- Create: `webgame/src/components/ConcessionsDock.tsx`
- Create: `webgame/src/components/__tests__/ConcessionsDock.test.tsx`

**Interfaces:**
- Consumes:
  - `phase: TurnPhase`
  - `isCurrentTurn: boolean`
  - `hand: readonly TileCoord[]`
  - `selectedCoord: TileCoord | null`
  - `hoveredCoord: TileCoord | null`
  - `activePlayerCash: number`
  - `board?: readonly (readonly CellState[])[]`
  - `companies: Partial<Record<CompanyId, CompanyState>>`
  - `definitions: readonly CompanyDefinition[]`
  - `safeThreshold?: number`
  - `onSelectTile: (coord: TileCoord | null) => void`
  - `onHoverTile: (coord: TileCoord | null) => void`
  - `onPlaceTile: (coord: TileCoord) => void`
  - `onDiscardDeadTile: (coord: TileCoord) => void`
  - `onConfirmPurchases: (purchases: SharePurchase[]) => void`
  - `onPassTurn: () => void`
- Produces:
  - `<ConcessionsDock />` combining tactical parcel leases, big **"DRILL WELL — PARCEL [COORD]"** actuator button, and Step 2 Petroleum Exchange.

- [ ] **Step 1: Write the failing unit test for `ConcessionsDock`**

```tsx
// webgame/src/components/__tests__/ConcessionsDock.test.tsx
import React from 'react';
import { describe, it, expect, vi } from 'vitest';
import { renderToStaticMarkup } from 'react-dom/server';
import { ConcessionsDock } from '../ConcessionsDock';
import { CLASSIC_COMPANIES } from '../../engine/config';

describe('ConcessionsDock', () => {
  it('renders drilling leases and drill actuator button during place_tile', () => {
    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[{ row: 1, col: 4 }]}
        selectedCoord={{ row: 1, col: 4 }}
        hoveredCoord={null}
        activePlayerCash={6400}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('B-5');
    expect(html).toContain('DRILL WELL');
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/ConcessionsDock.test.tsx`
Expected: FAIL with module not found `ConcessionsDock`.

- [ ] **Step 3: Implement `ConcessionsDock.tsx`**

Implement `ConcessionsDock.tsx` wrapping the tactical lease slips, the **"DRILL WELL"** actuator button, and delegating to `InlineStockMarket` during `buy_shares`.

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/ConcessionsDock.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 4**

```bash
git add webgame/src/components/ConcessionsDock.tsx webgame/src/components/__tests__/ConcessionsDock.test.tsx
git commit -m "feat(ui): add ConcessionsDock with tactical leases and drill actuator button"
```

---

### Task 5: Tactical Ledger Bottom Sheet (`TacticalLedgerSheet.tsx` / `LedgerDrawer.tsx`)

**Files:**
- Modify/Upgrade: `webgame/src/components/LedgerDrawer.tsx`
- Test: `webgame/src/components/__tests__/LedgerDrawer.test.tsx`

**Interfaces:**
- Produces: Smooth slide-up tactical operations sheet on demand (`Alt+L` / `📊`) with tabs for Portfolios, Share Matrix, Conglomerate diagnostics, and Action log.

- [ ] **Step 1: Update `LedgerDrawer.tsx` to tactical bottom sheet styling**

Ensure `LedgerDrawer.tsx` renders with `.tactical-sheet` styles, clean tab navigation, and proper Escape dismissal.

- [ ] **Step 2: Run LedgerDrawer tests to verify**

Run: `cd webgame && npx vitest run src/components/__tests__/LedgerDrawer.test.tsx`
Expected: PASS.

- [ ] **Step 3: Commit Task 5**

```bash
git add webgame/src/components/LedgerDrawer.tsx webgame/src/components/__tests__/LedgerDrawer.test.tsx
git commit -m "feat(ui): polish LedgerDrawer as tactical slide-up operations sheet"
```

---

### Task 6: Orchestrator Integration & Fullscreen Simulation Wiring (`App.tsx`)

**Files:**
- Modify: `webgame/src/App.tsx`
- Test: `webgame/src/__tests__/App.test.tsx`

**Interfaces:**
- Wire `TacticalOpsBar`, `GameBoard`, `ConcessionsDock`, and `LedgerDrawer` into full-viewport `.simulation-screen` layout in `App.tsx`.
- Connect dynamic directive text from move classifications.
- Verify keyboard shortcuts: `Alt+E` for drill / finish turn, `Alt+L` for ledger, `Space` for drill.

- [ ] **Step 1: Wire `App.tsx` with tactical simulation layout**

In `webgame/src/App.tsx`:
- Render `<div className="simulation-screen">` when `viewMode !== 'classic'`.
- Mount `<TacticalOpsBar />` at top.
- Mount `<GameBoard />` in center survey container.
- Mount `<ConcessionsDock />` at bottom.
- Mount `<LedgerDrawer />`.

- [ ] **Step 2: Run full test suite**

Run: `cd webgame && npm test`
Expected: All 352+ tests pass.

- [ ] **Step 3: Verify production build**

Run: `cd webgame && npm run build`
Expected: 0 errors, build succeeds.

- [ ] **Step 4: Commit Task 6**

```bash
git add webgame/src/App.tsx webgame/src/__tests__/App.test.tsx
git commit -m "feat(ui): integrate tactical simulation full-screen layout into App orchestrator"
```

---

### Task 7: End-to-End Verification & Final Polish

**Files:**
- Test: Full Vitest suite & Vite build

- [ ] **Step 1: Run comprehensive tests (`npm test`)**
- [ ] **Step 2: Run production build (`npm run build`)**
- [ ] **Step 3: Commit final polish**
