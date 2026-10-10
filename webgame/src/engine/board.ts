import { DEFAULT_CONFIG } from './config';
import type { CellState, CompanyId, CompanyState, TileCoord } from './types';

export type MoveClassificationType =
  | 'isolated'
  | 'expansion'
  | 'founding'
  | 'merger'
  | 'illegal_safe_merger';

export interface MoveClassificationIsolated {
  type: 'isolated';
  coord: TileCoord;
}

export interface MoveClassificationExpansion {
  type: 'expansion';
  coord: TileCoord;
  companyId: CompanyId;
  connectedCoords: TileCoord[];
}

export interface MoveClassificationFounding {
  type: 'founding';
  coord: TileCoord;
  connectedCoords: TileCoord[];
}

export interface MoveClassificationMerger {
  type: 'merger';
  coord: TileCoord;
  participatingCompanyIds: CompanyId[];
}

export interface MoveClassificationIllegalSafeMerger {
  type: 'illegal_safe_merger';
  coord: TileCoord;
  safeCompanyIds: CompanyId[];
  participatingCompanyIds: CompanyId[];
}

export type MoveClassification =
  | MoveClassificationIsolated
  | MoveClassificationExpansion
  | MoveClassificationFounding
  | MoveClassificationMerger
  | MoveClassificationIllegalSafeMerger;

/**
 * Creates an empty game board of given rows and columns.
 */
export function createEmptyBoard(rows: number, cols: number): CellState[][] {
  const board: CellState[][] = [];
  for (let r = 0; r < rows; r++) {
    const row: CellState[] = [];
    for (let c = 0; c < cols; c++) {
      row.push({ type: 'empty' });
    }
    board.push(row);
  }
  return board;
}

/**
 * Returns cardinal neighbors (N, S, W, E) that lie strictly within the board boundaries.
 * Returns an empty array if the origin coordinate itself is out of bounds.
 */
export function getAdjacentCoords(
  coord: TileCoord,
  rows: number,
  cols: number
): TileCoord[] {
  if (coord.row < 0 || coord.row >= rows || coord.col < 0 || coord.col >= cols) {
    return [];
  }

  const deltas: readonly [number, number][] = [
    [-1, 0], // North
    [1, 0],  // South
    [0, -1], // West
    [0, 1],  // East
  ];

  const results: TileCoord[] = [];
  for (const [dr, dc] of deltas) {
    const nr = coord.row + dr;
    const nc = coord.col + dc;
    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
      results.push({ row: nr, col: nc });
    }
  }

  return results;
}

/**
 * Traverses contiguous matching coordinates using a queue flood-fill (BFS).
 * If no predicate is supplied:
 * - Empty cells return []
 * - Unincorporated cells flood contiguous unincorporated cells
 * - Company cells flood contiguous cells belonging to the same companyId
 */
export function getContiguousCoordinates(
  board: readonly (readonly CellState[])[],
  startCoord: TileCoord,
  predicate?: (cell: CellState, coord: TileCoord) => boolean
): TileCoord[] {
  const rows = board.length;
  if (rows === 0) return [];
  const cols = board[0]?.length ?? 0;
  if (cols === 0) return [];

  if (
    startCoord.row < 0 ||
    startCoord.row >= rows ||
    startCoord.col < 0 ||
    startCoord.col >= cols
  ) {
    return [];
  }

  const startCell = board[startCoord.row]?.[startCoord.col];
  if (!startCell) return [];

  const matchFn: (cell: CellState, coord: TileCoord) => boolean =
    predicate ??
    ((cell: CellState) => {
      if (startCell.type === 'empty') return false;
      if (startCell.type === 'unincorporated') return cell.type === 'unincorporated';
      if (startCell.type === 'company') {
        return cell.type === 'company' && cell.companyId === startCell.companyId;
      }
      return false;
    });

  if (!matchFn(startCell, startCoord)) {
    return [];
  }

  const visited = new Set<string>();
  const queue: TileCoord[] = [startCoord];
  const result: TileCoord[] = [];

  visited.add(`${startCoord.row},${startCoord.col}`);

  while (queue.length > 0) {
    const current = queue.shift()!;
    result.push(current);

    const neighbors = getAdjacentCoords(current, rows, cols);
    for (const neighbor of neighbors) {
      const key = `${neighbor.row},${neighbor.col}`;
      if (!visited.has(key)) {
        const neighborCell = board[neighbor.row]?.[neighbor.col];
        if (neighborCell && matchFn(neighborCell, neighbor)) {
          visited.add(key);
          queue.push(neighbor);
        }
      }
    }
  }

  return result;
}

