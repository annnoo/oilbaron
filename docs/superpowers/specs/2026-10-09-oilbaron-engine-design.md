# Oil Baron Core TypeScript Engine Specification (Phase 2)

## 1. Overview & Architectural Goals

The goal of Phase 2 is to design and implement the **Core Game Engine** for the modern Oil Baron reimplementation in `webgame/src/engine/`. 

### Key Principles
* **Pure TypeScript, Zero UI Dependencies**: The engine has zero dependency on React, DOM, Canvas, or browser APIs. It is a completely headless, deterministic library that can run in Node.js, Vitest, web workers, or the main browser thread.
* **Immutable State Machine**: All state transitions occur through pure reducer functions: `(state: GameState, action: GameAction) => GameState`. This guarantees determinism, simplifies debugging, enables trivial unit testing, and provides out-of-the-box support for Undo/Redo and match replay recording.
* **Data-Driven & Extensible**: Companies are not hardcoded enums. Company definitions (names, tiers, base prices, price deltas, colors, icons) are defined as configurable data templates in `config.companies`, making adding new companies, custom board sizes, or modding trivial.
* **Configurable Rulesets**: Supports both authentic 1993 *Oil Baron* rules (10 founder bonus shares) and balanced modern play (1 founder bonus share), along with configurable grid dimensions, hand sizes, and safe thresholds.

---

## 2. Directory Structure & File Map

```
webgame/
├── package.json
├── tsconfig.json
├── vite.config.ts
└── src/
    └── engine/
        ├── types.ts          # Core type definitions, enums, interfaces, actions
        ├── config.ts         # Default rulesets, company definitions, game configurations
        ├── board.ts          # Grid model, adjacency scanning, chain detection, move classification
        ├── market.ts         # Stock price calculation, share inventory, valuation
        ├── merger.ts         # Merger resolution: survivor determination, bonus payouts, share disposition
        ├── reducer.ts        # Primary state reducer, turn lifecycle transitions, action validation
        ├── ai.ts             # Deterministic CPU player evaluation and decision heuristics
        └── __tests__/        # Exhaustive Vitest test suite
            ├── board.test.ts
            ├── market.test.ts
            ├── merger.test.ts
            ├── reducer.test.ts
            └── ai.test.ts
```

---

## 3. Data Models & Configuration

### 3.1 Types (`types.ts`)
```ts
export type CompanyId = string;
export type CompanyTier = 'low' | 'mid' | 'high' | 'custom';

export interface CompanyDefinition {
  id: CompanyId;
  name: string;
  tier: CompanyTier;
  basePrice: number;     // e.g. $200 (low), $300 (mid), $400 (high)
  priceDelta: number;    // e.g. $100 per size tier
  color: string;
  icon?: string;
}

export interface CompanyState {
  id: CompanyId;
  name: string;
  tier: CompanyTier;
  color: string;
  size: number;          // Number of connected tiles on the board
  isActive: boolean;     // True if currently founded on the board
  isSafe: boolean;       // True if size >= safeThreshold (cannot be merged out)
  availableShares: number; // Remaining shares in the bank (starts at totalShares)
}

export interface TileCoord {
  row: number; // 0..rows-1
  col: number; // 0..cols-1
}

export type CellState =
  | { type: 'empty' }
  | { type: 'unincorporated' }
  | { type: 'company'; companyId: CompanyId };

export interface PlayerState {
  id: string;
  name: string;
  isAi: boolean;
  cash: number;
  shares: Record<CompanyId, number>;
  hand: TileCoord[];
}

export type TurnPhase =
  | 'place_tile'
  | 'choose_founding_company'
  | 'choose_merger_survivor'
  | 'dispose_merger_shares'
  | 'buy_shares'
  | 'game_over';

export interface PendingFoundingState {
  coord: TileCoord;
  connectedCoords: TileCoord[];
  availableCompanyIds: CompanyId[];
}

export interface PendingMergerState {
  coord: TileCoord;
  participatingCompanyIds: CompanyId[];
  survivingCompanyId: CompanyId | null;
  defunctCompanyQueue: CompanyId[];
  currentDefunctCompanyId: CompanyId | null;
  shareholderQueue: string[]; // Player IDs with shares in currentDefunctCompanyId
  currentShareholderIndex: number;
  preMergerPrices: Record<CompanyId, number>;
}

export interface GameState {
  config: GameConfig;
  board: CellState[][];
  tilePool: TileCoord[];
  companies: Record<CompanyId, CompanyState>;
  players: PlayerState[];
  activePlayerIndex: number;
  phase: TurnPhase;
  pendingFounding: PendingFoundingState | null;
  pendingMerger: PendingMergerState | null;
  turnPurchasedSharesCount: number;
  history: GameAction[];
  winnerPlayerId: string | null;
}
```

