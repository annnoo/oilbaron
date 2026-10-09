# Oil Baron Core TypeScript Engine Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a completely decoupled, pure TypeScript game engine for Oil Baron in `webgame/src/engine/`, with an immutable state machine, data-driven company architecture, full merger resolution, deterministic AI bots, and 100% test coverage using Vitest.

**Architecture:** Pure Redux-style immutable reducer architecture `(state, action) => newState`, separating board geometry and graph traversal, stock market economics, merger resolution state machine, and turn lifecycle into focused, modular domain libraries tested with Vitest.

**Tech Stack:** TypeScript 5.x, Node.js, Vite, Vitest.

**Spec:** [docs/superpowers/specs/2026-10-09-oilbaron-engine-design.md](file:///home/anno/Code/gamedev/oilbaron/docs/superpowers/specs/2026-10-09-oilbaron-engine-design.md)

## Global Constraints
- Engine code must reside entirely in `webgame/src/engine/`.
- Zero browser, DOM, or React dependencies in the engine (pure TypeScript).
- All game state updates must be strictly immutable.
- 100% test pass rate with Vitest on `npm test`.
- Strict git discipline: NEVER commit `game/` binaries or node_modules.

---

### Task 1: Scaffolding Webgame Package & Test Runner

**Files:**
- Create: `webgame/package.json`
- Create: `webgame/tsconfig.json`
- Create: `webgame/vite.config.ts`
- Create: `webgame/src/engine/__tests__/setup.test.ts`
- Test: Vitest test execution via `npm test`

**Interfaces:**
- Consumes: Node.js environment
- Produces: Working TypeScript compilation and Vitest test runner in `webgame/`

- [ ] **Step 1: Create `webgame/package.json`**

```json
{
  "name": "oilbaron-webgame",
  "private": true,
  "version": "0.1.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "tsc && vite build",
    "test": "vitest run",
    "test:watch": "vitest"
  },
  "devDependencies": {
    "typescript": "^5.6.3",
    "vite": "^5.4.9",
    "vitest": "^2.1.3"
  }
}
```

- [ ] **Step 2: Create `webgame/tsconfig.json`**

```json
{
  "compilerOptions": {
    "target": "ES2022",
    "module": "ESNext",
    "moduleResolution": "bundler",
    "lib": ["ES2022"],
    "strict": true,
    "noUncheckedIndexedAccess": true,
    "skipLibCheck": true,
    "esModuleInterop": true,
    "isolatedModules": true
  },
  "include": ["src"]
}
```

- [ ] **Step 3: Create `webgame/vite.config.ts`**

```ts
import { defineConfig } from 'vitest/config';

export default defineConfig({
  test: {
    globals: true,
    environment: 'node',
  },
});
```

- [ ] **Step 4: Install dependencies in `webgame/`**

Run:
```bash
cd webgame && npm install
```

- [ ] **Step 5: Write initial setup smoke test**

Create `webgame/src/engine/__tests__/setup.test.ts`:
```ts
import { describe, it, expect } from 'vitest';

describe('Engine Test Environment', () => {
  it('runs vitest correctly', () => {
    expect(1 + 1).toBe(2);
  });
});
```

- [ ] **Step 6: Run test to verify runner works**

Run: `cd webgame && npm test`
Expected: PASS (1 test passing).

- [ ] **Step 7: Commit Task 1**

```bash
git add webgame/package.json webgame/package-lock.json webgame/tsconfig.json webgame/vite.config.ts webgame/src/engine/__tests__/setup.test.ts
git commit -m "chore(engine): scaffold webgame package and vitest test environment"
```

---

### Task 2: Core Types & Game Configuration

**Files:**
- Create: `webgame/src/engine/types.ts`
- Create: `webgame/src/engine/config.ts`
- Create: `webgame/src/engine/__tests__/config.test.ts`
- Test: Vitest verification of game state factory and configurations

**Interfaces:**
- Consumes: Task 1 test runner
- Produces: `types.ts`, `config.ts`, `createInitialGameState(config, playerNames)`

- [ ] **Step 1: Write the failing test for configuration & initial state**

Create `webgame/src/engine/__tests__/config.test.ts`:
```ts
import { describe, it, expect } from 'vitest';
import { DEFAULT_CONFIG, CLASSIC_COMPANIES, createInitialGameState } from '../config';

describe('Game Configuration & State Factory', () => {
  it('has default configuration matching spec', () => {
    expect(DEFAULT_CONFIG.boardRows).toBe(10);
    expect(DEFAULT_CONFIG.boardCols).toBe(12);
    expect(DEFAULT_CONFIG.handSize).toBe(6);
    expect(DEFAULT_CONFIG.startingCash).toBe(6000);
    expect(DEFAULT_CONFIG.safeThreshold).toBe(11);
    expect(CLASSIC_COMPANIES.length).toBeGreaterThanOrEqual(7);
  });

  it('creates an initial game state with dealt hands and empty board', () => {
    const players = [
      { name: 'Alice', isAi: false },
      { name: 'Bot 1', isAi: true },
      { name: 'Bot 2', isAi: true }
    ];
    const state = createInitialGameState(DEFAULT_CONFIG, players, 12345);

    expect(state.board.length).toBe(10);
    expect(state.board[0]?.length).toBe(12);
    expect(state.board[0]?.[0]?.type).toBe('empty');

    expect(state.players.length).toBe(3);
    expect(state.players[0]?.cash).toBe(6000);
    expect(state.players[0]?.hand.length).toBe(6);
    expect(state.phase).toBe('place_tile');
    expect(state.activePlayerIndex).toBe(0);

    // Total tiles pool should be (120 - 18 dealt) = 102
    expect(state.tilePool.length).toBe(120 - 3 * 6);
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npm test src/engine/__tests__/config.test.ts`
Expected: FAIL (modules not found).

- [ ] **Step 3: Implement `types.ts` and `config.ts`**

Create `webgame/src/engine/types.ts` with all types from spec:
- `CompanyId`, `CompanyTier`, `CompanyDefinition`, `CompanyState`.
- `TileCoord`, `CellState`, `PlayerState`.
- `TurnPhase`, `PendingFoundingState`, `PendingMergerState`, `GameState`, `GameAction`.

Create `webgame/src/engine/config.ts`:
- Define `GameConfig`, `CLASSIC_COMPANIES`.
- Define `DEFAULT_CONFIG`.
- Implement `createInitialGameState(config, players, seed?)` with deterministic tile pool shuffling and dealing.

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npm test src/engine/__tests__/config.test.ts`
Expected: PASS.

- [ ] **Step 5: Commit Task 2**

```bash
git add webgame/src/engine/types.ts webgame/src/engine/config.ts webgame/src/engine/__tests__/config.test.ts
git commit -m "feat(engine): implement core data types, configuration, and state factory"
```

---

### Task 3: Board Adjacency, Chain Detection & Move Legality

**Files:**
- Create: `webgame/src/engine/board.ts`
- Create: `webgame/src/engine/__tests__/board.test.ts`
- Test: Vitest test coverage for grid geometry and move classification

**Interfaces:**
- Consumes: `types.ts`, `config.ts`
- Produces: `getAdjacentCoords`, `classifyMove`, `applyTilePlacement`, `isTileDead`, `getContiguousCoordinates`

- [ ] **Step 1: Write the failing tests for board logic**

Create `webgame/src/engine/__tests__/board.test.ts`:
```ts
import { describe, it, expect } from 'vitest';
import { createEmptyBoard, getAdjacentCoords, classifyMove, isTileDead } from '../board';
import { DEFAULT_CONFIG } from '../config';
import { GameState } from '../types';

describe('Board Adjacency & Move Classification', () => {
  it('correctly finds adjacent coordinates within bounds', () => {
    // Corner
    const corner = getAdjacentCoords({ row: 0, col: 0 }, 10, 12);
    expect(corner).toHaveLength(2);
    expect(corner).toContainEqual({ row: 0, col: 1 });
    expect(corner).toContainEqual({ row: 1, col: 0 });

    // Center
    const center = getAdjacentCoords({ row: 4, col: 5 }, 10, 12);
    expect(center).toHaveLength(4);
  });

  it('classifies isolated tile placement', () => {
    const board = createEmptyBoard(10, 12);
    const result = classifyMove(board, { row: 2, col: 2 }, {}, 11);
    expect(result.type).toBe('isolated');
  });

  it('classifies company expansion', () => {
    const board = createEmptyBoard(10, 12);
    board[2][2] = { type: 'company', companyId: 'american' };
    const companies = {
      american: { id: 'american', size: 2, isSafe: false, isActive: true } as any
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('expansion');
    if (result.type === 'expansion') {
      expect(result.companyId).toBe('american');
    }
  });

  it('classifies company founding when touching unincorporated tiles', () => {
    const board = createEmptyBoard(10, 12);
    board[2][2] = { type: 'unincorporated' };
    const result = classifyMove(board, { row: 2, col: 3 }, {}, 11);
    expect(result.type).toBe('founding');
  });

  it('classifies merger when touching two active companies', () => {
    const board = createEmptyBoard(10, 12);
    board[2][2] = { type: 'company', companyId: 'american' };
    board[2][4] = { type: 'company', companyId: 'imperial' };
    const companies = {
      american: { id: 'american', size: 4, isSafe: false, isActive: true } as any,
      imperial: { id: 'imperial', size: 5, isSafe: false, isActive: true } as any
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('merger');
    if (result.type === 'merger') {
      expect(result.participatingCompanyIds).toContain('american');
      expect(result.participatingCompanyIds).toContain('imperial');
    }
  });

  it('detects dead tile when move would merge two safe companies', () => {
    const board = createEmptyBoard(10, 12);
    board[2][2] = { type: 'company', companyId: 'american' };
    board[2][4] = { type: 'company', companyId: 'imperial' };
    const companies = {
      american: { id: 'american', size: 12, isSafe: true, isActive: true } as any,
      imperial: { id: 'imperial', size: 15, isSafe: true, isActive: true } as any
    };
    const dead = isTileDead(board, { row: 2, col: 3 }, companies, 11);
    expect(dead).toBe(true);

    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('illegal_safe_merger');
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npm test src/engine/__tests__/board.test.ts`
Expected: FAIL.

- [ ] **Step 3: Implement `webgame/src/engine/board.ts`**

Implement:
- `createEmptyBoard(rows, cols)`
- `getAdjacentCoords(coord, rows, cols)`
- `getContiguousCoordinates(board, startCoord)` using queue flood-fill.
- `classifyMove(board, coord, companies, safeThreshold)`
- `isTileDead(board, coord, companies, safeThreshold)`

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npm test src/engine/__tests__/board.test.ts`
Expected: PASS.

- [ ] **Step 5: Commit Task 3**

```bash
git add webgame/src/engine/board.ts webgame/src/engine/__tests__/board.test.ts
git commit -m "feat(engine): implement board geometry, adjacency scanning, and move classification"
```

---

### Task 4: Stock Market, Price Curves & Transactions

**Files:**
- Create: `webgame/src/engine/market.ts`
- Create: `webgame/src/engine/__tests__/market.test.ts`
- Test: Vitest test coverage for stock pricing, portfolio valuation, and purchases

**Interfaces:**
- Consumes: `types.ts`, `config.ts`
- Produces: `calculateSharePrice`, `calculateNetWorth`, `buyShares`, `canBuyShares`

- [ ] **Step 1: Write the failing tests for stock market calculations**

Create `webgame/src/engine/__tests__/market.test.ts`:
```ts
import { describe, it, expect } from 'vitest';
import { calculateSharePrice, calculateNetWorth, buyShares } from '../market';
import { CompanyDefinition, CompanyState, PlayerState } from '../types';

describe('Stock Market & Pricing Formulas', () => {
  const lowDef: CompanyDefinition = { id: 'american', name: 'American', tier: 'low', basePrice: 200, priceDelta: 100, color: 'red' };
  const midDef: CompanyDefinition = { id: 'imperial', name: 'Imperial', tier: 'mid', basePrice: 300, priceDelta: 100, color: 'blue' };
  const highDef: CompanyDefinition = { id: 'royal', name: 'Royal', tier: 'high', basePrice: 400, priceDelta: 100, color: 'gold' };

  it('calculates stock price accurately across tiers and size bounds', () => {
    // Inactive company has price 0
    expect(calculateSharePrice(lowDef, 0)).toBe(0);

    // Size 2 (base price)
    expect(calculateSharePrice(lowDef, 2)).toBe(200);
    expect(calculateSharePrice(midDef, 2)).toBe(300);
    expect(calculateSharePrice(highDef, 2)).toBe(400);

    // Size 11 (Safe threshold: +9 deltas)
    expect(calculateSharePrice(lowDef, 11)).toBe(200 + 9 * 100); // 1100
    expect(calculateSharePrice(midDef, 11)).toBe(300 + 9 * 100); // 1200
    expect(calculateSharePrice(highDef, 11)).toBe(400 + 9 * 100); // 1300

    // Size 41+ (Cap at 41 tiles: 2 + 39 = 41)
    expect(calculateSharePrice(lowDef, 41)).toBe(200 + 39 * 100); // 4100
    expect(calculateSharePrice(lowDef, 50)).toBe(4100); // capped
  });

  it('calculates player net worth including cash and stock values', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 2500,
      shares: { american: 5, royal: 2 },
      hand: []
    };
    const companyStates: Record<string, CompanyState> = {
      american: { id: 'american', size: 11, isActive: true } as any,
      royal: { id: 'royal', size: 2, isActive: true } as any
    };
    const defs = { american: lowDef, royal: highDef };

    // Net worth = 2500 + (5 * 1100) + (2 * 400) = 2500 + 5500 + 800 = 8800
    const netWorth = calculateNetWorth(player, companyStates, defs);
    expect(netWorth).toBe(8800);
  });

  it('executes share purchases deducting cash and bank inventory', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 3000,
      shares: { american: 0 },
      hand: []
    };
    const company: CompanyState = {
      id: 'american',
      size: 5,
      isActive: true,
      availableShares: 20
    } as any;

    const result = buyShares(player, { american: company }, { american: lowDef }, [
      { companyId: 'american', count: 3 }
    ], 3);

    expect(result.success).toBe(true);
    // Price at size 5 = 200 + 3 * 100 = 500. Total cost = 1500.
    expect(result.player.cash).toBe(1500);
    expect(result.player.shares.american).toBe(3);
    expect(result.companies.american?.availableShares).toBe(17);
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npm test src/engine/__tests__/market.test.ts`
Expected: FAIL.

- [ ] **Step 3: Implement `webgame/src/engine/market.ts`**

Implement:
- `calculateSharePrice(definition, size)`
- `calculateNetWorth(player, companyStates, definitions)`
- `canBuyShares(player, companyStates, definitions, purchases, maxSharesPerPurchase)`
- `buyShares(player, companyStates, definitions, purchases, maxSharesPerPurchase)`

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npm test src/engine/__tests__/market.test.ts`
Expected: PASS.

- [ ] **Step 5: Commit Task 4**

```bash
git add webgame/src/engine/market.ts webgame/src/engine/__tests__/market.test.ts
git commit -m "feat(engine): implement stock market pricing formulas, portfolio valuation, and purchasing"
```

---

### Task 5: Merger Resolution Engine & Payouts

**Files:**
- Create: `webgame/src/engine/merger.ts`
- Create: `webgame/src/engine/__tests__/merger.test.ts`
- Test: Vitest test coverage for survivor selection, payouts, and share disposition

**Interfaces:**
- Consumes: `types.ts`, `config.ts`, `board.ts`, `market.ts`
- Produces: `determineSurvivingCompany`, `calculateMergerBonuses`, `applyShareDisposition`, `consolidateMergerTerritory`

- [ ] **Step 1: Write the failing tests for merger mechanics**

Create `webgame/src/engine/__tests__/merger.test.ts`:
```ts
import { describe, it, expect } from 'vitest';
import { determineSurvivingCompany, calculateMergerBonuses, applyShareDisposition } from '../merger';
import { CompanyState, PlayerState, CompanyDefinition } from '../types';

describe('Merger Resolution & Shareholder Payouts', () => {
  it('determines surviving company by largest size', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 10 } as any,
      imperial: { id: 'imperial', size: 8 } as any
    };
    const survivor = determineSurvivingCompany(['american', 'imperial'], companies);
    expect(survivor.survivorId).toBe('american');
    expect(survivor.hasTie).toBe(false);
  });

  it('detects ties in largest size', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 8 } as any,
      imperial: { id: 'imperial', size: 8 } as any
    };
    const survivor = determineSurvivingCompany(['american', 'imperial'], companies);
    expect(survivor.hasTie).toBe(true);
    expect(survivor.tiedCompanyIds).toEqual(['american', 'imperial']);
  });

  it('calculates majority and minority bonuses correctly with solo owner', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 5 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 0 }, hand: [], isAi: false }
    ];
    // Price = 500. Majority = 5000, Minority = 2500. Solo gets both = 7500.
    const bonuses = calculateMergerBonuses('american', 500, players);
    expect(bonuses.p1).toBe(7500);
    expect(bonuses.p2).toBe(0);
  });

  it('calculates majority and minority bonuses with 1st and 2nd place', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 8 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 4 }, hand: [], isAi: false },
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 2 }, hand: [], isAi: false }
    ];
    // Price = 600. Majority = 6000, Minority = 3000.
    const bonuses = calculateMergerBonuses('american', 600, players);
    expect(bonuses.p1).toBe(6000);
    expect(bonuses.p2).toBe(3000);
    expect(bonuses.p3).toBe(0);
  });

  it('splits bonus evenly on majority ties', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 6 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 6 }, hand: [], isAi: false },
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 2 }, hand: [], isAi: false }
    ];
    // Combined = 10*600 + 5*600 = 9000. Each tied 1st place gets 4500.
    const bonuses = calculateMergerBonuses('american', 600, players);
    expect(bonuses.p1).toBe(4500);
    expect(bonuses.p2).toBe(4500);
    expect(bonuses.p3).toBe(0);
  });

  it('applies share disposition: sell and trade 2-for-1', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Alice',
      cash: 1000,
      shares: { american: 5, imperial: 1 },
      hand: [],
      isAi: false
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 10
    } as any;

    // Player trades 4 american shares for 2 imperial shares, sells 1 american share at $400
    const result = applyShareDisposition(player, 'american', 400, survivor, {
      sell: 1,
      trade: 4,
      hold: 0
    });

    expect(result.player.shares.american).toBe(0);
    expect(result.player.shares.imperial).toBe(3); // 1 previous + 2 traded
    expect(result.player.cash).toBe(1400); // 1000 + 1 * 400
    expect(result.survivorCompany.availableShares).toBe(8); // 10 - 2
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npm test src/engine/__tests__/merger.test.ts`
Expected: FAIL.

- [ ] **Step 3: Implement `webgame/src/engine/merger.ts`**

Implement:
- `determineSurvivingCompany(companyIds, companies)`
- `calculateMergerBonuses(defunctCompanyId, sharePrice, players)`
- `applyShareDisposition(player, defunctCompanyId, defunctPrice, survivorCompany, choices)`
- `consolidateMergerTerritory(board, bridgeCoord, defunctCompanyId, survivorCompanyId)`

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npm test src/engine/__tests__/merger.test.ts`
Expected: PASS.

