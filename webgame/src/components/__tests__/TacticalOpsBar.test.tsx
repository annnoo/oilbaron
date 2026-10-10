import React from 'react';
import { describe, it, expect, vi } from 'vitest';
import { renderToStaticMarkup } from 'react-dom/server';
import { TacticalOpsBar } from '../TacticalOpsBar';
import { CLASSIC_COMPANIES } from '../../engine/config';

describe('TacticalOpsBar', () => {
  it('renders player funds, directive, and active pipeline tickers', () => {
    const html = renderToStaticMarkup(
      <TacticalOpsBar
        activePlayerName="Player 1"
        activePlayerCash={6400}
        activePlayerNetWorth={14200}
        activePlayerColor="#f59e0b"
        isAi={false}
        turnPhase="place_tile"
        directiveText="Drill parcel B-5 to expand American Oil field"
        directivePhase="Survey & Drill Operation"
        companies={{
          american: { id: 'american', isActive: true, isSafe: false, size: 5, stockPrice: 500, availableShares: 10, tiles: [] } as any
        }}
        definitions={CLASSIC_COMPANIES}
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenOptions={vi.fn()}
      />
    );

    expect(html).toContain('Player 1');
    expect(html).toContain('$6,400');
    expect(html).toContain('$14,200');
    expect(html).toContain('Drill parcel B-5');
    expect(html).toContain('Survey &amp; Drill Operation');
    expect(html).toContain('American Natural Gas');
    expect(html).toContain('$500 · 5 wells');
  });

  it('renders (CPU) badge for AI players', () => {
    const html = renderToStaticMarkup(
      <TacticalOpsBar
        activePlayerName="Baron Von Bot"
        activePlayerCash={8000}
        activePlayerColor="#10b981"
        isAi={true}
        turnPhase="place_tile"
        directiveText="Computing optimal parcel"
        directivePhase="Autonomous Survey"
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        soundMuted={true}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenOptions={vi.fn()}
      />
    );

    expect(html).toContain('Baron Von Bot (CPU)');
    expect(html).toContain('$8,000');
    expect(html).toContain('No active corporate pipeline networks.');
  });

  it('renders REFINERY HUB badge when company is safe', () => {
    const html = renderToStaticMarkup(
      <TacticalOpsBar
        activePlayerName="Player 2"
        activePlayerCash={3200}
        isAi={false}
        turnPhase="buy_shares"
        directiveText="Acquire stock shares"
        directivePhase="Stock Acquisition"
        companies={{
          royal: { id: 'royal', isActive: true, isSafe: true, size: 12, stockPrice: 1400, availableShares: 5 } as any
        }}
        definitions={CLASSIC_COMPANIES}
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenOptions={vi.fn()}
      />
    );

    expect(html).toContain('Royal Dutch');
    expect(html).toContain('REFINERY HUB');
    expect(html).toContain('$1400 · 12 wells');
  });

  it('renders classic view switch button when callback is provided', () => {
    const onSwitch = vi.fn();
    const html = renderToStaticMarkup(
      <TacticalOpsBar
        activePlayerName="Player 1"
        activePlayerCash={5000}
        isAi={false}
        turnPhase="place_tile"
        directiveText="Select a tile"
        directivePhase="Survey"
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenOptions={vi.fn()}
        onSwitchClassic={onSwitch}
      />
    );

    expect(html).toContain('aria-label="classic view"');
    expect(html).toContain('Switch to 1993 View');
    expect(html).toContain('Mute Audio');
  });

  it('renders dynamic price using calculateSharePrice when stockPrice is not precomputed', () => {
    // Imperial (tier mid, base 300, delta 100). Size 4 -> 300 + 2*100 = 500
    const html = renderToStaticMarkup(
      <TacticalOpsBar
        activePlayerName="Investor"
        activePlayerCash={4500}
        isAi={false}
        turnPhase="place_tile"
        directiveText="Inspect fields"
        directivePhase="Operations"
        companies={{
          imperial: { id: 'imperial', name: 'Imperial Petroleum', tier: 'mid', color: '#2a9d8f', isActive: true, isSafe: false, size: 4, availableShares: 20 }
        }}
        definitions={CLASSIC_COMPANIES}
        soundMuted={true}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenOptions={vi.fn()}
      />
    );

    expect(html).toContain('Imperial Petroleum');
    expect(html).toContain('$500 · 4 wells');
    expect(html).toContain('Unmute Audio');
  });

  it('triggers onToggleLedger, onToggleSound, onOpenOptions, and onSwitchClassic on click', () => {
    const onToggleSound = vi.fn();
    const onToggleLedger = vi.fn();
    const onOpenOptions = vi.fn();
    const onSwitchClassic = vi.fn();

    const element = TacticalOpsBar({
      activePlayerName: 'Player 1',
      activePlayerCash: 5000,
      isAi: false,
      turnPhase: 'place_tile',
      directiveText: 'Directive',
      directivePhase: 'Phase',
      companies: {},
      definitions: CLASSIC_COMPANIES,
      soundMuted: false,
      language: 'en',
      onToggleSound,
      onToggleLedger,
      onOpenOptions,
      onSwitchClassic,
    }) as any;

    // Structure: header.ops-bar-container > div.ops-bar (index 0) > div.ops-controls (index 2)
    const opsBar = element.props.children[0];
    const opsControls = opsBar.props.children[2];
    const buttons = opsControls.props.children;

    const [ledgerBtn, soundBtn, settingsBtn, classicBtn] = buttons;

    ledgerBtn.props.onClick();
    expect(onToggleLedger).toHaveBeenCalledTimes(1);

    soundBtn.props.onClick();
    expect(onToggleSound).toHaveBeenCalledTimes(1);

    settingsBtn.props.onClick();
    expect(onOpenOptions).toHaveBeenCalledTimes(1);

    classicBtn.props.onClick();
    expect(onSwitchClassic).toHaveBeenCalledTimes(1);
  });
});
