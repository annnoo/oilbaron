import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { TileInspector } from '../TileInspector';
import { createEmptyBoard } from '../../engine/board';
import { CLASSIC_COMPANIES } from '../../engine/config';
import type { CellState, CompanyState } from '../../engine/types';

describe('TileInspector Component', () => {
  const mockCompanies: Record<string, CompanyState> = {
    american: {
      id: 'american',
      name: 'American Natural Gas',
      tier: 'low',
      color: '#e63946',
      size: 5,
      isActive: true,
      isSafe: false,
      availableShares: 18,
    },
    imperial: {
      id: 'imperial',
      name: 'Imperial Petroleum',
      tier: 'mid',
      color: '#2a9d8f',
      size: 11,
      isActive: true,
      isSafe: true,
      availableShares: 12,
    },
  };

  it('renders nothing when coord is null or isOpen is false', () => {
    const board = createEmptyBoard(10, 12);
    const htmlNull = renderToStaticMarkup(
      <TileInspector coord={null} board={board} />
    );
    expect(htmlNull).toBe('');

    const htmlClosed = renderToStaticMarkup(
      <TileInspector coord={{ row: 0, col: 0 }} board={board} isOpen={false} />
    );
    expect(htmlClosed).toBe('');
  });

  it('renders nothing when coordinate is out of bounds', () => {
    const board = createEmptyBoard(10, 12);
    const htmlOutOfBounds = renderToStaticMarkup(
      <TileInspector coord={{ row: 15, col: 20 }} board={board} />
    );
    expect(htmlOutOfBounds).toBe('');
  });

  it('renders Quick Peek modal for an empty cell with move simulation', () => {
    const board = createEmptyBoard(10, 12);
    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 2, col: 3 }}
        board={board}
        companies={mockCompanies}
        mode="modal"
      />
    );

    expect(html).toContain('Quick Peek — Cell C-4');
    expect(html).toContain('Empty Grid Slot');
    expect(html).toContain('Isolated Wildcat Placement');
    expect(html).toContain('Cardinal Connectivity');
    expect(html).toContain('NORTH');
    expect(html).toContain('SOUTH');
    expect(html).toContain('WEST');
    expect(html).toContain('EAST');
  });

  it('renders Quick Peek modal for an unincorporated wildcat chit', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![3] = { type: 'unincorporated' };
    board[2]![4] = { type: 'unincorporated' };

    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 2, col: 3 }}
        board={board}
        companies={mockCompanies}
      />
    );

    expect(html).toContain('Quick Peek — Cell C-4');
    expect(html).toContain('Unincorporated Chit');
    expect(html).toContain('Wildcat Drill Site');
    expect(html).toContain('2 chit(s)');
    expect(html).toContain('Connected Chain Tiles');
    expect(html).toContain('C-4');
    expect(html).toContain('C-5');
  });

  it('renders company tile details with financial metrics and vulnerable status (<11 tiles)', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![3] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'company', companyId: 'american' };

    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 2, col: 3 }}
        board={board}
        companies={mockCompanies}
        safeThreshold={11}
      />
    );

    expect(html).toContain('American Natural Gas');
    expect(html).toContain('AM'); // Initials
    expect(html).toContain('LOW TIER');
    expect(html).toContain('VULNERABLE');
    expect(html).toContain('6 to safe'); // 11 - 5 = 6 tiles to safe
    expect(html).toContain('5 Tiles'); // size
    // Price for size 5 low tier: $200 + 3 * $100 = $500
    expect(html).toContain('$500');
    expect(html).toContain('18 / 25'); // bank shares
    expect(html).toContain('$5,000'); // 10 * 500 = $5,000 majority bonus
    expect(html).toContain('$2,500'); // 5 * 500 = $2,500 minority bonus
  });

  it('renders company tile details with SAFE badge when size >= safeThreshold (>= 11)', () => {
    const board = createEmptyBoard(10, 12);
    board[4]![5] = { type: 'company', companyId: 'imperial' };

    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 4, col: 5 }}
        board={board}
        companies={mockCompanies}
        safeThreshold={11}
      />
    );

    expect(html).toContain('Imperial Petroleum');
    expect(html).toContain('IM');
    expect(html).toContain('SAFE (11 tiles)');
    expect(html).toContain('Permanent Safe');
    // Price for size 11 mid tier: $300 + 9 * $100 = $1,200
    expect(html).toContain('$1,200');
    expect(html).toContain('$12,000'); // Majority bonus
    expect(html).toContain('$6,000'); // Minority bonus
  });

  it('renders cardinal neighbor connectivity with border edge detection', () => {
    const board = createEmptyBoard(10, 12);
    board[0]![1] = { type: 'unincorporated' };
    board[1]![0] = { type: 'company', companyId: 'american' };

    // Coordinate A-1 (0, 0) touches North and West borders
    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 0, col: 0 }}
        board={board}
        companies={mockCompanies}
      />
    );

    expect(html).toContain('Board Border'); // North and West are edges
    expect(html).toContain('Unincorporated Chit'); // East has chit
    expect(html).toContain('American Natural Gas'); // South has American
  });

  it('renders compact floating tooltip in tooltip mode', () => {
    const board = createEmptyBoard(10, 12);
    board[1]![2] = { type: 'company', companyId: 'american' };

    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 1, col: 2 }}
        board={board}
        companies={mockCompanies}
        mode="tooltip"
        anchorPosition={{ x: 200, y: 150 }}
      />
    );

    expect(html).toContain('tile-inspector-tooltip');
    expect(html).toContain('B-3');
    expect(html).toContain('American Natural Gas');
    expect(html).toContain('Size: 5');
    expect(html).toContain('$500');
    expect(html).toContain('role="tooltip"');
  });

  it('renders Place Chit action button when onPlaceTile callback is supplied for empty cell', () => {
    const board = createEmptyBoard(10, 12);
    const onPlace = vi.fn();

    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 3, col: 3 }}
        board={board}
        companies={mockCompanies}
        onPlaceTile={onPlace}
        isPlayableInHand={true}
      />
    );

    expect(html).toContain('Place Chit at D-4');
    expect(html).toContain('In Your Hand');
  });
});
