import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { WealthChartModal } from '../WealthChartModal';
import { ShareGraphModal } from '../ShareGraphModal';
import { WarRoomModal } from '../WarRoomModal';
import { SaveLoadModal } from '../SaveLoadModal';
import { HelpModal } from '../HelpModal';
import { GameOverModal } from '../GameOverModal';
import { CLASSIC_CONFIG, createInitialGameState } from '../../../engine/config';

describe('Task 8 Visualization & Utility Modals', () => {
  const dummyState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob']);

  describe('WealthChartModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <WealthChartModal isOpen={false} onClose={vi.fn()} gameState={dummyState} />
      );
      expect(html).toBe('');
    });

    it('renders net worth and liquid cash breakdown when open', () => {
      const html = renderToStaticMarkup(
        <WealthChartModal isOpen={true} onClose={vi.fn()} gameState={dummyState} />
      );
      expect(html).toContain('Wealth Bar Chart — Net Worth Valuation');
      expect(html).toContain('Liquid Cash');
      expect(html).toContain('Stock Portfolio');
      expect(html).toContain('Alice');
      expect(html).toContain('Bob');
    });
  });

  describe('ShareGraphModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <ShareGraphModal isOpen={false} onClose={vi.fn()} gameState={dummyState} />
      );
      expect(html).toBe('');
    });

    it('renders stock ownership matrix and mode switcher when open', () => {
      const html = renderToStaticMarkup(
        <ShareGraphModal isOpen={true} onClose={vi.fn()} gameState={dummyState} />
      );
      expect(html).toContain('Share Graph — Stock Ownership Matrix');
      expect(html).toContain('Totals');
      expect(html).toContain('Percentages (%)');
      expect(html).toContain('Bank Reserve');
    });
  });

  describe('WarRoomModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <WarRoomModal isOpen={false} onClose={vi.fn()} gameState={dummyState} />
      );
      expect(html).toBe('');
    });

    it('renders territory occupancy and strategic metrics when open', () => {
      const html = renderToStaticMarkup(
        <WarRoomModal isOpen={true} onClose={vi.fn()} gameState={dummyState} />
      );
      expect(html).toContain('War Room — Strategic Intelligence');
      expect(html).toContain('Grid Occupancy');
      expect(html).toContain('Safe Corporations');
      expect(html).toContain('Tiles in Draw Bag');
    });
  });

  describe('SaveLoadModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <SaveLoadModal isOpen={false} onClose={vi.fn()} gameState={dummyState} onLoadGame={vi.fn()} />
      );
      expect(html).toBe('');
    });

    it('renders save/load actions and JSON export/import when open', () => {
      const html = renderToStaticMarkup(
        <SaveLoadModal isOpen={true} onClose={vi.fn()} gameState={dummyState} onLoadGame={vi.fn()} />
      );
      expect(html).toContain('Save &amp; Load Match State');
      expect(html).toContain('Save Game');
      expect(html).toContain('Export Save as JSON');
      expect(html).toContain('Import JSON Save');
    });
  });

  describe('HelpModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(<HelpModal isOpen={false} onClose={vi.fn()} />);
      expect(html).toBe('');
    });

    it('renders rules of play and navigation tabs when open', () => {
      const html = renderToStaticMarkup(<HelpModal isOpen={true} onClose={vi.fn()} />);
      expect(html).toContain('Oil Baron — Rules of Play &amp; Strategy Guide');
      expect(html).toContain('Overview &amp; Board');
      expect(html).toContain('Founding Companies');
      expect(html).toContain('Stock Market');
      expect(html).toContain('Mergers &amp; Payouts');
    });
  });

  describe('GameOverModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <GameOverModal isOpen={false} onClose={vi.fn()} gameState={dummyState} onNewGame={vi.fn()} />
      );
      expect(html).toBe('');
    });

    it('renders winner celebration banner and final leaderboard when open', () => {
      const html = renderToStaticMarkup(
        <GameOverModal isOpen={true} onClose={vi.fn()} gameState={dummyState} onNewGame={vi.fn()} />
      );
      expect(html).toContain('Match Concluded — The Oil Baron Winner!');
      expect(html).toContain('Grand Petroleum Baron');
      expect(html).toContain('Final Portfolio &amp; Liquidated Wealth Leaderboard');
      expect(html).toContain('Start New Game');
    });
  });
});
