import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { PlayerScoreboard, formatCurrency } from '../PlayerScoreboard';
import { CLASSIC_COMPANIES } from '../../engine/config';
import type { CompanyState, PlayerState } from '../../engine/types';

describe('PlayerScoreboard Component', () => {
  const samplePlayers: PlayerState[] = [
    {
      id: 'p1',
      name: 'J.R. Ewing',
      isAi: false,
      cash: 6000,
      shares: {
        american: 10,
        imperial: 5,
        royal: 0,
      },
      hand: [
        { row: 0, col: 0 },
        { row: 1, col: 1 },
        { row: 2, col: 2 },
        { row: 3, col: 3 },
        { row: 4, col: 4 },
        { row: 5, col: 5 },
      ],
    },
    {
      id: 'p2',
      name: 'HAL 9000 (CPU)',
      isAi: true,
      cash: 4500,
      shares: {
        american: 2,
        imperial: 0,
        royal: 8,
      },
      hand: [
        { row: 6, col: 6 },
        { row: 7, col: 7 },
        { row: 8, col: 8 },
        { row: 9, col: 9 },
      ],
    },
    {
      id: 'p3',
      name: 'Wildcatter Bob',
      isAi: false,
      cash: 8000,
      shares: {},
      hand: [],
    },
  ];

  const sampleCompanies: Record<string, CompanyState> = {
    american: {
      id: 'american',
      name: 'American Natural Gas',
      tier: 'low',
      color: '#e63946',
      size: 5, // price: $200 + 3 * $100 = $500
      isActive: true,
      isSafe: false,
      availableShares: 13,
    },
    imperial: {
      id: 'imperial',
      name: 'Imperial Petroleum',
      tier: 'mid',
      color: '#2a9d8f',
      size: 4, // price: $300 + 2 * $100 = $500
      isActive: true,
      isSafe: false,
      availableShares: 20,
    },
    royal: {
      id: 'royal',
      name: 'Royal Dutch',
      tier: 'high',
      color: '#1d3557',
      size: 11, // price: $400 + 9 * $100 = $1,300
      isActive: true,
      isSafe: true,
      availableShares: 17,
    },
  };

  it('formats currency correctly with dollar signs and comma separators', () => {
    expect(formatCurrency(0)).toBe('$0');
    expect(formatCurrency(6000)).toBe('$6,000');
    expect(formatCurrency(12450)).toBe('$12,450');
    expect(formatCurrency(1000000)).toBe('$1,000,000');
  });

  it('renders all players with names, index tags, and total count', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={0}
        companies={sampleCompanies}
      />
    );

    expect(html).toContain('SCOREBOARD');
    expect(html).toContain('3 Players');
    expect(html).toContain('#1');
    expect(html).toContain('J.R. Ewing');
    expect(html).toContain('#2');
    expect(html).toContain('HAL 9000 (CPU)');
    expect(html).toContain('#3');
    expect(html).toContain('Wildcatter Bob');
  });

  it('highlights the active player card and displays TURN badge', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={1} // HAL 9000 is active
        companies={sampleCompanies}
      />
    );

    // Player 2 card should have is-active class and aria-current="step"
    expect(html).toContain('data-testid="player-card-1"');
    const card1Match = html.match(/<article[^>]*data-testid="player-card-1"[^>]*>/);
    expect(card1Match).not.toBeNull();
    expect(card1Match![0]).toContain('is-active');

    // Player 1 card should NOT be active
    const card0Match = html.match(/<article[^>]*data-testid="player-card-0"[^>]*>/);
    expect(card0Match).not.toBeNull();
    expect(card0Match![0]).not.toContain('is-active');

    // Active turn badge rendered for player 2
    expect(html).toContain('data-testid="badge-active-turn-1"');
    expect(html).toContain('TURN');
    expect(html).not.toContain('data-testid="badge-active-turn-0"');
  });

  it('renders Human and CPU badges accurately for each player', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={0}
      />
    );

    expect(html).toContain('badge-human');
    expect(html).toContain('badge-cpu');

    // Player 0 is Human
    const badge0Match = html.match(/<span[^>]*data-testid="badge-type-0"[^>]*>[\s\S]*?HUMAN<\/span>/);
    expect(badge0Match).not.toBeNull();

    // Player 1 is CPU
    const badge1Match = html.match(/<span[^>]*data-testid="badge-type-1"[^>]*>[\s\S]*?CPU<\/span>/);
    expect(badge1Match).not.toBeNull();
  });

  it('renders inline toggle button per player (Change to CPU vs Change to Human)', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={0}
      />
    );

    // Human player 0 has button to Change to CPU
    const toggle0Match = html.match(
      /<button[^>]*data-testid="btn-toggle-ai-0"[^>]*>[\s\S]*?Change to CPU[\s\S]*?<\/button>/
    );
    expect(toggle0Match).not.toBeNull();

    // CPU player 1 has button to Change to Human
    const toggle1Match = html.match(
      /<button[^>]*data-testid="btn-toggle-ai-1"[^>]*>[\s\S]*?Change to Human[\s\S]*?<\/button>/
    );
    expect(toggle1Match).not.toBeNull();
  });

  it('calculates net worth including held shares at market prices and awards leader badge', () => {
    // Player 1 (J.R.):
    // Cash: $6,000
    // American: 10 shares @ $500 = $5,000
    // Imperial: 5 shares @ $500 = $2,500
    // Total Net Worth = $6,000 + $5,000 + $2,500 = $13,500
    //
    // Player 2 (HAL 9000):
    // Cash: $4,500
    // American: 2 shares @ $500 = $1,000
    // Royal: 8 shares @ $1,300 = $10,400
    // Total Net Worth = $4,500 + $1,000 + $10,400 = $15,900 (LEADER!)
    //
    // Player 3 (Bob):
    // Cash: $8,000, 0 shares -> $8,000

    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={0}
        companies={sampleCompanies}
      />
    );

    expect(html).toContain('$13,500'); // J.R. net worth
    expect(html).toContain('$15,900'); // HAL 9000 net worth
    expect(html).toContain('$8,000');  // Bob net worth

    // Leader badge awarded to HAL 9000 (highest net worth $15,900)
    expect(html).toContain('player-leader-badge');
    expect(html).toContain('#1');
  });

  it('renders shareholdings portfolio summary with corporate codes and counts', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={0}
        companies={sampleCompanies}
      />
    );

    // Player 0 owns American (10) and Imperial (5)
    expect(html).toContain('data-testid="share-0-american"');
    expect(html).toContain('data-testid="share-0-imperial"');
    expect(html).toContain('AM');
    expect(html).toContain('IM');
    expect(html).toContain('Shares (15)'); // 10 + 5 = 15 total shares

    // Player 2 owns no shares
    expect(html).toContain('No shares owned');
  });

  it('renders hand chits count for each player', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={samplePlayers}
        activePlayerIndex={0}
      />
    );

    expect(html).toContain('6 chits');
    expect(html).toContain('4 chits');
    expect(html).toContain('0 chits');
  });

  it('supports full 6-player roster gracefully', () => {
    const sixPlayers: PlayerState[] = [
      ...samplePlayers,
      { id: 'p4', name: 'Player 4', isAi: true, cash: 6000, shares: {}, hand: [] },
      { id: 'p5', name: 'Player 5', isAi: false, cash: 6000, shares: {}, hand: [] },
      { id: 'p6', name: 'Player 6', isAi: true, cash: 6000, shares: {}, hand: [] },
    ];

    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={sixPlayers}
        activePlayerIndex={2}
      />
    );

    expect(html).toContain('6 Players');
    expect(html).toContain('Player 4');
    expect(html).toContain('Player 5');
    expect(html).toContain('Player 6');
    expect(html).toContain('#6');
  });
});
