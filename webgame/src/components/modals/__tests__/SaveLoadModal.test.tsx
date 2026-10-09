import { describe, it, expect, beforeEach, afterEach, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { SaveLoadModal } from '../SaveLoadModal';
import {
  saveToLocalStorage,
  loadFromLocalStorage,
  listLocalStorageSaveSlots,
  deleteLocalStorageSaveSlot,
  exportGameStateToJson,
  importGameStateFromJson,
} from '../../../engine/persistence';
import { createInitialGameState, CLASSIC_CONFIG } from '../../../engine/config';
import type { GameState } from '../../../engine/types';

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

describe('SaveLoadModal Component', () => {
  let mockStorage: MockStorage;
  let sampleState: GameState;

  beforeEach(() => {
    mockStorage = new MockStorage();
    Object.defineProperty(globalThis, 'localStorage', {
      value: mockStorage,
      writable: true,
      configurable: true,
    });
    sampleState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob'], 999);
  });

  afterEach(() => {
    // @ts-expect-error clean up global
    delete globalThis.localStorage;
  });

  it('renders nothing when isOpen is false', () => {
    const html = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={false}
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(html).toBe('');
  });

  it('renders title, save controls, and JSON export/import buttons when isOpen is true', () => {
    const html = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(html).toContain('Save &amp; Load Match State');
    expect(html).toContain('Save Game');
    expect(html).toContain('Export Save as JSON');
    expect(html).toContain('Import JSON Save');
  });

  it('displays empty state message when no saved slots exist in storage', () => {
    const html = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(html).toContain('No saved games in local browser storage.');
  });

  it('renders saved slot entries with player names when slots exist', () => {
    saveToLocalStorage(sampleState, 'Championship_1993');

    const html = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(html).toContain('Championship_1993');
    expect(html).toContain('Alice');
    expect(html).toContain('Bob');
    expect(html).toContain('Load');
  });

  it('supports initialMode="save" vs initialMode="load"', () => {
    const saveHtml = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={true}
        initialMode="save"
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(saveHtml).toContain('Save Game');

    const loadHtml = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={true}
        initialMode="load"
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(loadHtml).toContain('Load');
  });

  it('displays quick slot buttons (Slot 1, Slot 2, Slot 3, etc.)', () => {
    const html = renderToStaticMarkup(
      <SaveLoadModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={sampleState}
        onLoadGame={vi.fn()}
      />
    );
    expect(html).toContain('Slot 1');
    expect(html).toContain('Slot 2');
    expect(html).toContain('Slot 3');
  });
});
