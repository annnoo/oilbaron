import type { GameState, TurnPhase, CompanyId, CellState } from './types';

export const SAVE_SLOT_PREFIX = 'oilbaron_save_slot_';
export const DEFAULT_SAVE_SLOT = 'default';

export interface SaveSlotMetadata {
  slot: string;
  date: string;
  players: string[];
}

export interface SerializedEnvelope {
  version: number;
  savedAt: string;
  slot?: string;
  players?: string[];
  state: GameState;
}

const VALID_PHASES: TurnPhase[] = [
  'place_tile',
  'choose_founding_company',
  'choose_merger_survivor',
  'dispose_merger_shares',
  'buy_shares',
  'game_over',
];

function getStorage(): Storage | null {
  try {
    if (typeof window !== 'undefined' && window.localStorage) {
      return window.localStorage;
    }
    if (typeof localStorage !== 'undefined') {
      return localStorage;
    }
  } catch {
    return null;
  }
  return null;
}

function getSlotKey(slot: string): string {
  const cleanSlot = slot.trim() || DEFAULT_SAVE_SLOT;
  if (cleanSlot.startsWith(SAVE_SLOT_PREFIX)) {
    return cleanSlot;
  }
  return `${SAVE_SLOT_PREFIX}${cleanSlot}`;
}

function extractSlotName(key: string): string {
  if (key.startsWith(SAVE_SLOT_PREFIX)) {
    return key.slice(SAVE_SLOT_PREFIX.length);
  }
  return key;
}

/**
 * Validates whether an object conforms to the GameState schema.
 * Throws a descriptive Error if validation fails.
 */
export function validateGameState(candidate: unknown): asserts candidate is GameState {
  if (!candidate || typeof candidate !== 'object') {
    throw new Error('Invalid game state: expected an object');
  }

  const state = candidate as Partial<GameState>;

  // 1. Config validation
  if (!state.config || typeof state.config !== 'object') {
    throw new Error('Invalid game state: missing or invalid "config" object');
  }
  if (!state.config.ruleset || typeof state.config.boardRows !== 'number' || typeof state.config.boardCols !== 'number') {
    throw new Error('Invalid game state: "config" missing required ruleset or dimensions');
  }

  // 2. Board validation
  if (!Array.isArray(state.board)) {
    throw new Error('Invalid game state: "board" must be a 2D array');
  }
  if (state.board.length !== state.config.boardRows) {
    throw new Error(
      `Invalid game state: board row count (${state.board.length}) does not match config (${state.config.boardRows})`
    );
  }
  for (let r = 0; r < state.board.length; r++) {
    const row = state.board[r];
    if (!Array.isArray(row) || row.length !== state.config.boardCols) {
      throw new Error(`Invalid game state: board row ${r} length does not match config cols`);
    }
  }

  // 3. Players validation
  if (!Array.isArray(state.players) || state.players.length === 0) {
    throw new Error('Invalid game state: "players" must be a non-empty array');
  }
  for (let i = 0; i < state.players.length; i++) {
    const p = state.players[i];
    if (
      !p ||
      typeof p.id !== 'string' ||
      typeof p.name !== 'string' ||
      typeof p.cash !== 'number' ||
      typeof p.shares !== 'object' ||
      !Array.isArray(p.hand)
    ) {
      throw new Error(`Invalid game state: player at index ${i} is missing required fields (id, name, cash, shares, hand)`);
    }
  }

  // 4. Companies validation
  if (!state.companies || typeof state.companies !== 'object') {
    throw new Error('Invalid game state: missing or invalid "companies" object');
  }

  // 5. Tile pool validation
  if (!Array.isArray(state.tilePool)) {
    throw new Error('Invalid game state: "tilePool" must be an array');
  }

  // 6. Turn phase validation
  if (!state.phase || !VALID_PHASES.includes(state.phase as TurnPhase)) {
    throw new Error(`Invalid game state: invalid "phase" (${String(state.phase)})`);
  }

  // 7. Active player index validation
  if (
    typeof state.activePlayerIndex !== 'number' ||
    state.activePlayerIndex < 0 ||
    state.activePlayerIndex >= state.players.length
  ) {
    throw new Error(
      `Invalid game state: activePlayerIndex (${String(state.activePlayerIndex)}) is out of range for ${state.players.length} players`
    );
  }

  // 8. History validation
  if (!Array.isArray(state.history)) {
    throw new Error('Invalid game state: "history" must be an array');
  }
}

/**
 * Serializes a GameState into a JSON string with format metadata.
 */
export function serializeGameState(state: GameState): string {
  validateGameState(state);

  const envelope: SerializedEnvelope = {
    version: 1,
    savedAt: new Date().toISOString(),
    players: state.players.map((p) => p.name),
    state,
  };

  return JSON.stringify(envelope, null, 2);
}

/**
 * Deserializes a JSON string into a validated GameState.
 * Supports both enveloped ({ version, state }) and raw GameState representations.
 */
