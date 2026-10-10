import React from 'react';
import { Crosshair, AlertTriangle, Trash2 } from 'lucide-react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  SharePurchase,
  TileCoord,
  TurnPhase,
} from '../engine/types';
import {
  classifyMove,
  coordEquals,
  formatCoord,
  isTileDead,
  type MoveClassification,
} from '../engine/board';
import { playClick } from '../assets/audio';
import { InlineStockMarket } from './InlineStockMarket';

export interface ConcessionsDockProps {
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
  onSelectTile: (coord: TileCoord | null) => void;
  onHoverTile: (coord: TileCoord | null) => void;
  onPlaceTile: (coord: TileCoord) => void;
  onDiscardDeadTile: (coord: TileCoord) => void;
  onConfirmPurchases: (purchases: SharePurchase[]) => void;
  onPassTurn: () => void;
}

interface TacticalTagInfo {
  tag: string;
  chipClass: string;
}

function getTacticalTag(
  coord: TileCoord,
  isDead: boolean,
  classification: MoveClassification | null
): TacticalTagInfo {
  if (coord.isBlank) {
    return { tag: 'WILDCARD', chipClass: 'lease-chip-found' };
  }
  if (isDead) {
    return { tag: 'DISCARD', chipClass: 'lease-chip-discard' };
  }
  if (!classification) {
    return { tag: 'OPEN', chipClass: 'lease-chip-open' };
  }

  switch (classification.type) {
    case 'expansion':
      return { tag: 'EXPAND', chipClass: 'lease-chip-expand' };
    case 'founding':
      return { tag: 'FOUND', chipClass: 'lease-chip-found' };
    case 'merger':
      return { tag: 'MERGER', chipClass: 'lease-chip-merger' };
    case 'illegal_safe_merger':
      return { tag: 'DISCARD', chipClass: 'lease-chip-discard' };
    case 'isolated':
    default:
      return { tag: 'SOLITARY', chipClass: 'lease-chip-open' };
  }
}

export const ConcessionsDock: React.FC<ConcessionsDockProps> = ({
  phase,
  isCurrentTurn,
  hand,
  selectedCoord,
  hoveredCoord,
  activePlayerCash,
  board,
  companies,
  definitions,
  safeThreshold = 11,
  onSelectTile,
  onHoverTile,
  onPlaceTile,
  onDiscardDeadTile,
  onConfirmPurchases,
  onPassTurn,
}) => {
  // If in buy_shares phase and active player's turn, unfold InlineStockMarket
  if (phase === 'buy_shares' && isCurrentTurn) {
    return (
      <div className="concessions-dock">
        <InlineStockMarket
          activePlayerCash={activePlayerCash}
          companies={companies}
          definitions={definitions}
          onConfirmPurchases={onConfirmPurchases}
          onPass={onPassTurn}
        />
      </div>
    );
  }

  const isSelectedDead = selectedCoord && !selectedCoord.isBlank && board
    ? isTileDead(board, selectedCoord, companies, safeThreshold)
    : false;

  const handleTileClick = (coord: TileCoord) => {
    playClick();
    const isAlreadySelected = coordEquals(selectedCoord, coord);
    onSelectTile(isAlreadySelected ? null : coord);
  };

  const handleActuatorClick = () => {
    if (!selectedCoord || !isCurrentTurn || phase !== 'place_tile') return;
    playClick();
    if (isSelectedDead) {
      onDiscardDeadTile(selectedCoord);
      onSelectTile(null);
    } else {
      onPlaceTile(selectedCoord);
    }
  };

  const selectedCoordLabel = selectedCoord ? formatCoord(selectedCoord) : null;

  return (
    <div className="concessions-dock">
      {/* Tactical Leases Tray */}
      <div className="leases-tray" role="radiogroup" aria-label="Parcel leases in hand">
        {hand.map((tile, idx) => {
          const isBlank = Boolean(tile.isBlank);
          const formatted = isBlank ? 'WILD' : formatCoord(tile);
          const isSelected = coordEquals(selectedCoord, tile);
          const isHovered = coordEquals(hoveredCoord, tile);
          const isDead = Boolean(board && !isBlank && isTileDead(board, tile, companies, safeThreshold));
          const classification = board && !isBlank
            ? classifyMove(board, tile, companies, safeThreshold)
            : null;
          const { tag, chipClass } = getTacticalTag(tile, isDead, classification);

          return (
            <button
              key={isBlank ? `blank-${idx}` : `${tile.row}-${tile.col}`}
              type="button"
              className={`lease-slip ${isSelected ? 'selected' : ''} ${isDead ? 'dead' : ''} ${
                isHovered ? 'hovered' : ''
              }`}
              onClick={() => handleTileClick(tile)}
              onMouseEnter={() => onHoverTile(tile)}
              onMouseLeave={() => onHoverTile(null)}
              aria-pressed={isSelected}
              aria-label={`Parcel lease ${formatted} ${tag}`}
              title={`Parcel ${formatted}: ${tag}`}
            >
              <span className="lease-coord">{formatted}</span>
              <span className={`lease-status ${chipClass}`}>{tag}</span>
            </button>
          );
        })}
      </div>

      {/* Industrial Drill Actuator Button */}
      {isSelectedDead ? (
        <button
          type="button"
          className="drill-actuator discard"
          onClick={handleActuatorClick}
          aria-label={`Discard dead parcel ${selectedCoordLabel}`}
        >
          <span className="actuator-title" style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <Trash2 size={16} /> DISCARD DEAD PARCEL
          </span>
          <span className="actuator-sub">PARCEL {selectedCoordLabel}</span>
        </button>
      ) : (
        <button
          type="button"
          className="drill-actuator"
          disabled={!selectedCoord || !isCurrentTurn || phase !== 'place_tile'}
          onClick={handleActuatorClick}
          aria-label={
            selectedCoord
              ? `Drill well at parcel ${selectedCoordLabel}`
              : 'Select a parcel lease to drill'
          }
        >
          <span className="actuator-title" style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <Crosshair size={16} /> DRILL WELL
          </span>
          <span className="actuator-sub">
            {selectedCoord ? `PARCEL ${selectedCoordLabel}` : 'SELECT A PARCEL LEASE'}
          </span>
        </button>
      )}
    </div>
  );
};

export default ConcessionsDock;