- [ ] **Step 5: Commit Task 5**

```bash
git add webgame/src/engine/merger.ts webgame/src/engine/__tests__/merger.test.ts
git commit -m "feat(engine): implement merger mechanics, shareholder bonuses, and share disposition"
```

---

### Task 6: Primary Game State Reducer & Turn Lifecycle

**Files:**
- Create: `webgame/src/engine/reducer.ts`
- Create: `webgame/src/engine/__tests__/reducer.test.ts`
- Test: Vitest test coverage for complete turn lifecycle and game over conditions

**Interfaces:**
- Consumes: `types.ts`, `config.ts`, `board.ts`, `market.ts`, `merger.ts`
- Produces: `gameReducer(state: GameState, action: GameAction): GameState`

- [ ] **Step 1: Write the failing tests for game reducer**

Create `webgame/src/engine/__tests__/reducer.test.ts`:
```ts
import { describe, it, expect } from 'vitest';
import { gameReducer } from '../reducer';
import { createInitialGameState, DEFAULT_CONFIG } from '../config';

describe('Game State Reducer & Turn Flow', () => {
  it('places isolated tile and transitions to buy_shares phase', () => {
    const state = createInitialGameState(DEFAULT_CONFIG, [
      { name: 'P1', isAi: false },
      { name: 'P2', isAi: false }
    ], 42);

    const tile = state.players[0]!.hand[0]!;
    const nextState = gameReducer(state, { type: 'PLACE_TILE', coord: tile });

    expect(nextState.board[tile.row]![tile.col]!.type).toBe('unincorporated');
    expect(nextState.phase).toBe('buy_shares');
    expect(nextState.players[0]!.hand).not.toContainEqual(tile);
  });

  it('triggers founding company and awards founder shares', () => {
    let state = createInitialGameState(DEFAULT_CONFIG, [
      { name: 'P1', isAi: false },
      { name: 'P2', isAi: false }
    ], 42);

    // Force place an unincorporated tile at (2, 2)
    state.board[2]![2] = { type: 'unincorporated' };
    state.players[0]!.hand[0] = { row: 2, col: 3 };

    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 3 } });
    expect(state.phase).toBe('choose_founding_company');

    state = gameReducer(state, { type: 'CHOOSE_FOUNDED_COMPANY', companyId: 'american' });
    expect(state.phase).toBe('buy_shares');
    expect(state.companies.american?.isActive).toBe(true);
    expect(state.companies.american?.size).toBe(2);
    // Founder bonus in classic is 10 shares
    expect(state.players[0]!.shares.american).toBe(10);
    expect(state.companies.american?.availableShares).toBe(15);
  });

  it('completes turn, draws replacement tile, and advances active player', () => {
    let state = createInitialGameState(DEFAULT_CONFIG, [
      { name: 'P1', isAi: false },
      { name: 'P2', isAi: false }
    ], 42);

    const tile = state.players[0]!.hand[0]!;
    state = gameReducer(state, { type: 'PLACE_TILE', coord: tile });
    state = gameReducer(state, { type: 'BUY_SHARES', purchases: [] });
    state = gameReducer(state, { type: 'END_TURN' });

    expect(state.activePlayerIndex).toBe(1);
    expect(state.phase).toBe('place_tile');
    expect(state.players[0]!.hand.length).toBe(6); // Replenished
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npm test src/engine/__tests__/reducer.test.ts`
Expected: FAIL.

