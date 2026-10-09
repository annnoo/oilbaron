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

export interface PlayerInit {
  id?: string;
  name: string;
  isAi?: boolean;
}

export type PlayerInput = string | PlayerInit;

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

export interface SharePurchase {
  companyId: CompanyId;
  count: number;
}

export interface ShareDispositionChoices {
  sell: number;
  trade: number;
  hold: number;
}

export type GameAction =
  | { type: 'PLACE_TILE'; coord: TileCoord }
  | { type: 'CHOOSE_FOUNDED_COMPANY'; companyId: CompanyId }
  | { type: 'CHOOSE_MERGER_SURVIVOR'; survivorCompanyId: CompanyId }
  | { type: 'DISPOSE_MERGER_SHARES'; choices: ShareDispositionChoices }
  | { type: 'BUY_SHARES'; purchases: SharePurchase[] }
  | { type: 'DISCARD_DEAD_TILE'; coord: TileCoord }
  | { type: 'END_TURN' };

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
