import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { NewGameModal } from '../NewGameModal';
import { OptionsModal } from '../OptionsModal';
import { FoundingCompanyModal } from '../FoundingCompanyModal';
import { DiscardedTilesModal } from '../DiscardedTilesModal';
import { CLASSIC_CONFIG, createInitialGameState } from '../../../engine/config';

describe('Task 5 Modals', () => {
  const dummyState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob']);

  describe('NewGameModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <NewGameModal
          isOpen={false}
          onClose={vi.fn()}
          onStartGame={vi.fn()}
        />
      );
      expect(html).toBe('');
    });

    it('renders player setup when isOpen is true', () => {
      const html = renderToStaticMarkup(
        <NewGameModal
          isOpen={true}
          onClose={vi.fn()}
          onStartGame={vi.fn()}
        />
      );
      expect(html).toContain('New Match — Player Setup');
      expect(html).toContain('Quick Roster Presets');
      expect(html).toContain('Start Game');
    });

    it('renders AI difficulty selector (Smart AI / Dumb AI) for CPU players', () => {
      const html = renderToStaticMarkup(
        <NewGameModal
          isOpen={true}
          onClose={vi.fn()}
          onStartGame={vi.fn()}
          initialPlayers={[
            { id: 'p1', name: 'Alice', color: '#f59e0b', isAi: false },
            { id: 'p2', name: 'Bot 1', color: '#38bdf8', isAi: true, aiDifficulty: 'dumb' },
          ]}
        />
      );
      // AI difficulty selector should be present for CPU player
      expect(html).toContain('ai-difficulty-select');
      expect(html).toContain('Smart AI');
      expect(html).toContain('Dumb AI');
      expect(html).toContain('value="dumb" selected=""');
    });
  });

  describe('OptionsModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <OptionsModal
          isOpen={false}
          onClose={vi.fn()}
          config={CLASSIC_CONFIG}
          onSaveConfig={vi.fn()}
          slowCpu={false}
        />
      );
      expect(html).toBe('');
    });

    it('renders ruleset paradigms and slider controls when isOpen is true', () => {
      const html = renderToStaticMarkup(
        <OptionsModal
          isOpen={true}
          onClose={vi.fn()}
          config={CLASSIC_CONFIG}
          onSaveConfig={vi.fn()}
          slowCpu={false}
        />
      );
      expect(html).toContain('Game Options &amp; Ruleset');
      expect(html).toContain('1993 Oil Baron Classic');
      expect(html).toContain('Modern Balanced');
      expect(html).toContain('Safe Company Threshold');
      expect(html).toContain('Slow Computer Turns');
      expect(html).toContain('Show Discarded Markers (&#x27;X&#x27;)');
    });
  });

  describe('FoundingCompanyModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <FoundingCompanyModal
          isOpen={false}
          gameState={dummyState}
          availableCompanyIds={['american', 'imperial']}
          onSelectCompany={vi.fn()}
        />
      );
      expect(html).toBe('');
    });

    it('renders available corporations and founder bonus badge', () => {
      const html = renderToStaticMarkup(
        <FoundingCompanyModal
          isOpen={true}
          gameState={dummyState}
          availableCompanyIds={['american', 'imperial']}
          onSelectCompany={vi.fn()}
        />
      );
      expect(html).toContain('Form New Oil Company');
      expect(html).toContain('+10 Free Shares');
      expect(html).toContain('American');
      expect(html).toContain('Imperial');
      expect(html).toContain('Incorporate &amp; Claim Bonus');
    });
  });

  describe('DiscardedTilesModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <DiscardedTilesModal
          isOpen={false}
          onClose={vi.fn()}
          gameState={dummyState}
        />
      );
      expect(html).toBe('');
    });

    it('renders metrics summary and navigation tabs', () => {
      const html = renderToStaticMarkup(
        <DiscardedTilesModal
          isOpen={true}
          onClose={vi.fn()}
          gameState={dummyState}
        />
      );
      expect(html).toContain('Discarded &amp; Dead Pieces');
      expect(html).toContain('Permanently Discarded');
      expect(html).toContain('Dead Tiles in Hands');
      expect(html).toContain('Remaining in Bag');
    });
  });
});
