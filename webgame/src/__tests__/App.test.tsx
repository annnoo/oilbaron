import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import App, { handleAppKeyDown } from '../App';

describe('App Integration', () => {
  it('renders modern App shell containing TacticalOpsBar, survey map GameBoard, and ConcessionsDock', () => {
    const html = renderToStaticMarkup(<App />);

    // Modern simulation-screen wrapper
    expect(html).toContain('class="simulation-screen"');

    // TacticalOpsBar at top
    expect(html).toContain('class="ops-bar-container"');
    expect(html).toContain('class="ops-bar"');
    expect(html).toContain('Player 1');
    expect(html).toContain('SURVEY &amp; DRILL OPERATION');
    expect(html).toContain('aria-label="ledger"');
    expect(html).toContain('aria-label="sound"');
    expect(html).toContain('aria-label="settings"');
    expect(html).toContain('aria-label="classic view"');

    // Corporate pipeline ticker bar
    expect(html).toContain('class="network-status-bar"');

    // Legacy elements should NOT be present in modern simulation mode
    expect(html).not.toContain('role="menubar"');
    expect(html).not.toContain('<span class="mnemonic">G</span>ame');
    expect(html).not.toContain('class="game-header"');
    expect(html).not.toContain('Turn Guidance &amp; Onboarding');

    // Center survey map container & GameBoard (with hideToolbar={true} for clean tactical map)
    expect(html).toContain('class="survey-map-container"');
    expect(html).toContain('Oil Baron 10x12 Game Board');
    expect(html).toContain('role="grid"');
    expect(html).not.toContain('class="board-toolbar"');
    expect(html).not.toContain('10 × 12 Grid');

    // Bottom ConcessionsDock with tactical leases and drill actuator
    expect(html).toContain('class="concessions-dock"');
    expect(html).toContain('class="leases-tray"');
    expect(html).toContain('aria-label="Parcel leases in hand"');
    expect(html).toContain('class="drill-actuator"');
    expect(html).toContain('DRILL WELL');
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

  describe('handleAppKeyDown', () => {
    it('opens StockMarketModal on Alt+B in classic mode', () => {
      const setStockMarketModalOpen = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleAppKeyDown(
        { key: 'b', altKey: true, preventDefault },
        {
          viewMode: 'classic',
          phase: 'place_tile',
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen,
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          dispatch: vi.fn(),
        }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(setStockMarketModalOpen).toHaveBeenCalledWith(true);
    });

    it('does NOT open StockMarketModal on Alt+B in modern mode', () => {
      const setStockMarketModalOpen = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleAppKeyDown(
        { key: 'b', altKey: true, preventDefault },
        {
          viewMode: 'modern',
          phase: 'place_tile',
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen,
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          dispatch: vi.fn(),
        }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(setStockMarketModalOpen).not.toHaveBeenCalled();
    });

    it('focuses/scrolls action dock on Alt+B in modern mode during buy_shares phase', () => {
      const setStockMarketModalOpen = vi.fn();
      const preventDefault = vi.fn();
      const scrollIntoView = vi.fn();
      const focus = vi.fn();

      const mockDock = { scrollIntoView, focus };
      (globalThis as any).document = {
        querySelector: vi.fn((sel: string) => (sel.includes('dock') ? mockDock : null)),
      };

      const handled = handleAppKeyDown(
        { key: 'b', altKey: true, preventDefault },
        {
          viewMode: 'modern',
          phase: 'buy_shares',
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen,
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          dispatch: vi.fn(),
        }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(setStockMarketModalOpen).not.toHaveBeenCalled();
      expect(scrollIntoView).toHaveBeenCalled();
      expect(focus).toHaveBeenCalled();
    });

    it('toggles LedgerDrawer on Alt+L', () => {
      const setLedgerDrawerOpen = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleAppKeyDown(
        { key: 'l', altKey: true, preventDefault },
        {
          viewMode: 'modern',
          phase: 'place_tile',
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen,
          setStockMarketModalOpen: vi.fn(),
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          dispatch: vi.fn(),
        }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(setLedgerDrawerOpen).toHaveBeenCalled();
    });

    it('places tile on Alt+E or Space when selectedHandCoord is present in place_tile phase', () => {
      const onPlaceTile = vi.fn();
      const preventDefault = vi.fn();

      // Test Alt+E
      const handledAltE = handleAppKeyDown(
        { key: 'e', altKey: true, preventDefault },
        {
          viewMode: 'modern',
          phase: 'place_tile',
          selectedHandCoord: { row: 2, col: 5 },
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen: vi.fn(),
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          onPlaceTile,
          dispatch: vi.fn(),
        }
      );

      expect(handledAltE).toBe(true);
      expect(onPlaceTile).toHaveBeenCalledWith({ row: 2, col: 5 });

      // Test Space
      const handledSpace = handleAppKeyDown(
        { key: ' ', altKey: false, preventDefault },
        {
          viewMode: 'modern',
          phase: 'place_tile',
          selectedHandCoord: { row: 3, col: 7 },
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen: vi.fn(),
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          onPlaceTile,
          dispatch: vi.fn(),
        }
      );

      expect(handledSpace).toBe(true);
      expect(onPlaceTile).toHaveBeenCalledWith({ row: 3, col: 7 });
    });

    it('ends turn on Alt+E during buy_shares phase', () => {
      const dispatch = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleAppKeyDown(
        { key: 'e', altKey: true, preventDefault },
        {
          viewMode: 'modern',
          phase: 'buy_shares',
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen: vi.fn(),
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          dispatch,
        }
      );

      expect(handled).toBe(true);
      expect(dispatch).toHaveBeenCalledWith({ type: 'END_TURN' });
    });

    it('ignores shortcuts when target is an input element', () => {
      const setStockMarketModalOpen = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleAppKeyDown(
        { key: 'b', altKey: true, preventDefault, target: { tagName: 'INPUT' } },
        {
          viewMode: 'classic',
          phase: 'place_tile',
          ledgerDrawerOpen: false,
          setLedgerDrawerOpen: vi.fn(),
          setStockMarketModalOpen,
          setWealthChartModalOpen: vi.fn(),
          setShareGraphModalOpen: vi.fn(),
          setCompanyStatsModalOpen: vi.fn(),
          setWarRoomModalOpen: vi.fn(),
          dispatch: vi.fn(),
        }
      );

      expect(handled).toBe(false);
      expect(preventDefault).not.toHaveBeenCalled();
      expect(setStockMarketModalOpen).not.toHaveBeenCalled();
    });
  });
});
