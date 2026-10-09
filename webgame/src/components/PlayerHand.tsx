import React, { useState } from 'react';
import {
  Eye,
  EyeOff,
  Trash2,
  Play,
  AlertTriangle,
  Lock,
  Sparkles,
  ShieldAlert,
} from 'lucide-react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
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

export interface PlayerHandProps {
  /**
   * Coordinates of tiles currently held in the active player's hand.
   */
  hand: readonly TileCoord[];

  /**
   * Display name of the active player (e.g. "Player 1", "Computer 1").
   */
  activePlayerName?: string;

  /**
   * 0-based index of the active player in the players array.
   */
  activePlayerIndex?: number;

  /**
   * Whether it is currently this player's active turn. Defaults to true.
   */
  isCurrentTurn?: boolean;

  /**
   * Current turn phase (e.g. 'place_tile', 'buy_shares', etc.).
   */
  turnPhase?: TurnPhase;

  /**
   * Whether the active player is AI controlled.
   */
  isAi?: boolean;

  /**
   * Current 2D board state, used to detect dead tiles and preview move classifications.
   */
  board?: readonly (readonly CellState[])[];

  /**
   * Map of companies, used for safeThreshold and merger dead tile detection.
   */
  companies?: Partial<Record<CompanyId, CompanyState>>;

  /**
   * Optional company definitions for color lookups.
   */
  definitions?: readonly CompanyDefinition[];

  /**
   * Safe threshold (default 11 tiles).
   */
  safeThreshold?: number;

  /**
   * Currently selected tile in hand (controlled).
   */
  selectedCoord?: TileCoord | null;

  /**
   * Currently hovered tile in hand (controlled).
   */
  hoveredCoord?: TileCoord | null;

  /**
   * Pass-and-play hotseat privacy toggle: if true, conceals chits from view (controlled).
   */
  hideChits?: boolean;

  /**
   * Callback when a tile in hand is selected or deselected.
   */
  onSelectTile?: (coord: TileCoord | null) => void;

  /**
   * Callback when a tile in hand is hovered (for board preview).
   */
  onHoverTile?: (coord: TileCoord | null) => void;

  /**
   * Callback to execute tile placement on the board.
   */
  onPlaceTile?: (coord: TileCoord) => void;

  /**
   * Callback to discard a dead tile from hand.
   */
  onDiscardDeadTile?: (coord: TileCoord) => void;

  /**
   * Callback when hotseat hide chits toggle is clicked.
   */
  onToggleHideChits?: (hide: boolean) => void;

  /**
   * Whether hand actions are disabled.
   */
  disabled?: boolean;

  /**
   * Optional custom CSS class.
   */
  className?: string;
}

/**
 * Returns a human-friendly short pill description and style for move classification.
 */
function getClassificationPill(classification: MoveClassification | null): {
  label: string;
  className: string;
} | null {
  if (!classification) return null;
  switch (classification.type) {
    case 'founding':
      return { label: 'Founding', className: 'preview-pill-founding' };
    case 'merger':
      return { label: 'Merger', className: 'preview-pill-merger' };
    case 'expansion':
      return { label: 'Expansion', className: 'preview-pill-expansion' };
    case 'isolated':
      return { label: 'Wildcat', className: 'preview-pill-isolated' };
    case 'illegal_safe_merger':
      return { label: 'Dead Tile', className: 'preview-pill-illegal_safe_merger' };
    default:
      return null;
  }
}

