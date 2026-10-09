# Oil Baron Web Game Modern UX Redesign Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Transform the *Oil Baron* web game interface from an enterprise-feeling modal-heavy desktop app into a clean, modern strategy board game with intuitive turn progression, contextual guidance, zero routine modals, and on-demand deep analytics.

**Architecture:** Replace the legacy desktop menu bar and redundant status bars with a cohesive tabletop command layout consisting of: a sleek `GameHeader`, a conversational `TurnAdvisor` banner translating move impacts into plain English, a unified bottom `ActionDock` integrating `PlayerHand` and `InlineStockMarket`, and a slide-out `LedgerDrawer` providing deep charts on demand. The core engine (`reducer.ts`, `board.ts`, `ai.ts`, `types.ts`) remains 100% intact.

**Tech Stack:** React 19, TypeScript, Lucide React, Modern CSS (custom properties, flexbox/grid), Vitest, React Testing Library.

**Spec:** `docs/superpowers/specs/2026-10-09-oilbaron-modern-ux-redesign.md`

## Global Constraints
- Zero breaking changes to `src/engine/` algorithms, rules, or state machines.
- All game actions (founding, mergers, payouts, AI turns, save/load) must function seamlessly.
- Keyboard shortcuts (`Alt+E`, `Alt+B`, `Alt+P`, `Alt+D`, `Alt+L`) must be preserved or enhanced.
- Fully compatible with internationalization (`i18n.ts` / German and English).
- Build (`npm run build`) and test suite (`npm test`) must pass with 0 errors.

---

### Task 1: Modern Game Header Component (`GameHeader.tsx`)

**Files:**
- Create: `webgame/src/components/GameHeader.tsx`
- Create: `webgame/src/components/__tests__/GameHeader.test.tsx`

**Interfaces:**
- Consumes:
  - `activePlayerName: string`
  - `activePlayerColor?: string`
  - `isAi: boolean`
  - `turnPhase: TurnPhase`
  - `soundMuted: boolean`
  - `language: 'en' | 'de'`
  - `onToggleSound: () => void`
  - `onToggleLanguage: () => void`
  - `onToggleLedger: () => void`
  - `onOpenRules: () => void`
  - `onOpenNewGame: () => void`
  - `onSwitchClassic?: () => void`
- Produces:
  - `<GameHeader />` component rendering clean game title, active turn badge with 2-step indicator, and utility actions.

- [ ] **Step 1: Write the failing unit test for `GameHeader`**

```tsx
// webgame/src/components/__tests__/GameHeader.test.tsx
import React from 'react';
import { render, screen, fireEvent } from '@testing-library/react';
import { describe, it, expect, vi } from 'vitest';
import { GameHeader } from '../GameHeader';

describe('GameHeader', () => {
  it('renders title, active player name, and turn step', () => {
    render(
      <GameHeader
        activePlayerName="Player 1"
        activePlayerColor="#f59e0b"
        isAi={false}
        turnPhase="place_tile"
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLanguage={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
      />
    );

    expect(screen.getByText(/OIL BARON/i)).toBeInTheDocument();
    expect(screen.getByText(/Player 1/i)).toBeInTheDocument();
    expect(screen.getByText(/Step 1: Drill Well/i)).toBeInTheDocument();
  });

  it('triggers onToggleLedger and onToggleSound callbacks', () => {
    const onToggleLedger = vi.fn();
    const onToggleSound = vi.fn();

    render(
      <GameHeader
        activePlayerName="Computer 1"
        isAi={true}
        turnPhase="buy_shares"
        soundMuted={false}
        language="en"
        onToggleSound={onToggleSound}
        onToggleLanguage={vi.fn()}
        onToggleLedger={onToggleLedger}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
      />
    );

    const ledgerBtn = screen.getByRole('button', { name: /ledger/i });
    fireEvent.click(ledgerBtn);
    expect(onToggleLedger).toHaveBeenCalledTimes(1);

    const soundBtn = screen.getByRole('button', { name: /sound/i });
    fireEvent.click(soundBtn);
    expect(onToggleSound).toHaveBeenCalledTimes(1);
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/GameHeader.test.tsx`
Expected: FAIL with module not found `GameHeader`.

- [ ] **Step 3: Implement `GameHeader.tsx`**