- [ ] **Step 3: Implement `webgame/src/engine/reducer.ts`**

Implement:
- `gameReducer(state, action)`
- Handling of all actions (`PLACE_TILE`, `CHOOSE_FOUNDED_COMPANY`, `CHOOSE_MERGER_SURVIVOR`, `DISPOSE_MERGER_SHARES`, `BUY_SHARES`, `DISCARD_DEAD_TILE`, `END_TURN`).
- Turn replenishment, dead tile cleanup.
- Game over evaluation and final portfolio liquidation.

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npm test src/engine/__tests__/reducer.test.ts`
Expected: PASS.

- [ ] **Step 5: Commit Task 6**

```bash
git add webgame/src/engine/reducer.ts webgame/src/engine/__tests__/reducer.test.ts
git commit -m "feat(engine): implement primary state reducer, turn lifecycle, and game over triggers"
```

---

### Task 7: AI Bot Decision Engine & Full Match Simulation

**Files:**
- Create: `webgame/src/engine/ai.ts`
- Create: `webgame/src/engine/__tests__/ai.test.ts`
- Test: Vitest test coverage for AI decision heuristics and complete simulated bot games

**Interfaces:**
- Consumes: All engine modules
- Produces: `getBestTileMove`, `getBestSharePurchases`, `getBestMergerDisposition`, `simulateAiTurn`

- [ ] **Step 1: Write the failing tests for AI bot decision making**

Create `webgame/src/engine/__tests__/ai.test.ts`:
```ts
import { describe, it, expect } from 'vitest';
import { getBestTileMove, getBestSharePurchases, getBestMergerDisposition, playFullGameAi } from '../ai';
import { createInitialGameState, DEFAULT_CONFIG } from '../config';

