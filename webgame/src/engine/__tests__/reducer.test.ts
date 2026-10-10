import { describe, it, expect } from 'vitest';
import { gameReducer, isGameOver } from '../reducer';
import {
  createInitialGameState,
  DEFAULT_CONFIG,
  MODERN_BALANCED_CONFIG,
} from '../config';
import type { GameState } from '../types';

describe('Game State Reducer & Turn Flow', () => {
  it('places isolated tile and transitions to buy_shares phase', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    const tile = state.players[0]!.hand[0]!;
    const nextState = gameReducer(state, { type: 'PLACE_TILE', coord: tile });

    expect(nextState.board[tile.row]![tile.col]!.type).toBe('unincorporated');
    expect(nextState.phase).toBe('buy_shares');
    expect(nextState.players[0]!.hand).not.toContainEqual(tile);
    expect(nextState.history).toHaveLength(1);
    expect(nextState.history[0]).toEqual({ type: 'PLACE_TILE', coord: tile });
  });

  it('triggers founding company and awards founder shares', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // Force place an unincorporated tile at (2, 2)
    state.board[2]![2] = { type: 'unincorporated' };
    state.players[0]!.hand[0] = { row: 2, col: 3 };

    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 3 } });
    expect(state.phase).toBe('choose_founding_company');
    expect(state.pendingFounding).toBeDefined();
    expect(state.pendingFounding?.availableCompanyIds).toContain('american');

    state = gameReducer(state, {
      type: 'CHOOSE_FOUNDED_COMPANY',
      companyId: 'american',
    });
    expect(state.phase).toBe('buy_shares');
    expect(state.pendingFounding).toBeNull();
    expect(state.companies.american?.isActive).toBe(true);
    expect(state.companies.american?.size).toBe(2);
    // Founder bonus in classic is 10 shares
    expect(state.players[0]!.shares.american).toBe(10);
    expect(state.companies.american?.availableShares).toBe(15);
  });

  it('completes turn, draws replacement tile, and advances active player', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    const tile = state.players[0]!.hand[0]!;
    state = gameReducer(state, { type: 'PLACE_TILE', coord: tile });
    state = gameReducer(state, { type: 'BUY_SHARES', purchases: [] });
    state = gameReducer(state, { type: 'END_TURN' });

    expect(state.activePlayerIndex).toBe(1);
    expect(state.phase).toBe('place_tile');
    expect(state.players[0]!.hand.length).toBe(6); // Replenished
  });

  it('handles company expansion and absorbs unincorporated neighbors', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // Set up active company 'american' of size 2 at (2, 2) and (2, 3)
    state.board[2]![2] = { type: 'company', companyId: 'american' };
    state.board[2]![3] = { type: 'company', companyId: 'american' };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 2,
    };

    // Unincorporated tile at (2, 5)
    state.board[2]![5] = { type: 'unincorporated' };

    // Player places tile at (2, 4) bridging the company and unincorporated tile
    state.players[0]!.hand[0] = { row: 2, col: 4 };

    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 4 } });
    expect(state.phase).toBe('buy_shares');
    expect(state.board[2]![4]!.type).toBe('company');
    expect(state.board[2]![5]!.type).toBe('company');
    expect(state.companies.american?.size).toBe(4);
  });

  it('executes share purchases deducting cash and decrementing bank', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 4, // Price at size 4: $200 + 2 * $100 = $400
    };

    const tile = state.players[0]!.hand[0]!;
    state = gameReducer(state, { type: 'PLACE_TILE', coord: tile });

    state = gameReducer(state, {
      type: 'BUY_SHARES',
      purchases: [{ companyId: 'american', count: 3 }],
    });

    // 3 shares at $400 = $1200. Cash was $6000 -> $4800
    expect(state.players[0]!.cash).toBe(4800);
    expect(state.players[0]!.shares.american).toBe(3);
    expect(state.companies.american?.availableShares).toBe(22);
    expect(state.turnPurchasedSharesCount).toBe(3);

    // Attempting to buy another share exceeds limit of 3
    expect(() =>
      gameReducer(state, {
        type: 'BUY_SHARES',
        purchases: [{ companyId: 'american', count: 1 }],
      })
    ).toThrow();
  });

  it('resolves a complete merger lifecycle with bonuses and share disposition', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American: size 4 (cells 2,0; 2,1; 2,2; 2,3)
    for (let c = 0; c <= 3; c++) {
      state.board[2]![c] = { type: 'company', companyId: 'american' };
    }
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 4,
      availableShares: 20,
    };

    // Imperial: size 2 (cells 2,5; 2,6)
    state.board[2]![5] = { type: 'company', companyId: 'imperial' };
    state.board[2]![6] = { type: 'company', companyId: 'imperial' };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 2,
      availableShares: 15,
    };

    // P1 holds 4 Imperial shares, P2 holds 2 Imperial shares
    state.players[0]!.shares.imperial = 4;
    state.players[1]!.shares.imperial = 2;

    // Imperial size 2 basePrice = $300.
    // Majority bonus = 10 * 300 = $3000 to P1.
    // Minority bonus = 5 * 300 = $1500 to P2.

    state.players[0]!.hand[0] = { row: 2, col: 4 };

    // Place merger tile at (2, 4)
    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 4 } });

    // Since American (size 4) > Imperial (size 2), American survives automatically!
    // Bonuses are paid immediately when entering disposition
    expect(state.phase).toBe('dispose_merger_shares');
    expect(state.players[0]!.cash).toBe(6000 + 3000); // 9000
    expect(state.players[1]!.cash).toBe(6000 + 1500); // 7500

    expect(state.pendingMerger?.survivingCompanyId).toBe('american');
    expect(state.pendingMerger?.currentDefunctCompanyId).toBe('imperial');

    // First shareholder in queue is active player P1: trade 4 Imperial for 2 American
    state = gameReducer(state, {
      type: 'DISPOSE_MERGER_SHARES',
      choices: { sell: 0, trade: 4, hold: 0 },
    });

    expect(state.players[0]!.shares.imperial).toBe(0);
    expect(state.players[0]!.shares.american).toBe(2);
    expect(state.companies.american?.availableShares).toBe(18);

    // Second shareholder in queue is P2: sell 2 Imperial at $300 = +$600
    expect(state.phase).toBe('dispose_merger_shares');
    state = gameReducer(state, {
      type: 'DISPOSE_MERGER_SHARES',
      choices: { sell: 2, trade: 0, hold: 0 },
    });

    expect(state.players[1]!.shares.imperial).toBe(0);
    expect(state.players[1]!.cash).toBe(7500 + 600); // 8100

    // Merger complete! Phase transitions to buy_shares
    expect(state.phase).toBe('buy_shares');
    expect(state.pendingMerger).toBeNull();
    expect(state.companies.imperial?.isActive).toBe(false);
    expect(state.companies.imperial?.size).toBe(0);

    // Survivor (American) has consolidated all tiles: 4 original + 2 imperial + 1 bridge = 7
    expect(state.companies.american?.size).toBe(7);
    expect(state.board[2]![4]!.type).toBe('company');
    expect(state.board[2]![5]!.type).toBe('company');
    expect(state.board[2]![6]!.type).toBe('company');
  });

  it('prompts survivor choice on merger tie', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American: size 2 at (2, 2) and (2, 3)
    state.board[2]![2] = { type: 'company', companyId: 'american' };
    state.board[2]![3] = { type: 'company', companyId: 'american' };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 2,
    };

    // Imperial: size 2 at (2, 5) and (2, 6)
    state.board[2]![5] = { type: 'company', companyId: 'imperial' };
    state.board[2]![6] = { type: 'company', companyId: 'imperial' };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 2,
    };

    // P1 holds 2 shares in Imperial
    state.players[0]!.shares.imperial = 2;

    state.players[0]!.hand[0] = { row: 2, col: 4 };

    // Place merger tile at (2, 4)
    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 4 } });
    expect(state.phase).toBe('choose_merger_survivor');

    // Player chooses American as survivor
    state = gameReducer(state, {
      type: 'CHOOSE_MERGER_SURVIVOR',
      survivorCompanyId: 'american',
    });

    expect(state.phase).toBe('dispose_merger_shares');
    expect(state.pendingMerger?.survivingCompanyId).toBe('american');
    expect(state.pendingMerger?.currentDefunctCompanyId).toBe('imperial');
  });

  it('allows discarding dead tiles and draws replacements', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // Make American safe (size 11)
    state.board[2]![2] = { type: 'company', companyId: 'american' };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      isSafe: true,
      size: 11,
    };

    // Make Imperial safe (size 11)
    state.board[2]![4] = { type: 'company', companyId: 'imperial' };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      isSafe: true,
      size: 11,
    };

    // Tile at (2, 3) would bridge two safe companies -> dead tile!
    const deadTile = { row: 2, col: 3 };
    state.players[0]!.hand[0] = deadTile;

    const initialPoolCount = state.tilePool.length;

    // Attempting to place dead tile throws error
    expect(() =>
      gameReducer(state, { type: 'PLACE_TILE', coord: deadTile })
    ).toThrow();

    // Discarding dead tile succeeds
    state = gameReducer(state, { type: 'DISCARD_DEAD_TILE', coord: deadTile });
    expect(state.players[0]!.hand).not.toContainEqual(deadTile);
    expect(state.players[0]!.hand.length).toBe(6);
    expect(state.tilePool.length).toBe(initialPoolCount - 1);
    expect(state.phase).toBe('place_tile');
  });

  it('awards modern balanced founder bonus of 1 share', () => {
    let state = createInitialGameState(
      MODERN_BALANCED_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    state.board[2]![2] = { type: 'unincorporated' };
    state.players[0]!.hand[0] = { row: 2, col: 3 };

    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 3 } });
    state = gameReducer(state, {
      type: 'CHOOSE_FOUNDED_COMPANY',
      companyId: 'american',
    });

    // Modern balanced bonus is 1 share
    expect(state.players[0]!.shares.american).toBe(1);
    expect(state.companies.american?.availableShares).toBe(24);
  });

  it('triggers game over and full portfolio liquidation when all active companies are safe', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // Active safe company: American, size 11 ($1100 per share)
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[1]![1] = { type: 'company', companyId: 'american' };

    // P1 holds 5 shares, P2 holds 2 shares
    state.players[0]!.shares.american = 5;
    state.players[1]!.shares.american = 2;

    const tile = state.players[0]!.hand[0]!;
    state = gameReducer(state, { type: 'PLACE_TILE', coord: tile });
    state = gameReducer(state, { type: 'BUY_SHARES', purchases: [] });
    state = gameReducer(state, { type: 'END_TURN' });

    expect(state.phase).toBe('game_over');
    expect(state.winnerPlayerId).toBe('p1');

    // Share price = $1100.
    // Majority bonus = $11000 to P1. Minority bonus = $5500 to P2.
    // P1 stock liquidation = 5 * $1100 = $5500.
    // P2 stock liquidation = 2 * $1100 = $2200.
    // P1 final cash: 6000 + 11000 + 5500 = 22500.
    // P2 final cash: 6000 + 5500 + 2200 = 13700.
    expect(state.players[0]!.cash).toBe(22500);
    expect(state.players[1]!.cash).toBe(13700);

    // All active company shares should be liquidated (set to 0)
    expect(state.players[0]!.shares.american).toBe(0);
    expect(state.players[1]!.shares.american).toBe(0);
  });

  it('detects game over when any company reaches size >= 41', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 41,
    };

    expect(isGameOver(state)).toBe(true);
  });

  it('resolves a 3-company cascade merger sequentially', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American: size 4 at (2,0), (2,1), (2,2), (2,3)
    for (let c = 0; c <= 3; c++) {
      state.board[2]![c] = { type: 'company', companyId: 'american' };
    }
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 4,
    };

    // Imperial: size 3 at (0,4), (1,4), (1,5)
    state.board[0]![4] = { type: 'company', companyId: 'imperial' };
    state.board[1]![4] = { type: 'company', companyId: 'imperial' };
    state.board[1]![5] = { type: 'company', companyId: 'imperial' };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 3,
    };

    // Supra: size 2 at (3,4), (4,4)
    state.board[3]![4] = { type: 'company', companyId: 'supra' };
    state.board[4]![4] = { type: 'company', companyId: 'supra' };
    state.companies.supra = {
      ...state.companies.supra!,
      isActive: true,
      size: 2,
    };

    // P1 holds 2 Supra shares; P2 holds 2 Imperial shares
    state.players[0]!.shares.supra = 2;
    state.players[1]!.shares.imperial = 2;

    // Bridge tile at (2, 4) connects American (W), Imperial (N), and Supra (S)
    state.players[0]!.hand[0] = { row: 2, col: 4 };

    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 4 } });

    // American (size 4) survives.
    // Defunct queue is [supra (size 2), imperial (size 3)] (smallest first).
    // Supra is first defunct company:
    expect(state.phase).toBe('dispose_merger_shares');
    expect(state.pendingMerger?.currentDefunctCompanyId).toBe('supra');
    expect(state.pendingMerger?.survivingCompanyId).toBe('american');

    // P1 disposes Supra shares (sell 2)
    state = gameReducer(state, {
      type: 'DISPOSE_MERGER_SHARES',
      choices: { sell: 2, trade: 0, hold: 0 },
    });

    // Now moves to next defunct company: Imperial!
    expect(state.phase).toBe('dispose_merger_shares');
    expect(state.pendingMerger?.currentDefunctCompanyId).toBe('imperial');

    // P2 disposes Imperial shares (trade 2 for 1 American)
    state = gameReducer(state, {
      type: 'DISPOSE_MERGER_SHARES',
      choices: { sell: 0, trade: 2, hold: 0 },
    });

    // All defunct companies resolved!
    expect(state.phase).toBe('buy_shares');
    expect(state.pendingMerger).toBeNull();
    expect(state.companies.supra?.isActive).toBe(false);
    expect(state.companies.imperial?.isActive).toBe(false);
    expect(state.companies.american?.size).toBe(4 + 3 + 2 + 1); // 10
  });

  it('skips disposition if no players hold defunct company shares', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American: size 3; Imperial: size 2
    state.board[2]![2] = { type: 'company', companyId: 'american' };
    state.board[2]![3] = { type: 'company', companyId: 'american' };
    state.board[1]![2] = { type: 'company', companyId: 'american' };
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 3,
    };

    state.board[2]![5] = { type: 'company', companyId: 'imperial' };
    state.board[2]![6] = { type: 'company', companyId: 'imperial' };
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 2,
    };

    // Neither player holds Imperial shares
    state.players[0]!.shares.imperial = 0;
    state.players[1]!.shares.imperial = 0;

    state.players[0]!.hand[0] = { row: 2, col: 4 };

    // Place merger tile at (2, 4)
    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 4 } });

    // Directly transitions to buy_shares because no shareholders to dispose
    expect(state.phase).toBe('buy_shares');
    expect(state.pendingMerger).toBeNull();
    expect(state.companies.imperial?.isActive).toBe(false);
    expect(state.companies.american?.size).toBe(6);
  });

  it('rejects invalid action for current phase', () => {
    const state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // Initial phase is 'place_tile' -> BUY_SHARES is invalid
    expect(() =>
      gameReducer(state, { type: 'BUY_SHARES', purchases: [] })
    ).toThrow(/Cannot buy shares during phase 'place_tile'/);

    // END_TURN is also invalid during 'place_tile'
    expect(() => gameReducer(state, { type: 'END_TURN' })).toThrow(
      /Cannot end turn during phase 'place_tile'/
    );
  });

  it('triggers game over when tile pool is empty and all player tiles are dead', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American is safe (size 11)
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[2]![2] = { type: 'company', companyId: 'american' };

    // Imperial is safe (size 11)
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[2]![4] = { type: 'company', companyId: 'imperial' };

    // Empty tile pool
    state.tilePool = [];

    // All tiles in hands are dead (between safe companies)
    state.players[0]!.hand = [{ row: 2, col: 3 }];
    state.players[1]!.hand = [{ row: 2, col: 3 }];

    expect(isGameOver(state)).toBe(true);
  });

  it('automatically transitions next player with no legal moves to buy_shares so they can finish turn', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American is safe (size 11)
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[2]![2] = { type: 'company', companyId: 'american' };

    // Imperial is safe (size 11)
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[2]![4] = { type: 'company', companyId: 'imperial' };

    // Supra is active but NOT safe (size 5), so all-safe game-over doesn't trigger
    state.companies.supra = {
      ...state.companies.supra!,
      isActive: true,
      isSafe: false,
      size: 5,
    };
    state.board[8]![8] = { type: 'company', companyId: 'supra' };

    // P1 has 2 playable isolated tiles
    state.players[0]!.hand = [{ row: 0, col: 0 }, { row: 9, col: 9 }];

    // P2 has only dead tiles (bridging safe companies) and empty tile pool
    state.players[1]!.hand = [{ row: 2, col: 3 }];
    state.tilePool = [];

    // P1 plays first tile and ends turn
    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 0, col: 0 } });
    state = gameReducer(state, { type: 'END_TURN' });

    // Turn advances to P2, but P2 has no playable tiles!
    // Since P1 still holds a playable tile, the game is not over yet,
    // so P2's phase automatically transitions to 'buy_shares'
    expect(state.activePlayerIndex).toBe(1);
    expect(state.phase).toBe('buy_shares');

    // P2 can complete their turn by ending it
    state = gameReducer(state, { type: 'END_TURN' });

    // Turn advances back to P1 who still has a playable tile
    expect(state.activePlayerIndex).toBe(0);
    expect(state.phase).toBe('place_tile');

    // P1 plays their last tile and ends turn
    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 9, col: 9 } });
    state = gameReducer(state, { type: 'END_TURN' });

    // Now neither P1 nor P2 has playable tiles -> game over!
    expect(state.phase).toBe('game_over');
  });

  it('rejects choosing a non-tied smaller company as merger survivor in CHOOSE_MERGER_SURVIVOR', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American: size 4
    for (let c = 0; c <= 3; c++) {
      state.board[2]![c] = { type: 'company', companyId: 'american' };
    }
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      size: 4,
    };

    // Imperial: size 4 (tied with American)
    for (let r = 0; r <= 3; r++) {
      state.board[r]![5] = { type: 'company', companyId: 'imperial' };
    }
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      size: 4,
    };

    // Supra: size 2 (smaller, non-tied)
    state.board[3]![4] = { type: 'company', companyId: 'supra' };
    state.board[4]![4] = { type: 'company', companyId: 'supra' };
    state.companies.supra = {
      ...state.companies.supra!,
      isActive: true,
      size: 2,
    };

    // P1 holds shares in American and Supra
    state.players[0]!.shares.american = 2;
    state.players[0]!.shares.supra = 2;

    // Bridge tile at (2, 4) connects American, Imperial, and Supra
    state.players[0]!.hand[0] = { row: 2, col: 4 };
    state = gameReducer(state, { type: 'PLACE_TILE', coord: { row: 2, col: 4 } });

    expect(state.phase).toBe('choose_merger_survivor');

    // Choosing Supra (size 2) must fail because American and Imperial are tied for max size 4
    expect(() =>
      gameReducer(state, {
        type: 'CHOOSE_MERGER_SURVIVOR',
        survivorCompanyId: 'supra',
      })
    ).toThrow(/not an eligible survivor among tied contenders/);

    // Choosing Imperial (size 4) succeeds
    state = gameReducer(state, {
      type: 'CHOOSE_MERGER_SURVIVOR',
      survivorCompanyId: 'imperial',
    });
    expect(state.phase).toBe('dispose_merger_shares');
    expect(state.pendingMerger?.survivingCompanyId).toBe('imperial');
  });

  it('transitions to buy_shares or triggers game_over when discarding dead tile with exhausted pool', () => {
    let state = createInitialGameState(
      DEFAULT_CONFIG,
      [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ],
      42
    );

    // American is safe (size 11)
    state.companies.american = {
      ...state.companies.american!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[2]![2] = { type: 'company', companyId: 'american' };

    // Imperial is safe (size 11)
    state.companies.imperial = {
      ...state.companies.imperial!,
      isActive: true,
      isSafe: true,
      size: 11,
    };
    state.board[2]![4] = { type: 'company', companyId: 'imperial' };

    // Supra is active but NOT safe (size 5)
    state.companies.supra = {
      ...state.companies.supra!,
      isActive: true,
      isSafe: false,
      size: 5,
    };
    state.board[8]![8] = { type: 'company', companyId: 'supra' };

    state.tilePool = [];

    // P1 holds dead tile at (2, 3)
    const deadTile = { row: 2, col: 3 };
    state.players[0]!.hand = [deadTile];

    // Case A: P2 still has a playable tile at (0, 0)
    state.players[1]!.hand = [{ row: 0, col: 0 }];

    // P1 discards dead tile -> pool is empty, no playable tiles remain in P1's hand
    // Since P2 still has a playable tile, game is not over, so P1 transitions to 'buy_shares'
    let stateA = gameReducer(state, { type: 'DISCARD_DEAD_TILE', coord: deadTile });
    expect(stateA.phase).toBe('buy_shares');
    expect(stateA.players[0]!.hand).toHaveLength(0);

    // Case B: Neither player has playable tiles (P2 also holds only dead tile)
    let stateBSetup = {
      ...state,
      players: [
        { ...state.players[0]!, hand: [deadTile] },
        { ...state.players[1]!, hand: [deadTile] },
      ],
    };
    let stateB = gameReducer(stateBSetup, { type: 'DISCARD_DEAD_TILE', coord: deadTile });
    expect(stateB.phase).toBe('game_over');
    expect(stateB.winnerPlayerId).toBeDefined();
  });

  describe('Share Selling and Dividend Lifecycle in Reducer', () => {
    it('executes SELL_SHARES during buy_shares phase, updating cash and bank inventory', () => {
      let state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      // Set up American as active of size 2 ($200)
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2,
        availableShares: 15,
      };
      // Give P1 5 shares of American and $1000 cash
      state.players[0]!.shares.american = 5;
      state.players[0]!.cash = 1000;
      state.phase = 'buy_shares';

      state = gameReducer(state, {
        type: 'SELL_SHARES',
        sales: [{ companyId: 'american', count: 3 }],
      });

      expect(state.players[0]!.cash).toBe(1000 + 3 * 200); // $1600
      expect(state.players[0]!.shares.american).toBe(2);
      expect(state.companies.american?.availableShares).toBe(18); // 15 + 3
      expect(state.turnSoldShares.american).toBe(3);
      expect(state.history[state.history.length - 1]).toEqual({
        type: 'SELL_SHARES',
        sales: [{ companyId: 'american', count: 3 }],
      });
    });

    it('rejects SELL_SHARES during invalid turn phases', () => {
      let state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );
      state.phase = 'place_tile';
      state.players[0]!.shares.american = 5;

      expect(() =>
        gameReducer(state, {
          type: 'SELL_SHARES',
          sales: [{ companyId: 'american', count: 1 }],
        })
      ).toThrow("Cannot sell shares during phase 'place_tile'");
    });

    it('executes SELL_ALL_SHARES to liquidate all active holdings of the active player', () => {
      let state = createInitialGameState(
        DEFAULT_CONFIG,
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2, // $200
        availableShares: 15,
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 2, // $400
        availableShares: 18,
      };
      state.companies.imperial = {
        ...state.companies.imperial!,
        isActive: false, // Inactive: should NOT be sold!
        size: 0,
        availableShares: 25,
      };

      state.players[0]!.shares.american = 3; // $600
      state.players[0]!.shares.royal = 2; // $800
      state.players[0]!.shares.imperial = 4; // inactive
      state.players[0]!.cash = 500;
      state.phase = 'buy_shares';

      state = gameReducer(state, { type: 'SELL_ALL_SHARES' });

      expect(state.players[0]!.cash).toBe(500 + 600 + 800); // $1900
      expect(state.players[0]!.shares.american).toBe(0);
      expect(state.players[0]!.shares.royal).toBe(0);
      expect(state.players[0]!.shares.imperial).toBe(4); // Inactive kept
      expect(state.companies.american?.availableShares).toBe(18);
      expect(state.companies.royal?.availableShares).toBe(20);
      expect(state.turnSoldShares.american).toBe(3);
      expect(state.turnSoldShares.royal).toBe(2);
    });

    it('allows repurchasing shares sold within the same turn beyond maxSharesPerPurchase limit', () => {
      let state = createInitialGameState(
        DEFAULT_CONFIG, // maxSharesPerPurchase = 3
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2, // $200
        availableShares: 20,
      };
      state.players[0]!.shares.american = 4;
      state.players[0]!.cash = 10000;
      state.phase = 'buy_shares';

      // P1 sells 2 shares of American -> turnSoldShares.american = 2
      state = gameReducer(state, {
        type: 'SELL_SHARES',
        sales: [{ companyId: 'american', count: 2 }],
      });

      // Allowed to buy up to 3 + 2 = 5 shares of American!
      state = gameReducer(state, {
        type: 'BUY_SHARES',
        purchases: [{ companyId: 'american', count: 5 }],
      });

      expect(state.players[0]!.shares.american).toBe(4 - 2 + 5); // 7 shares
      expect(state.turnPurchasedSharesCount).toBe(5);
    });

    it('pays dividends to active player upon END_TURN when payDividends is enabled', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, payDividends: true },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2, // $200, yield 0.10 = $20 per share
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 2, // $400, yield 0.125 = $50 per share
      };

      state.players[0]!.shares.american = 10; // 10 * 20 = $200
      state.players[0]!.shares.royal = 4; // 4 * 50 = $200
      state.players[0]!.cash = 5000;
      state.phase = 'buy_shares';

      state = gameReducer(state, { type: 'END_TURN' });

      // P1 should receive $400 in dividends
      expect(state.players[0]!.cash).toBe(5400);
      expect(state.lastDividendPayout).toEqual({
        playerId: 'p1',
        totalDividend: 400,
        breakdown: {
          american: 200,
          royal: 200,
        },
      });
      // turnSoldShares should be reset
      expect(state.turnSoldShares).toEqual({});
    });

    it('does not pay dividends upon END_TURN when payDividends is disabled', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, payDividends: false },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 2,
      };
      state.players[0]!.shares.american = 10;
      state.players[0]!.cash = 5000;
      state.phase = 'buy_shares';

      state = gameReducer(state, { type: 'END_TURN' });

      expect(state.players[0]!.cash).toBe(5000);
      expect(state.lastDividendPayout).toBeNull();
    });
  });

  describe('On-Grid Discarded Markers (showDiscard)', () => {
    it('marks grid cell as discarded when DISCARD_DEAD_TILE is called and showDiscard is true', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, showDiscard: true },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      // Setup 2 safe companies
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.board[2]![2] = { type: 'company', companyId: 'american' };
      state.board[2]![4] = { type: 'company', companyId: 'royal' };

      const deadTile = { row: 2, col: 3 };
      state.players[0]!.hand[0] = deadTile;

      expect(state.board[deadTile.row]![deadTile.col]!.type).toBe('empty');

      state = gameReducer(state, { type: 'DISCARD_DEAD_TILE', coord: deadTile });

      expect(state.board[deadTile.row]![deadTile.col]).toEqual({ type: 'discarded' });
    });

    it('leaves cell empty on DISCARD_DEAD_TILE when showDiscard is false or undefined', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, showDiscard: false },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.board[2]![2] = { type: 'company', companyId: 'american' };
      state.board[2]![4] = { type: 'company', companyId: 'royal' };

      const deadTile = { row: 2, col: 3 };
      state.players[0]!.hand[0] = deadTile;

      state = gameReducer(state, { type: 'DISCARD_DEAD_TILE', coord: deadTile });

      expect(state.board[deadTile.row]![deadTile.col]).toEqual({ type: 'empty' });
    });

    it('rejects tile placement on discarded cells', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, showDiscard: true },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.board[1]![1] = { type: 'discarded' };
      state.players[0]!.hand[0] = { row: 1, col: 1 };

      expect(() => {
        gameReducer(state, { type: 'PLACE_TILE', coord: { row: 1, col: 1 } });
      }).toThrow(/already occupied/);
    });

    it('marks dead tiles as discarded during END_TURN dead tile cleanup when showDiscard is true', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, showDiscard: true },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.board[2]![2] = { type: 'company', companyId: 'american' };
      state.board[2]![4] = { type: 'company', companyId: 'royal' };

      const deadTile = { row: 2, col: 3 };
      state.players[0]!.hand = [deadTile, { row: 0, col: 0 }];
      state.phase = 'buy_shares';

      state = gameReducer(state, { type: 'END_TURN' });

      expect(state.board[deadTile.row]![deadTile.col]).toEqual({ type: 'discarded' });
    });
  });

  describe('PLACE_BLANK_TILE & Wildcard Blank Pieces', () => {
    it('places blank tile as isolated wildcat, removes blank from hand, removes targetCoord from pool, and advances phase to buy_shares', () => {
      let state = createInitialGameState(
        { ...DEFAULT_CONFIG, allowBlanks: true },
        [
          { name: 'P1', isAi: false },
          { name: 'P2', isAi: false },
        ],
        42
      );

      const blankTile = { row: -1, col: -1, isBlank: true };
      const targetCoord = { row: 5, col: 5 };
      state.players[0]!.hand = [blankTile, { row: 0, col: 0 }];
      state.tilePool = [targetCoord, { row: 1, col: 1 }];

      const nextState = gameReducer(state, {
        type: 'PLACE_BLANK_TILE',
        blankTileCoord: blankTile,
        targetCoord,
      });

      expect(nextState.board[targetCoord.row]![targetCoord.col]!.type).toBe('unincorporated');
      expect(nextState.phase).toBe('buy_shares');
      expect(nextState.players[0]!.hand).not.toContainEqual(blankTile);
      // targetCoord must be purged from tilePool so it cannot be drawn again
      expect(nextState.tilePool).not.toContainEqual(targetCoord);
      expect(nextState.tilePool).toContainEqual({ row: 1, col: 1 });
      expect(nextState.history).toHaveLength(1);
      expect(nextState.history[0]).toEqual({
        type: 'PLACE_BLANK_TILE',
        blankTileCoord: blankTile,
        targetCoord,
      });
    });

    it('founds company when blank tile is placed adjacent to unincorporated tile', () => {
      let state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      const blankTile = { row: -1, col: -1, isBlank: true };
      state.board[2]![2] = { type: 'unincorporated' };
      state.players[0]!.hand = [blankTile];

      const nextState = gameReducer(state, {
        type: 'PLACE_BLANK_TILE',
        blankTileCoord: blankTile,
        targetCoord: { row: 2, col: 3 },
      });

      expect(nextState.phase).toBe('choose_founding_company');
      expect(nextState.pendingFounding).toBeDefined();
      expect(nextState.pendingFounding?.coord).toEqual({ row: 2, col: 3 });
      expect(nextState.players[0]!.hand).not.toContainEqual(blankTile);
    });

    it('expands active company when blank tile is placed adjacent to it', () => {
      let state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      state.board[2]![2] = { type: 'company', companyId: 'american' };
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 1,
      };
      const blankTile = { row: -1, col: -1, isBlank: true };
      state.players[0]!.hand = [blankTile];

      const nextState = gameReducer(state, {
        type: 'PLACE_BLANK_TILE',
        blankTileCoord: blankTile,
        targetCoord: { row: 2, col: 3 },
      });

      expect(nextState.phase).toBe('buy_shares');
      expect(nextState.board[2]![3]).toEqual({ type: 'company', companyId: 'american' });
      expect(nextState.companies.american?.size).toBe(2);
      expect(nextState.players[0]!.hand).not.toContainEqual(blankTile);
    });

    it('triggers merger when blank tile connects two active companies', () => {
      let state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      state.board[2]![2] = { type: 'company', companyId: 'american' };
      state.board[2]![4] = { type: 'company', companyId: 'royal' };
      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 3,
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 2,
      };
      const blankTile = { row: -1, col: -1, isBlank: true };
      state.players[0]!.hand = [blankTile];

      const nextState = gameReducer(state, {
        type: 'PLACE_BLANK_TILE',
        blankTileCoord: blankTile,
        targetCoord: { row: 2, col: 3 },
      });

      // American (size 3) > Royal (size 2), American survives, so royal goes into defunct queue
      expect(['choose_merger_survivor', 'dispose_merger_shares', 'buy_shares']).toContain(nextState.phase);
      expect(nextState.board[2]![3]!.type).toBe('company');
      expect(nextState.players[0]!.hand).not.toContainEqual(blankTile);
    });

    it('rejects placing blank tile if player does not have a blank tile in hand', () => {
      const state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      state.players[0]!.hand = [{ row: 0, col: 0 }];

      expect(() => {
        gameReducer(state, {
          type: 'PLACE_BLANK_TILE',
          blankTileCoord: { row: -1, col: -1, isBlank: true },
          targetCoord: { row: 5, col: 5 },
        });
      }).toThrow(/not in active player's hand/i);
    });

    it('rejects placing blank tile on an occupied cell', () => {
      const state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      const blankTile = { row: -1, col: -1, isBlank: true };
      state.players[0]!.hand = [blankTile];
      state.board[5]![5] = { type: 'unincorporated' };

      expect(() => {
        gameReducer(state, {
          type: 'PLACE_BLANK_TILE',
          blankTileCoord: blankTile,
          targetCoord: { row: 5, col: 5 },
        });
      }).toThrow(/already occupied/i);
    });

    it('rejects placing blank tile out of bounds', () => {
      const state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      const blankTile = { row: -1, col: -1, isBlank: true };
      state.players[0]!.hand = [blankTile];

      expect(() => {
        gameReducer(state, {
          type: 'PLACE_BLANK_TILE',
          blankTileCoord: blankTile,
          targetCoord: { row: -1, col: 5 },
        });
      }).toThrow(/out of bounds/i);
    });

    it('rejects placing blank tile on illegal safe merger coordinate', () => {
      const state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      state.companies.american = {
        ...state.companies.american!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.companies.royal = {
        ...state.companies.royal!,
        isActive: true,
        size: 11,
        isSafe: true,
      };
      state.board[2]![2] = { type: 'company', companyId: 'american' };
      state.board[2]![4] = { type: 'company', companyId: 'royal' };

      const blankTile = { row: -1, col: -1, isBlank: true };
      state.players[0]!.hand = [blankTile];

      expect(() => {
        gameReducer(state, {
          type: 'PLACE_BLANK_TILE',
          blankTileCoord: blankTile,
          targetCoord: { row: 2, col: 3 },
        });
      }).toThrow(/illegal|dead/i);
    });

    it('rejects placing blank tile during wrong phase', () => {
      const state = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'P1', isAi: false },
        { name: 'P2', isAi: false },
      ]);

      const blankTile = { row: -1, col: -1, isBlank: true };
      state.players[0]!.hand = [blankTile];
      state.phase = 'buy_shares';

      expect(() => {
        gameReducer(state, {
          type: 'PLACE_BLANK_TILE',
          blankTileCoord: blankTile,
          targetCoord: { row: 2, col: 3 },
        });
      }).toThrow(/Cannot place.*phase/i);
    });
  });
});

