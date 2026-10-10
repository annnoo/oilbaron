import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  GameConfig,
  GameState,
  PlayerInput,
  PlayerState,
  TileCoord,
} from './types';

export type { GameConfig };

export const CLASSIC_COMPANIES: CompanyDefinition[] = [
  { id: 'american', name: 'American Natural Gas', tier: 'low', basePrice: 200, priceDelta: 100, color: '#e63946', dividendYield: 0.10 },
  { id: 'arabian', name: 'Arabian Oil', tier: 'low', basePrice: 200, priceDelta: 100, color: '#f4a261', dividendYield: 0.10 },
  { id: 'imperial', name: 'Imperial Petroleum', tier: 'mid', basePrice: 300, priceDelta: 100, color: '#2a9d8f', dividendYield: 0.125 },
  { id: 'supra', name: 'Supra Oil', tier: 'mid', basePrice: 300, priceDelta: 100, color: '#e76f51', dividendYield: 0.067 },
  { id: 'tower', name: 'Tower Drilling', tier: 'mid', basePrice: 300, priceDelta: 100, color: '#457b9d', dividendYield: 0.10 },
  { id: 'royal', name: 'Royal Dutch', tier: 'high', basePrice: 400, priceDelta: 100, color: '#1d3557', dividendYield: 0.125 },
  { id: 'weemoco', name: 'Weemoco Oil', tier: 'high', basePrice: 400, priceDelta: 100, color: '#38b000', dividendYield: 0.067 },
  { id: 'purple', name: 'Purple Oil', tier: 'high', basePrice: 400, priceDelta: 100, color: '#7209b7', dividendYield: 0.067 },
];

export const DEFAULT_CONFIG: GameConfig = {
  ruleset: 'oilbaron_classic',
  founderBonusShares: 10,
  boardRows: 10,
  boardCols: 12,
  handSize: 6,
  safeThreshold: 11,
  maxSharesPerPurchase: 3,
  totalSharesPerCompany: 25,
  startingCash: 6000,
  companies: CLASSIC_COMPANIES,
  payDividends: false,
  hardShareLimits: true,
  allowBlanks: false,
  purchaseLimit: true,
  showDiscard: false,
};

export const CLASSIC_CONFIG = DEFAULT_CONFIG;

export const MODERN_BALANCED_CONFIG: GameConfig = {
  ruleset: 'modern_balanced',
  founderBonusShares: 1,
  boardRows: 10,
  boardCols: 12,
  handSize: 6,
  safeThreshold: 11,
  maxSharesPerPurchase: 3,
  totalSharesPerCompany: 25,
  startingCash: 6000,
  companies: CLASSIC_COMPANIES,
  payDividends: false,
  hardShareLimits: true,
  allowBlanks: false,
  purchaseLimit: true,
  showDiscard: false,
};

export const DEFAULT_PLAYER_COLORS: readonly string[] = [
  '#f59e0b', // Gold / Amber
  '#38bdf8', // Cyan / Sky Blue
  '#22c55e', // Emerald Green
  '#a855f7', // Violet / Purple
  '#f97316', // Coral / Orange
  '#ef4444', // Crimson / Red
];

/**
 * Creates a deterministic pseudo-random number generator (Mulberry32).
 */
export function createRng(seed: number): () => number {
  let s = seed >>> 0;
  return function next(): number {
    s = (s + 0x6d2b79f5) | 0;
    let t = Math.imul(s ^ (s >>> 15), 1 | s);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

/**
 * Shuffles an array immutably using Fisher-Yates with the provided RNG.
 */
export function shuffle<T>(array: readonly T[], rng: () => number): T[] {
  const result = [...array];
  for (let i = result.length - 1; i > 0; i--) {
    const j = Math.floor(rng() * (i + 1));
    const temp = result[i]!;
    result[i] = result[j]!;
    result[j] = temp;
  }
  return result;
}

/**
 * Creates an empty game board of given rows and columns.
 */
export function createEmptyBoard(rows: number, cols: number): CellState[][] {
  const board: CellState[][] = [];
  for (let r = 0; r < rows; r++) {
    const row: CellState[] = [];
    for (let c = 0; c < cols; c++) {
      row.push({ type: 'empty' });
    }
    board.push(row);
  }
  return board;
}

/**
 * Initializes a new GameState with shuffled and dealt tiles, empty board,
 * and initialized player portfolios and company bank inventories.
 */
export function createInitialGameState(
  config: GameConfig,
  players: readonly PlayerInput[],
  seed?: number
): GameState {
  const rng = seed !== undefined ? createRng(seed) : Math.random;

  // Generate all grid tile coordinates
  const allTiles: TileCoord[] = [];
  for (let r = 0; r < config.boardRows; r++) {
    for (let c = 0; c < config.boardCols; c++) {
      allTiles.push({ row: r, col: c });
    }
  }

  // Inject wildcard blank pieces if enabled (classic 1993 Oil Baron: 4 blank pieces)
  if (config.allowBlanks) {
    const blankCount = 4;
    for (let i = 0; i < blankCount; i++) {
      allTiles.push({ row: -1, col: -1 - i, isBlank: true });
    }
  }

  // Shuffle tile pool
  const shuffledTiles = shuffle(allTiles, rng);

  // Deal starting hands to players
  const playerStates: PlayerState[] = [];
  let cursor = 0;

  for (let i = 0; i < players.length; i++) {
    const input = players[i]!;
    const name = typeof input === 'string' ? input : input.name;
    const isAi = typeof input === 'string' ? false : (input.isAi ?? false);
    const id = typeof input === 'string' ? `p${i + 1}` : (input.id ?? `p${i + 1}`);

    const hand = shuffledTiles.slice(cursor, cursor + config.handSize);
    cursor += config.handSize;

    const shares: Record<CompanyId, number> = {};
    for (const company of config.companies) {
      shares[company.id] = 0;
    }

    const color =
      typeof input === 'string'
        ? DEFAULT_PLAYER_COLORS[i % DEFAULT_PLAYER_COLORS.length]
        : (input.color ?? DEFAULT_PLAYER_COLORS[i % DEFAULT_PLAYER_COLORS.length]);

    playerStates.push({
      id,
      name,
      isAi,
      cash: config.startingCash,
      shares,
      hand,
      color,
    });
  }

  const remainingTilePool = shuffledTiles.slice(cursor);

  // Initialize company states
  const companyStates: Record<CompanyId, CompanyState> = {};
  for (const def of config.companies) {
    companyStates[def.id] = {
      id: def.id,
      name: def.name,
      tier: def.tier,
      color: def.color,
      size: 0,
      isActive: false,
      isSafe: false,
      availableShares: config.totalSharesPerCompany,
    };
  }

  return {
    config,
    board: createEmptyBoard(config.boardRows, config.boardCols),
    tilePool: remainingTilePool,
    companies: companyStates,
    players: playerStates,
    activePlayerIndex: 0,
    phase: 'place_tile',
    pendingFounding: null,
    pendingMerger: null,
    turnPurchasedSharesCount: 0,
    turnSoldShares: {},
    lastDividendPayout: null,
    history: [],
    winnerPlayerId: null,
  };
}
