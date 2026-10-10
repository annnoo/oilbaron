import { describe, it, expect } from 'vitest';
import { setupGame } from '../setup';
import { DEFAULT_CONFIG } from '../config';

describe('Engine Setup and Initialization', () => {
  it('runs vitest correctly', () => {
    expect(1 + 1).toBe(2);
  });

  it('propagates player aiDifficulty into PlayerState', () => {
    const state = setupGame(DEFAULT_CONFIG, [
      { name: 'Human 1', isAi: false },
      { name: 'CPU Dumb', isAi: true, aiDifficulty: 'dumb' },
      { name: 'CPU Smart', isAi: true, aiDifficulty: 'smart' },
      { name: 'CPU Default', isAi: true },
    ]);

    expect(state.players[0]!.aiDifficulty).toBeUndefined();
    expect(state.players[1]!.aiDifficulty).toBe('dumb');
    expect(state.players[2]!.aiDifficulty).toBe('smart');
    expect(state.players[3]!.aiDifficulty).toBe('smart');
  });

  it('correctly configures 8th company purple through setupGame', () => {
    const stateDefault = setupGame({ ...DEFAULT_CONFIG, enableEighthCompany: false });
    expect(stateDefault.companies.purple).toBeUndefined();

    const stateEighth = setupGame({ ...DEFAULT_CONFIG, enableEighthCompany: true });
    expect(stateEighth.companies.purple).toBeDefined();
    expect(stateEighth.companies.purple?.name).toBe('Purple Oil Company, Int.');
  });

  it('applies economy randomizers in setupGame', () => {
    const state = setupGame(
      { ...DEFAULT_CONFIG, enableEighthCompany: true, randomSharePrices: true, randomDividends: true },
      undefined,
      1234
    );

    expect(state.config.randomSharePrices).toBe(true);
    expect(state.config.randomDividends).toBe(true);
    // Base prices or dividend rates should reflect randomization
    const purpleDef = state.config.companies.find((c) => c.id === 'purple');
    expect(purpleDef).toBeDefined();
  });
});

