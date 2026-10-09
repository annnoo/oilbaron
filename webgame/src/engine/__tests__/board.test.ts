import { describe, it, expect } from 'vitest';
import {
  createEmptyBoard,
  getAdjacentCoords,
  classifyMove,
  isTileDead,
  getContiguousCoordinates,
  applyTilePlacement,
} from '../board';
import { DEFAULT_CONFIG } from '../config';
import { GameState } from '../types';

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

  it('classifies isolated tile placement', () => {
    const board = createEmptyBoard(10, 12);
    const result = classifyMove(board, { row: 2, col: 2 }, {}, 11);
    expect(result.type).toBe('isolated');
  });

  it('classifies company expansion', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    const companies = {
      american: { id: 'american', size: 2, isSafe: false, isActive: true } as any,
    };
    const result = classifyMove(board, { row: 2, col: 3 }, companies, 11);
    expect(result.type).toBe('expansion');
    if (result.type === 'expansion') {
      expect(result.companyId).toBe('american');
    }
  });

  it('classifies company founding when touching unincorporated tiles', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'unincorporated' };
    const result = classifyMove(board, { row: 2, col: 3 }, {}, 11);
    expect(result.type).toBe('founding');
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
});