export const PlayerHand: React.FC<PlayerHandProps> = ({
  hand,
  activePlayerName = 'Player 1',
  activePlayerIndex = 0,
  isCurrentTurn = true,
  turnPhase = 'place_tile',
  isAi = false,
  board,
  companies = {},
  safeThreshold = 11,
  selectedCoord: controlledSelected,
  hoveredCoord: controlledHovered,
  hideChits: controlledHideChits,
  onSelectTile,
  onHoverTile,
  onPlaceTile,
  onDiscardDeadTile,
  onToggleHideChits,
  disabled = false,
  className = '',
}) => {
  // Uncontrolled fallback states if parent doesn't manage them
  const [uncontrolledSelected, setUncontrolledSelected] = useState<TileCoord | null>(null);
  const [uncontrolledHovered, setUncontrolledHovered] = useState<TileCoord | null>(null);
  const [uncontrolledHideChits, setUncontrolledHideChits] = useState<boolean>(false);

  const selectedCoord = controlledSelected !== undefined ? controlledSelected : uncontrolledSelected;
  const hoveredCoord = controlledHovered !== undefined ? controlledHovered : uncontrolledHovered;
  const hideChits = controlledHideChits !== undefined ? controlledHideChits : uncontrolledHideChits;

  const isPlacementAllowed = turnPhase === 'place_tile' && !disabled;

  // Identify all dead tiles in the player's hand
  const deadTiles: TileCoord[] = React.useMemo(() => {
    if (!board) return [];
    return hand.filter((coord) => isTileDead(board, coord, companies, safeThreshold));
  }, [hand, board, companies, safeThreshold]);

  const hasDeadTiles = deadTiles.length > 0;

  // Check if currently selected tile is dead
  const isSelectedTileDead = React.useMemo(() => {
    if (!selectedCoord || !board) return false;
    return isTileDead(board, selectedCoord, companies, safeThreshold);
  }, [selectedCoord, board, companies, safeThreshold]);

  const handleTileClick = (coord: TileCoord) => {
    if (disabled) return;
    playClick();
    const isAlreadySelected = coordEquals(selectedCoord, coord);
    const nextCoord = isAlreadySelected ? null : coord;

    if (onSelectTile) {
      onSelectTile(nextCoord);
    } else {
      setUncontrolledSelected(nextCoord);
    }
  };

  const handleTileMouseEnter = (coord: TileCoord) => {
    if (disabled || hideChits) return;
    if (onHoverTile) {
      onHoverTile(coord);
    } else {
      setUncontrolledHovered(coord);
    }
  };

  const handleTileMouseLeave = () => {
    if (disabled || hideChits) return;
    if (onHoverTile) {
      onHoverTile(null);
    } else {
      setUncontrolledHovered(null);
    }
  };

  const handleToggleHideChits = () => {
    playClick();
    const nextHide = !hideChits;
    if (onToggleHideChits) {
      onToggleHideChits(nextHide);
    } else {
      setUncontrolledHideChits(nextHide);
    }
  };

  const handlePlaceChitClick = () => {
    if (!selectedCoord || isSelectedTileDead || !isPlacementAllowed) return;
    playClick();
    onPlaceTile?.(selectedCoord);
  };

  const handleDiscardDeadClick = () => {
    if (!hasDeadTiles) return;
    playClick();

    // If selected tile is dead, discard that one; otherwise discard the first dead tile in hand
    const targetTile = isSelectedTileDead && selectedCoord ? selectedCoord : deadTiles[0];
    if (targetTile) {
      onDiscardDeadTile?.(targetTile);
      if (coordEquals(selectedCoord, targetTile)) {
        if (onSelectTile) {
          onSelectTile(null);
        } else {
          setUncontrolledSelected(null);
        }
      }
    }
  };

  return (
    <section
      className={`player-hand-tray ${className}`}
      aria-label={`${activePlayerName}'s Hand Tray`}
      data-testid="player-hand-tray"
    >
      {/* Hand Header */}
      <div className="hand-tray-header">
        <div className="hand-header-info">
          <span className="hand-player-title">
            <span className="hand-player-indicator" />
            {activePlayerName}'s Hand:
          </span>
          <span className="hand-counter-badge">
            {hand.length} / 6 chits
          </span>
          {isAi && (
            <span className="badge badge-active" title="Computer player turn">
              CPU
            </span>
          )}
          {hasDeadTiles && (
            <span
              className="badge"
              style={{
                backgroundColor: 'rgba(239, 68, 68, 0.2)',
                color: '#f87171',
                border: '1px solid rgba(239, 68, 68, 0.4)',
              }}
              title="Dead tiles detected (cannot be placed on board)"
            >
              <AlertTriangle size={11} /> {deadTiles.length} Dead Chit{deadTiles.length > 1 ? 's' : ''}
            </span>
          )}
        </div>

        {/* Hotseat Hide Chits Toggle Button */}
        <div className="hand-header-actions">
          <button
            type="button"
            className={`btn btn-sm ${hideChits ? 'btn-primary' : 'btn-secondary'}`}
            onClick={handleToggleHideChits}
            aria-label={hideChits ? 'Show Chits' : 'Hide Chits'}
            aria-pressed={hideChits}
            title={hideChits ? 'Reveal chits for current player' : 'Hide chits for pass-and-play hotseat privacy'}
          >
            {hideChits ? <Eye size={13} /> : <EyeOff size={13} />}
            <span>{hideChits ? 'Show Chits' : 'Hide Chits'}</span>
          </button>
        </div>
      </div>

      {/* Chits Tray Content Area */}
      <div className="hand-tray-content">
        {hideChits ? (
          /* Privacy Obscured View */
          <div className="hand-privacy-overlay" data-testid="hand-privacy-view">
            <div className="hand-privacy-message">
              <Lock size={18} className="text-gold" />
              <span>Chits concealed for pass-and-play hotseat privacy.</span>
              <button
                type="button"
                className="btn btn-sm btn-primary"
                onClick={handleToggleHideChits}
              >
                <Eye size={12} /> Reveal Chits
              </button>
            </div>
            <div className="hand-chits-rack">
              {hand.map((tile, idx) => (
                <div
                  key={`hidden-${tile.row}-${tile.col}-${idx}`}
                  className="hand-chit-card is-hidden"
                  aria-label="Concealed chit"
                >
                  <Lock size={14} style={{ opacity: 0.5 }} />
                  <span className="chit-coord-hidden">???</span>
                </div>
              ))}
            </div>
          </div>
        ) : hand.length === 0 ? (
          /* Empty Hand View */
          <div className="hand-empty-notice">
            <span>No chits currently held in hand.</span>
          </div>
        ) : (
          /* Visible Chits Rack */
          <div className="hand-chits-rack" role="radiogroup" aria-label="Available chits in hand">
            {hand.map((tile) => {
              const formatted = formatCoord(tile);
              const isSelected = coordEquals(selectedCoord, tile);
              const isHovered = coordEquals(hoveredCoord, tile);
              const isDead = board ? isTileDead(board, tile, companies, safeThreshold) : false;
              const classification = board ? classifyMove(board, tile, companies, safeThreshold) : null;
              const pill = getClassificationPill(classification);

              return (
                <button
                  key={`${tile.row}-${tile.col}`}
                  type="button"
                  role="radio"
                  aria-checked={isSelected}
                  aria-label={`Chit ${formatted}${isDead ? ' (Dead tile, unplayable)' : ''}`}
                  className={`hand-chit-card ${isSelected ? 'is-selected' : ''} ${
                    isDead ? 'is-dead' : ''
                  } ${isHovered ? 'is-hovered' : ''}`}
                  onClick={() => handleTileClick(tile)}
                  onMouseEnter={() => handleTileMouseEnter(tile)}
                  onMouseLeave={handleTileMouseLeave}
                  disabled={disabled}
                  title={
                    isDead
                      ? `${formatted}: DEAD TILE — Merges 2+ safe corporations. Discard to draw a replacement.`
                      : `${formatted}: Click to select for placement; hover to preview on board`
                  }
                  data-coord={formatted}
                  data-testid={`chit-${formatted}`}
                >
                  <div className="chit-card-top">
                    <span className="chit-coord-tag">{formatted}</span>
                    {isDead ? (
                      <span className="chit-dead-badge" title="Unplayable dead tile">
                        DEAD
                      </span>
                    ) : (
                      pill && (
                        <span className={`chit-move-pill ${pill.className}`}>
                          {pill.label}
                        </span>
                      )
                    )}
                  </div>

                  <div className="chit-card-body">
                    <span className="chit-marker-dot" />
                    <span className="chit-card-type">
                      {isDead ? 'ILLEGAL' : 'WILDCAT'}
                    </span>
                  </div>

                  {isSelected && (
                    <div className="chit-selected-indicator" aria-hidden="true">
                      ✓
                    </div>
                  )}

                  {isDead && (
                    <button
                      type="button"
                      className="chit-inline-discard-btn"
                      onClick={(e) => {
                        e.stopPropagation();
                        playClick();
                        onDiscardDeadTile?.(tile);
                      }}
                      title={`Discard dead tile ${formatted} immediately`}
                      aria-label={`Discard dead tile ${formatted}`}
                    >
                      <Trash2 size={10} /> Discard
                    </button>
                  )}
                </button>
              );
            })}
          </div>
        )}
      </div>

      {/* Action Controls Bar */}
      <div className="hand-actions-bar">
        <div className="hand-action-group">
          {/* Place Chit Button */}
          <button
            type="button"
            className={`btn btn-sm ${
              selectedCoord && !isSelectedTileDead && isPlacementAllowed
                ? 'btn-primary'
                : 'btn-secondary'
            }`}
            onClick={handlePlaceChitClick}
            disabled={!selectedCoord || isSelectedTileDead || !isPlacementAllowed || hideChits}
            aria-label={
              selectedCoord
                ? `Place Chit ${formatCoord(selectedCoord)}`
                : 'Place Chit'
            }
            title={
              !selectedCoord
                ? 'Select a chit from your hand first'
                : isSelectedTileDead
                ? 'Dead tiles cannot be placed on the board; discard instead'
                : !isPlacementAllowed
                ? 'Not in tile placement phase'
                : `Place chit ${formatCoord(selectedCoord)} onto board`
            }
          >
            <Play size={13} />
            <span>
              {selectedCoord && !isSelectedTileDead
                ? `Place Chit (${formatCoord(selectedCoord)})`
                : isSelectedTileDead
                ? 'Cannot Place Dead Chit'
                : 'Place Chit'}
            </span>
          </button>

          {/* Discard Dead Tile Button */}
          <button
            type="button"
            className={`btn btn-sm ${hasDeadTiles ? 'btn-danger' : 'btn-secondary'}`}
            onClick={handleDiscardDeadClick}
            disabled={!hasDeadTiles || hideChits || disabled}
            aria-label="Discard Dead Tile"
            title={
              hasDeadTiles
                ? `Discard dead tile${
                    selectedCoord && isSelectedTileDead
                      ? ` (${formatCoord(selectedCoord)})`
                      : ''
                  } and draw a replacement from the bank`
                : 'No dead tiles in hand'
            }
          >
            <Trash2 size={13} />
            <span>
              Discard Dead Tile
              {hasDeadTiles ? ` (${deadTiles.length})` : ''}
            </span>
          </button>
        </div>

        {/* Selected Chit Status Message */}
        <div className="hand-selection-status">
          {hideChits ? (
            <span className="text-muted">Hand hidden for hotseat privacy</span>
          ) : selectedCoord ? (
            isSelectedTileDead ? (
              <span className="status-dead-warning">
                <ShieldAlert size={12} /> {formatCoord(selectedCoord)} is a dead tile (cannot merge safe corporations)
              </span>
            ) : (
              <span className="status-selected-info">
                Selected: <strong className="text-amber">{formatCoord(selectedCoord)}</strong> — Ready to place
              </span>
            )
          ) : (
            <span className="text-muted">Click a chit above to select it for placement</span>
          )}
        </div>
      </div>
    </section>
  );
};

export default PlayerHand;
