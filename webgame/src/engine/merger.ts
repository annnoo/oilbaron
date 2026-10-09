import { getAdjacentCoords, getContiguousCoordinates } from './board';
import type {
  CellState,
  CompanyId,
  CompanyState,
  PlayerState,
  ShareDispositionChoices,
  TileCoord,
} from './types';

export interface SurvivingCompanyResult {
  survivorId: CompanyId | null;
  hasTie: boolean;
  tiedCompanyIds: CompanyId[];
}

export interface ShareDispositionValidation {
  valid: boolean;
  reason?: string;
}

export interface ShareDispositionResult {
  player: PlayerState;
  survivorCompany: CompanyState;
  cashReceived: number;
  sharesReceived: number;
}

/**
 * Determines the surviving company in a merger based on board size.
 *
 * Rules:
 * - The company with the strictly largest tile size survives.
 * - If two or more companies tie for the largest size, `hasTie` is true,
 *   `survivorId` is null, and `tiedCompanyIds` contains the tied contenders.
 * - If no companies are provided, returns survivorId null and hasTie false.
 */
export function determineSurvivingCompany(
  companyIds: readonly CompanyId[],
  companies: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>
): SurvivingCompanyResult {
  const uniqueCompanyIds = [...new Set(companyIds)];
  if (uniqueCompanyIds.length === 0) {
    return {
      survivorId: null,
      hasTie: false,
      tiedCompanyIds: [],
    };
  }

  let maxSize = -1;
  for (const id of uniqueCompanyIds) {
    const size = companies[id]?.size ?? 0;
    if (size > maxSize) {
      maxSize = size;
    }
  }

  const tied = uniqueCompanyIds.filter((id) => (companies[id]?.size ?? 0) === maxSize);

  if (tied.length > 1) {
    return {
      survivorId: null,
      hasTie: true,
      tiedCompanyIds: tied,
    };
  }

  return {
    survivorId: tied[0] ?? null,
    hasTie: false,
    tiedCompanyIds: [],
  };
}

/**
 * Calculates majority and minority shareholder bonuses for a defunct company.
 *
 * Rules:
 * - Majority Bonus Pool = 10 * sharePrice
 * - Minority Bonus Pool = 5 * sharePrice
 *
 * Tie-splitting and distribution:
 * 1. Solo shareholder: receives both majority and minority bonuses (15 * sharePrice).
 * 2. Single 1st place, single 2nd place:
 *    - 1st place gets 10 * sharePrice
 *    - 2nd place gets 5 * sharePrice
 * 3. Multiple 1st place ties:
 *    - Combined pool (15 * sharePrice) split equally among tied 1st place players.
 *    - No 2nd place bonus is awarded.
 * 4. Single 1st place, multiple 2nd place ties:
 *    - 1st place gets 10 * sharePrice
 *    - Minority bonus (5 * sharePrice) split equally among tied 2nd place players.
 * 5. Players with 0 shares receive 0 bonus.
 */
export function calculateMergerBonuses(
  defunctCompanyId: CompanyId,
  sharePrice: number,
  players: readonly PlayerState[]
): Record<string, number> {
  const bonuses: Record<string, number> = {};
  for (const p of players) {
    bonuses[p.id] = 0;
  }

  if (sharePrice <= 0) {
    return bonuses;
  }

  // Filter players holding at least 1 share in the defunct company
  const shareholders = players.filter(
    (p) => (p.shares[defunctCompanyId] ?? 0) > 0
  );

  if (shareholders.length === 0) {
    return bonuses;
  }

  const majorityPool = 10 * sharePrice;
  const minorityPool = 5 * sharePrice;

  // Solo shareholder monopoly
  if (shareholders.length === 1) {
    bonuses[shareholders[0]!.id] = majorityPool + minorityPool;
    return bonuses;
  }

  // Find highest share count
  let maxShares = 0;
  for (const p of shareholders) {
    const count = p.shares[defunctCompanyId]!;
    if (count > maxShares) {
      maxShares = count;
    }
  }

  const firstPlace = shareholders.filter(
    (p) => p.shares[defunctCompanyId] === maxShares
  );

  // Multiple 1st place ties
  if (firstPlace.length > 1) {
    const combinedPool = majorityPool + minorityPool;
    const splitBonus = Math.floor(combinedPool / firstPlace.length);
    for (const p of firstPlace) {
      bonuses[p.id] = splitBonus;
    }
    return bonuses;
  }

  // Single 1st place
  bonuses[firstPlace[0]!.id] = majorityPool;

  // Find 2nd highest share count
  let secondMaxShares = 0;
  for (const p of shareholders) {
    const count = p.shares[defunctCompanyId]!;
    if (count < maxShares && count > secondMaxShares) {
      secondMaxShares = count;
    }
  }

  if (secondMaxShares > 0) {
    const secondPlace = shareholders.filter(
      (p) => p.shares[defunctCompanyId] === secondMaxShares
    );
    const splitMinority = Math.floor(minorityPool / secondPlace.length);
    for (const p of secondPlace) {
      bonuses[p.id] = splitMinority;
    }
  }

  return bonuses;
}

