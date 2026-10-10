import React from 'react';
import { Sparkles, AlertCircle, TrendingUp, Compass, ShoppingBag, ShieldAlert } from 'lucide-react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  TileCoord,
  TurnPhase,
} from '../engine/types';
import { classifyMove, formatCoord } from '../engine/board';
import { calculateSharePrice } from '../engine/market';

export function getStockPrice(tier: string, size: number): number {
  if (size < 2) return 0;
  const base = tier === 'high' ? 400 : tier === 'mid' ? 300 : 200;
  return base + Math.min(Math.max(0, size - 2), 39) * 100;
}

export interface TurnAdvisorProps {
  phase: TurnPhase;
  isAi: boolean;
  activePlayerName: string;
  selectedCoord: TileCoord | null;
  hoveredCoord: TileCoord | null;
  board: readonly (readonly CellState[])[];
  companies?: Partial<Record<CompanyId, CompanyState>>;
  definitions?: readonly CompanyDefinition[];
  safeThreshold?: number;
}

export const TurnAdvisor: React.FC<TurnAdvisorProps> = ({
  phase,
  isAi,
  activePlayerName,
  selectedCoord,
  hoveredCoord,
  board,
  companies = {},
  definitions = [],
  safeThreshold = 11,
}) => {
  const getAdvisorMessage = () => {
    if (isAi) {
      return {
        icon: <Sparkles size={16} className="advisor-icon pulse" />,
        headline: `${activePlayerName} (CPU) is analyzing the board...`,
        detail: 'Evaluating territory potential and market acquisitions.',
      };
    }

    if (phase === 'buy_shares') {
      return {
        icon: <ShoppingBag size={16} className="advisor-icon text-amber" />,
        headline: 'Stock Exchange Open: Invest in up to 3 company shares',
        detail: 'Use the quick-buy shelf below to purchase shares or click Finish Turn to pass.',
      };
    }

    if (phase === 'choose_founding_company') {
      return {
        icon: <TrendingUp size={16} className="advisor-icon text-green" />,
        headline: 'Company Incorporation',
        detail: 'Choose an available corporation to claim your free Founder Bonus share.',
      };
    }

    if (phase === 'choose_merger_survivor') {
      return {
        icon: <AlertCircle size={16} className="advisor-icon text-orange" />,
        headline: 'Merger Tiebreak',
        detail: 'Multiple chains have identical size. Select which company absorbs the other.',
      };
    }

    if (phase === 'dispose_merger_shares') {
      return {
        icon: <TrendingUp size={16} className="advisor-icon text-blue" />,
        headline: 'Merger Payouts & Share Disposition',
        detail: 'Decide whether to hold, sell for cash, or trade 2-for-1 for surviving stock.',
      };
    }

    if (phase === 'place_tile') {
      const activeCoord = hoveredCoord || selectedCoord;
      if (!activeCoord) {
        return {
          icon: <Compass size={16} className="advisor-icon" />,
          headline: 'Select an oil parcel from your hand tray below to drill',
          detail: 'Hover over your chits to preview their impact on the board grid.',
        };
      }

      const classification = classifyMove(board, activeCoord, companies, safeThreshold);
      const coordName = formatCoord(activeCoord);

      switch (classification.type as string) {
        case 'isolated':
          return {
            icon: <Compass size={16} className="advisor-icon text-blue" />,
            headline: `${coordName} — Isolated well`,
            detail: 'No adjacent wells. Establishes a wildcat claim on the territory.',
          };
        case 'expansion':
        case 'expand': {
          const companyId = 'companyId' in classification ? classification.companyId : undefined;
          const comp = companyId ? definitions.find((d) => d.id === companyId) : undefined;
          const cState = companyId ? companies[companyId] : null;
          const newSize = (cState?.size ?? 0) + 1;
          const newPrice = comp
            ? calculateSharePrice(comp, newSize)
            : getStockPrice('low', newSize);
          return {
            icon: <TrendingUp size={16} className="advisor-icon text-yellow" />,
            headline: `${coordName} — Expands ${comp?.name ?? companyId}`,
            detail: `Grows chain to ${newSize} wells. Share price will rise to $${newPrice}.`,
          };
        }
        case 'founding':
        case 'found':
          return {
            icon: <Sparkles size={16} className="advisor-icon text-green" />,
            headline: `${coordName} — Founds a new oil corporation!`,
            detail: 'Connects adjacent wildcat wells. You will receive 1 free founder share.',
          };
        case 'merger':
          return {
            icon: <AlertCircle size={16} className="advisor-icon text-orange" />,
            headline: `${coordName} — Triggers an Oil Merger!`,
            detail: 'Adjacent companies will merge. Major shareholders receive cash bonuses.',
          };
        case 'illegal_safe_merger':
        case 'illegal_dead':
          return {
            icon: <ShieldAlert size={16} className="advisor-icon text-red" />,
            headline: `${coordName} — Dead / Unplayable Tile`,
            detail: 'Tile connects two permanent safe companies (11+ wells). Click Discard on chit.',
          };
        default:
          return {
            icon: <Compass size={16} className="advisor-icon" />,
            headline: `Tile ${coordName} selected`,
            detail: 'Click cell on the board to confirm placement.',
          };
      }
    }

    return {
      icon: <Compass size={16} className="advisor-icon" />,
      headline: 'Match in progress',
      detail: 'Follow on-screen instructions.',
    };
  };

  const { icon, headline, detail } = getAdvisorMessage();

  return (
    <div className="turn-advisor-banner">
      <div className="advisor-content">
        <span className="advisor-icon-wrap">{icon}</span>
        <div className="advisor-text">
          <span className="advisor-headline">{headline}</span>
          <span className="advisor-detail">{detail}</span>
        </div>
      </div>
    </div>
  );
};
