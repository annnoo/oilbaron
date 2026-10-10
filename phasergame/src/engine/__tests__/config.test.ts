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

    for (const companyDef of state.config.companies) {
      const companyState = state.companies[companyDef.id];
      expect(companyState).toBeDefined();
      expect(companyState?.name).toBe(companyDef.name);
      expect(companyState?.tier).toBe(companyDef.tier);
      expect(companyState?.color).toBe(companyDef.color);
      expect(companyState?.size).toBe(0);
      expect(companyState?.isActive).toBe(false);
      expect(companyState?.isSafe).toBe(false);
      expect(companyState?.availableShares).toBe(state.config.totalSharesPerCompany);
    }
  });

  it('initializes player portfolios with 0 shares for all companies', () => {
    const players = [{ name: 'Player 1', isAi: false }];
    const state = createInitialGameState(DEFAULT_CONFIG, players, 1);
    const p1 = state.players[0];
    expect(p1).toBeDefined();
    for (const companyDef of state.config.companies) {
      expect(p1?.shares[companyDef.id]).toBe(0);
    }
  });

  it('injects 4 blank tiles into the tile pool when allowBlanks is true', () => {
    const players = [
      { name: 'Alice', isAi: false },
      { name: 'Bob', isAi: false },
    ];
    const normalState = createInitialGameState({ ...DEFAULT_CONFIG, allowBlanks: false }, players, 1);
    const blanksState = createInitialGameState({ ...DEFAULT_CONFIG, allowBlanks: true }, players, 1);

    const normalBlankCount =
      normalState.tilePool.filter((t) => t.isBlank).length +
      normalState.players.reduce((sum, p) => sum + p.hand.filter((t) => t.isBlank).length, 0);

    const blanksTotalCount =
      blanksState.tilePool.filter((t) => t.isBlank).length +
      blanksState.players.reduce((sum, p) => sum + p.hand.filter((t) => t.isBlank).length, 0);

    expect(normalBlankCount).toBe(0);
    expect(blanksTotalCount).toBe(4);
    // Total tiles (pool + hands) should be 120 + 4 = 124
    const totalTilesWithBlanks =
      blanksState.tilePool.length +
      blanksState.players.reduce((sum, p) => sum + p.hand.length, 0);
    expect(totalTilesWithBlanks).toBe(124);
  });

  describe('Task 5: 8th Corporation ("Purple Oil Company, Int.") & Economy Randomizers', () => {
    it('defines Purple Oil Company with correct attributes in CLASSIC_COMPANIES', () => {
      const purple = CLASSIC_COMPANIES.find((c) => c.id === 'purple');
      expect(purple).toBeDefined();
      expect(purple?.name).toBe('Purple Oil Company, Int.');
      expect(purple?.letter).toBe('P');
      expect(purple?.tier).toBe('low');
      expect(purple?.color).toBe('#8b5cf6');
      expect(purple?.basePrice).toBe(200);
      expect(purple?.dividendYield).toBe(0.067);
    });

    it('filters out 8th company purple by default when enableEighthCompany is false or undefined, and includes it when true', () => {
      const stateWithout = createInitialGameState({ ...DEFAULT_CONFIG, enableEighthCompany: false }, ['P1', 'P2']);
      expect(stateWithout.companies.purple).toBeUndefined();
      expect(stateWithout.config.companies.some((c) => c.id === 'purple')).toBe(false);

      const stateWith = createInitialGameState({ ...DEFAULT_CONFIG, enableEighthCompany: true }, ['P1', 'P2']);
      expect(stateWith.companies.purple).toBeDefined();
      expect(stateWith.companies.purple?.name).toBe('Purple Oil Company, Int.');
      expect(stateWith.config.companies.some((c) => c.id === 'purple')).toBe(true);
    });

    it('randomizes base share prices when randomSharePrices is true', () => {
      const normalState = createInitialGameState({ ...DEFAULT_CONFIG, randomSharePrices: false }, ['P1'], 42);
      const randState1 = createInitialGameState({ ...DEFAULT_CONFIG, randomSharePrices: true }, ['P1'], 42);
      const randState2 = createInitialGameState({ ...DEFAULT_CONFIG, randomSharePrices: true }, ['P1'], 9999);

      // Base prices should vary between seeded random states
      const normalPrices = normalState.config.companies.map((c) => c.basePrice);
      const randPrices1 = randState1.config.companies.map((c) => c.basePrice);
      const randPrices2 = randState2.config.companies.map((c) => c.basePrice);

      expect(randPrices1).not.toEqual(normalPrices);
      expect(randPrices1).not.toEqual(randPrices2);
      // All random prices should be valid positive increments of $100
      randPrices1.forEach((price) => {
        expect(price).toBeGreaterThanOrEqual(100);
        expect(price % 100).toBe(0);
      });
    });

    it('randomizes dividend yields when randomDividends is true', () => {
      const normalState = createInitialGameState({ ...DEFAULT_CONFIG, randomDividends: false }, ['P1'], 42);
      const randState1 = createInitialGameState({ ...DEFAULT_CONFIG, randomDividends: true }, ['P1'], 42);
      const randState2 = createInitialGameState({ ...DEFAULT_CONFIG, randomDividends: true }, ['P1'], 9999);

      const normalYields = normalState.config.companies.map((c) => c.dividendYield);
      const randYields1 = randState1.config.companies.map((c) => c.dividendYield);
      const randYields2 = randState2.config.companies.map((c) => c.dividendYield);

      expect(randYields1).not.toEqual(normalYields);
      expect(randYields1).not.toEqual(randYields2);
      randYields1.forEach((yieldRate) => {
        expect(yieldRate).toBeGreaterThan(0.01);
        expect(yieldRate).toBeLessThan(0.30);
      });
    });
  });
});