/**
 * Validates a player's merger share disposition choices.
 */
export function validateShareDisposition(
  player: PlayerState,
  defunctCompanyId: CompanyId,
  survivorCompany: CompanyState,
  choices: Partial<ShareDispositionChoices>
): ShareDispositionValidation {
  const currentDefunctShares = player.shares[defunctCompanyId] ?? 0;
  const sell = choices.sell ?? 0;
  const trade = choices.trade ?? 0;
  const hold = choices.hold !== undefined
    ? choices.hold
    : currentDefunctShares - (sell + trade);

  if (
    !Number.isInteger(sell) ||
    !Number.isInteger(trade) ||
    !Number.isInteger(hold)
  ) {
    return {
      valid: false,
      reason: 'Disposition counts must be integers',
    };
  }

  if (sell < 0 || trade < 0 || hold < 0) {
    return {
      valid: false,
      reason: 'Disposition choices cannot be negative',
    };
  }

  if (trade % 2 !== 0) {
    return {
      valid: false,
      reason: 'Trade count must be an even number (2 defunct shares for 1 survivor share)',
    };
  }

  const totalAllocated = sell + trade + hold;
  if (totalAllocated > currentDefunctShares) {
    return {
      valid: false,
      reason: `Total shares allocated (${totalAllocated}) exceed player's held shares (${currentDefunctShares})`,
    };
  }

  if (totalAllocated < currentDefunctShares) {
    return {
      valid: false,
      reason: `Total shares allocated (${totalAllocated}) must equal player's held shares (${currentDefunctShares})`,
    };
  }

  const survivorSharesNeeded = Math.floor(trade / 2);
  if (survivorSharesNeeded > survivorCompany.availableShares) {
    return {
      valid: false,
      reason: `Insufficient survivor shares available in bank (needed ${survivorSharesNeeded}, available ${survivorCompany.availableShares})`,
    };
  }

  return { valid: true };
}

/**
 * Immutably applies share disposition for a player during a merger:
 * - Liquidates `sell` defunct shares at `defunctPrice` into player cash.
 * - Trades `trade` defunct shares (2:1 ratio) for survivor company shares from bank.
 * - Retains `hold` defunct shares in player portfolio (defaults to unallocated shares).
 */
export function applyShareDisposition(
  player: PlayerState,
  defunctCompanyId: CompanyId,
  defunctPrice: number,
  survivorCompany: CompanyState,
  choices: Partial<ShareDispositionChoices>
): ShareDispositionResult {
  const currentDefunctShares = player.shares[defunctCompanyId] ?? 0;
  const sell = choices.sell ?? 0;
  const trade = choices.trade ?? 0;
  const hold = choices.hold !== undefined
    ? choices.hold
    : currentDefunctShares - (sell + trade);

  const normalizedChoices: ShareDispositionChoices = {
    sell,
    trade,
    hold,
  };

  const validation = validateShareDisposition(
    player,
    defunctCompanyId,
    survivorCompany,
    normalizedChoices
  );

  if (!validation.valid) {
    throw new Error(validation.reason);
  }

  const survivorSharesGained = Math.floor(normalizedChoices.trade / 2);
  const cashGained = normalizedChoices.sell * defunctPrice;

  const newShares: Record<CompanyId, number> = {
    ...player.shares,
    [defunctCompanyId]: normalizedChoices.hold,
    [survivorCompany.id]:
      (player.shares[survivorCompany.id] ?? 0) + survivorSharesGained,
  };

  const newPlayer: PlayerState = {
    ...player,
    cash: player.cash + cashGained,
    shares: newShares,
    hand: [...player.hand],
  };

  const newSurvivorCompany: CompanyState = {
    ...survivorCompany,
    availableShares: survivorCompany.availableShares - survivorSharesGained,
  };

  return {
    player: newPlayer,
    survivorCompany: newSurvivorCompany,
    cashReceived: cashGained,
    sharesReceived: survivorSharesGained,
  };
}

