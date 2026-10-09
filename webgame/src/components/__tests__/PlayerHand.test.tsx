import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { PlayerHand } from '../PlayerHand';
import { createEmptyBoard } from '../../engine/board';
import type { CellState, CompanyState, TileCoord } from '../../engine/types';

describe('PlayerHand Component', () => {
  const sampleHand: TileCoord[] = [
    { row: 2, col: 3 }, // C-4
    { row: 2, col: 4 }, // C-5
    { row: 5, col: 7 }, // F-8
    { row: 0, col: 0 }, // A-1
    { row: 9, col: 11 }, // J-12
    { row: 4, col: 5 }, // E-6
  ];

  const emptyBoard = createEmptyBoard(10, 12);

  it('renders active player hand tray with formatted coordinate tags', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        activePlayerName="Player 1"
        board={emptyBoard}
      />
    );

    expect(html).toContain('Player 1&#x27;s Hand:');
    expect(html).toContain('6 / 6 chits');
    expect(html).toContain('C-4');
    expect(html).toContain('C-5');
    expect(html).toContain('F-8');
    expect(html).toContain('A-1');
    expect(html).toContain('J-12');
    expect(html).toContain('E-6');
  });

  it('renders selected tile state and enables Place Chit button', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        selectedCoord={{ row: 2, col: 3 }} // C-4
        board={emptyBoard}
      />
    );

    // Selected chit card highlights
    expect(html).toContain('is-selected');
    expect(html).toContain('✓');

    // Place Chit button is enabled and targets C-4
    expect(html).toContain('Place Chit (C-4)');
    expect(html).not.toMatch(/<button[^>]*disabled=""[^>]*>[\s\S]*?Place Chit \(C-4\)/);

    // Status message confirms selection
    expect(html).toContain('Selected:');
    expect(html).toContain('C-4');
  });

  it('disables Place Chit button when no tile is selected', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        selectedCoord={null}
        board={emptyBoard}
      />
    );

    // Button should be disabled
    expect(html).toMatch(/<button[^>]*disabled=""[^>]*>[\s\S]*?<span>Place Chit<\/span>/);
    expect(html).toContain('Click a chit above to select it for placement');
  });

  it('disables Discard Dead Tile button when hand contains no dead tiles', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        board={emptyBoard}
      />
    );

    expect(html).toMatch(/<button[^>]*disabled=""[^>]*>[\s\S]*?<span>Discard Dead Tile<\/span>/);
  });

  it('detects dead tiles, highlights them with warning badge, and enables Discard Dead Tile button', () => {
    // Setup board where C-5 connects Safe Imperial and Safe Royal
    const board = createEmptyBoard(10, 12);
    // Place Imperial at C-4
    board[2]![3] = { type: 'company', companyId: 'imperial' };
    // Place Royal at C-6
    board[2]![5] = { type: 'company', companyId: 'royal' };

    const companies: Record<string, CompanyState> = {
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        size: 11,
        isActive: true,
        isSafe: true, // Safe
        availableShares: 10,
      },
      royal: {
        id: 'royal',
        name: 'Royal Dutch',
        tier: 'high',
        color: '#1d3557',
        size: 12,
        isActive: true,
        isSafe: true, // Safe
        availableShares: 10,
      },
    };

    // sampleHand has { row: 2, col: 4 } which is C-5 (connects C-4 and C-6 -> merges 2 safe companies -> dead!)
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        board={board}
        companies={companies}
        safeThreshold={11}
      />
    );

    // Warning badge in tray header
    expect(html).toContain('1 Dead Chit');

    // Dead tile card styling and badges
    expect(html).toContain('is-dead');
    expect(html).toContain('DEAD');
    expect(html).toContain('ILLEGAL');
    expect(html).toContain('Discard'); // Inline discard button

    // Discard Dead Tile button in actions bar should be ENABLED
    expect(html).toContain('Discard Dead Tile (1)');
    const discardBtnMatch = html.match(/<button[^>]*aria-label="Discard Dead Tile"[^>]*>/);
    expect(discardBtnMatch).not.toBeNull();
    expect(discardBtnMatch![0]).not.toContain('disabled');
  });

  it('disables Place Chit button when selected tile is dead', () => {
    const board = createEmptyBoard(10, 12);
    board[2]![3] = { type: 'company', companyId: 'imperial' };
    board[2]![5] = { type: 'company', companyId: 'royal' };

    const companies: Record<string, CompanyState> = {
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        size: 11,
        isActive: true,
        isSafe: true,
        availableShares: 10,
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

    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        selectedCoord={{ row: 2, col: 4 }} // C-5 is dead
        board={board}
        companies={companies}
        safeThreshold={11}
      />
    );

    expect(html).toContain('Cannot Place Dead Chit');
    expect(html).toMatch(/<button[^>]*disabled=""[^>]*>[\s\S]*?<span>Cannot Place Dead Chit<\/span>/);
    expect(html).toContain('is a dead tile (cannot merge safe corporations)');
  });

  it('renders hotseat privacy concealed view when hideChits is true', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        hideChits={true}
        board={emptyBoard}
      />
    );

    expect(html).toContain('data-testid="hand-privacy-view"');
    expect(html).toContain('Chits concealed for pass-and-play hotseat privacy');
    expect(html).toContain('???');
    expect(html).toContain('Reveal Chits');
    expect(html).toContain('Show Chits');

    // Chits should be obscured; coordinate tags not exposed in clear cards
    expect(html).not.toContain('class="chit-coord-tag">C-4</span>');

    // Action buttons should be disabled in hideChits mode
    expect(html).toMatch(/<button[^>]*disabled=""[^>]*>[\s\S]*?<span>Place Chit<\/span>/);
  });

  it('renders CPU badge when active player is an AI player', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={sampleHand}
        activePlayerName="Computer 1"
        isAi={true}
        board={emptyBoard}
      />
    );

    expect(html).toContain('Computer 1&#x27;s Hand:');
    expect(html).toContain('CPU');
  });

  it('renders notice when hand is empty', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={[]}
        activePlayerName="Player 1"
        board={emptyBoard}
      />
    );

    expect(html).toContain('No chits currently held in hand');
    expect(html).toContain('0 / 6 chits');
  });
});
