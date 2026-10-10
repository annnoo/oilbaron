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

  it('renders classic blank tile bitmap and label for blank tiles in player rack', () => {
    const stateWithBlank = {
      ...dummyState,
      players: dummyState.players.map((p, idx) =>
        idx === 0
          ? {
              ...p,
              hand: [{ row: -1, col: -1, isBlank: true }, ...p.hand.slice(1)],
            }
          : p
      ),
    };

    const html = renderToStaticMarkup(
      <ClassicGameView
        gameState={stateWithBlank}
        selectedHandCoord={{ row: -1, col: -1, isBlank: true }}
        hoveredHandCoord={null}
        showHints={true}
        onToggleHints={vi.fn()}
        onSelectHandCoord={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onPlayBlankTile={vi.fn()}
        onMenuAction={vi.fn()}
        onSwitchToModern={vi.fn()}
        onTogglePlayerAi={vi.fn()}
        onOpenStockMarket={vi.fn()}
        onEndTurn={vi.fn()}
      />
    );

    expect(html).toContain('/classic_assets/blank.png');
    expect(html).toContain('BLANK');
  });

  it('renders dynamic grid dimensions (e.g. 15x18) in ClassicGameView', () => {
    const config15x18 = { ...CLASSIC_CONFIG, boardRows: 15, boardCols: 18 };
    const customState = createInitialGameState(config15x18, ['Alice', 'Bob']);

    const html = renderToStaticMarkup(
      <ClassicGameView
        gameState={customState}
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

    expect(html).toContain('Petroleum Oil Board: 15x18');
    expect(html).toContain('grid-template-columns:26px repeat(18, 38px)');
    expect(html).toContain('grid-template-rows:20px repeat(15, 38px)');
    expect(html).toContain('win31-coord-hdr">18</div>');
    expect(html).toContain('win31-coord-hdr">O</div>');
  });
});