/**
 * Checks if a company is safe from acquisition.
 * A company is safe if marked isSafe or if its size reaches or exceeds the safe threshold.
 */
export function isCompanySafe(
  company: CompanyState | undefined,
  safeThreshold: number = DEFAULT_CONFIG.safeThreshold
): boolean {
  if (!company) return false;
  return company.isSafe === true || (company.size !== undefined && company.size >= safeThreshold);
}

/**
 * Collects the newly placed coordinate plus all contiguous unincorporated coordinates
 * reachable from adjacent unincorporated tiles.
 */
function collectConnectedUnincorporated(
  placedCoord: TileCoord,
  unincorporatedNeighbors: readonly TileCoord[],
  board: readonly (readonly CellState[])[]
): TileCoord[] {
  const connectedSet = new Set<string>();
  connectedSet.add(`${placedCoord.row},${placedCoord.col}`);

  for (const uninc of unincorporatedNeighbors) {
    const cluster = getContiguousCoordinates(board, uninc);
    for (const c of cluster) {
      connectedSet.add(`${c.row},${c.col}`);
    }
  }

  return Array.from(connectedSet).map((s) => {
    const [r, c] = s.split(',').map(Number);
    return { row: r!, col: c! };
  });
}

/**
 * Evaluates the board topology around a placed tile coordinate and classifies the move:
 * - 'isolated': no occupied adjacent tiles
 * - 'expansion': exactly 1 distinct adjacent active company
 * - 'founding': 1+ adjacent unincorporated chits and 0 active companies
 * - 'merger': 2+ distinct active companies with < 2 safe companies
 * - 'illegal_safe_merger': 2+ distinct safe companies connected
 */
export function classifyMove(
  board: readonly (readonly CellState[])[],
  coord: TileCoord,
  companies: Partial<Record<CompanyId, CompanyState>> = {},
  safeThreshold: number = DEFAULT_CONFIG.safeThreshold
): MoveClassification {
  const rows = board.length;
  const cols = board[0]?.length ?? 0;
  const neighbors = getAdjacentCoords(coord, rows, cols);

  const adjacentCompanyIdsSet = new Set<CompanyId>();
  const unincorporatedNeighbors: TileCoord[] = [];

  for (const n of neighbors) {
    const cell = board[n.row]?.[n.col];
    if (!cell) continue;

    if (cell.type === 'company') {
      adjacentCompanyIdsSet.add(cell.companyId);
    } else if (cell.type === 'unincorporated') {
      unincorporatedNeighbors.push(n);
    }
  }

  const adjacentCompanyIds = Array.from(adjacentCompanyIdsSet);

  if (adjacentCompanyIds.length >= 2) {
    const safeCompanyIds = adjacentCompanyIds.filter((id) =>
      isCompanySafe(companies[id], safeThreshold)
    );

    if (safeCompanyIds.length >= 2) {
      return {
        type: 'illegal_safe_merger',
        coord,
        safeCompanyIds,
        participatingCompanyIds: adjacentCompanyIds,
      };
    }

    return {
      type: 'merger',
      coord,
      participatingCompanyIds: adjacentCompanyIds,
    };
  }

  if (adjacentCompanyIds.length === 1) {
    const companyId = adjacentCompanyIds[0]!;
    const connectedCoords = collectConnectedUnincorporated(
      coord,
      unincorporatedNeighbors,
      board
    );

    return {
      type: 'expansion',
      coord,
      companyId,
      connectedCoords,
    };
  }

  if (unincorporatedNeighbors.length >= 1) {
    const connectedCoords = collectConnectedUnincorporated(
      coord,
      unincorporatedNeighbors,
      board
    );

    return {
      type: 'founding',
      coord,
      connectedCoords,
    };
  }

  return {
    type: 'isolated',
    coord,
  };
}

