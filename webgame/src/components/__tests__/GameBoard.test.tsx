import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { GameBoard } from '../GameBoard';
import { createEmptyBoard } from '../../engine/board';
import type { CompanyState, TileCoord } from '../../engine/types';

describe('GameBoard Component', () => {
  const mockCompanies: Record<string, CompanyState> = {
    american: {
      id: 'american',
      name: 'American Natural Gas',
      tier: 'low',
      color: '#e63946',
      size: 4,
      isActive: true,
      isSafe: false,
      availableShares: 20,
    },
    imperial: {
      id: 'imperial',
      name: 'Imperial Petroleum',
      tier: 'mid',
      color: '#2a9d8f',
      size: 11,
      isActive: true,
      isSafe: true,
      availableShares: 14,
    },
    royal: {
      id: 'royal',
      name: 'Royal Dutch',
      tier: 'high',
      color: '#1d3557',
      size: 12,
      isActive: true,
      isSafe: true,
      availableShares: 10,
    },
  };

  it('renders standard 10x12 grid with row headers (A-J) and column headers (1-12)', () => {
    const board = createEmptyBoard(10, 12);
    const html = renderToStaticMarkup(<GameBoard board={board} />);

    // Row headers A through J
    const letters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'];
    for (const letter of letters) {
      expect(html).toContain(`row-header" role="rowheader">${letter}</div>`);
    }

    // Column headers 1 through 12
    for (let c = 1; c <= 12; c++) {
      expect(html).toContain(`col-header" role="columnheader">${c}</div>`);
    }

    // 120 grid cells
    const cellMatches = html.match(/role="gridcell"/g);
    expect(cellMatches).toHaveLength(120);
  });

  it('renders empty cells with subtle coordinate tags', () => {
    const board = createEmptyBoard(10, 12);
    const html = renderToStaticMarkup(<GameBoard board={board} showCoordinates={true} />);

    expect(html).toContain('A-1');
    expect(html).toContain('J-12');
    expect(html).toContain('data-type="empty"');
  });

  it('renders placed unincorporated wildcat chits', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![3] = { type: 'unincorporated' };

    const html = renderToStaticMarkup(<GameBoard board={board} />);

    expect(html).toContain('unincorporated');
    expect(html).toContain('chit-dot');
    expect(html).toContain('C-4');
  });

  it('renders company-controlled tiles with logos, initials, and size badges', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![3] = { type: 'company', companyId: 'american' };
    board[4]![5] = { type: 'company', companyId: 'imperial' };

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        safeThreshold={11}
      />
    );

    // American: initials AM, size 4 (vulnerable)
    expect(html).toContain('AM');
    expect(html).toContain('tile-size-badge');
    expect(html).toContain('background-color:#e63946');

    // Imperial: initials IM, size 11 (Safe shield icon)
    expect(html).toContain('IM');
    expect(html).toContain('background-color:#2a9d8f');
  });

  it('renders move classification preview for isolated tile placement', () => {
    const board = createEmptyBoard(10, 12);
    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        previewTileCoord={{ row: 2, col: 2 }} // C-3
      />
    );

    expect(html).toContain('preview-isolated');
    expect(html).toContain('ISOLATED');
    expect(html).toContain('C-3: Isolated wildcat chit');
  });

  it('renders move classification preview for company expansion and highlights connected tiles', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' };
    board[2]![4] = { type: 'unincorporated' };

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        previewTileCoord={{ row: 2, col: 3 }} // Placed between company and chit
      />
    );

    expect(html).toContain('preview-expansion');
    expect(html).toContain('EXPANSION');
    expect(html).toContain('Expands American Natural Gas');
    expect(html).toContain('highlight-connected');
  });

  it('renders move classification preview for company founding', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'unincorporated' };

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        previewTileCoord={{ row: 2, col: 3 }}
      />
    );

    expect(html).toContain('preview-founding');
    expect(html).toContain('FOUNDING');
    expect(html).toContain('Founds a new corporation');
    expect(html).toContain('highlight-connected');
  });

  it('renders move classification preview for corporate merger and highlights participants', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'american' }; // Size 4 (< 11, vulnerable)
    board[2]![4] = { type: 'company', companyId: 'imperial' }; // Size 11 (Safe, 1 safe company is legal)

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        previewTileCoord={{ row: 2, col: 3 }}
        safeThreshold={11}
      />
    );

    expect(html).toContain('preview-merger');
    expect(html).toContain('MERGER');
    expect(html).toContain('Corporate Takeover Merger!');
    expect(html).toContain('highlight-merger');
  });

  it('renders move classification preview for illegal safe merger (DEAD TILE)', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![2] = { type: 'company', companyId: 'imperial' }; // Safe (size 11)
    board[2]![4] = { type: 'company', companyId: 'royal' };    // Safe (size 12)

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        previewTileCoord={{ row: 2, col: 3 }}
        safeThreshold={11}
      />
    );

    expect(html).toContain('preview-illegal_safe_merger');
    expect(html).toContain('DEAD TILE');
    expect(html).toContain('Cannot merge safe corporations');
  });

  it('highlights tiles currently in the active player hand and selected tile', () => {
    const board = createEmptyBoard(10, 12);
    const hand: TileCoord[] = [
      { row: 0, col: 0 }, // A-1
      { row: 3, col: 4 }, // D-5
    ];

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        playerHand={hand}
        selectedTileCoord={{ row: 3, col: 4 }}
      />
    );

    expect(html).toContain('is-in-hand');
    expect(html).toContain('cell-in-hand-indicator');
    expect(html).toContain('is-selected-hand');
  });

  it('renders Quick Peek inspector modal when inspectedCoord is provided', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![3] = { type: 'company', companyId: 'american' };

    const html = renderToStaticMarkup(
      <GameBoard
        board={board}
        companies={mockCompanies}
        inspectedCoord={{ row: 2, col: 3 }}
      />
    );

    expect(html).toContain('Quick Peek — Cell C-4');
    expect(html).toContain('American Natural Gas');
    expect(html).toContain('Cell Topology &amp; Ownership');
  });

  it('provides accessible grid semantics and keyboard navigation', () => {
    const board = createEmptyBoard(10, 12);
    const html = renderToStaticMarkup(<GameBoard board={board} />);

    expect(html).toContain('role="grid"');
    expect(html).toContain('role="rowheader"');
    expect(html).toContain('role="columnheader"');
    expect(html).toContain('tabindex="0"');
    expect(html).toContain('aria-label="Oil Baron 10x12 Game Board"');
  });

  it('renders discarded marker ("X") on cells with type discarded', () => {
    const board = createEmptyBoard(10, 12);
    board[3]![4] = { type: 'discarded' };

    const html = renderToStaticMarkup(<GameBoard board={board} />);

    expect(html).toContain('board-cell discarded');
    expect(html).toContain('discarded-marker');
    expect(html).toContain('discarded-x-mark');
    expect(html).toContain('✕');
  });

  it('renders dynamic 26x26 board grid correctly with A-Z headers and 676 cells', () => {
    const board = createEmptyBoard(26, 26);
    const html = renderToStaticMarkup(<GameBoard board={board} />);

    expect(html).toContain('Oil Baron 26x26 Game Board');
    expect(html).toContain('26 × 26 Grid');
    expect(html).toContain('row-header" role="rowheader">Z</div>');
    expect(html).toContain('col-header" role="columnheader">26</div>');
    expect(html).toContain('grid-template-columns:28px repeat(26, minmax(32px, 52px))');

    const cellMatches = html.match(/role="gridcell"/g);
    expect(cellMatches).toHaveLength(676);
  });
});

