import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { StockMarketModal } from '../StockMarketModal';
import { CompanyStatsModal } from '../CompanyStatsModal';
import { CLASSIC_CONFIG, createInitialGameState } from '../../../engine/config';

describe('Task 6 Modals', () => {
  const dummyState = createInitialGameState(CLASSIC_CONFIG, ['Alice', 'Bob']);

  describe('StockMarketModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <StockMarketModal
          isOpen={false}
          onClose={vi.fn()}
          gameState={dummyState}
          onConfirmPurchase={vi.fn()}
        />
      );
      expect(html).toBe('');
    });

    it('renders stock exchange dialog and purchase quota when open', () => {
      const html = renderToStaticMarkup(
        <StockMarketModal
          isOpen={true}
          onClose={vi.fn()}
          gameState={dummyState}
          onConfirmPurchase={vi.fn()}
        />
      );
      expect(html).toContain('Stock Exchange — Buy Shares');
      expect(html).toContain('Buyer Portfolio');
      expect(html).toContain('Purchase quota:');
      expect(html).toContain('Skip / Pass Turn');
    });

    it('renders active company listing when active companies exist', () => {
      const activeCompany = dummyState.companies.american!;
      const activeState: typeof dummyState = {
        ...dummyState,
        companies: {
          ...dummyState.companies,
          american: {
            ...activeCompany,
            isActive: true,
            size: 5,
            availableShares: 20,
          },
        },
      };

      const html = renderToStaticMarkup(
        <StockMarketModal
          isOpen={true}
          onClose={vi.fn()}
          gameState={activeState}
          onConfirmPurchase={vi.fn()}
        />
      );
      expect(html).toContain('American Natural Gas');
      expect(html).toContain('Size: <strong>5</strong>');
      expect(html).toContain('Bank: <strong>20</strong> left');
    });
  });

  describe('CompanyStatsModal', () => {
    it('returns null when isOpen is false', () => {
      const html = renderToStaticMarkup(
        <CompanyStatsModal
          isOpen={false}
          onClose={vi.fn()}
          gameState={dummyState}
        />
      );
      expect(html).toBe('');
    });

    it('renders corporate statistics, metrics, and shareholder register when open', () => {
      const html = renderToStaticMarkup(
        <CompanyStatsModal
          isOpen={true}
          onClose={vi.fn()}
          gameState={dummyState}
        />
      );
      expect(html).toContain('Corporate Statistics &amp; Shareholder Register');
      expect(html).toContain('Grid Size (Tiles)');
      expect(html).toContain('Stock Price');
      expect(html).toContain('Bank Shares');
      expect(html).toContain('Prospective Merger Bonuses');
      expect(html).toContain('Shareholder Distribution');
    });

    it('renders shareholder rankings when players hold shares', () => {
      const activeCompany = dummyState.companies.american!;
      const stateWithShares: typeof dummyState = {
        ...dummyState,
        companies: {
          ...dummyState.companies,
          american: {
            ...activeCompany,
            isActive: true,
            size: 6,
            availableShares: 15,
          },
        },
        players: [
          {
            ...dummyState.players[0]!,
            shares: { ...dummyState.players[0]!.shares, american: 8 },
          },
          {
            ...dummyState.players[1]!,
            shares: { ...dummyState.players[1]!.shares, american: 2 },
          },
        ],
      };

      const html = renderToStaticMarkup(
        <CompanyStatsModal
          isOpen={true}
          onClose={vi.fn()}
          gameState={stateWithShares}
        />
      );
      expect(html).toContain('Alice');
      expect(html).toContain('Majority Holder');
      expect(html).toContain('Bob');
      expect(html).toContain('Minority Holder');
    });
  });
});
