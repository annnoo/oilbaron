import { describe, it, expect, beforeEach, afterEach, vi } from 'vitest';
import {
  serializeGameState,
  deserializeGameState,
  saveToLocalStorage,
  loadFromLocalStorage,
  listLocalStorageSaveSlots,
  deleteLocalStorageSaveSlot,
} from '../persistence';
import { createInitialGameState, CLASSIC_CONFIG } from '../config';
import type { GameState } from '../types';

class MockStorage implements Storage {
  private store = new Map<string, string>();

  get length(): number {
    return this.store.size;
  }

  clear(): void {
    this.store.clear();
  }

  getItem(key: string): string | null {
    return this.store.get(key) ?? null;
  }

  key(index: number): string | null {
    return Array.from(this.store.keys())[index] ?? null;
  }

  removeItem(key: string): void {
    this.store.delete(key);
  }

  setItem(key: string, value: string): void {
    this.store.set(key, String(value));
  }
}

describe('Engine Persistence (Save & Load)', () => {
  let mockStorage: MockStorage;
  let sampleState: GameState;

  beforeEach(() => {
    mockStorage = new MockStorage();
    Object.defineProperty(globalThis, 'localStorage', {
      value: mockStorage,
      writable: true,
      configurable: true,
    });
    sampleState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob', 'Charlie'], 12345);
  });

  afterEach(() => {
    // @ts-expect-error cleanup global mock
    delete globalThis.localStorage;
  });

  describe('serializeGameState', () => {
    it('serializes a valid GameState into a parseable JSON string', () => {
      const json = serializeGameState(sampleState);
      expect(typeof json).toBe('string');

      const parsed = JSON.parse(json);
      expect(parsed).toBeDefined();
    });

    it('round-trips full state without data loss', () => {
      const json = serializeGameState(sampleState);
      const restored = deserializeGameState(json);

      expect(restored.config.ruleset).toBe(sampleState.config.ruleset);
      expect(restored.board.length).toBe(sampleState.board.length);
      expect(restored.board[0]!.length).toBe(sampleState.board[0]!.length);
      expect(restored.players.length).toBe(3);
      expect(restored.players[0]!.name).toBe('Alice');
      expect(restored.players[1]!.name).toBe('Bob');
      expect(restored.players[2]!.name).toBe('Charlie');
      expect(restored.activePlayerIndex).toBe(sampleState.activePlayerIndex);
      expect(restored.phase).toBe(sampleState.phase);
      expect(restored.tilePool.length).toBe(sampleState.tilePool.length);
      expect(restored.history).toEqual(sampleState.history);
    });

    it('throws error when serializing an invalid game state', () => {
      expect(() => serializeGameState(null as unknown as GameState)).toThrow();
      expect(() => serializeGameState({} as unknown as GameState)).toThrow();
    });
  });

  describe('deserializeGameState & Schema Validation', () => {
    it('throws error on invalid JSON syntax', () => {
      expect(() => deserializeGameState('not valid json {')).toThrow(/JSON/i);
    });

    it('throws error on non-object JSON values', () => {
      expect(() => deserializeGameState('42')).toThrow(/Invalid game state/i);
      expect(() => deserializeGameState('"a string"')).toThrow(/Invalid game state/i);
      expect(() => deserializeGameState('null')).toThrow(/Invalid game state/i);
    });

    it('throws error when missing config', () => {
      const copy: any = { ...sampleState };
      delete copy.config;
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/config/i);
    });

    it('throws error when missing board or invalid board dimensions', () => {
      const copy: any = { ...sampleState };
      delete copy.board;
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/board/i);

      const invalidBoard: any = { ...sampleState, board: 'not an array' };
      expect(() => deserializeGameState(JSON.stringify(invalidBoard))).toThrow(/board/i);
    });

    it('throws error when missing players or empty players array', () => {
      const copy: any = { ...sampleState };
      delete copy.players;
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/players/i);

      const emptyPlayers: any = { ...sampleState, players: [] };
      expect(() => deserializeGameState(JSON.stringify(emptyPlayers))).toThrow(/players/i);
    });

    it('throws error when player is missing mandatory fields', () => {
      const invalidPlayerState: any = {
        ...sampleState,
        players: [{ id: 'p1', name: 'Alice' }], // missing cash, shares, hand
      };
      expect(() => deserializeGameState(JSON.stringify(invalidPlayerState))).toThrow(/player/i);
    });

    it('throws error when missing companies', () => {
      const copy: any = { ...sampleState };
      delete copy.companies;
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/companies/i);
    });

    it('throws error when missing tilePool', () => {
      const copy: any = { ...sampleState };
      delete copy.tilePool;
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/tilePool/i);
    });

    it('throws error on invalid turn phase', () => {
      const copy: any = { ...sampleState, phase: 'invalid_phase' };
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/phase/i);
    });

    it('throws error on out-of-range activePlayerIndex', () => {
      const copy: any = { ...sampleState, activePlayerIndex: 99 };
      expect(() => deserializeGameState(JSON.stringify(copy))).toThrow(/activePlayerIndex/i);
    });

    it('supports unwrapping envelope format with version and savedAt', () => {
      const envelope = {
        version: 1,
        savedAt: new Date().toISOString(),
        state: sampleState,
      };
      const restored = deserializeGameState(JSON.stringify(envelope));
      expect(restored.players[0]!.name).toBe('Alice');
    });
  });

  describe('saveToLocalStorage and loadFromLocalStorage', () => {
    it('saves to default slot when slot is omitted and loads it back', () => {
      const success = saveToLocalStorage(sampleState);
      expect(success).toBe(true);

      const loaded = loadFromLocalStorage();
      expect(loaded).not.toBeNull();
      expect(loaded?.players[0]?.name).toBe('Alice');
    });

    it('saves and loads from specific slot name', () => {
      const success = saveToLocalStorage(sampleState, 'slot_2');
      expect(success).toBe(true);

      expect(loadFromLocalStorage('slot_1')).toBeNull();

      const loaded = loadFromLocalStorage('slot_2');
      expect(loaded).not.toBeNull();
      expect(loaded?.players[1]?.name).toBe('Bob');
    });

    it('overwrites previous save in the same slot', () => {
      saveToLocalStorage(sampleState, 'slot_1');

      const firstPlayer = sampleState.players[0]!;
      const modifiedState: GameState = {
        ...sampleState,
        players: [
          { ...firstPlayer, cash: 99999 },
          ...sampleState.players.slice(1),
        ],
      };
      saveToLocalStorage(modifiedState, 'slot_1');

      const loaded = loadFromLocalStorage('slot_1');
      expect(loaded?.players[0]?.cash).toBe(99999);
    });

    it('returns null when loading non-existent slot', () => {
      const loaded = loadFromLocalStorage('nonexistent_slot');
      expect(loaded).toBeNull();
    });

    it('returns null when loading corrupted save in localStorage', () => {
      mockStorage.setItem('oilbaron_save_slot_corrupted', 'this is { not valid json');
      const loaded = loadFromLocalStorage('corrupted');
      expect(loaded).toBeNull();
    });

    it('returns false when localStorage.setItem throws (e.g. QuotaExceededError)', () => {
      vi.spyOn(mockStorage, 'setItem').mockImplementation(() => {
        throw new Error('QuotaExceededError');
      });

      const success = saveToLocalStorage(sampleState, 'slot_1');
      expect(success).toBe(false);
    });

    it('returns null and false gracefully if localStorage is unavailable', () => {
      // @ts-expect-error remove localStorage
      delete globalThis.localStorage;

      expect(saveToLocalStorage(sampleState, 'slot_1')).toBe(false);
      expect(loadFromLocalStorage('slot_1')).toBeNull();
      expect(listLocalStorageSaveSlots()).toEqual([]);
    });
  });

  describe('listLocalStorageSaveSlots', () => {
    it('returns empty array when no slots exist', () => {
      expect(listLocalStorageSaveSlots()).toEqual([]);
    });

    it('lists all saved slots with slot name, date, and player names', () => {
      saveToLocalStorage(sampleState, 'slot_1');

      const state2 = createInitialGameState(CLASSIC_CONFIG, ['Dave', 'Eve']);
      saveToLocalStorage(state2, 'slot_2');

      // Unrelated keys in storage
      mockStorage.setItem('oilbaron_muted', 'true');
      mockStorage.setItem('other_app_key', '123');

      const slots = listLocalStorageSaveSlots();
      expect(slots.length).toBe(2);

      const slot1 = slots.find((s) => s.slot === 'slot_1');
      expect(slot1).toBeDefined();
      expect(slot1?.players).toEqual(['Alice', 'Bob', 'Charlie']);
      expect(typeof slot1?.date).toBe('string');

      const slot2 = slots.find((s) => s.slot === 'slot_2');
      expect(slot2).toBeDefined();
      expect(slot2?.players).toEqual(['Dave', 'Eve']);
    });

    it('skips corrupted slot entries in listLocalStorageSaveSlots', () => {
      saveToLocalStorage(sampleState, 'slot_1');
      mockStorage.setItem('oilbaron_save_slot_corrupted', 'broken json {');

      const slots = listLocalStorageSaveSlots();
      expect(slots.length).toBe(1);
      expect(slots[0]!.slot).toBe('slot_1');
    });
  });

  describe('deleteLocalStorageSaveSlot', () => {
    it('deletes existing slot and returns true', () => {
      saveToLocalStorage(sampleState, 'slot_1');
      expect(loadFromLocalStorage('slot_1')).not.toBeNull();

      const result = deleteLocalStorageSaveSlot('slot_1');
      expect(result).toBe(true);

      expect(loadFromLocalStorage('slot_1')).toBeNull();
      expect(listLocalStorageSaveSlots()).toEqual([]);
    });

    it('returns false when trying to delete non-existent slot', () => {
      const result = deleteLocalStorageSaveSlot('nonexistent');
      expect(result).toBe(false);
    });
  });
});
