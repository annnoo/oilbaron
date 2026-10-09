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

  it('produces deterministic tile distribution with the same seed', () => {
    const players = [
      { name: 'Alice', isAi: false },
      { name: 'Bob', isAi: false }
    ];
    const state1 = createInitialGameState(DEFAULT_CONFIG, players, 42);
    const state2 = createInitialGameState(DEFAULT_CONFIG, players, 42);

    expect(state1.tilePool).toEqual(state2.tilePool);
    expect(state1.players[0]?.hand).toEqual(state2.players[0]?.hand);
    expect(state1.players[1]?.hand).toEqual(state2.players[1]?.hand);
  });

  it('initializes companies in game state properly from config', () => {
    const players = [{ name: 'Solo', isAi: false }];
    const state = createInitialGameState(DEFAULT_CONFIG, players, 1);

    for (const companyDef of DEFAULT_CONFIG.companies) {
      const companyState = state.companies[companyDef.id];
      expect(companyState).toBeDefined();
      expect(companyState?.name).toBe(companyDef.name);
      expect(companyState?.tier).toBe(companyDef.tier);
      expect(companyState?.color).toBe(companyDef.color);
      expect(companyState?.size).toBe(0);
      expect(companyState?.isActive).toBe(false);
      expect(companyState?.isSafe).toBe(false);
      expect(companyState?.availableShares).toBe(DEFAULT_CONFIG.totalSharesPerCompany);
    }
  });

  it('initializes player portfolios with 0 shares for all companies', () => {
    const players = [{ name: 'Player 1', isAi: false }];
    const state = createInitialGameState(DEFAULT_CONFIG, players, 1);
    const p1 = state.players[0];
    expect(p1).toBeDefined();
    for (const companyDef of DEFAULT_CONFIG.companies) {
      expect(p1?.shares[companyDef.id]).toBe(0);
    }
  });
});
