import { describe, it, expect } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import App from '../App';

describe('App Integration', () => {
  it('renders App shell containing MenuBar, CompanyTicker, Board, Hand, and Scoreboard', () => {
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

    // Turn status banner
    expect(html).toContain('Player 1');
    expect(html).toContain('Turn (Human)');
    expect(html).toContain('PLACE TILE');

    // Game board integration
    expect(html).toContain('Oil Baron 10x12 Game Board');
    expect(html).toContain('10 × 12 Grid');
    expect(html).toContain('role="grid"');

    // Turn Guidance Bar with Beginner Hints
    expect(html).toContain('Turn Guidance &amp; Onboarding');
    expect(html).toContain('STEP 1 OF 3: SELECT CHIT');
    expect(html).toContain('Hints: ON');

    // 1993 Classic switcher button
    expect(html).toContain('1993 Classic View');

    // PlayerHand Tray
    expect(html).toContain('data-testid="player-hand-tray"');
    expect(html).toContain('Place Chit');
    expect(html).toContain('Discard Dead Tile');
    expect(html).toContain('Hide Chits');

    // PlayerScoreboard
    expect(html).toContain('data-testid="player-scoreboard"');
    expect(html).toContain('SCOREBOARD');
    expect(html).toContain('2 Players');
    expect(html).toContain('Change to CPU');
    expect(html).toContain('Change to Human');
  });

  it('renders 1993 Windows 3.1 Classic View when initialViewMode is classic', () => {
    const html = renderToStaticMarkup(<App initialViewMode="classic" />);
    expect(html).toContain('win31-root');
    expect(html).toContain('Oil Baron (1993 Windows 3.1 Classic Edition)');
    expect(html).toContain('Switch to Modern Studio UI');
    expect(html).toContain('win31-window');
    expect(html).toContain('win31-titlebar');
    expect(html).toContain('data-testid="classic-board"');
    // Retro guidance inside classic window
    expect(html).toContain('STEP 1 OF 3: SELECT CHIT');
  });
});
