import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { MergerSurvivorModal } from '../MergerSurvivorModal';
import { MergerStatsModal } from '../MergerStatsModal';
import { ShareDispositionModal } from '../ShareDispositionModal';
import { CLASSIC_CONFIG, createInitialGameState } from '../../../engine/config';
import { GameState } from '../../../engine/types';

describe('Task 7 Merger Modals', () => {
  const dummyState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob']);

  describe('MergerSurvivorModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <MergerSurvivorModal
          isOpen={false}
          gameState={dummyState}
          tiedCompanyIds={['american', 'arabian']}
          onChooseSurvivor={vi.fn()}
        />
      );
      expect(html).toBe('');
    });

    it('renders tied corporations and tie-break instructions when open', () => {
      const html = renderToStaticMarkup(
        <MergerSurvivorModal
          isOpen={true}
          gameState={dummyState}
          tiedCompanyIds={['american', 'arabian']}
          onChooseSurvivor={vi.fn()}
        />
      );
      expect(html).toContain('Merger Conflict — Tie-Break Decision');
      expect(html).toContain('Multiple Corporations Tie for Largest Size!');
      expect(html).toContain('American Natural Gas');
      expect(html).toContain('Arabian Oil');
      expect(html).toContain('Confirm Survivor');
    });
  });

  describe('MergerStatsModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <MergerStatsModal
          isOpen={false}
          onClose={vi.fn()}
          gameState={dummyState}
          defunctCompanyId="american"
          survivorCompanyId="imperial"
          preMergerPrice={400}
        />
      );
      expect(html).toBe('');
    });

    it('renders bonus pools and payout distribution table when open', () => {
      const stateWithHoldings: GameState = {
        ...dummyState,
        players: [
          {
            ...dummyState.players[0]!,
            shares: { ...dummyState.players[0]!.shares, american: 10 },
          },
          {
            ...dummyState.players[1]!,
            shares: { ...dummyState.players[1]!.shares, american: 4 },
          },
        ],
      };

      const html = renderToStaticMarkup(
        <MergerStatsModal
          isOpen={true}
          onClose={vi.fn()}
          gameState={stateWithHoldings}
          defunctCompanyId="american"
          survivorCompanyId="imperial"
          preMergerPrice={500}
        />
      );
      expect(html).toContain('Merger Payouts &amp; Shareholder Bonuses');
      expect(html).toContain('1st Majority Bonus Pool (10×)');
      expect(html).toContain('$5,000');
      expect(html).toContain('2nd Minority Bonus Pool (5×)');
      expect(html).toContain('$2,500');
      expect(html).toContain('Alice');
      expect(html).toContain('+$5,000');
      expect(html).toContain('Bob');
      expect(html).toContain('+$2,500');
      expect(html).toContain('Proceed to Share Disposition');
    });
  });

  describe('ShareDispositionModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <ShareDispositionModal
          isOpen={false}
          gameState={dummyState}
          defunctCompanyId="american"
          survivorCompanyId="imperial"
          preMergerPrice={300}
          onConfirmDisposition={vi.fn()}
        />
      );
      expect(html).toBe('');
    });

    it('renders disposition controls (sell, trade, hold) for active shareholder', () => {
      const mergerState: GameState = {
        ...dummyState,
        pendingMerger: {
          coord: { row: 0, col: 0 },
          participatingCompanyIds: ['american', 'imperial'],
          survivingCompanyId: 'imperial',
          defunctCompanyQueue: ['american'],
          currentDefunctCompanyId: 'american',
          shareholderQueue: [dummyState.players[0]!.id],
          currentShareholderIndex: 0,
          preMergerPrices: { american: 300, imperial: 500 },
        },
        players: [
          {
            ...dummyState.players[0]!,
            shares: { ...dummyState.players[0]!.shares, american: 6 },
          },
          dummyState.players[1]!,
        ],
      };

      const html = renderToStaticMarkup(
        <ShareDispositionModal
          isOpen={true}
          gameState={mergerState}
          defunctCompanyId="american"
          survivorCompanyId="imperial"
          preMergerPrice={300}
          onConfirmDisposition={vi.fn()}
        />
      );
      expect(html).toContain('Merger Share Disposition');
      expect(html).toContain('Shareholder Disposition Turn');
      expect(html).toContain('Sell Shares');
      expect(html).toContain('Trade 2-for-1');
      expect(html).toContain('Hold Defunct Shares');
      expect(html).toContain('Confirm Share Disposition');
    });
  });
});
