import React from 'react';
import { describe, it, expect, vi } from 'vitest';
import { renderToStaticMarkup } from 'react-dom/server';
import { ConcessionsDock } from '../ConcessionsDock';
import { CLASSIC_COMPANIES } from '../../engine/config';
import { createEmptyBoard } from '../../engine/board';
import type { CompanyState } from '../../engine/types';

describe('ConcessionsDock', () => {
  const emptyBoard = createEmptyBoard(10, 12);

  const makeCompany = (overrides: Partial<CompanyState> = {}): CompanyState => ({
    id: 'american',
    name: 'American',
    tier: 'low',
    color: '#ef4444',
    size: 2,
    isActive: true,
    isSafe: false,
    availableShares: 20,
    ...overrides,
  });

  it('renders drilling leases and drill actuator button during place_tile', () => {
    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[{ row: 1, col: 4 }]}
        selectedCoord={{ row: 1, col: 4 }}
        hoveredCoord={null}
        activePlayerCash={6400}
        board={emptyBoard}
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

    expect(html).toContain('concessions-dock');
    expect(html).toContain('leases-tray');
    expect(html).toContain('lease-slip');
    expect(html).toContain('B-5');
    expect(html).toContain('drill-actuator');
    expect(html).toContain('DRILL WELL');
    expect(html).toContain('PARCEL B-5');
  });

  it('renders disabled drill actuator button when no parcel lease is selected', () => {
    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[{ row: 1, col: 4 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6400}
        board={emptyBoard}
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

    expect(html).toContain('disabled');
    expect(html).toContain('SELECT A PARCEL LEASE');
  });

  it('renders tactical move category tags for leases (EXPAND, FOUND, MERGER, WILDCAT/OPEN, DEAD/DISCARD)', () => {
    // Put a company on the board next to row 0, col 0
    const boardWithCompany = createEmptyBoard(10, 12);
    boardWithCompany[0]![1] = { type: 'company', companyId: 'american' };

    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[{ row: 0, col: 0 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6400}
        board={boardWithCompany}
        companies={{ american: makeCompany({ id: 'american', size: 2, isActive: true }) }}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('lease-chip-expand');
    expect(html).toContain('EXPAND');
  });

  it('renders dead tile status and allows discard when dead tile is present', () => {
    const boardWithTwoSafe = createEmptyBoard(10, 12);
    boardWithTwoSafe[0]![0] = { type: 'company', companyId: 'american' };
    boardWithTwoSafe[0]![2] = { type: 'company', companyId: 'imperial' };

    const deadCoord = { row: 0, col: 1 };

    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="place_tile"
        isCurrentTurn={true}
        hand={[deadCoord]}
        selectedCoord={deadCoord}
        hoveredCoord={null}
        activePlayerCash={6400}
        board={boardWithTwoSafe}
        companies={{
          american: makeCompany({ id: 'american', size: 11, isActive: true, isSafe: true }),
          imperial: makeCompany({ id: 'imperial', name: 'Imperial', size: 11, isActive: true, isSafe: true }),
        }}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('lease-chip-discard');
    expect(html).toContain('DISCARD');
  });

  it('renders petroleum equity exchange (InlineStockMarket) during buy_shares phase', () => {
    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="buy_shares"
        isCurrentTurn={true}
        hand={[{ row: 1, col: 4 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6400}
        board={emptyBoard}
        companies={{ american: makeCompany({ id: 'american', size: 3, isActive: true }) }}
        definitions={CLASSIC_COMPANIES}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('inline-stock-shelf');
  });

  it('forwards share liquidation callbacks and playerShares to InlineStockMarket in buy_shares phase', () => {
    const onConfirmSales = vi.fn();
    const onSellAll = vi.fn();
    const playerShares = { american: 5 };

    const html = renderToStaticMarkup(
      <ConcessionsDock
        phase="buy_shares"
        isCurrentTurn={true}
        hand={[{ row: 1, col: 4 }]}
        selectedCoord={null}
        hoveredCoord={null}
        activePlayerCash={6400}
        board={emptyBoard}
        companies={{ american: makeCompany({ id: 'american', size: 3, isActive: true }) }}
        definitions={CLASSIC_COMPANIES}
        playerShares={playerShares}
        onSelectTile={vi.fn()}
        onHoverTile={vi.fn()}
        onPlaceTile={vi.fn()}
        onDiscardDeadTile={vi.fn()}
        onConfirmPurchases={vi.fn()}
        onConfirmSales={onConfirmSales}
        onSellAll={onSellAll}
        onPassTurn={vi.fn()}
      />
    );

    expect(html).toContain('inline-stock-shelf');
    expect(html).toContain('LIQUIDATE SHARES (SELL)');
    expect(html).toContain('tab-shares-badge');
    expect(html).toContain('>5<');
  });
});