/**
 * Immutably consolidates board territory during a merger:
 * - Converts all board cells of `defunctCompanyId` into `survivorCompanyId`.
 * - Sets the bridging coordinate `bridgeCoord` to `survivorCompanyId`.
 * - Absorbs any contiguous unincorporated tiles adjacent to the bridging coordinate.
 */
export function consolidateMergerTerritory(
  board: readonly (readonly CellState[])[],
  bridgeCoord: TileCoord | null | undefined,
  defunctCompanyId: CompanyId,
  survivorCompanyId: CompanyId
): CellState[][] {
  const rows = board.length;
  if (rows === 0) return [];
  const cols = board[0]?.length ?? 0;

  // Transform defunct company cells into survivor company cells
  const newBoard: CellState[][] = board.map((row) =>
    row.map((cell) => {
      if (cell.type === 'company' && cell.companyId === defunctCompanyId) {
        return { type: 'company', companyId: survivorCompanyId };
      }
      return cell;
    })
  );

  // Place and absorb bridge coordinate if valid
  if (
    bridgeCoord &&
    bridgeCoord.row >= 0 &&
    bridgeCoord.row < rows &&
    bridgeCoord.col >= 0 &&
    bridgeCoord.col < cols
  ) {
    newBoard[bridgeCoord.row]![bridgeCoord.col] = {
      type: 'company',
      companyId: survivorCompanyId,
    };

    // Absorb any contiguous unincorporated clusters touching the bridge
    const neighbors = getAdjacentCoords(bridgeCoord, rows, cols);
    for (const n of neighbors) {
      const neighborCell = newBoard[n.row]?.[n.col];
      if (neighborCell && neighborCell.type === 'unincorporated') {
        const cluster = getContiguousCoordinates(newBoard, n);
        for (const coord of cluster) {
          newBoard[coord.row]![coord.col] = {
            type: 'company',
            companyId: survivorCompanyId,
          };
        }
      }
    }
  }

  return newBoard;
}

/**
 * Returns the queue of defunct companies sorted by size ascending (smallest first)
 * for sequential liquidation in multi-company / cascade mergers.
 */
export function getDefunctCompanyQueue(
  participatingCompanyIds: readonly CompanyId[],
  survivingCompanyId: CompanyId,
  companies: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>
): CompanyId[] {
  const uniqueCompanyIds = [...new Set(participatingCompanyIds)];
  return uniqueCompanyIds
    .filter((id) => id !== survivingCompanyId)
    .sort((a, b) => {
      const sizeA = companies[a]?.size ?? 0;
      const sizeB = companies[b]?.size ?? 0;
      return sizeA - sizeB;
    });
}

/**
 * Returns an ordered queue of player IDs who own shares in the defunct company,
 * starting from `activePlayerIndex` and proceeding clockwise around the table.
 */
export function getShareholderQueue(
  defunctCompanyId: CompanyId,
  players: readonly PlayerState[],
  activePlayerIndex: number
): string[] {
  const queue: string[] = [];
  const total = players.length;
  if (total === 0) return queue;

  const normalizedActiveIndex = ((activePlayerIndex % total) + total) % total;

  for (let i = 0; i < total; i++) {
    const idx = (normalizedActiveIndex + i) % total;
    const player = players[idx]!;
    if ((player.shares[defunctCompanyId] ?? 0) > 0) {
      queue.push(player.id);
    }
  }

  return queue;
}
