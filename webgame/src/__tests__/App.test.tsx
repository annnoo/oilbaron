import { describe, it, expect } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import App from '../App';

describe('App Integration', () => {
  it('renders modern App shell containing GameHeader, TurnAdvisor, CompanyTicker, Board, ActionDock, and Scoreboard', () => {
    const html = renderToStaticMarkup(<App />);

    // Modern GameHeader
    expect(html).toContain('class="game-header"');
    expect(html).toContain('OIL BARON');
    expect(html).toContain('STRATEGY');
    expect(html).toContain('Player 1');
    expect(html).toContain('Step 1: Drill Well');
    expect(html).toContain('aria-label="ledger"');
    expect(html).toContain('aria-label="sound"');
    expect(html).toContain('aria-label="language"');
    expect(html).toContain('aria-label="rules"');
    expect(html).toContain('aria-label="new game"');
    expect(html).toContain('aria-label="save load"');
    expect(html).toContain('aria-label="options"');
    expect(html).toContain('aria-label="classic view"');
    expect(html).toContain('1993 View');

    // Legacy MenuBar should NOT be present in modern view mode
    expect(html).not.toContain('role="menubar"');
    expect(html).not.toContain('<span class="mnemonic">G</span>ame');

    // Company ticker
    expect(html).toContain('role="region"');
    expect(html).toContain('MARKET');
    expect(html).toContain('Available:');

    // Modern TurnAdvisor banner
    expect(html).toContain('class="turn-advisor-banner"');
    expect(html).toContain('class="advisor-content"');
    expect(html).toContain('Select an oil parcel from your hand tray below to drill');

    // Redundant legacy TurnGuidanceBar should NOT be present in modern view mode
    expect(html).not.toContain('Turn Guidance &amp; Onboarding');

    // Game board integration
    expect(html).toContain('Oil Baron 10x12 Game Board');
    expect(html).toContain('10 × 12 Grid');
    expect(html).toContain('role="grid"');

    // Modern ActionDock wrapping player hand
    expect(html).toContain('class="unified-action-dock"');
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
    // Retro menu and guidance inside classic window
    expect(html).toContain('role="menubar"');
    expect(html).toContain('STEP 1 OF 3: SELECT CHIT');
  });
});