### 3.2 Configuration (`config.ts`)
```ts
export interface GameConfig {
  ruleset: 'oilbaron_classic' | 'modern_balanced';
  founderBonusShares: number;    // 10 for classic, 1 for modern
  boardRows: number;             // Default 10
  boardCols: number;             // Default 12 (120 tiles)
  handSize: number;              // Default 6
  safeThreshold: number;         // Default 11 tiles
  maxSharesPerPurchase: number;  // Default 3 shares per turn
  totalSharesPerCompany: number; // Default 25 shares
  startingCash: number;          // Default $6,000
  companies: CompanyDefinition[];// Data-driven list of company templates
}

export const CLASSIC_COMPANIES: CompanyDefinition[] = [
  { id: 'american', name: 'American Natural Gas', tier: 'low', basePrice: 200, priceDelta: 100, color: '#e63946' },
  { id: 'arabian', name: 'Arabian Oil', tier: 'low', basePrice: 200, priceDelta: 100, color: '#f4a261' },
  { id: 'imperial', name: 'Imperial Petroleum', tier: 'mid', basePrice: 300, priceDelta: 100, color: '#2a9d8f' },
  { id: 'supra', name: 'Supra Oil', tier: 'mid', basePrice: 300, priceDelta: 100, color: '#e76f51' },
  { id: 'tower', name: 'Tower Drilling', tier: 'mid', basePrice: 300, priceDelta: 100, color: '#457b9d' },
  { id: 'royal', name: 'Royal Dutch', tier: 'high', basePrice: 400, priceDelta: 100, color: '#1d3557' },
  { id: 'weemoco', name: 'Weemoco Oil', tier: 'high', basePrice: 400, priceDelta: 100, color: '#38b000' },
  { id: 'purple', name: 'Purple Oil', tier: 'high', basePrice: 400, priceDelta: 100, color: '#7209b7' },
];
```

---

## 4. Subsystems Specification

### 4.1 Board & Move Classification (`board.ts`)
* **Adjacency**: Evaluates cardinal neighbors:
  $$\mathcal{N}(r, c) = \{(r-1, c), (r+1, c), (r, c-1), (r, c+1)\} \cap \text{BoardBounds}$$
* **Classification**:
  1. **Isolated**: 0 adjacent occupied tiles $\rightarrow$ becomes unincorporated chit.
  2. **Expansion**: Adjacent to unincorporated tiles and exactly **1** active company $\rightarrow$ company absorbs placed tile and contiguous unincorporated chits.
  3. **Founding**: Adjacent to 1+ unincorporated chits and **0** active companies $\rightarrow$ triggers `choose_founding_company`.
  4. **Merger**: Adjacent to **2+** distinct active companies $\rightarrow$
     - If $\ge 2$ of the touching companies are *safe* ($\text{size} \ge 11$), move is **illegal/dead tile**.
     - Otherwise $\rightarrow$ triggers `choose_merger_survivor` or enters merger resolution.
* **Dead Tile Detection**:
  A tile is dead if placing it would merge two or more safe companies. Players can discard dead tiles to draw fresh replacements.

