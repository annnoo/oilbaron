import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { ClassicGameView } from '../ClassicGameView';
import { CLASSIC_CONFIG, createInitialGameState } from '../../../engine/config';

describe('ClassicGameView Component', () => {
  const dummyState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob']);

  it('renders authentic Windows 3.1 titlebar and desktop elements', () => {
    const html = renderToStaticMarkup(
      <ClassicGameView
        gameState={dummyState}
        selectedHandCoord={null}
        hoveredHandCoord={null}
        showHints={true}
        onToggleHints={vi.fn()}
        onSelectHandCoord={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onMenuAction={vi.fn()}
        onSwitchToModern={vi.fn()}
        onTogglePlayerAi={vi.fn()}
        onOpenStockMarket={vi.fn()}
        onEndTurn={vi.fn()}
      />
    );

    expect(html).toContain('win31-root');
    expect(html).toContain('OILBARON.EXE');
    expect(html).toContain('1993 Windows 3.1 Classic Edition');
    expect(html).toContain('Switch to Modern Studio UI');
    expect(html).toContain('win31-board-grid');
    expect(html).toContain('Player Standings');
  });

  it('renders original company bitmaps and oil chits in board and ticker', () => {
    const html = renderToStaticMarkup(
      <ClassicGameView
        gameState={dummyState}
        selectedHandCoord={{ row: 1, col: 2 }}
        hoveredHandCoord={null}
        showHints={true}
        onToggleHints={vi.fn()}
        onSelectHandCoord={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onMenuAction={vi.fn()}
        onSwitchToModern={vi.fn()}
        onTogglePlayerAi={vi.fn()}
        onOpenStockMarket={vi.fn()}
        onEndTurn={vi.fn()}
      />
    );

    expect(html).toContain('/classic_assets/american.png');
    expect(html).toContain('/classic_assets/chit.png');
    expect(html).toContain('win31-chit-card');
  });

  it('renders classic discard marker square bitmap for discarded cells', () => {
    const stateWithDiscard = {
      ...dummyState,
      board: dummyState.board.map((row, r) =>
        r === 2
          ? row.map((cell, c) => (c === 3 ? { type: 'discarded' as const } : cell))
          : [...row]
      ),
    };

    const html = renderToStaticMarkup(
      <ClassicGameView
        gameState={stateWithDiscard}
        selectedHandCoord={null}
        hoveredHandCoord={null}
        showHints={true}
        onToggleHints={vi.fn()}
        onSelectHandCoord={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onMenuAction={vi.fn()}
        onSwitchToModern={vi.fn()}
        onTogglePlayerAi={vi.fn()}
        onOpenStockMarket={vi.fn()}
        onEndTurn={vi.fn()}
      />
    );

    expect(html).toContain('/classic_assets/discard_square.png');
    expect(html).toContain('data-testid="classic-discarded-marker"');
  });
});
