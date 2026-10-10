import React from 'react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  SharePurchase,
  TileCoord,
  TurnPhase,
} from '../engine/types';
import { PlayerHand } from './PlayerHand';
import { InlineStockMarket } from './InlineStockMarket';

export interface ActionDockProps {
  phase: TurnPhase;
  isCurrentTurn: boolean;
  hand: readonly TileCoord[];
  selectedCoord: TileCoord | null;
  hoveredCoord: TileCoord | null;
  activePlayerCash: number;
  activePlayerName?: string;
  board?: readonly (readonly CellState[])[];
  companies: Partial<Record<CompanyId, CompanyState>>;
  definitions: readonly CompanyDefinition[];
  safeThreshold?: number;
  hideChits?: boolean;
  onSelectTile: (coord: TileCoord | null) => void;
  onHoverTile: (coord: TileCoord | null) => void;
  onPlaceTile: (coord: TileCoord) => void;
  onDiscardDeadTile: (coord: TileCoord) => void;
  onPlayBlankTile?: (coord: TileCoord) => void;
  onToggleHideChits?: (hidden: boolean) => void;
  onConfirmPurchases: (purchases: SharePurchase[]) => void;
  onPassTurn: () => void;
}

export const ActionDock: React.FC<ActionDockProps> = ({
  phase,
  isCurrentTurn,
  hand,
  selectedCoord,
  hoveredCoord,
  activePlayerCash,
  activePlayerName,
  board,
  companies,
  definitions,
  safeThreshold = 11,
  hideChits = false,
  onSelectTile,
  onHoverTile,
  onPlaceTile,
  onDiscardDeadTile,
  onPlayBlankTile,
  onToggleHideChits,
  onConfirmPurchases,
  onPassTurn,
}) => {
  return (
    <div className="unified-action-dock">
      {phase === 'buy_shares' && isCurrentTurn ? (
        <InlineStockMarket
          activePlayerCash={activePlayerCash}
          companies={companies}
          definitions={definitions}
          onConfirmPurchases={onConfirmPurchases}
          onPass={onPassTurn}
        />
      ) : (
        <PlayerHand
          hand={hand}
          activePlayerName={activePlayerName}
          isCurrentTurn={isCurrentTurn}
          turnPhase={phase}
          board={board}
          companies={companies}
          definitions={definitions}
          safeThreshold={safeThreshold}
          selectedCoord={selectedCoord}
          hoveredCoord={hoveredCoord}
          hideChits={hideChits}
          onSelectTile={onSelectTile}
          onHoverTile={onHoverTile}
          onPlaceTile={onPlaceTile}
          onDiscardDeadTile={onDiscardDeadTile}
          onPlayBlankTile={onPlayBlankTile}
          onToggleHideChits={onToggleHideChits}
        />
      )}
    </div>
  );
};

export default ActionDock;
