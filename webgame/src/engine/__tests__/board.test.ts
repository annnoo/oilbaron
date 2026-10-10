import { describe, it, expect } from 'vitest';
import {
  createEmptyBoard,
  getAdjacentCoords,
  classifyMove,
  isTileDead,
  getContiguousCoordinates,
  applyTilePlacement,
  formatCoord,
  parseCoord,
  coordEquals,
  getNeighborDetails,
  generateTilePool,
  isValidCoord,
  coordToString,
  stringToCoord,
} from '../board';

describe('Board Adjacency & Move Classification', () => {
  it('correctly finds adjacent coordinates within bounds', () => {
    // Corner
    const corner = getAdjacentCoords({ row: 0, col: 0 }, 10, 12);
    expect(corner).toHaveLength(2);
    expect(corner).toContainEqual({ row: 0, col: 1 });
    expect(corner).toContainEqual({ row: 1, col: 0 });

    // Center
    const center = getAdjacentCoords({ row: 4, col: 5 }, 10, 12);
    expect(center).toHaveLength(4);
  });

  it('returns empty array when coordinate is out of bounds', () => {
    expect(getAdjacentCoords({ row: -1, col: 0 }, 10, 12)).toEqual([]);
    expect(getAdjacentCoords({ row: 10, col: 0 }, 10, 12)).toEqual([]);
    expect(getAdjacentCoords({ row: 0, col: -1 }, 10, 12)).toEqual([]);
    expect(getAdjacentCoords({ row: 0, col: 12 }, 10, 12)).toEqual([]);
  });

  it('classifies isolated tile placement', () => {
    const board = createEmptyBoard(10, 12);
    const result = classifyMove(board, { row: 2, col: 2 }, {}, 11);
    expect(result.type).toBe('isolated');
  });

  it('classifies company expansion and populates connectedCoords', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    const companies = {
      american: { id: 'american', size: 2, isSafe: false, isActive: true } as any,
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('expansion');
    if (result.type === 'expansion') {
      expect(result.companyId).toBe('american');
      expect(result.connectedCoords).toHaveLength(1);
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 3 });
    }
  });

  it('classifies company expansion with adjacent unincorporated tiles in connectedCoords', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'unincorporated' };
    board[2]![5] = { type: 'unincorporated' };
    const companies = {
      american: { id: 'american', size: 2, isSafe: false, isActive: true } as any,
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies);
    expect(result.type).toBe('expansion');
    if (result.type === 'expansion') {
      expect(result.companyId).toBe('american');
      expect(result.connectedCoords).toHaveLength(3);
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 3 });
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 4 });
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 5 });
    }
  });

  it('classifies company founding when touching unincorporated tiles and populates connectedCoords', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'unincorporated' };
    board[2]![1] = { type: 'unincorporated' };
    const result = classifyMove(board, { row: 2, col: 3 }, {}, 11);
    expect(result.type).toBe('founding');
    if (result.type === 'founding') {
      expect(result.connectedCoords).toHaveLength(3);
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 3 });
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 2 });
      expect(result.connectedCoords).toContainEqual({ row: 2, col: 1 });
    }
  });

  it('classifies merger when touching two active companies', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'company', companyId: 'imperial' };
    const companies = {
      american: { id: 'american', size: 4, isSafe: false, isActive: true } as any,
      imperial: { id: 'imperial', size: 5, isSafe: false, isActive: true } as any,
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('merger');
    if (result.type === 'merger') {
      expect(result.participatingCompanyIds).toContain('american');
      expect(result.participatingCompanyIds).toContain('imperial');
    }
  });

  it('detects dead tile when move would merge two safe companies', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'company', companyId: 'imperial' };
    const companies = {
      american: { id: 'american', size: 12, isSafe: true, isActive: true } as any,
      imperial: { id: 'imperial', size: 15, isSafe: true, isActive: true } as any,
    };
    const dead = isTileDead(board, { row: 2, col: 3 }, companies, 11);
    expect(dead).toBe(true);

    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('illegal_safe_merger');
  });

  it('classifies expansion when touching multiple tiles of the same company', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'company', companyId: 'american' };
    const companies = {
      american: { id: 'american', size: 6, isSafe: false, isActive: true } as any,
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('expansion');
    if (result.type === 'expansion') {
      expect(result.companyId).toBe('american');
    }
    expect(isTileDead(board, { row: 2, col: 3 }, companies, 11)).toBe(false);
  });

  it('allows merger between one safe and one unsafe company', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'company', companyId: 'imperial' };
    const companies = {
      american: { id: 'american', size: 12, isSafe: true, isActive: true } as any,
      imperial: { id: 'imperial', size: 5, isSafe: false, isActive: true } as any,
    };
    const dead = isTileDead(board, { row: 2, col: 3 }, companies, 11);
    expect(dead).toBe(false);

    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('merger');
  });

  it('applies tile placement immutably', () => {
    const board = createEmptyBoard(10, 12);
    const updated = applyTilePlacement(board, { row: 3, col: 4 }, { type: 'unincorporated' });

    expect(board[3]?.[4]?.type).toBe('empty');
    expect(updated[3]?.[4]?.type).toBe('unincorporated');
    expect(updated).not.toBe(board);
  });

  it('finds contiguous coordinates using flood-fill for company and unincorporated', () => {
    const board = createEmptyBoard(10, 12);
    board[1]![1] = { type: 'company', companyId: 'american' };
    board[1]![2] = { type: 'company', companyId: 'american' };
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![3] = { type: 'company', companyId: 'imperial' }; // Different company adjacent
    board[5]![5] = { type: 'company', companyId: 'american' }; // Disconnected cluster

    const americanCoords = getContiguousCoordinates(board, { row: 1, col: 1 });
    expect(americanCoords).toHaveLength(3);
    expect(americanCoords).toContainEqual({ row: 1, col: 1 });
    expect(americanCoords).toContainEqual({ row: 1, col: 2 });
    expect(americanCoords).toContainEqual({ row: 2, col: 2 });

    // Empty cell returns empty array
    expect(getContiguousCoordinates(board, { row: 0, col: 0 })).toEqual([]);

    // Unincorporated flood-fill
    board[7]![7] = { type: 'unincorporated' };
    board[7]![8] = { type: 'unincorporated' };
    const unincCoords = getContiguousCoordinates(board, { row: 7, col: 7 });
    expect(unincCoords).toHaveLength(2);
    expect(unincCoords).toContainEqual({ row: 7, col: 7 });
    expect(unincCoords).toContainEqual({ row: 7, col: 8 });
  });

  it('correctly formats and parses board coordinates (A-1 through J-12)', () => {
    expect(formatCoord({ row: 0, col: 0 })).toBe('A-1');
    expect(formatCoord({ row: 2, col: 3 })).toBe('C-4');
    expect(formatCoord({ row: 9, col: 11 })).toBe('J-12');

    expect(parseCoord('A-1')).toEqual({ row: 0, col: 0 });
    expect(parseCoord('a1')).toEqual({ row: 0, col: 0 });
    expect(parseCoord('C-4')).toEqual({ row: 2, col: 3 });
    expect(parseCoord('c4')).toEqual({ row: 2, col: 3 });
    expect(parseCoord('J-12')).toEqual({ row: 9, col: 11 });
    expect(parseCoord('j12')).toEqual({ row: 9, col: 11 });

    // Invalid coordinates for default 26x26 grid
    expect(parseCoord('Z-27')).toBeNull();
    expect(parseCoord('AA-1')).toBeNull();
    expect(parseCoord('invalid')).toBeNull();

    // Custom row/col boundary enforcement
    expect(parseCoord('K-1', 10, 12)).toBeNull();
    expect(parseCoord('A-13', 10, 12)).toBeNull();

    // Coordinate equality
    expect(coordEquals({ row: 1, col: 2 }, { row: 1, col: 2 })).toBe(true);
    expect(coordEquals({ row: 1, col: 2 }, { row: 1, col: 3 })).toBe(false);
    expect(coordEquals(null, { row: 1, col: 2 })).toBe(false);
  });

  it('supports full 26x26 grid dimensions and A-Z row labels', () => {
    // A-1 to Z-26 formatting
    expect(formatCoord({ row: 0, col: 0 })).toBe('A-1');
    expect(formatCoord({ row: 10, col: 0 })).toBe('K-1');
    expect(formatCoord({ row: 25, col: 25 })).toBe('Z-26');

    // Parsing A-1 through Z-26
    expect(parseCoord('K-1')).toEqual({ row: 10, col: 0 });
    expect(parseCoord('k1')).toEqual({ row: 10, col: 0 });
    expect(parseCoord('Z-26')).toEqual({ row: 25, col: 25 });
    expect(parseCoord('z26')).toEqual({ row: 25, col: 25 });
    expect(parseCoord('M-15')).toEqual({ row: 12, col: 14 });

    // Out of bounds for 26x26
    expect(parseCoord('AA-1')).toBeNull();
    expect(parseCoord('Z-27')).toBeNull();
    expect(parseCoord('A-0')).toBeNull();

    // Board creation and adjacency on 26x26
    const board = createEmptyBoard(26, 26);
    expect(board).toHaveLength(26);
    expect(board[25]).toHaveLength(26);

    const corner = getAdjacentCoords({ row: 25, col: 25 }, 26, 26);
    expect(corner).toHaveLength(2);
    expect(corner).toContainEqual({ row: 24, col: 25 });
    expect(corner).toContainEqual({ row: 25, col: 24 });
  });

  it('tests generateTilePool, isValidCoord, coordToString and stringToCoord', () => {
    // 26x26 grid pool generation
    const pool26 = generateTilePool(26, 26);
    expect(pool26).toHaveLength(676);
    expect(pool26[0]).toEqual({ row: 0, col: 0 });
    expect(pool26[675]).toEqual({ row: 25, col: 25 });

    // With wildcard blanks
    const poolWithBlanks = generateTilePool(10, 12, true);
    expect(poolWithBlanks).toHaveLength(124);
    const blanks = poolWithBlanks.filter((t) => t.isBlank);
    expect(blanks).toHaveLength(4);

    // isValidCoord
    expect(isValidCoord({ row: 0, col: 0 }, 10, 12)).toBe(true);
    expect(isValidCoord({ row: 9, col: 11 }, 10, 12)).toBe(true);
    expect(isValidCoord({ row: 10, col: 11 }, 10, 12)).toBe(false);
    expect(isValidCoord({ row: 25, col: 25 }, 26, 26)).toBe(true);
    expect(isValidCoord({ row: -1, col: -1, isBlank: true }, 10, 12)).toBe(true);

    // Aliases
    expect(coordToString({ row: 3, col: 7 })).toBe('D-8');
    expect(stringToCoord('D-8')).toEqual({ row: 3, col: 7 });
  });

  it('provides comprehensive 4-way neighbor details including edges', () => {
    const board = createEmptyBoard(10, 12);
    board[0]![1] = { type: 'unincorporated' };
    board[1]![0] = { type: 'company', companyId: 'american' };

    // Corner (0, 0)
    const cornerNeighbors = getNeighborDetails(board, { row: 0, col: 0 });
    expect(cornerNeighbors).toHaveLength(4);

    const north = cornerNeighbors.find((n) => n.direction === 'north')!;
    expect(north.isEdge).toBe(true);
    expect(north.cell).toBeNull();

    const west = cornerNeighbors.find((n) => n.direction === 'west')!;
    expect(west.isEdge).toBe(true);
    expect(west.cell).toBeNull();

    const east = cornerNeighbors.find((n) => n.direction === 'east')!;
    expect(east.isEdge).toBe(false);
    expect(east.cell?.type).toBe('unincorporated');

    const south = cornerNeighbors.find((n) => n.direction === 'south')!;
    expect(south.isEdge).toBe(false);
    expect(south.cell?.type).toBe('company');
  });
});