describe('AI Player Decision Engine', () => {
  it('selects a valid playable tile from its hand', () => {
    const state = createInitialGameState(DEFAULT_CONFIG, [
      { name: 'Bot 1', isAi: true },
      { name: 'Bot 2', isAi: true }
    ], 100);

    const move = getBestTileMove(state, 0);
    expect(move).toBeDefined();
    expect(state.players[0]!.hand).toContainEqual(move);
  });

  it('plays a complete 4-player AI game to completion without errors', () => {
    const config = { ...DEFAULT_CONFIG, boardRows: 6, boardCols: 6 }; // smaller board for quick test
    const finalState = playFullGameAi(config, 4, 9999, 500);

    expect(finalState.phase).toBe('game_over');
    expect(finalState.winnerPlayerId).toBeDefined();
    expect(finalState.players[0]!.cash).toBeGreaterThan(0);
  });
});
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd webgame && npm test src/engine/__tests__/ai.test.ts`
Expected: FAIL.

- [ ] **Step 3: Implement `webgame/src/engine/ai.ts`**

Implement:
- `getBestTileMove(state, playerIndex)`
- `getBestSharePurchases(state, playerIndex)`
- `getBestMergerDisposition(state, playerIndex, defunctCompanyId)`
- `simulateAiTurn(state)`
- `playFullGameAi(config, playerCount, seed, maxTurns)`

- [ ] **Step 4: Run test to verify it passes**

Run: `cd webgame && npm test src/engine/__tests__/ai.test.ts`
Expected: PASS.

- [ ] **Step 5: Run full test suite across entire engine**

Run: `cd webgame && npm test`
Expected: All test suites PASS with 100% success.

- [ ] **Step 6: Commit Task 7**

```bash
git add webgame/src/engine/ai.ts webgame/src/engine/__tests__/ai.test.ts
git commit -m "feat(engine): implement AI decision heuristics and full match simulation"
```