/**
 * Determines whether a tile move is unplayable/dead (would merge two or more safe companies).
 */
export function isTileDead(
  board: readonly (readonly CellState[])[],
  coord: TileCoord,
  companies: Partial<Record<CompanyId, CompanyState>> = {},
  safeThreshold: number = DEFAULT_CONFIG.safeThreshold
): boolean {
  if (coord.isBlank) return false;
  const result = classifyMove(board, coord, companies, safeThreshold);
  return result.type === 'illegal_safe_merger';
}

/**
 * Immutably places a cell state at a coordinate, returning a new board.
 */
export function applyTilePlacement(
  board: readonly (readonly CellState[])[],
  coord: TileCoord,
  cellState: CellState = { type: 'unincorporated' }
): CellState[][] {
  const rows = board.length;
  const cols = board[0]?.length ?? 0;

  if (coord.row < 0 || coord.row >= rows || coord.col < 0 || coord.col >= cols) {
    throw new Error(`Coordinate (${coord.row}, ${coord.col}) out of bounds`);
  }

  return board.map((row, r) =>
    r === coord.row
      ? row.map((cell, c) => (c === coord.col ? cellState : cell))
      : [...row]
  );
}

export const ROW_LABELS = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'] as const;

/**
 * Formats a TileCoord into standard grid notation (e.g. { row: 2, col: 4 } -> "C-5", blank -> "BLANK").
 */
export function formatCoord(coord: TileCoord): string {
  if (coord.isBlank) {
    return 'BLANK';
  }
  const rowLabel = ROW_LABELS[coord.row] ?? String.fromCharCode(65 + coord.row);
  return `${rowLabel}-${coord.col + 1}`;
}

/**
 * Parses coordinate text (e.g. "C-5", "c5", "A-1") into a TileCoord.
 * Returns null if invalid or out of bounds.
 */
export function parseCoord(text: string): TileCoord | null {
  const trimmed = text.trim().toUpperCase();
  const match = trimmed.match(/^([A-J])-?([1-9]|1[0-2])$/);
  if (!match) return null;
  const row = match[1]!.charCodeAt(0) - 65;
  const col = parseInt(match[2]!, 10) - 1;
  return { row, col };
}

/**
 * Checks whether two coordinates represent the exact same board cell.
 */
export function coordEquals(
  a: TileCoord | null | undefined,
  b: TileCoord | null | undefined
): boolean {
  if (!a || !b) return false;
  if (a.isBlank || b.isBlank) {
    if (!a.isBlank || !b.isBlank) return false;
    return a.row === b.row && a.col === b.col;
  }
  return a.row === b.row && a.col === b.col;
}

export interface NeighborDetail {
  direction: 'north' | 'south' | 'west' | 'east';
  coord: TileCoord;
  isEdge: boolean;
  cell: CellState | null;
}

/**
 * Returns detailed state for all 4 cardinal neighbors (N, S, W, E), including border edges.
 */
export function getNeighborDetails(
  board: readonly (readonly CellState[])[],
  coord: TileCoord
): NeighborDetail[] {
  const rows = board.length;
  const cols = board[0]?.length ?? 0;

  const directions: readonly [NeighborDetail['direction'], number, number][] = [
    ['north', -1, 0],
    ['south', 1, 0],
    ['west', 0, -1],
    ['east', 0, 1],
  ];

  return directions.map(([dir, dr, dc]) => {
    const nr = coord.row + dr;
    const nc = coord.col + dc;
    const isEdge = nr < 0 || nr >= rows || nc < 0 || nc >= cols;
    const neighborCoord = { row: nr, col: nc };
    const cell = isEdge ? null : (board[nr]?.[nc] ?? null);
    return {
      direction: dir,
      coord: neighborCoord,
      isEdge,
      cell,
    };
  });
}

