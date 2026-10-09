import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { TurnGuidanceBar } from '../TurnGuidanceBar';

describe('TurnGuidanceBar Component', () => {
  it('renders Step 1 instruction when placing tile with no selection', () => {
    const html = renderToStaticMarkup(
      <TurnGuidanceBar
        phase="place_tile"
        activePlayerName="Player 1"
        isAi={false}
        selectedCoord={null}
        showHints={true}
        onToggleHints={vi.fn()}
      />
    );

    expect(html).toContain('STEP 1 OF 3: SELECT CHIT');
    expect(html).toContain('Select an oil lease chit from your hand tray below');
    expect(html).toContain('Hints: ON');
  });

  it('renders Step 2 instruction and drill action button when tile is selected', () => {
    const html = renderToStaticMarkup(
      <TurnGuidanceBar
        phase="place_tile"
        activePlayerName="Player 1"
        isAi={false}
        selectedCoord={{ row: 3, col: 4 }} // D-5
        showHints={true}
        onToggleHints={vi.fn()}
        onPlaceSelected={vi.fn()}
      />
    );

    expect(html).toContain('STEP 2 OF 3: DRILL WELL');
    expect(html).toContain('Chit [D-5] selected!');
    expect(html).toContain('Drill Well at D-5');
  });

  it('renders Step 3 stock market actions when in buy_shares phase', () => {
    const html = renderToStaticMarkup(
      <TurnGuidanceBar
        phase="buy_shares"
        activePlayerName="Player 1"
        isAi={false}
        selectedCoord={null}
        cash={4500}
        showHints={true}
        onToggleHints={vi.fn()}
        onOpenStockMarket={vi.fn()}
        onEndTurn={vi.fn()}
      />
    );

    expect(html).toContain('STEP 3 OF 3: STOCK MARKET');
    expect(html).toContain('$4,500 cash available');
    expect(html).toContain('Buy Shares (Alt+B)');
    expect(html).toContain('End Turn / Pass (Alt+E)');
  });

  it('renders CPU status when active player is AI', () => {
    const html = renderToStaticMarkup(
      <TurnGuidanceBar
        phase="place_tile"
        activePlayerName="Computer 1"
        isAi={true}
        selectedCoord={null}
        showHints={false}
        onToggleHints={vi.fn()}
      />
    );

    expect(html).toContain('CPU THINKING');
    expect(html).toContain('Computer 1 (Computer) is calculating move');
    expect(html).toContain('Hints: OFF');
  });

  it('renders repurchase allowance and dividend payout banner when available', () => {
    const html = renderToStaticMarkup(
      <TurnGuidanceBar
        phase="buy_shares"
        activePlayerName="Player 1"
        isAi={false}
        selectedCoord={null}
        cash={3000}
        showHints={true}
        repurchaseAllowance={2}
        lastDividendPayout={{
          playerId: 'p1',
          totalDividend: 450,
          breakdown: { texaco: 450 },
        }}
        onToggleHints={vi.fn()}
      />
    );

    expect(html).toContain('Repurchase Allowance: +2');
    expect(html).toContain('+$450 Dividends');
  });
});

