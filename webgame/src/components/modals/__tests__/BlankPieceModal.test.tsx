import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { BlankPieceModal } from '../BlankPieceModal';
import { CLASSIC_CONFIG, createInitialGameState } from '../../../engine/config';
import type { GameState, TileCoord } from '../../../engine/types';

describe('BlankPieceModal Component', () => {
  const dummyState: GameState = createInitialGameState(
    { ...CLASSIC_CONFIG, allowBlanks: true },
    ['Alice', 'Bob'],
    123
  );

  const blankTile: TileCoord = { row: -1, col: -1, isBlank: true };

  it('returns null when isOpen is false', () => {
    const html = renderToStaticMarkup(
      <BlankPieceModal
        isOpen={false}
        onClose={vi.fn()}
        gameState={dummyState}
        blankTileCoord={blankTile}
        onConfirmPlacement={vi.fn()}
      />
    );
    expect(html).toBe('');
  });

  it('renders authentic 1993 BLANK_MOVE title, prompt and format guidance', () => {
    const html = renderToStaticMarkup(
      <BlankPieceModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
        blankTileCoord={blankTile}
        onConfirmPlacement={vi.fn()}
      />
    );

    expect(html).toContain('Move Blank: Keyboard');
    expect(html).toContain('Please Choose Square to Place Blank:');
    expect(html).toContain('Format Xn (example A12)');
    expect(html).toContain('OK');
    expect(html).toContain('Cancel');
  });

  it('renders coordinate input and row/column selectors', () => {
    const html = renderToStaticMarkup(
      <BlankPieceModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
        blankTileCoord={blankTile}
        onConfirmPlacement={vi.fn()}
      />
    );

    expect(html).toContain('coord-input');
    expect(html).toContain('row-select');
    expect(html).toContain('col-select');
  });

  it('displays move consequence preview for valid empty coordinate', () => {
    const html = renderToStaticMarkup(
      <BlankPieceModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
        blankTileCoord={blankTile}
        onConfirmPlacement={vi.fn()}
      />
    );

    // Initial default cell is empty/wildcat
    expect(html).toContain('Wildcat');
  });

  it('identifies and warns against illegal safe mergers', () => {
    const testState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob'], 1);
    testState.companies.american = {
      ...testState.companies.american!,
      isActive: true,
      size: 11,
      isSafe: true,
    };
    testState.companies.royal = {
      ...testState.companies.royal!,
      isActive: true,
      size: 11,
      isSafe: true,
    };
    testState.board[2]![2] = { type: 'company', companyId: 'american' };
    testState.board[2]![4] = { type: 'company', companyId: 'royal' };

    const html = renderToStaticMarkup(
      <BlankPieceModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={testState}
        blankTileCoord={blankTile}
        onConfirmPlacement={vi.fn()}
        initialCoord={{ row: 2, col: 3 }}
      />
    );

    expect(html).toContain('Illegal');
  });
});