```tsx
// webgame/src/components/GameHeader.tsx
import React from 'react';
import {
  Volume2,
  VolumeX,
  BookOpen,
  BarChart3,
  RotateCcw,
  Languages,
  Monitor,
} from 'lucide-react';
import type { TurnPhase } from '../engine/types';
import { playClick } from '../assets/audio';

export interface GameHeaderProps {
  activePlayerName: string;
  activePlayerColor?: string;
  isAi: boolean;
  turnPhase: TurnPhase;
  soundMuted: boolean;
  language: 'en' | 'de';
  onToggleSound: () => void;
  onToggleLanguage: () => void;
  onToggleLedger: () => void;
  onOpenRules: () => void;
  onOpenNewGame: () => void;
  onSwitchClassic?: () => void;
}

export const GameHeader: React.FC<GameHeaderProps> = ({
  activePlayerName,
  activePlayerColor = '#f59e0b',
  isAi,
  turnPhase,
  soundMuted,
  language,
  onToggleSound,
  onToggleLanguage,
  onToggleLedger,
  onOpenRules,
  onOpenNewGame,
  onSwitchClassic,
}) => {
  const getStepLabel = () => {
    if (isAi) return 'AI Turn in Progress';
    switch (turnPhase) {
      case 'place_tile':
        return 'Step 1: Drill Well';
      case 'choose_founding_company':
        return 'Incorporate Company';
      case 'choose_merger_survivor':
        return 'Merger Tiebreak';
      case 'dispose_merger_shares':
        return 'Merger Disposition';
      case 'buy_shares':
        return 'Step 2: Invest in Stocks';
      case 'game_over':
        return 'Game Over';
      default:
        return 'Active Turn';
    }
  };

  return (
    <header className="game-header">
      <div className="header-brand">
        <div className="brand-logo-badge">🛢️</div>
        <div className="brand-title">
          <span className="brand-name">OIL BARON</span>
          <span className="brand-sub">STRATEGY</span>
        </div>
      </div>

      <div className="header-turn-status">
        <span
          className="player-color-dot"
          style={{ backgroundColor: activePlayerColor }}
        />
        <span className="player-turn-name">
          {activePlayerName} {isAi ? '(CPU)' : ''}
        </span>
        <span className="turn-step-pill">{getStepLabel()}</span>
      </div>

      <div className="header-actions">
        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onToggleLedger();
          }}
          title="Ledger & Strategic Analytics"
          aria-label="ledger"
        >
          <BarChart3 size={16} />
          <span className="btn-label">Ledger</span>
        </button>

        <button
          type="button"
          className="header-icon-btn"
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
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onToggleLanguage();
          }}
          title="Switch Language (EN / DE)"
          aria-label="language"
        >
          <Languages size={16} />
          <span className="btn-label">{language.toUpperCase()}</span>
        </button>

        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onOpenRules();
          }}
          title="Game Rules & Guide"
          aria-label="rules"
        >
          <BookOpen size={16} />
        </button>

        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onOpenNewGame();
          }}
          title="New Match"
          aria-label="new game"
        >
          <RotateCcw size={16} />
        </button>

        {onSwitchClassic && (
          <button
            type="button"
            className="header-icon-btn classic-switch-btn"
            onClick={() => {
              playClick();
              onSwitchClassic();
            }}
            title="Switch to 1993 Windows 3.1 Classic View"
            aria-label="classic view"
          >
            <Monitor size={16} />
            <span className="btn-label">1993 View</span>
          </button>
        )}
      </div>
    </header>
  );
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/GameHeader.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 1**

```bash
git add webgame/src/components/GameHeader.tsx webgame/src/components/__tests__/GameHeader.test.tsx
git commit -m "feat(ui): add modern GameHeader component with turn indicator and utilities"
```

---

### Task 2: Dynamic Move Advisor Banner (`TurnAdvisor.tsx`)

**Files:**
- Create: `webgame/src/components/TurnAdvisor.tsx`
- Create: `webgame/src/components/__tests__/TurnAdvisor.test.tsx`

**Interfaces:**
- Consumes:
  - `phase: TurnPhase`
  - `isAi: boolean`
  - `activePlayerName: string`
  - `selectedCoord: TileCoord | null`
  - `hoveredCoord: TileCoord | null`
  - `board: readonly (readonly CellState[])[]`
  - `companies: Partial<Record<CompanyId, CompanyState>>`
  - `definitions: readonly CompanyDefinition[]`
  - `safeThreshold?: number`
- Produces:
  - `<TurnAdvisor />` component rendering plain-English guidance and real-time hovered/selected move consequences.

- [ ] **Step 1: Write the failing unit test for `TurnAdvisor`**

```tsx
// webgame/src/components/__tests__/TurnAdvisor.test.tsx
import React from 'react';
import { render, screen } from '@testing-library/react';
import { describe, it, expect } from 'vitest';
import { TurnAdvisor } from '../TurnAdvisor';
import { createEmptyBoard } from '../../engine/board';
import { CLASSIC_COMPANIES } from '../../engine/config';