export function deserializeGameState(json: string): GameState {
  if (typeof json !== 'string') {
    throw new Error('Invalid game state: input must be a JSON string');
  }

  let parsed: unknown;
  try {
    parsed = JSON.parse(json);
  } catch (err) {
    throw new Error(`Invalid JSON syntax: ${(err as Error).message}`);
  }

  if (!parsed || typeof parsed !== 'object') {
    throw new Error('Invalid game state: root JSON value must be an object');
  }

  // Support envelope unwrapping
  let candidateState: unknown = parsed;
  if ('state' in parsed && (parsed as Record<string, unknown>).state) {
    candidateState = (parsed as Record<string, unknown>).state;
  }

  validateGameState(candidateState);
  return candidateState;
}

/**
 * Saves a GameState to the browser's localStorage under the given slot name.
 * Defaults to 'default' if slot is omitted.
 */
export function saveToLocalStorage(state: GameState, slot: string = DEFAULT_SAVE_SLOT): boolean {
  const storage = getStorage();
  if (!storage) return false;

  try {
    validateGameState(state);

    const slotName = slot.trim() || DEFAULT_SAVE_SLOT;
    const key = getSlotKey(slotName);
    const dateStr = new Date().toISOString();

    const payload = {
      version: 1,
      slot: slotName,
      date: dateStr,
      players: state.players.map((p) => p.name),
      state,
    };

    storage.setItem(key, JSON.stringify(payload));
    return true;
  } catch (err) {
    console.error('Failed to save to localStorage:', err);
    return false;
  }
}

/**
 * Loads and deserializes a GameState from localStorage for the given slot.
 * Returns null if slot does not exist or save data is corrupted/invalid.
 */
export function loadFromLocalStorage(slot: string = DEFAULT_SAVE_SLOT): GameState | null {
  const storage = getStorage();
  if (!storage) return null;

  try {
    const slotName = slot.trim() || DEFAULT_SAVE_SLOT;
    const key = getSlotKey(slotName);
    const raw = storage.getItem(key);
    if (!raw) return null;

    return deserializeGameState(raw);
  } catch (err) {
    console.warn(`Failed to load save from localStorage slot "${slot}":`, err);
    return null;
  }
}

/**
 * Lists all valid save slots stored in localStorage.
 */
export function listLocalStorageSaveSlots(): SaveSlotMetadata[] {
  const storage = getStorage();
  if (!storage) return [];

  const slots: SaveSlotMetadata[] = [];

  try {
    const keys: string[] = [];
    for (let i = 0; i < storage.length; i++) {
      const key = storage.key(i);
      if (key && key.startsWith(SAVE_SLOT_PREFIX)) {
        keys.push(key);
      }
    }

    for (const key of keys) {
      const raw = storage.getItem(key);
      if (!raw) continue;

      try {
        const parsed = JSON.parse(raw);
        if (parsed && typeof parsed === 'object') {
          const slotName = parsed.slot || extractSlotName(key);
          const date = typeof parsed.date === 'string' ? parsed.date : (parsed.savedAt ?? '');
          const players = Array.isArray(parsed.players)
            ? parsed.players
            : (parsed.state?.players?.map((p: any) => p.name) ?? []);

          slots.push({
            slot: slotName,
            date,
            players,
          });
        }
      } catch {
        // Skip corrupted entries
      }
    }
  } catch (err) {
    console.warn('Error reading save slots from localStorage:', err);
  }

  // Sort newest first
  slots.sort((a, b) => {
    if (!a.date && !b.date) return a.slot.localeCompare(b.slot);
    if (!a.date) return 1;
    if (!b.date) return -1;
    return new Date(b.date).getTime() - new Date(a.date).getTime();
  });

  return slots;
}

/**
 * Deletes a save slot from localStorage.
 * Returns true if the slot existed and was removed, false otherwise.
 */
export function deleteLocalStorageSaveSlot(slot: string): boolean {
  const storage = getStorage();
  if (!storage) return false;

  const key = getSlotKey(slot);
  if (storage.getItem(key) === null) {
    return false;
  }

  try {
    storage.removeItem(key);
    return true;
  } catch {
    return false;
  }
}

/**
 * Helper to trigger a browser file download of a serialized GameState.
 */
export function exportGameStateToJson(state: GameState, filename?: string): void {
  if (typeof document === 'undefined') return;

  const jsonString = serializeGameState(state);
  const blob = new Blob([jsonString], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const anchor = document.createElement('a');

  const defaultFilename = filename || `oilbaron-save-${new Date().toISOString().replace(/[:.]/g, '-')}.json`;
  anchor.href = url;
  anchor.download = defaultFilename;
  document.body.appendChild(anchor);
  anchor.click();
  anchor.remove();
  URL.revokeObjectURL(url);
}

/**
 * Helper to parse a JSON string imported from a file.
 */
export function importGameStateFromJson(json: string): GameState {
  return deserializeGameState(json);
}
