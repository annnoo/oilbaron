import { describe, it, expect } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import App from '../App';

describe('App Integration', () => {
  it('renders App shell containing MenuBar and CompanyTicker', () => {
    const html = renderToStaticMarkup(<App />);

    // Top menu bar
    expect(html).toContain('role="menubar"');
    expect(html).toContain('<span class="mnemonic">G</span>ame');
    expect(html).toContain('<span class="mnemonic">A</span>ctions');
    expect(html).toContain('<span class="mnemonic">V</span>iews');
    expect(html).toContain('<span class="mnemonic">S</span>ettings');
    expect(html).toContain('<span class="mnemonic">H</span>elp');

    // Company ticker
    expect(html).toContain('role="region"');
    expect(html).toContain('MARKET');
    expect(html).toContain('Available:');

    // App panel verification
    expect(html).toContain('Company Ticker Controls');
    expect(html).toContain('Found American');
    expect(html).toContain('Make American Safe (11 tiles)');

    // Game board integration
    expect(html).toContain('Oil Baron 10x12 Game Board');
    expect(html).toContain('10 × 12 Grid');
    expect(html).toContain('role="grid"');
    expect(html).toContain('Player 1&#x27;s Hand:');
    expect(html).toContain('Seed Founding');
    expect(html).toContain('Seed Merger');
    expect(html).toContain('Seed Dead Tile');

    // Task 4: PlayerHand Tray
    expect(html).toContain('data-testid="player-hand-tray"');
    expect(html).toContain('Place Chit');
    expect(html).toContain('Discard Dead Tile');
    expect(html).toContain('Hide Chits');

    // Task 4: PlayerScoreboard
    expect(html).toContain('data-testid="player-scoreboard"');
    expect(html).toContain('SCOREBOARD');
    expect(html).toContain('2 Players');
    expect(html).toContain('Change to CPU');
    expect(html).toContain('Change to Human');
  });
});