describe('TurnAdvisor', () => {
  const emptyBoard = createEmptyBoard();

  it('renders default prompt when placing tile with no selection', () => {
    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Select an oil parcel from your hand tray/i)).toBeInTheDocument();
  });

  it('renders explanatory text when hovered coord is isolated', () => {
    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={{ row: 3, col: 5 }}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/D-6/i)).toBeInTheDocument();
    expect(screen.getByText(/Isolated well/i)).toBeInTheDocument();
  });

  it('renders stock market guidance during buy_shares phase', () => {
    render(
      <TurnAdvisor
        phase="buy_shares"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Stock Exchange Open/i)).toBeInTheDocument();
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/TurnAdvisor.test.tsx`
Expected: FAIL with module not found `TurnAdvisor`.

- [ ] **Step 3: Implement `TurnAdvisor.tsx`**

```tsx
// webgame/src/components/TurnAdvisor.tsx
import React from 'react';
import { Sparkles, AlertCircle, TrendingUp, Compass, ShoppingBag, ShieldAlert } from 'lucide-react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  TileCoord,
  TurnPhase,
} from '../engine/types';
import { classifyMove, formatCoord } from '../engine/board';
import { getStockPrice } from '../engine/market';

export interface TurnAdvisorProps {
  phase: TurnPhase;
  isAi: boolean;
  activePlayerName: string;
  selectedCoord: TileCoord | null;
  hoveredCoord: TileCoord | null;
  board: readonly (readonly CellState[])[];
  companies?: Partial<Record<CompanyId, CompanyState>>;
  definitions?: readonly CompanyDefinition[];
  safeThreshold?: number;
}

