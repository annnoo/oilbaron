import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { describe, it, expect, vi } from 'vitest';
import { ActionDock } from '../ActionDock';
import { CLASSIC_COMPANIES } from '../../engine/config';
import { createEmptyBoard } from '../../engine/board';

// Register toBeInTheDocument matcher for vitest Chai
expect.extend({
  toBeInTheDocument(received) {
    const pass = Boolean(received);
    return {
      pass,
      message: () =>
        pass
          ? 'expected element not to be in document'
          : 'expected element to be in document',
    };
  },
});

declare module 'vitest' {
  interface Assertion<T = any> {
    toBeInTheDocument(): T;
  }
}

let currentHtml = '';

function render(ui: React.ReactElement) {
  currentHtml = renderToStaticMarkup(ui);
  return { html: currentHtml };
}

const decodeHtml = (str: string) =>
  str
    .replace(/&amp;/g, '&')
    .replace(/&lt;/g, '<')
    .replace(/&gt;/g, '>')
    .replace(/&quot;/g, '"')
    .replace(/&#39;/g, "'");

const screen = {
  getByText: (matcher: RegExp | string) => {
    const regex = typeof matcher === 'string' ? new RegExp(matcher, 'i') : matcher;
    const decoded = decodeHtml(currentHtml);
    const matches = regex.test(decoded);
    if (!matches) {
      throw new Error(`Unable to find text matching ${matcher} in: ${decoded}`);
    }
    return { matches, inDocument: true };
  },
  queryByText: (matcher: RegExp | string) => {
    const regex = typeof matcher === 'string' ? new RegExp(matcher, 'i') : matcher;
    const decoded = decodeHtml(currentHtml);
    return regex.test(decoded);
  },
};

describe('ActionDock', () => {
  const emptyBoard = createEmptyBoard(10, 12);

  it('renders hand chits when in place_tile phase', () => {
    const { html } = render(
      <ActionDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[{ row: 0, col: 0 }, { row: 1, col: 1 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6000}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('unified-action-dock');
    expect(screen.getByText('A-1')).toBeInTheDocument();
    expect(screen.getByText('B-2')).toBeInTheDocument();
    expect(screen.queryByText(/Stock Exchange: Invest in up to 3 shares/i)).toBe(false);
  });

  it('renders InlineStockMarket when in buy_shares phase and active player turn', () => {
    const { html } = render(
      <ActionDock
        phase="buy_shares"
        isCurrentTurn={true}
        hand={[]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6000}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('unified-action-dock');
    expect(screen.getByText(/Stock Exchange: Invest in up to 3 shares/i)).toBeInTheDocument();
    expect(html).not.toContain('player-hand-tray');
  });

  it('renders PlayerHand when in buy_shares phase but not active player turn', () => {
    const { html } = render(
      <ActionDock
        phase="buy_shares"
        isCurrentTurn={false}
        hand={[{ row: 2, col: 3 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6000}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('unified-action-dock');
    expect(html).toContain('player-hand-tray');
    expect(screen.getByText('C-4')).toBeInTheDocument();
    expect(screen.queryByText(/Stock Exchange: Invest in up to 3 shares/i)).toBe(false);
  });

  it('renders PlayerHand with board and custom props during other phases', () => {
    const { html } = render(
      <ActionDock
        phase="choose_founding_company"
        isCurrentTurn={true}
        hand={[{ row: 0, col: 0 }]}
        selectedCoord={{ row: 0, col: 0 }}
        hoveredCoord={null}
        activePlayerCash={5000}
        activePlayerName="Oil Tycoon"
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        safeThreshold={11}
        hideChits={false}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('Oil Tycoon');
    expect(screen.getByText('A-1')).toBeInTheDocument();
    expect(html).toContain('is-selected');
  });
});
