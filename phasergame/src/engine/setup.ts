import { createInitialGameState, DEFAULT_CONFIG } from './config';
import type { GameConfig, GameState, PlayerInput } from './types';

/**
 * Creates and initializes the game state from a configuration and player list,
 * correctly propagating player configurations (including aiDifficulty) to PlayerState.
 */
export function setupGame(
  config: GameConfig = DEFAULT_CONFIG,
  players?: readonly PlayerInput[],
  seed?: number
): GameState {
  return createInitialGameState(config, players ?? ['Player 1', 'Computer 1 (CPU)'], seed);
}

export { createInitialGameState } from './config';