export const TurnAdvisor: React.FC<TurnAdvisorProps> = ({
  phase,
  isAi,
  activePlayerName,
  selectedCoord,
  hoveredCoord,
  board,
  companies = {},
  definitions = [],
  safeThreshold = 11,
}) => {
  const getAdvisorMessage = () => {
    if (isAi) {
      return {
        icon: <Sparkles size={16} className="advisor-icon pulse" />,
        headline: `${activePlayerName} (CPU) is analyzing the board...`,
        detail: 'Evaluating territory potential and market acquisitions.',
      };
    }

    if (phase === 'buy_shares') {
      return {
        icon: <ShoppingBag size={16} className="advisor-icon text-amber" />,
        headline: 'Stock Exchange Open: Invest in up to 3 company shares',
        detail: 'Use the quick-buy shelf below to purchase shares or click Finish Turn to pass.',
      };
    }

    if (phase === 'choose_founding_company') {
      return {
        icon: <TrendingUp size={16} className="advisor-icon text-green" />,
        headline: 'Company Incorporation',
        detail: 'Choose an available corporation to claim your free Founder Bonus share.',
      };
    }

    if (phase === 'choose_merger_survivor') {
      return {
        icon: <AlertCircle size={16} className="advisor-icon text-orange" />,
        headline: 'Merger Tiebreak',
        detail: 'Multiple chains have identical size. Select which company absorbs the other.',
      };
    }

    if (phase === 'dispose_merger_shares') {
      return {
        icon: <TrendingUp size={16} className="advisor-icon text-blue" />,
        headline: 'Merger Payouts & Share Disposition',
        detail: 'Decide whether to hold, sell for cash, or trade 2-for-1 for surviving stock.',
      };
    }

    if (phase === 'place_tile') {
      const activeCoord = hoveredCoord || selectedCoord;
      if (!activeCoord) {
        return {
          icon: <Compass size={16} className="advisor-icon" />,
          headline: 'Select an oil parcel from your hand tray below to drill',
          detail: 'Hover over your chits to preview their impact on the board grid.',
        };
      }

      const classification = classifyMove(activeCoord, board, companies, safeThreshold);
      const coordName = formatCoord(activeCoord);

      switch (classification.type) {
        case 'isolated':
          return {
            icon: <Compass size={16} className="advisor-icon text-blue" />,
            headline: `${coordName} — Isolated well`,
            detail: 'No adjacent wells. Establishes a wildcat claim on the territory.',
          };
        case 'expand': {
          const comp = definitions.find((d) => d.id === classification.companyId);
          const cState = classification.companyId ? companies[classification.companyId] : null;
          const newSize = (cState?.size ?? 0) + 1;
          const tier = comp?.tier ?? 'low';
          const newPrice = getStockPrice(tier, newSize);
          return {
            icon: <TrendingUp size={16} className="advisor-icon text-yellow" />,
            headline: `${coordName} — Expands ${comp?.name ?? classification.companyId}`,
            detail: `Grows chain to ${newSize} wells. Share price will rise to $${newPrice}.`,
          };
        }
        case 'found':
          return {
            icon: <Sparkles size={16} className="advisor-icon text-green" />,
            headline: `${coordName} — Founds a new oil corporation!`,
            detail: 'Connects adjacent wildcat wells. You will receive 1 free founder share.',
          };
        case 'merger':
          return {
            icon: <AlertCircle size={16} className="advisor-icon text-orange" />,
            headline: `${coordName} — Triggers an Oil Merger!`,
            detail: 'Adjacent companies will merge. Major shareholders receive cash bonuses.',
          };
        case 'illegal_dead':
          return {
            icon: <ShieldAlert size={16} className="advisor-icon text-red" />,
            headline: `${coordName} — Dead / Unplayable Tile`,
            detail: 'Tile connects two permanent safe companies (11+ wells). Click Discard on chit.',
          };
        default:
          return {
            icon: <Compass size={16} className="advisor-icon" />,
            headline: `Tile ${coordName} selected`,
            detail: 'Click cell on the board to confirm placement.',
          };
      }
    }

    return {
      icon: <Compass size={16} className="advisor-icon" />,
      headline: 'Match in progress',
      detail: 'Follow on-screen instructions.',
    };
  };

  const { icon, headline, detail } = getAdvisorMessage();

  return (
    <div className="turn-advisor-banner">
      <div className="advisor-content">
        <span className="advisor-icon-wrap">{icon}</span>
        <div className="advisor-text">
          <span className="advisor-headline">{headline}</span>
          <span className="advisor-detail">{detail}</span>
        </div>
      </div>
    </div>
  );
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/TurnAdvisor.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 2**

```bash
git add webgame/src/components/TurnAdvisor.tsx webgame/src/components/__tests__/TurnAdvisor.test.tsx
git commit -m "feat(ui): add TurnAdvisor banner providing plain-English move consequences"
```

---

### Task 3: Inline Stock Market Shelf Component (`InlineStockMarket.tsx`)

**Files:**
- Create: `webgame/src/components/InlineStockMarket.tsx`
- Create: `webgame/src/components/__tests__/InlineStockMarket.test.tsx`

**Interfaces:**
- Consumes:
  - `activePlayerCash: number`
  - `companies: Partial<Record<CompanyId, CompanyState>>`
  - `definitions: readonly CompanyDefinition[]`
  - `onConfirmPurchases: (purchases: SharePurchase[]) => void`
  - `onPass: () => void`
- Produces:
  - `<InlineStockMarket />` component rendering horizontal shelf with quick `+`/`-` buttons, budget validation, and primary CTA.

- [ ] **Step 1: Write the failing unit test for `InlineStockMarket`**

```tsx
// webgame/src/components/__tests__/InlineStockMarket.test.tsx
import React from 'react';
import { render, screen, fireEvent } from '@testing-library/react';
import { describe, it, expect, vi } from 'vitest';
import { InlineStockMarket } from '../InlineStockMarket';
import { CLASSIC_COMPANIES } from '../../engine/config';
import type { CompanyState } from '../../engine/types';

describe('InlineStockMarket', () => {
  const activeCompanies: Record<string, CompanyState> = {
    american: {
      id: 'american',
      isActive: true,
      isSafe: false,
      size: 4,
      stockPrice: 400,
      availableShares: 15,
      tiles: [],
    },
  };

  it('renders active company and allows incrementing shares up to 3', () => {
    const onConfirm = vi.fn();
    render(
      <InlineStockMarket
        activePlayerCash={2000}
        companies={activeCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={onConfirm}
        onPass={vi.fn()}
      />
    );

    expect(screen.getByText(/American/i)).toBeInTheDocument();
    expect(screen.getByText(/\$400/i)).toBeInTheDocument();

    const incBtn = screen.getByRole('button', { name: /increase american/i });
    fireEvent.click(incBtn);
    fireEvent.click(incBtn);

    expect(screen.getByText(/2 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$800/i)).toBeInTheDocument();

    const confirmBtn = screen.getByRole('button', { name: /finish turn/i });
    fireEvent.click(confirmBtn);

    expect(onConfirm).toHaveBeenCalledWith([{ companyId: 'american', count: 2 }]);
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/InlineStockMarket.test.tsx`
Expected: FAIL with module not found `InlineStockMarket`.

- [ ] **Step 3: Implement `InlineStockMarket.tsx`**

```tsx
// webgame/src/components/InlineStockMarket.tsx
import React, { useState } from 'react';
import { Plus, Minus, Check, ArrowRight, ShoppingBag } from 'lucide-react';
import type { CompanyDefinition, CompanyId, CompanyState, SharePurchase } from '../engine/types';
import { playCashChime, playClick } from '../assets/audio';

export interface InlineStockMarketProps {
  activePlayerCash: number;
  companies: Partial<Record<CompanyId, CompanyState>>;
  definitions: readonly CompanyDefinition[];
  onConfirmPurchases: (purchases: SharePurchase[]) => void;
  onPass: () => void;
}

export const InlineStockMarket: React.FC<InlineStockMarketProps> = ({
  activePlayerCash,
  companies,
  definitions,
  onConfirmPurchases,
  onPass,
}) => {
  const [quantities, setQuantities] = useState<Record<CompanyId, number>>({});

  const activeCompanyList = definitions.filter((def) => {
    const c = companies[def.id];
    return c && c.isActive && c.availableShares > 0;
  });

  const totalSelectedShares = Object.values(quantities).reduce((a, b) => a + b, 0);

  const totalCost = activeCompanyList.reduce((sum, def) => {
    const qty = quantities[def.id] || 0;
    const price = companies[def.id]?.stockPrice || 0;
    return sum + qty * price;
  }, 0);

  const handleIncrement = (id: CompanyId) => {
    const c = companies[id];
    if (!c) return;
    const currentQty = quantities[id] || 0;
    if (totalSelectedShares >= 3) return;
    if (currentQty >= c.availableShares) return;
    if (totalCost + c.stockPrice > activePlayerCash) return;

    playClick();
    setQuantities((prev) => ({ ...prev, [id]: (prev[id] || 0) + 1 }));
  };

  const handleDecrement = (id: CompanyId) => {
    const currentQty = quantities[id] || 0;
    if (currentQty <= 0) return;

    playClick();
    setQuantities((prev) => {
      const next = { ...prev, [id]: currentQty - 1 };
      if (next[id] === 0) delete next[id];
      return next;
    });
  };

  const handleFinish = () => {
    const purchases: SharePurchase[] = Object.entries(quantities)
      .filter(([_, count]) => count > 0)
      .map(([companyId, count]) => ({ companyId: companyId as CompanyId, count }));

    if (purchases.length > 0) {
      playCashChime();
      onConfirmPurchases(purchases);
    } else {
      playClick();
      onPass();
    }
  };

  return (
    <div className="inline-stock-shelf">
      <div className="shelf-header">
        <span className="shelf-title">
          <ShoppingBag size={14} style={{ marginRight: '6px' }} />
          Stock Exchange: Invest in up to 3 shares
        </span>
        <span className="shelf-budget">
          Selected: <strong>{totalSelectedShares} / 3 shares</strong> | Total:{' '}
          <strong>${totalCost}</strong> | Cash: <strong>${activePlayerCash}</strong>
        </span>
      </div>

      <div className="shelf-companies-row">
        {activeCompanyList.length === 0 ? (
          <div className="no-companies-note">No active companies with available shares.</div>
        ) : (
          activeCompanyList.map((def) => {
            const cState = companies[def.id];
            const qty = quantities[def.id] || 0;
            const price = cState?.stockPrice || 0;
            const canAdd =
              totalSelectedShares < 3 &&
              qty < (cState?.availableShares || 0) &&
              totalCost + price <= activePlayerCash;

            return (
              <div
                key={def.id}
                className="inline-company-card"
                style={{ borderLeftColor: def.color }}
              >
                <div className="company-info">
                  <span className="company-name">{def.name}</span>
                  <span className="company-price">${price}</span>
                  <span className="company-avail">{cState?.availableShares} left</span>
                </div>
                <div className="company-counter">
                  <button
                    type="button"
                    className="counter-btn"
                    onClick={() => handleDecrement(def.id)}
                    disabled={qty <= 0}
                    aria-label={`decrease ${def.name}`}
                  >
                    <Minus size={12} />
                  </button>
                  <span className="counter-val">{qty}</span>
                  <button
                    type="button"
                    className="counter-btn"
                    onClick={() => handleIncrement(def.id)}
                    disabled={!canAdd}
                    aria-label={`increase ${def.name}`}
                  >
                    <Plus size={12} />
                  </button>
                </div>
              </div>
            );
          })
        )}
      </div>

      <div className="shelf-footer-actions">
        <button
          type="button"
          className="btn btn-primary finish-turn-btn"
          onClick={handleFinish}
          aria-label="finish turn"
        >
          <Check size={14} style={{ marginRight: '6px' }} />
          {totalSelectedShares > 0 ? `Buy Shares & Finish Turn (Alt+E)` : `Pass & Finish Turn (Alt+E)`}
        </button>
      </div>
    </div>
  );
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/InlineStockMarket.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 3**

```bash
git add webgame/src/components/InlineStockMarket.tsx webgame/src/components/__tests__/InlineStockMarket.test.tsx
git commit -m "feat(ui): add InlineStockMarket shelf with zero-modal share purchasing"
```

---

### Task 4: Unified Action Dock Component (`ActionDock.tsx`)

**Files:**
- Create: `webgame/src/components/ActionDock.tsx`
- Create: `webgame/src/components/__tests__/ActionDock.test.tsx`

**Interfaces:**
- Consumes:
  - `phase: TurnPhase`
  - `isCurrentTurn: boolean`
  - `hand: readonly TileCoord[]`
  - Hand interaction props (`selectedCoord`, `hoveredCoord`, `onSelectTile`, `onHoverTile`, `onPlaceTile`, `onDiscardDeadTile`)
  - Market props (`activePlayerCash`, `companies`, `definitions`, `onConfirmPurchases`, `onPassTurn`)
- Produces:
  - `<ActionDock />` wrapping Step 1 (PlayerHand rack) and Step 2 (InlineStockMarket) cleanly below the board.

- [ ] **Step 1: Write the failing unit test for `ActionDock`**

```tsx
// webgame/src/components/__tests__/ActionDock.test.tsx
import React from 'react';
import { render, screen } from '@testing-library/react';
import { describe, it, expect, vi } from 'vitest';
import { ActionDock } from '../ActionDock';
import { CLASSIC_COMPANIES } from '../../engine/config';

describe('ActionDock', () => {
  it('renders hand chits when in place_tile phase', () => {
    render(
      <ActionDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[{ row: 0, col: 0 }, { row: 1, col: 1 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6000}
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

    expect(screen.getByText('A-1')).toBeInTheDocument();
    expect(screen.getByText('B-2')).toBeInTheDocument();
  });

  it('renders InlineStockMarket when in buy_shares phase', () => {
    render(
      <ActionDock
        phase="buy_shares"
        isCurrentTurn={true}
        hand={[]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6000}
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

    expect(screen.getByText(/Stock Exchange: Invest in up to 3 shares/i)).toBeInTheDocument();
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/ActionDock.test.tsx`
Expected: FAIL with module not found `ActionDock`.

- [ ] **Step 3: Implement `ActionDock.tsx`**

```tsx
// webgame/src/components/ActionDock.tsx
import React from 'react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  SharePurchase,
  TileCoord,
  TurnPhase,
} from '../engine/types';
import { PlayerHand } from './PlayerHand';
import { InlineStockMarket } from './InlineStockMarket';

export interface ActionDockProps {
  phase: TurnPhase;
  isCurrentTurn: boolean;
  hand: readonly TileCoord[];
  selectedCoord: TileCoord | null;
  hoveredCoord: TileCoord | null;
  activePlayerCash: number;
  activePlayerName?: string;
  board?: readonly (readonly CellState[])[];
  companies: Partial<Record<CompanyId, CompanyState>>;
  definitions: readonly CompanyDefinition[];
  safeThreshold?: number;
  hideChits?: boolean;
  onSelectTile: (coord: TileCoord | null) => void;
  onHoverTile: (coord: TileCoord | null) => void;
  onPlaceTile: (coord: TileCoord) => void;
  onDiscardDeadTile: (coord: TileCoord) => void;
  onConfirmPurchases: (purchases: SharePurchase[]) => void;
  onPassTurn: () => void;
}

export const ActionDock: React.FC<ActionDockProps> = ({
  phase,
  isCurrentTurn,
  hand,
  selectedCoord,
  hoveredCoord,
  activePlayerCash,
  activePlayerName,
  board,
  companies,
  definitions,
  safeThreshold = 11,
  hideChits = false,
  onSelectTile,
  onHoverTile,
  onPlaceTile,
  onDiscardDeadTile,
  onConfirmPurchases,
  onPassTurn,
}) => {
  return (
    <div className="unified-action-dock">
      {phase === 'buy_shares' && isCurrentTurn ? (
        <InlineStockMarket
          activePlayerCash={activePlayerCash}
          companies={companies}
          definitions={definitions}
          onConfirmPurchases={onConfirmPurchases}
          onPass={onPassTurn}
        />
      ) : (
        <PlayerHand
          hand={hand}
          activePlayerName={activePlayerName}
          isCurrentTurn={isCurrentTurn}
          turnPhase={phase}
          board={board}
          companies={companies}
          definitions={definitions}
          safeThreshold={safeThreshold}
          selectedCoord={selectedCoord}
          hoveredCoord={hoveredCoord}
          hideChits={hideChits}
          onSelectTile={onSelectTile}
          onHoverTile={onHoverTile}
          onPlaceTile={onPlaceTile}
          onDiscardDeadTile={onDiscardDeadTile}
        />
      )}
    </div>
  );
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/ActionDock.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 4**

```bash
git add webgame/src/components/ActionDock.tsx webgame/src/components/__tests__/ActionDock.test.tsx
git commit -m "feat(ui): add ActionDock unifying chit tray and inline market"
```

---

### Task 5: Slide-Out Ledger Drawer Component (`LedgerDrawer.tsx`)

**Files:**
- Create: `webgame/src/components/LedgerDrawer.tsx`
- Create: `webgame/src/components/__tests__/LedgerDrawer.test.tsx`

**Interfaces:**
- Consumes:
  - `isOpen: boolean`
  - `onClose: () => void`
  - `gameState: GameState`
- Produces:
  - `<LedgerDrawer />` component rendering tabbed views for Wealth, Share Matrix, Company Stats, and War Room dominance.

- [ ] **Step 1: Write the failing unit test for `LedgerDrawer`**

```tsx
// webgame/src/components/__tests__/LedgerDrawer.test.tsx
import React from 'react';
import { render, screen, fireEvent } from '@testing-library/react';
import { describe, it, expect, vi } from 'vitest';
import { LedgerDrawer } from '../LedgerDrawer';
import { DEFAULT_CONFIG, createInitialGameState } from '../../engine/config';

describe('LedgerDrawer', () => {
  const gameState = createInitialGameState(DEFAULT_CONFIG, [
    { name: 'Player 1', isAi: false },
    { name: 'Player 2', isAi: true },
  ]);

  it('renders tabs and closes on close button click', () => {
    const onClose = vi.fn();
    render(<LedgerDrawer isOpen={true} onClose={onClose} gameState={gameState} />);

    expect(screen.getByText(/Ledger & Strategic Analytics/i)).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /wealth/i })).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /shares/i })).toBeInTheDocument();

    const closeBtn = screen.getByRole('button', { name: /close ledger/i });
    fireEvent.click(closeBtn);
    expect(onClose).toHaveBeenCalledTimes(1);
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npx vitest run src/components/__tests__/LedgerDrawer.test.tsx`
Expected: FAIL with module not found `LedgerDrawer`.

- [ ] **Step 3: Implement `LedgerDrawer.tsx`**

```tsx
// webgame/src/components/LedgerDrawer.tsx
import React, { useState } from 'react';
import { X, BarChart2, PieChart, Building2, Shield, History } from 'lucide-react';
import type { GameState } from '../engine/types';
import { calculatePlayerNetWorth } from '../engine/market';
import { playClick } from '../assets/audio';

export interface LedgerDrawerProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

type LedgerTab = 'wealth' | 'shares' | 'companies' | 'history';

export const LedgerDrawer: React.FC<LedgerDrawerProps> = ({ isOpen, onClose, gameState }) => {
  const [activeTab, setActiveTab] = useState<LedgerTab>('wealth');

  if (!isOpen) return null;

  const { players, companies, config } = gameState;

  return (
    <div className="ledger-drawer-backdrop" onClick={onClose}>
      <div className="ledger-drawer-panel" onClick={(e) => e.stopPropagation()}>
        <div className="drawer-header">
          <div className="drawer-title-group">
            <span className="drawer-title">Ledger & Strategic Analytics</span>
            <span className="drawer-subtitle">Comprehensive financial registry</span>
          </div>
          <button
            type="button"
            className="drawer-close-btn"
            onClick={() => {
              playClick();
              onClose();
            }}
            aria-label="close ledger"
          >
            <X size={18} />
          </button>
        </div>

        <div className="drawer-tabs">
          <button
            type="button"
            className={`drawer-tab ${activeTab === 'wealth' ? 'active' : ''}`}
            onClick={() => setActiveTab('wealth')}
            aria-label="wealth"
          >
            <BarChart2 size={14} />
            <span>Wealth</span>
          </button>
          <button
            type="button"
            className={`drawer-tab ${activeTab === 'shares' ? 'active' : ''}`}
            onClick={() => setActiveTab('shares')}
            aria-label="shares"
          >
            <PieChart size={14} />
            <span>Shareholdings</span>
          </button>
          <button
            type="button"
            className={`drawer-tab ${activeTab === 'companies' ? 'active' : ''}`}
            onClick={() => setActiveTab('companies')}
            aria-label="companies"
          >
            <Building2 size={14} />
            <span>Companies</span>
          </button>
        </div>

        <div className="drawer-body">
          {activeTab === 'wealth' && (
            <div className="tab-pane-wealth">
              <h4 className="pane-title">Net Worth & Liquidity</h4>
              <div className="wealth-list">
                {players.map((p) => {
                  const netWorth = calculatePlayerNetWorth(p, companies);
                  const stockVal = netWorth - p.cash;
                  return (
                    <div key={p.id} className="wealth-card">
                      <div className="wealth-card-header">
                        <span className="player-name">{p.name}</span>
                        <span className="player-networth">${netWorth.toLocaleString()}</span>
                      </div>
                      <div className="wealth-progress-bar">
                        <div
                          className="bar-cash"
                          style={{ width: `${Math.min(100, (p.cash / (netWorth || 1)) * 100)}%` }}
                          title={`Cash: $${p.cash}`}
                        />
                        <div
                          className="bar-stocks"
                          style={{
                            width: `${Math.min(100, (stockVal / (netWorth || 1)) * 100)}%`,
                          }}
                          title={`Stock: $${stockVal}`}
                        />
                      </div>
                      <div className="wealth-breakdown">
                        <span>Cash: ${p.cash.toLocaleString()}</span>
                        <span>Stocks: ${stockVal.toLocaleString()}</span>
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          )}

          {activeTab === 'shares' && (
            <div className="tab-pane-shares">
              <h4 className="pane-title">Stock Ownership Matrix</h4>
              <div className="matrix-table-wrap">
                <table className="matrix-table">
                  <thead>
                    <tr>
                      <th>Player</th>
                      {config.companies.map((c) => (
                        <th key={c.id} style={{ color: c.color }}>
                          {c.name.slice(0, 3)}
                        </th>
                      ))}
                    </tr>
                  </thead>
                  <tbody>
                    {players.map((p) => (
                      <tr key={p.id}>
                        <td className="row-player-name">{p.name}</td>
                        {config.companies.map((c) => (
                          <td key={c.id} className="row-shares-val">
                            {p.shares[c.id] || 0}
                          </td>
                        ))}
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          )}

          {activeTab === 'companies' && (
            <div className="tab-pane-companies">
              <h4 className="pane-title">Corporation Overview</h4>
              <div className="companies-grid">
                {config.companies.map((def) => {
                  const cState = companies[def.id];
                  const active = cState?.isActive ?? false;
                  return (
                    <div
                      key={def.id}
                      className={`company-stat-tile ${active ? 'active' : 'inactive'}`}
                      style={{ borderLeftColor: def.color }}
                    >
                      <div className="stat-name">{def.name}</div>
                      <div className="stat-details">
                        <span>Status: {active ? (cState?.isSafe ? '🛡️ Safe' : 'Active') : 'Dormant'}</span>
                        <span>Wells: {cState?.size ?? 0}</span>
                        <span>Price: ${cState?.stockPrice ?? 0}</span>
                        <span>Bank Shares: {cState?.availableShares ?? config.totalSharesPerCompany}</span>
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npx vitest run src/components/__tests__/LedgerDrawer.test.tsx`
Expected: PASS.

- [ ] **Step 5: Commit Task 5**

```bash
git add webgame/src/components/LedgerDrawer.tsx webgame/src/components/__tests__/LedgerDrawer.test.tsx
git commit -m "feat(ui): add LedgerDrawer slide-out panel for on-demand analytics"
```

---

### Task 6: App Orchestrator Integration & Modern Tabletop CSS Styling

**Files:**
- Modify: `webgame/src/App.tsx`
- Modify: `webgame/src/index.css`
- Test: `webgame/src/__tests__/App.test.tsx`

**Interfaces:**
- Wire `GameHeader`, `TurnAdvisor`, `ActionDock`, and `LedgerDrawer` into `App.tsx`.
- Remove the legacy desktop MenuBar from modern mode.
- Remove redundant duplicate turn status banner.
- Add CSS design tokens and styling rules for header, advisor banner, action dock, and ledger drawer.

- [ ] **Step 1: Update `index.css` with modern strategy tabletop styles**

Add CSS classes for:
- `.game-header`, `.header-brand`, `.header-turn-status`, `.turn-step-pill`, `.header-icon-btn`
- `.turn-advisor-banner`, `.advisor-content`, `.advisor-headline`, `.advisor-detail`
- `.unified-action-dock`, `.inline-stock-shelf`, `.inline-company-card`
- `.ledger-drawer-backdrop`, `.ledger-drawer-panel`, `.drawer-tabs`, `.drawer-tab`

- [ ] **Step 2: Update `App.tsx` layout**

In `webgame/src/App.tsx`:
- Import `GameHeader`, `TurnAdvisor`, `ActionDock`, `LedgerDrawer`.
- Add `ledgerDrawerOpen` state: `const [ledgerDrawerOpen, setLedgerDrawerOpen] = useState(false);`.
- Replace `MenuBar` in modern mode with `<GameHeader />`.
- Mount `<TurnAdvisor />` directly above `<GameBoard />`.
- Mount `<ActionDock />` below `<GameBoard />`.
- Mount `<LedgerDrawer />`.
- Retain keyboard shortcuts (`Alt+L` toggles Ledger).

- [ ] **Step 3: Run all unit and component tests**

Run: `cd webgame && npm test`
Expected: All tests pass.

- [ ] **Step 4: Verify production build**

Run: `cd webgame && npm run build`
Expected: 0 errors, build succeeds.

- [ ] **Step 5: Commit Task 6**

```bash
git add webgame/src/App.tsx webgame/src/index.css
git commit -m "feat(ui): integrate modern GameHeader, TurnAdvisor, ActionDock, and LedgerDrawer"
```

---

### Task 7: Final Verification & Polish

**Files:**
- Test: `webgame/src/__tests__/App.test.tsx`
- Run: `npm test` and `npm run build`

- [ ] **Step 1: Run comprehensive tests and linter**
- [ ] **Step 2: Verify in browser that the UX feels like a clean strategy game**
- [ ] **Step 3: Commit final polish**
