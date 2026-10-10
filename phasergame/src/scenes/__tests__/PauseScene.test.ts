import { describe, it, expect } from 'vitest';
import { listLocalStorageSaveSlots, saveToLocalStorage, loadFromLocalStorage } from '../../engine/persistence';
import { createInitialGameState, DEFAULT_CONFIG } from '../../engine/config';

class MockStorage implements Storage {
  private store = new Map<string, string>();
  get length(): number { return this.store.size; }
  clear(): void { this.store.clear(); }
  getItem(key: string): string | null { return this.store.get(key) ?? null; }
  key(index: number): string | null { return Array.from(this.store.keys())[index] ?? null; }
  removeItem(key: string): void { this.store.delete(key); }
  setItem(key: string, value: string): void { this.store.set(key, String(value)); }
}

describe('Pause & Save/Load scene integration logic', () => {
  beforeEach(() => {
    (globalThis as unknown as { localStorage: Storage }).localStorage = new MockStorage();
  });

  it('manages save slots cleanly through persistence API', () => {
    const state = createInitialGameState(DEFAULT_CONFIG, [
      { name: 'Baron Alpha', isAi: false },
      { name: 'AI Rival', isAi: true, aiDifficulty: 'medium' },
    ]);

    const success = saveToLocalStorage(state, 'slot-1');
    expect(success).toBe(true);

    const slots = listLocalStorageSaveSlots();
    const slot1 = slots.find((s) => s.slot === 'slot-1');
    expect(slot1).toBeDefined();
    expect(slot1?.players.length).toBe(2);

    const loaded = loadFromLocalStorage('slot-1');
    expect(loaded).toBeDefined();
    expect(loaded?.players.length).toBe(2);
  });
});
