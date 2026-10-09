import { describe, it, expect } from 'vitest';
import {
  determineSurvivingCompany,
  calculateMergerBonuses,
  applyShareDisposition,
  consolidateMergerTerritory,
  getDefunctCompanyQueue,
  getShareholderQueue,
} from '../merger';
import { createEmptyBoard } from '../board';
import type { CompanyState, PlayerState, CellState } from '../types';

describe('Merger Resolution & Shareholder Payouts', () => {
  it('determines surviving company by largest size', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 10 } as any,
      imperial: { id: 'imperial', size: 8 } as any,
    };
    const survivor = determineSurvivingCompany(['american', 'imperial'], companies);
    expect(survivor.survivorId).toBe('american');
    expect(survivor.hasTie).toBe(false);
  });

  it('detects ties in largest size', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 8 } as any,
      imperial: { id: 'imperial', size: 8 } as any,
    };
    const survivor = determineSurvivingCompany(['american', 'imperial'], companies);
    expect(survivor.hasTie).toBe(true);
    expect(survivor.tiedCompanyIds).toEqual(['american', 'imperial']);
  });

  it('calculates majority and minority bonuses correctly with solo owner', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 5 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 0 }, hand: [], isAi: false },
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
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 2 }, hand: [], isAi: false },
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
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 2 }, hand: [], isAi: false },
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
      isAi: false,
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 10,
    } as any;

    // Player trades 4 american shares for 2 imperial shares, sells 1 american share at $400
    const result = applyShareDisposition(player, 'american', 400, survivor, {
      sell: 1,
      trade: 4,
      hold: 0,
    });

    expect(result.player.shares.american).toBe(0);
    expect(result.player.shares.imperial).toBe(3); // 1 previous + 2 traded
    expect(result.player.cash).toBe(1400); // 1000 + 1 * 400
    expect(result.survivorCompany.availableShares).toBe(8); // 10 - 2
  });

  it('determines survivor correctly in multi-company mergers', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 12 } as any,
      imperial: { id: 'imperial', size: 8 } as any,
      royal: { id: 'royal', size: 8 } as any,
    };
    // 3-way merger: american is largest
    const survivor = determineSurvivingCompany(['american', 'imperial', 'royal'], companies);
    expect(survivor.survivorId).toBe('american');
    expect(survivor.hasTie).toBe(false);

    // 3-way merger with tie for largest between american and imperial
    companies.imperial!.size = 12;
    const tiedSurvivor = determineSurvivingCompany(['american', 'imperial', 'royal'], companies);
    expect(tiedSurvivor.hasTie).toBe(true);
    expect(tiedSurvivor.tiedCompanyIds).toEqual(['american', 'imperial']);

    // Empty list
    const emptySurvivor = determineSurvivingCompany([], companies);
    expect(emptySurvivor.survivorId).toBeNull();
    expect(emptySurvivor.hasTie).toBe(false);
  });

  it('calculates bonuses correctly with ties in 2nd place', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 8 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 4 }, hand: [], isAi: false },
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 4 }, hand: [], isAi: false },
    ];
    // Price = 600. 1st place: 6000. 2nd place pool: 3000 / 2 = 1500 each.
    const bonuses = calculateMergerBonuses('american', 600, players);
    expect(bonuses.p1).toBe(6000);
    expect(bonuses.p2).toBe(1500);
    expect(bonuses.p3).toBe(1500);
  });

  it('calculates bonuses when 3 players tie for 1st place', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 4 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 4 }, hand: [], isAi: false },
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 4 }, hand: [], isAi: false },
      { id: 'p4', name: 'Dave', cash: 1000, shares: { american: 1 }, hand: [], isAi: false },
    ];
    // Price = 600. Combined = 9000 / 3 = 3000 each. Dave gets 0.
    const bonuses = calculateMergerBonuses('american', 600, players);
    expect(bonuses.p1).toBe(3000);
    expect(bonuses.p2).toBe(3000);
    expect(bonuses.p3).toBe(3000);
    expect(bonuses.p4).toBe(0);
  });

  it('calculates zero bonuses when no players own shares', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 0 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 0 }, hand: [], isAi: false },
    ];
    const bonuses = calculateMergerBonuses('american', 500, players);
    expect(bonuses.p1).toBe(0);
    expect(bonuses.p2).toBe(0);
  });

  it('handles share disposition edge cases: hold all and sell all', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Alice',
      cash: 1000,
      shares: { american: 6, imperial: 0 },
      hand: [],
      isAi: false,
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 20,
    } as any;

    // Hold all shares
    const holdResult = applyShareDisposition(player, 'american', 400, survivor, {
      sell: 0,
      trade: 0,
      hold: 6,
    });
    expect(holdResult.player.shares.american).toBe(6);
    expect(holdResult.player.shares.imperial).toBe(0);
    expect(holdResult.player.cash).toBe(1000);
    expect(holdResult.survivorCompany.availableShares).toBe(20);

    // Sell all shares
    const sellResult = applyShareDisposition(player, 'american', 400, survivor, {
      sell: 6,
      trade: 0,
      hold: 0,
    });
    expect(sellResult.player.shares.american).toBe(0);
    expect(sellResult.player.cash).toBe(3400); // 1000 + 6 * 400
    expect(sellResult.survivorCompany.availableShares).toBe(20);
  });

  it('throws descriptive errors on invalid share disposition choices', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Alice',
      cash: 1000,
      shares: { american: 4, imperial: 0 },
      hand: [],
      isAi: false,
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 1, // Only 1 share available
    } as any;

    // Disposing more shares than owned
    expect(() =>
      applyShareDisposition(player, 'american', 400, survivor, {
        sell: 5,
        trade: 0,
        hold: 0,
      })
    ).toThrow(/exceed/i);

    // Negative counts
    expect(() =>
      applyShareDisposition(player, 'american', 400, survivor, {
        sell: -1,
        trade: 4,
        hold: 1,
      })
    ).toThrow(/negative/i);

    // Odd trade count
    expect(() =>
      applyShareDisposition(player, 'american', 400, survivor, {
        sell: 1,
        trade: 3,
        hold: 0,
      })
    ).toThrow(/even/i);

    // Survivor bank exhausted (player wants 2 survivor shares, but bank has only 1)
    expect(() =>
      applyShareDisposition(player, 'american', 400, survivor, {
        sell: 0,
        trade: 4,
        hold: 0,
      })
    ).toThrow(/insufficient/i);
  });

  it('preserves immutability in share disposition', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Alice',
      cash: 1000,
      shares: { american: 4, imperial: 1 },
      hand: [],
      isAi: false,
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 10,
    } as any;

    const result = applyShareDisposition(player, 'american', 500, survivor, {
      sell: 2,
      trade: 2,
      hold: 0,
    });

    expect(player.shares.american).toBe(4);
    expect(player.shares.imperial).toBe(1);
    expect(player.cash).toBe(1000);
    expect(survivor.availableShares).toBe(10);
    expect(result.player).not.toBe(player);
    expect(result.survivorCompany).not.toBe(survivor);
  });

  it('consolidates merger territory on the board', () => {
    const board = createEmptyBoard(10, 12);
    // American company tiles
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![3] = { type: 'company', companyId: 'american' };
    // Imperial company tiles
    board[2]![5] = { type: 'company', companyId: 'imperial' };
    board[2]![6] = { type: 'company', companyId: 'imperial' };

    // Bridge at (2, 4)
    const newBoard = consolidateMergerTerritory(
      board,
      { row: 2, col: 4 },
      'imperial',
      'american'
    );

    // All tiles should now belong to american
    expect(newBoard[2]![2]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[2]![3]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[2]![4]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[2]![5]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[2]![6]).toEqual({ type: 'company', companyId: 'american' });

    // Original board must not be mutated
    expect(board[2]![5]).toEqual({ type: 'company', companyId: 'imperial' });
    expect(board[2]![4]).toEqual({ type: 'empty' });
  });

  it('absorbs contiguous unincorporated tiles during merger territory consolidation', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'company', companyId: 'imperial' };
    // Unincorporated tile touching bridge at (1, 3)
    board[1]![3] = { type: 'unincorporated' };

    const newBoard = consolidateMergerTerritory(
      board,
      { row: 2, col: 3 },
      'imperial',
      'american'
    );

    expect(newBoard[2]![2]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[2]![3]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[2]![4]).toEqual({ type: 'company', companyId: 'american' });
    expect(newBoard[1]![3]).toEqual({ type: 'company', companyId: 'american' });
  });

  it('orders defunct companies smallest size first for cascade mergers', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 15 } as any,
      tower: { id: 'tower', size: 6 } as any,
      imperial: { id: 'imperial', size: 2 } as any,
      royal: { id: 'royal', size: 4 } as any,
    };

    const queue = getDefunctCompanyQueue(
      ['american', 'tower', 'imperial', 'royal'],
      'american',
      companies
    );

    // Survivor 'american' is excluded; remaining are ordered smallest first: imperial (2), royal (4), tower (6)
    expect(queue).toEqual(['imperial', 'royal', 'tower']);
  });

  it('creates shareholder queue starting from active player index and normalizes negative indices', () => {
    const players: PlayerState[] = [
      { id: 'p1', name: 'Alice', cash: 1000, shares: { american: 0 }, hand: [], isAi: false },
      { id: 'p2', name: 'Bob', cash: 1000, shares: { american: 5 }, hand: [], isAi: false },
      { id: 'p3', name: 'Charlie', cash: 1000, shares: { american: 2 }, hand: [], isAi: false },
      { id: 'p4', name: 'Dave', cash: 1000, shares: { american: 3 }, hand: [], isAi: false },
    ];

    // Active player is Charlie (index 2)
    // Order around table: Charlie (p3), Dave (p4), Alice (p1 - 0 shares, skipped), Bob (p2)
    const queue = getShareholderQueue('american', players, 2);
    expect(queue).toEqual(['p3', 'p4', 'p2']);

    // Negative index: -1 wraps to index 3 (Dave) -> Dave (p4), Alice (p1 - skipped), Bob (p2), Charlie (p3)
    const queueNeg = getShareholderQueue('american', players, -1);
    expect(queueNeg).toEqual(['p4', 'p2', 'p3']);
  });

  it('deduplicates company IDs to prevent false ties and duplicates', () => {
    const companies: Record<string, CompanyState> = {
      american: { id: 'american', size: 10 } as any,
      tower: { id: 'tower', size: 5 } as any,
    };

    // Duplicate IDs in determineSurvivingCompany
    const survivor = determineSurvivingCompany(['american', 'american'], companies);
    expect(survivor.survivorId).toBe('american');
    expect(survivor.hasTie).toBe(false);

    // Duplicate IDs in getDefunctCompanyQueue
    const queue = getDefunctCompanyQueue(['american', 'tower', 'tower'], 'american', companies);
    expect(queue).toEqual(['tower']);
  });

  it('automatically holds unallocated shares when hold is omitted', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Alice',
      cash: 1000,
      shares: { american: 5, imperial: 0 },
      hand: [],
      isAi: false,
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 10,
    } as any;

    // Player trades 2 and sells 1, omitting hold. 2 unallocated shares must be retained!
    const result = applyShareDisposition(player, 'american', 400, survivor, {
      sell: 1,
      trade: 2,
    });

    expect(result.player.shares.american).toBe(2); // 5 - (1 + 2) = 2 retained!
    expect(result.player.shares.imperial).toBe(1); // 2:1 trade -> 1 share
    expect(result.player.cash).toBe(1400);
    expect(result.survivorCompany.availableShares).toBe(9);
  });

  it('rejects under-allocation when hold is explicitly provided and rejects non-integers', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Alice',
      cash: 1000,
      shares: { american: 5, imperial: 0 },
      hand: [],
      isAi: false,
    };
    const survivor: CompanyState = {
      id: 'imperial',
      availableShares: 10,
    } as any;

    // Explicit hold that does not equal total owned shares (1 + 2 + 0 = 3 !== 5)
    expect(() =>
      applyShareDisposition(player, 'american', 400, survivor, {
        sell: 1,
        trade: 2,
        hold: 0,
      })
    ).toThrow(/equal player's held shares/i);

    // Non-integer counts
    expect(() =>
      applyShareDisposition(player, 'american', 400, survivor, {
        sell: 1.5,
        trade: 2,
        hold: 1.5,
      })
    ).toThrow(/integer/i);
  });
});
