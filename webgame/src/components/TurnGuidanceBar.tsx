import React from 'react';
import {
  Compass,
  ArrowRight,
  ShoppingBag,
  CheckCircle2,
  HelpCircle,
  Sparkles,
  Flame,
  Check,
  ChevronRight,
} from 'lucide-react';
import { playClick } from '../assets/audio';
import type { TileCoord, TurnPhase } from '../engine/types';
import { formatCoord } from '../engine/board';

export interface TurnGuidanceBarProps {
  phase: TurnPhase;
  activePlayerName: string;
  activePlayerColor?: string;
  isAi: boolean;
  selectedCoord: TileCoord | null;
  cash?: number;
  showHints: boolean;
  onToggleHints: (hints: boolean) => void;
  onPlaceSelected?: () => void;
  onOpenStockMarket?: () => void;
  onEndTurn?: () => void;
}

export const TurnGuidanceBar: React.FC<TurnGuidanceBarProps> = ({
  phase,
  activePlayerName,
  activePlayerColor,
  isAi,
  selectedCoord,
  cash = 6000,
  showHints,
  onToggleHints,
  onPlaceSelected,
  onOpenStockMarket,
  onEndTurn,
}) => {
  const getGuidanceContent = () => {
    if (isAi) {
      return {
        badge: 'CPU THINKING',
        badgeColor: '#c084fc',
        title: `${activePlayerName} (Computer) is calculating move...`,
        detail: 'The AI player evaluates territory heuristics and market values automatically.',
        actions: null,
      };
    }

    switch (phase) {
      case 'place_tile':
        if (!selectedCoord) {
          return {
            badge: 'STEP 1 OF 3: SELECT CHIT',
            badgeColor: 'var(--accent-gold)',
            title: 'Select an oil lease chit from your hand tray below',
            detail:
              'Click any of your 6 chits in the rack at the bottom of the screen to prepare your drilling site.',
            actions: null,
          };
        }
        return {
          badge: 'STEP 2 OF 3: DRILL WELL',
          badgeColor: '#38bdf8',
          title: `Chit [${formatCoord(selectedCoord)}] selected! Click ${formatCoord(selectedCoord)} on the grid`,
          detail: `Click cell ${formatCoord(selectedCoord)} on the 10×12 board to drill. Adjacent wells will expand companies or trigger mergers.`,
          actions: onPlaceSelected ? (
            <button
              type="button"
              className="btn btn-primary btn-sm"
              onClick={onPlaceSelected}
              style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}
            >
              <Flame size={14} />
              <span>Drill Well at {formatCoord(selectedCoord)} (Alt+P)</span>
            </button>
          ) : null,
        };

      case 'choose_founding_company':
        return {
          badge: 'COMPANY FOUNDED!',
          badgeColor: '#f59e0b',
          title: 'You connected wildcat wells! Select an oil corporation to incorporate',
          detail: 'Choose an available corporation in the modal dialog to claim your free Founder Bonus shares.',
          actions: null,
        };

      case 'choose_merger_survivor':
        return {
          badge: 'MERGER TIE-BREAK',
          badgeColor: '#ef4444',
          title: 'Multiple corporations tie for largest size! Choose the survivor',
          detail: 'Select which corporation survives and acquires the rival companies.',
          actions: null,
        };

      case 'dispose_merger_shares':
        return {
          badge: 'SHARE DISPOSITION',
          badgeColor: '#a855f7',
          title: 'Hostile takeover in progress: Dispose of your defunct company stock',
          detail: 'Sell shares for pre-merger cash, trade 2-for-1 for surviving stock, or hold for dormant reserve.',
          actions: null,
        };

      case 'buy_shares':
        return {
          badge: 'STEP 3 OF 3: STOCK MARKET',
          badgeColor: '#22c55e',
          title: `Drilling complete! Buy up to 3 corporate shares ($${cash.toLocaleString()} cash available)`,
          detail:
            'You may purchase stock in any active corporations to build portfolio value, or click End Turn to pass.',
          actions: (
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
              {onOpenStockMarket && (
                <button
                  type="button"
                  className="btn btn-primary btn-sm"
                  onClick={onOpenStockMarket}
                  style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}
                >
                  <ShoppingBag size={14} />
                  <span>Buy Shares (Alt+B)</span>
                </button>
              )}
              {onEndTurn && (
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={onEndTurn}
                  style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}
                >
                  <Check size={14} />
                  <span>End Turn / Pass (Alt+E)</span>
                </button>
              )}
            </div>
          ),
        };

      case 'game_over':
        return {
          badge: 'MATCH CONCLUDED',
          badgeColor: '#eab308',
          title: 'Game Over! All safe monopolies completed or grid filled',
          detail: 'All company portfolios have been liquidated for cash. Inspect the final leaderboard!',
          actions: null,
        };

      default:
        return {
          badge: 'OIL BARON',
          badgeColor: 'var(--accent-gold)',
          title: 'Take your turn',
          detail: 'Follow on-screen prompts to manage your oil empire.',
          actions: null,
        };
    }
  };

  const guidance = getGuidanceContent();

  return (
    <div
      role="region"
      aria-label="Turn Guidance & Onboarding"
      style={{
        padding: '12px 16px',
        background: 'linear-gradient(135deg, rgba(245, 158, 11, 0.12), rgba(30, 41, 59, 0.85))',
        border: '2px solid rgba(245, 158, 11, 0.45)',
        boxShadow: '0 4px 16px rgba(0, 0, 0, 0.3)',
        borderRadius: 'var(--radius-sm)',
        display: 'flex',
        flexDirection: 'column',
        gap: '6px',
        marginBottom: '10px',
      }}
    >
      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '8px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
          <span
            style={{
              fontSize: '11px',
              fontWeight: 800,
              letterSpacing: '0.06em',
              textTransform: 'uppercase',
              padding: '3px 8px',
              borderRadius: '4px',
              background: guidance.badgeColor,
              color: '#0f172a',
            }}
          >
            {guidance.badge}
          </span>

          <span
            style={{
              fontSize: '14px',
              fontWeight: 700,
              color: 'var(--text-primary)',
              display: 'inline-flex',
              alignItems: 'center',
              gap: '6px',
            }}
          >
            {activePlayerColor && (
              <span
                style={{
                  width: '9px',
                  height: '9px',
                  borderRadius: '50%',
                  backgroundColor: activePlayerColor,
                  display: 'inline-block',
                }}
              />
            )}
            <span>{guidance.title}</span>
          </span>
        </div>

        <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
          {guidance.actions}

          <button
            type="button"
            className="btn btn-ghost btn-sm"
            style={{
              fontSize: '11px',
              color: showHints ? 'var(--text-amber)' : 'var(--text-muted)',
              padding: '3px 8px',
            }}
            onClick={() => {
              playClick();
              onToggleHints(!showHints);
            }}
            title="Toggle step-by-step beginner guidance"
          >
            <HelpCircle size={13} style={{ marginRight: '4px', verticalAlign: 'middle' }} />
            Hints: {showHints ? 'ON' : 'OFF'}
          </button>
        </div>
      </div>

      {showHints && (
        <div
          style={{
            fontSize: '12px',
            color: 'var(--text-secondary)',
            lineHeight: 1.4,
            borderTop: '1px solid rgba(255, 255, 255, 0.08)',
            paddingTop: '6px',
            display: 'flex',
            alignItems: 'center',
            gap: '6px',
          }}
        >
          <Compass size={13} style={{ color: 'var(--accent-gold)', flexShrink: 0 }} />
          <span>{guidance.detail}</span>
        </div>
      )}
    </div>
  );
};
