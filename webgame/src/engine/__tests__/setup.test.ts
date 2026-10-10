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
});

