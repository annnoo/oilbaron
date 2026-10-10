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
import { useTranslation } from '../i18n';

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
  const { t } = useTranslation();

  const getAdvisorMessage = () => {
    if (isAi) {
      return {
        icon: <Sparkles size={16} className="advisor-icon pulse" />,
        headline: t('advisor.aiAnalyzing', { name: activePlayerName }),
        detail: t('advisor.aiDetail'),
      };
    }

    if (phase === 'buy_shares') {
      return {
        icon: <ShoppingBag size={16} className="advisor-icon text-amber" />,
        headline: t('advisor.buyShares'),
        detail: t('advisor.buySharesDetail'),
      };
    }

    if (phase === 'choose_founding_company') {
      return {
        icon: <TrendingUp size={16} className="advisor-icon text-green" />,
        headline: t('advisor.incorporate'),
        detail: t('advisor.incorporateDetail'),
      };
    }

    if (phase === 'choose_merger_survivor') {
      return {
        icon: <AlertCircle size={16} className="advisor-icon text-orange" />,
        headline: t('advisor.mergerTiebreak'),
        detail: t('advisor.mergerTiebreakDetail'),
      };
    }

    if (phase === 'dispose_merger_shares') {
      return {
        icon: <TrendingUp size={16} className="advisor-icon text-blue" />,
        headline: t('advisor.mergerPayouts'),
        detail: t('advisor.mergerPayoutsDetail'),
      };
    }

    if (phase === 'place_tile') {
      const activeCoord = hoveredCoord || selectedCoord;
      if (!activeCoord) {
        return {
          icon: <Compass size={16} className="advisor-icon" />,
          headline: t('advisor.selectParcel'),
          detail: t('advisor.selectParcelDetail'),
        };
      }

      const classification = classifyMove(board, activeCoord, companies, safeThreshold);
      const coordName = formatCoord(activeCoord);

      switch (classification.type as string) {
        case 'isolated':
          return {
            icon: <Compass size={16} className="advisor-icon text-blue" />,
            headline: t('advisor.isolated', { coord: coordName }),
            detail: t('advisor.isolatedDetail'),
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
            headline: t('advisor.expands', { coord: coordName, name: comp?.name ?? companyId ?? '' }),
            detail: t('advisor.expandsDetail', { size: newSize, price: newPrice }),
          };
        }
        case 'founding':
        case 'found':
          return {
            icon: <Sparkles size={16} className="advisor-icon text-green" />,
            headline: t('advisor.founding', { coord: coordName }),
            detail: t('advisor.foundingDetail'),
          };
        case 'merger':
          return {
            icon: <AlertCircle size={16} className="advisor-icon text-orange" />,
            headline: t('advisor.merger', { coord: coordName }),
            detail: t('advisor.mergerDetail'),
          };
        case 'illegal_safe_merger':
        case 'illegal_dead':
          return {
            icon: <ShieldAlert size={16} className="advisor-icon text-red" />,
            headline: t('advisor.dead', { coord: coordName }),
            detail: t('advisor.deadDetail'),
          };
        default:
          return {
            icon: <Compass size={16} className="advisor-icon" />,
            headline: t('advisor.tileSelected', { coord: coordName }),
            detail: t('advisor.tileSelectedDetail'),
          };
      }
    }

    return {
      icon: <Compass size={16} className="advisor-icon" />,
      headline: t('advisor.matchInProgress'),
      detail: t('advisor.followInstructions'),
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
