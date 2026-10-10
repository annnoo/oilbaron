import { describe, it, expect } from 'vitest';
import {
  getBestTileMove,
  getBestFoundedCompany,
  getBestMergerSurvivor,
  getBestSharePurchases,
  getBestMergerDisposition,
  simulateAiTurn,
  playFullGameAi,
} from '../ai';
import {
  createInitialGameState,
  DEFAULT_CONFIG,
  MODERN_BALANCED_CONFIG,
} from '../config';

describe('AI Player Decision Engine', () => {
  it('selects a valid playable tile from its hand', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    const move = getBestTileMove(state, 0);
    expect(move).toBeDefined();
    expect(state.players[0]!.hand).toContainEqual(move);
  });

  it('returns null when player has no playable tiles', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    state.players[0]!.hand = [];
    const move = getBestTileMove(state, 0);
    expect(move).toBeNull();
  });

  it('prioritizes founding over isolated tile placement', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    // Place an unincorporated chit at (3, 3)
    state.board[3]![3] = { type: 'unincorporated' };

    // Hand contains an isolated tile (0, 0) and a founding tile (3, 4)
    state.players[0]!.hand = [
      { row: 0, col: 0 },
      { row: 3, col: 4 },
    ];

    const move = getBestTileMove(state, 0);
    expect(move).toEqual({ row: 3, col: 4 });
  });

  it('prioritizes expanding owned company over unowned company', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    // American at (2, 2)
    state.board[2]![2] = { type: 'company', companyId: 'american' };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 2,
    };

    // Imperial at (7, 7)
    state.board[7]![7] = { type: 'company', companyId: 'imperial' };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 2,
    };

    // Bot owns shares in American but none in Imperial
    state.players[0]!.shares.american = 5;
    state.players[0]!.shares.imperial = 0;

    // Hand has tile (2, 3) expanding American, and (7, 8) expanding Imperial
    state.players[0]!.hand = [
      { row: 2, col: 3 },
      { row: 7, col: 8 },
    ];

    const move = getBestTileMove(state, 0);
    expect(move).toEqual({ row: 2, col: 3 });
  });

  it('chooses the best available company to found', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [{ name: 'Bot 1', isAi: true }],
      100
    );

    const chosen = getBestFoundedCompany(state, ['american', 'imperial', 'royal']);
    expect(['american', 'imperial', 'royal']).toContain(chosen);
    // American is low tier ($200 base price) vs Royal high tier ($400 base price)
    expect(chosen).toBe('american');
  });

  it('prefers founding a company where player already holds legacy shares', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [{ name: 'Bot 1', isAi: true }],
      100
    );

    // Player holds 6 legacy shares in royal
    state.players[0]!.shares.royal = 6;

    const chosen = getBestFoundedCompany(state, ['american', 'imperial', 'royal']);
    expect(chosen).toBe('royal');
  });

  it('selects best merger survivor among tied contenders', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    // Tied companies American and Imperial of equal size
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 6,
    };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 6,
    };

    // Bot 1 owns 8 shares of American and 0 shares of Imperial
    state.players[0]!.shares.american = 8;
    state.players[0]!.shares.imperial = 0;

    const survivor = getBestMergerSurvivor(state, ['american', 'imperial']);
    expect(['american', 'imperial']).toContain(survivor);
  });

  it('selects share purchases within budget and constraints', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 4,
    };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 5,
    };

    const purchases = getBestSharePurchases(state, 0);
    expect(purchases.length).toBeGreaterThan(0);
    const totalCount = purchases.reduce((sum, p) => sum + p.count, 0);
    expect(totalCount).toBeLessThanOrEqual(DEFAULT_CONFIG.maxSharesPerPurchase);
  });

  it('returns empty share purchases when cash is 0 or no companies active', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [{ name: 'Bot 1', isAi: true }],
      100
    );

    // No companies active
    expect(getBestSharePurchases(state, 0)).toEqual([]);

    // With active company but 0 cash
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 4,
    };
    state.players[0]!.cash = 0;
    expect(getBestSharePurchases(state, 0)).toEqual([]);
  });

  it('disposes merger shares via 2-for-1 trade when survivor is valuable', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [{ name: 'Bot 1', isAi: true }],
      100
    );

    // Defunct price is $200 (low tier size 2), Survivor price is $700 (size 5 high tier)
    // 2 * defunct ($400) < survivor ($700), so 2-for-1 trading is optimal
    state.companies.royal = {
      ...state.companies.royal!,
      isActive: true,
      size: 5,
      availableShares: 10,
    };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 2,
    };
    state.players[0]!.shares.american = 4;

    state.pendingMerger = {
      coord: { row: 2, col: 2 },
      participatingCompanyIds: ['royal', 'american'],
      survivingCompanyId: 'royal',
      defunctCompanyQueue: ['american'],
      currentDefunctCompanyId: 'american',
      shareholderQueue: ['p1'],
      currentShareholderIndex: 0,
      preMergerPrices: { american: 200, royal: 700 },
    };

    const disposition = getBestMergerDisposition(state, 0, 'american');
    expect(disposition.trade).toBe(4);
    expect(disposition.sell).toBe(0);
    expect(disposition.hold).toBe(0);
  });

  it('sells shares during merger when survivor shares are depleted or trade unfavorable', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [{ name: 'Bot 1', isAi: true }],
      100
    );

    // Defunct price is $600 (American size 6), Survivor price is $400 (Imperial size 3)
    // 2 * defunct ($1200) > survivor ($400), so selling gives 3x more cash!
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 3,
      availableShares: 10,
    };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 6,
    };
    state.players[0]!.shares.american = 4;

    state.pendingMerger = {
      coord: { row: 2, col: 2 },
      participatingCompanyIds: ['imperial', 'american'],
      survivingCompanyId: 'imperial',
      defunctCompanyQueue: ['american'],
      currentDefunctCompanyId: 'american',
      shareholderQueue: ['p1'],
      currentShareholderIndex: 0,
      preMergerPrices: { american: 600, imperial: 400 },
    };

    const disposition = getBestMergerDisposition(state, 0, 'american');
    expect(disposition.trade).toBe(0);
    expect(disposition.sell).toBe(4);
    expect(disposition.hold).toBe(0);
  });

  it('simulates a single AI turn advancing phase or active player', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'Bot 1', isAi: true },
        { name: 'Bot 2', isAi: true },
      ],
      100
    );

    const nextState = simulateAiTurn(state);
    expect(nextState.activePlayerIndex).toBe(1);
    expect(nextState.history.length).toBeGreaterThan(0);
  });

  it('plays a complete 4-player AI game to completion without errors (6x6 board)', () => {
    const config = { ...DEFAULT_CONFIG, boardRows: 6, boardCols: 6 }; // smaller board for quick test
    const finalState = playFullGameAi(config, 4, 9999, 500);

    expect(finalState.phase).toBe('game_over');
    expect(finalState.winnerPlayerId).toBeDefined();
    expect(finalState.players[0]!.cash).toBeGreaterThan(0);
  });

  it('plays a complete 2-player AI game with modern balanced ruleset', () => {
    const config = { ...MODERN_BALANCED_CONFIG, boardRows: 6, boardCols: 6 };
    const finalState = playFullGameAi(config, 2, 1234, 500);

    expect(finalState.phase).toBe('game_over');
    expect(finalState.winnerPlayerId).toBeDefined();
    expect(finalState.players).toHaveLength(2);
    expect(finalState.players.some((p) => p.cash > 0)).toBe(true);
  });

  it('plays a complete full-scale game on standard 10x12 board with default config', () => {
    const finalState = playFullGameAi(DEFAULT_CONFIG, 3, 4242, 500);

    expect(finalState.phase).toBe('game_over');
    expect(finalState.winnerPlayerId).toBeDefined();
    expect(finalState.players).toHaveLength(3);
    for (const player of finalState.players) {
      expect(player.cash).toBeGreaterThan(0);
    }
  });

  it('plays an AI match with payDividends enabled, accumulating dividends across turns', () => {
    const config = { ...DEFAULT_CONFIG, boardRows: 6, boardCols: 6, payDividends: true };
    const finalState = playFullGameAi(config, 2, 8888, 500);

    expect(finalState.phase).toBe('game_over');
    expect(finalState.winnerPlayerId).toBeDefined();
    expect(finalState.players.every((p) => p.cash > 0)).toBe(true);
  });

  describe('AI Difficulty Heuristics (Dumb AI vs Smart AI)', () => {
    it('Smart AI deterministically selects optimal founding tile over isolated tile', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'Smart Bot', isAi: true, aiDifficulty: 'smart' },
          { name: 'Opponent', isAi: true },
        ],
        100
      );
      state.board[3]![3] = { type: 'unincorporated' };
      state.players[0]!.hand = [
        { row: 0, col: 0 },
        { row: 3, col: 4 },
      ];

      const move = getBestTileMove(state, 0, 'smart');
      expect(move).toEqual({ row: 3, col: 4 });
    });

    it('Dumb AI introduces noise causing suboptimal isolated tile choice over founding tile', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'Dumb Bot', isAi: true, aiDifficulty: 'dumb' },
          { name: 'Opponent', isAi: true },
        ],
        100
      );
      state.board[3]![3] = { type: 'unincorporated' };
      state.players[0]!.hand = [
        { row: 0, col: 0 },
        { row: 3, col: 4 },
      ];

      // RNG favoring row 0 candidate with maximum positive noise
      const move = getBestTileMove(state, 0, 'dumb', (candidate) => {
        return candidate.row === 0 ? 0.99 : 0.01;
      });
      expect(move).toEqual({ row: 0, col: 0 });
    });

    it('Smart AI defends majority by purchasing contested company shares', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'Smart Bot', isAi: true, aiDifficulty: 'smart' },
          { name: 'Opponent', isAi: true },
        ],
        100
      );
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 4,
        availableShares: 10,
      };
      state.companies.imperial = {
        ...state.companies.imperial!,
        isActive: true,
        size: 4,
        availableShares: 10,
      };
      state.players[0]!.shares.american = 4;
      state.players[1]!.shares.american = 5;
      state.players[0]!.shares.imperial = 0;
      state.players[1]!.shares.imperial = 0;

      const purchases = getBestSharePurchases(state, 0, 'smart');
      expect(purchases.length).toBeGreaterThan(0);
      const americanPurchase = purchases.find((p) => p.companyId === 'american');
      expect(americanPurchase).toBeDefined();
      expect(americanPurchase!.count).toBeGreaterThan(0);
    });

    it('Dumb AI purchases random affordable shares without majority defense strategy', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'Dumb Bot', isAi: true, aiDifficulty: 'dumb' },
          { name: 'Opponent', isAi: true },
        ],
        100
      );
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 4,
        availableShares: 10,
      };
      state.companies.imperial = {
        ...state.companies.imperial!,
        isActive: true,
        size: 4,
        availableShares: 10,
      };
      state.players[0]!.shares.american = 4;
      state.players[1]!.shares.american = 5;

      const mockRng = () => 0.99;
      const purchases = getBestSharePurchases(state, 0, 'dumb', mockRng);
      expect(purchases.length).toBeGreaterThan(0);
      const totalCount = purchases.reduce((sum, p) => sum + p.count, 0);
      expect(totalCount).toBeLessThanOrEqual(DEFAULT_CONFIG.maxSharesPerPurchase);
      expect(purchases.some((p) => p.companyId === 'imperial')).toBe(true);
    });

    it('Smart AI trades merger shares 2-for-1 when favorable and sells when unfavorable', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [{ name: 'Smart Bot', isAi: true, aiDifficulty: 'smart' }],
        100
      );
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 5,
        availableShares: 10,
      };
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2,
      };
      state.players[0]!.shares.american = 4;
      state.pendingMerger = {
        coord: { row: 2, col: 2 },
        participatingCompanyIds: ['royal', 'american'],
        survivingCompanyId: 'royal',
        defunctCompanyQueue: ['american'],
        currentDefunctCompanyId: 'american',
        shareholderQueue: ['p1'],
        currentShareholderIndex: 0,
        preMergerPrices: { american: 200, royal: 700 },
      };

      const smartDisp = getBestMergerDisposition(state, 0, 'american', 'smart');
      expect(smartDisp).toEqual({ trade: 4, sell: 0, hold: 0 });
    });

    it('Dumb AI can make suboptimal merger disposition decisions (e.g. holding or selling)', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [{ name: 'Dumb Bot', isAi: true, aiDifficulty: 'dumb' }],
        100
      );
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 5,
        availableShares: 10,
      };
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2,
      };
      state.players[0]!.shares.american = 4;
      state.pendingMerger = {
        coord: { row: 2, col: 2 },
        participatingCompanyIds: ['royal', 'american'],
        survivingCompanyId: 'royal',
        defunctCompanyQueue: ['american'],
        currentDefunctCompanyId: 'american',
        shareholderQueue: ['p1'],
        currentShareholderIndex: 0,
        preMergerPrices: { american: 200, royal: 700 },
      };

      const dumbHold = getBestMergerDisposition(state, 0, 'american', 'dumb', () => 0.1);
      expect(dumbHold.hold).toBeGreaterThan(0);

      const dumbSell = getBestMergerDisposition(state, 0, 'american', 'dumb', () => 0.5);
      expect(dumbSell.sell).toBe(4);
      expect(dumbSell.trade).toBe(0);
    });

    it('simulateAiTurn respects player aiDifficulty and executes turn', () => {
      const state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'Dumb Bot', isAi: true, aiDifficulty: 'dumb' },
          { name: 'Smart Bot', isAi: true, aiDifficulty: 'smart' },
        ],
        100
      );

      expect(state.players[0]!.aiDifficulty).toBe('dumb');
      expect(state.players[1]!.aiDifficulty).toBe('smart');

      const nextState = simulateAiTurn(state);
      expect(nextState.activePlayerIndex).toBe(1);
    });

    it('plays a complete game with all Dumb AI players to completion', () => {
      const config = { ...DEFAULT_CONFIG, boardRows: 6, boardCols: 6 };
      const finalState = playFullGameAi(config, 3, 5555, 500, 'dumb');

      expect(finalState.phase).toBe('game_over');
      expect(finalState.winnerPlayerId).toBeDefined();
      expect(finalState.players.every((p) => p.aiDifficulty === 'dumb')).toBe(true);
    });

    it('plays a complete game with mixed Dumb AI and Smart AI players', () => {
      const config = { ...DEFAULT_CONFIG, boardRows: 6, boardCols: 6 };
      let state = createInitialGameState(
        config,
        [
          { name: 'Dumb 1', isAi: true, aiDifficulty: 'dumb' },
          { name: 'Smart 1', isAi: true, aiDifficulty: 'smart' },
        ],
        7777
      );

      let turns = 0;
      while (state.phase !== 'game_over' && turns < 500) {
        state = simulateAiTurn(state);
        turns++;
      }

      expect(state.phase).toBe('game_over');
      expect(state.winnerPlayerId).toBeDefined();
    });
  });
});