### 4.2 Stock Market & Pricing (`market.ts`)
* **Price Curve**:
  For an active company of size $S$:
  $$P(S) = \text{basePrice} + \min(\max(0, S - 2), 39) \times \text{priceDelta}$$
* **Net Worth Calculation**:
  $$\text{NetWorth}(P) = \text{cash} + \sum_{C \in \text{Companies}} \text{shares}[C] \times P(\text{size}_C)$$

### 4.3 Merger Resolution State Machine (`merger.ts`)
1. **Survivor Selection**:
   * Highest size survives.
   * On ties: active player chooses survivor (or classic deterministic scan order).
2. **Sequential Defunct Queue**:
   * Defunct companies are ordered (smallest size first).
3. **Shareholder Bonuses (per defunct company)**:
   * Rank players by share count in defunct company.
   * Solo owner: receives Majority + Minority ($15 \times P$).
   * Single 1st, single 2nd: 1st gets $10 \times P$, 2nd gets $5 \times P$.
   * Multiple 1st place ties: split $15 \times P$ equally among 1st place ties; no 2nd place bonus.
   * Single 1st, multiple 2nd place ties: 1st gets $10 \times P$, 2nd place ties split $5 \times P$ equally.
4. **Share Disposition**:
   * Turn order starting from active player:
     * **Sell**: shares liquidated at pre-merger price $P$.
     * **Trade**: 2 defunct shares $\rightarrow$ 1 survivor share (while survivor bank stock exists).
     * **Hold**: keep defunct shares in portfolio.
5. **Board Consolidation**:
   * Survivor claims all defunct tiles + bridging tile.
   * Defunct company resets to inactive ($S = 0$) and returns to the available company pool.

### 4.4 Turn Reducer & Game Lifecycle (`reducer.ts`)
Handles actions:
* `PLACE_TILE`
* `CHOOSE_FOUNDED_COMPANY`
* `CHOOSE_MERGER_SURVIVOR`
* `DISPOSE_MERGER_SHARES`
* `BUY_SHARES`
* `DISCARD_DEAD_TILE`
* `END_TURN`

#### Game Over Check:
* All active companies on board are safe ($\ge 11$), OR any company reaches $\ge 41$, OR tile pool exhausted and no moves remain.
* On Game Over:
  * Award majority & minority bonuses for all active companies on board.
  * Liquidate all shares at current market value.
  * Rank players by final cash.

### 4.5 AI Player Decision Engine (`ai.ts`)
* **Bot Move Decision**:
  Evaluates each playable tile in bot's hand with heuristic utility scoring:
  - Founding: utility bonus for free shares in a fresh low-entry-cost company.
  - Expansion: utility proportional to growth in owned companies.
  - Merger: evaluates net gain from merger bonuses and 2-for-1 trades vs opponent gains.
* **Bot Stock Purchases**:
  - Prioritizes buying shares to establish or defend majority holdings.
  - Buys high-growth undervalued shares while maintaining liquidity.
* **Bot Merger Disposition**:
  - Trades 2-for-1 when $P_{\text{survivor}} > 2 \times P_{\text{defunct}}$.
  - Sells shares when cash is needed or survivor stock is depleted.

---

## 5. Testing & Verification Strategy

All components will be tested using **Vitest** in `webgame/src/engine/__tests__/`:
1. `board.test.ts`: Grid sizing, isolated tiles, expansion, company founding, safe company dead-tile blocking.
2. `market.test.ts`: Price curve calculation across all tiers, share purchasing limits, bank inventory deductions.
3. `merger.test.ts`: Survivor selection, solo bonus, majority/minority splits, 2-for-1 trades, multi-company cascade mergers.
4. `reducer.test.ts`: Full turn cycle (Place $\rightarrow$ Form $\rightarrow$ Buy $\rightarrow$ End Turn), game over triggers, final portfolio liquidation.
5. `ai.test.ts`: AI tile evaluation, stock purchase decisions, complete bot vs bot simulated game to completion.
