import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { describe, it, expect } from 'vitest';
import { TurnAdvisor } from '../TurnAdvisor';
import { createEmptyBoard } from '../../engine/board';
import { CLASSIC_COMPANIES } from '../../engine/config';
import type { CellState, CompanyState } from '../../engine/types';

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
};

describe('TurnAdvisor', () => {
  const emptyBoard = createEmptyBoard(10, 12);

  it('renders default prompt when placing tile with no selection', () => {
    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Select an oil parcel from your hand tray/i)).toBeInTheDocument();
  });

  it('renders explanatory text when hovered coord is isolated', () => {
    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={{ row: 3, col: 5 }}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/D-6/i)).toBeInTheDocument();
    expect(screen.getByText(/Isolated well/i)).toBeInTheDocument();
  });

  it('renders stock market guidance during buy_shares phase', () => {
    render(
      <TurnAdvisor
        phase="buy_shares"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Stock Exchange Open/i)).toBeInTheDocument();
  });

  it('renders AI thinking state when isAi is true', () => {
    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={true}
        activePlayerName="Baron AI"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Baron AI \(CPU\) is analyzing the board/i)).toBeInTheDocument();
  });

  it('renders company founding consequence when adjacent to unincorporated well', () => {
    const board: CellState[][] = emptyBoard.map((row) => [...row]);
    board[3]![4] = { type: 'unincorporated' };

    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={{ row: 3, col: 5 }}
        board={board}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Founds a new oil corporation!/i)).toBeInTheDocument();
    expect(screen.getByText(/free founder share/i)).toBeInTheDocument();
  });

  it('renders expansion consequence with new size and stock price', () => {
    const board: CellState[][] = emptyBoard.map((row) => [...row]);
    board[3]![4] = { type: 'company', companyId: 'american' };

    const companies: Record<string, CompanyState> = {
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 2,
        availableShares: 23,
        isActive: true,
        isSafe: false,
      },
    };

    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={{ row: 3, col: 5 }}
        board={board}
        companies={companies}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Expands American Natural Gas/i)).toBeInTheDocument();
    expect(screen.getByText(/Grows chain to 3 wells/i)).toBeInTheDocument();
    expect(screen.getByText(/Share price will rise to \$300/i)).toBeInTheDocument();
  });

  it('renders merger consequence when connecting distinct active companies', () => {
    const board: CellState[][] = emptyBoard.map((row) => [...row]);
    board[3]![4] = { type: 'company', companyId: 'american' };
    board[3]![6] = { type: 'company', companyId: 'imperial' };

    const companies: Record<string, CompanyState> = {
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 3,
        availableShares: 20,
        isActive: true,
        isSafe: false,
      },
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        size: 4,
        availableShares: 20,
        isActive: true,
        isSafe: false,
      },
    };

    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={{ row: 3, col: 5 }}
        board={board}
        companies={companies}
        definitions={CLASSIC_COMPANIES}
      />
    );

    expect(screen.getByText(/Triggers an Oil Merger!/i)).toBeInTheDocument();
  });

  it('renders dead tile consequence when connecting safe companies', () => {
    const board: CellState[][] = emptyBoard.map((row) => [...row]);
    board[3]![4] = { type: 'company', companyId: 'american' };
    board[3]![6] = { type: 'company', companyId: 'imperial' };

    const companies: Record<string, CompanyState> = {
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 11,
        availableShares: 10,
        isActive: true,
        isSafe: true,
      },
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        size: 12,
        availableShares: 10,
        isActive: true,
        isSafe: true,
      },
    };

    render(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={{ row: 3, col: 5 }}
        board={board}
        companies={companies}
        definitions={CLASSIC_COMPANIES}
        safeThreshold={11}
      />
    );

    expect(screen.getByText(/Dead \/ Unplayable Tile/i)).toBeInTheDocument();
  });

  it('renders instructions for special decision phases', () => {
    render(
      <TurnAdvisor
        phase="choose_founding_company"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
      />
    );
    expect(screen.getByText(/Company Incorporation/i)).toBeInTheDocument();

    render(
      <TurnAdvisor
        phase="choose_merger_survivor"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
      />
    );
    expect(screen.getByText(/Merger Tiebreak/i)).toBeInTheDocument();

    render(
      <TurnAdvisor
        phase="dispose_merger_shares"
        isAi={false}
        activePlayerName="Player 1"
        selectedCoord={null}
        hoveredCoord={null}
        board={emptyBoard}
      />
    );
    expect(screen.getByText(/Merger Payouts & Share Disposition/i)).toBeInTheDocument();
  });
});
