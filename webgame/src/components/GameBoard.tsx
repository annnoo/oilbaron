import React, { useState, useMemo } from 'react';
import {
  Grid,
  Eye,
  Crosshair,
  Info,
  ShieldCheck,
  Ban,
  Sparkles,
  HelpCircle,
  Check,
} from 'lucide-react';
import type {
  CellState,
  CompanyDefinition,
  CompanyId,
  CompanyState,
  TileCoord,
} from '../engine/types';
import { CLASSIC_COMPANIES, DEFAULT_CONFIG } from '../engine/config';
import {
  classifyMove,
  formatCoord,
  coordEquals,
  ROW_LABELS,
  type MoveClassification,
} from '../engine/board';
import { TileInspector, getCompanyVisual } from './TileInspector';
import { playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

/**
 * Standard tactical glyph mapping for corporate wellheads on the geological survey map.
 */
export const COMPANY_GLYPHS: Record<CompanyId, string> = {
  american: '▲',
  arabian: '▲',
  imperial: '◆',
  supra: '◆',
  tower: '▲',
  royal: '◆',
  weemoco: '▲',
  purple: '◆',
};

export interface GameBoardProps {
  /**
   * The 2D grid matrix of board cells (rows x cols).
   */
  board: CellState[][];

  /**
   * Company states indexed by companyId.
   */
  companies?: Partial<Record<CompanyId, CompanyState>>;

  /**
   * Company definitions for styling and names. Defaults to CLASSIC_COMPANIES.
   */
  definitions?: readonly CompanyDefinition[];

  /**
   * Safe size threshold (defaults to 11).
   */
  safeThreshold?: number;

  /**
   * Coordinate of tile currently selected in active player's hand.
   */
  selectedTileCoord?: TileCoord | null;

  /**
   * Coordinate of tile currently hovered in player's hand tray.
   */
  previewTileCoord?: TileCoord | null;

  /**
   * Active player's current hand of tile coordinates.
   */
  playerHand?: readonly TileCoord[];

  /**
   * Whether the board allows clicking to place tiles.
   */
  interactive?: boolean;

  /**
   * Whether to show coordinate labels on empty cells. Defaults to false in survey map mode.
   */
  showCoordinates?: boolean;

  /**
   * Whether hover move classification preview is enabled.
   */
  showMovePreviews?: boolean;

  /**
   * Coordinate currently being inspected in TileInspector (if externally controlled).
   */
  inspectedCoord?: TileCoord | null;

  /**
   * Callback fired when a player clicks an empty cell to place a tile.
   */
  onPlaceTile?: (coord: TileCoord) => void;

  /**
   * Callback fired when any cell is clicked.
   */
  onCellClick?: (coord: TileCoord, cell: CellState) => void;

  /**
   * Callback fired when a cell is right-clicked or inspected.
   */
  onInspectCoord?: (coord: TileCoord | null) => void;

  /**
   * Optional custom CSS class name.
   */
  className?: string;
}

export const GameBoard: React.FC<GameBoardProps> = ({
  board,
  companies = {},
  definitions = CLASSIC_COMPANIES,
  safeThreshold = DEFAULT_CONFIG.safeThreshold,
  selectedTileCoord = null,
  previewTileCoord = null,
  playerHand = [],
  interactive = true,
  showCoordinates = false,
  showMovePreviews = true,
  inspectedCoord: externalInspectedCoord,
  onPlaceTile,
  onCellClick,
  onInspectCoord,
  className = '',
}) => {
  const { t } = useTranslation();

  // Local state for hover, inspector, and toggles
  const [hoveredCellCoord, setHoveredCellCoord] = useState<TileCoord | null>(null);
  const [localInspectedCoord, setLocalInspectedCoord] = useState<TileCoord | null>(null);
  const [inspectModeActive, setInspectModeActive] = useState<boolean>(false);
  const [showLegend, setShowLegend] = useState<boolean>(false);
  const [tooltipPos, setTooltipPos] = useState<{ x: number; y: number } | null>(null);
  const [hoverTooltipEnabled, setHoverTooltipEnabled] = useState<boolean>(false);

  const activeInspectedCoord = externalInspectedCoord !== undefined
    ? externalInspectedCoord
    : localInspectedCoord;

  const handleSetInspectedCoord = (coord: TileCoord | null) => {
    if (onInspectCoord) {
      onInspectCoord(coord);
    }
    setLocalInspectedCoord(coord);
  };

  const rows = board.length;
  const cols = board[0]?.length ?? 12;

  // Active target coordinate for move preview:
  // 1. If user hovers a chit in hand, previewTileCoord takes precedence
  // 2. Or if a tile is selected in hand, selectedTileCoord
  // 3. Or if user hovers directly over an empty cell on board, hoveredCellCoord
  const activePreviewTarget: TileCoord | null = useMemo(() => {
    if (!showMovePreviews) return null;
    if (previewTileCoord) return previewTileCoord;
    if (hoveredCellCoord) {
      const cell = board[hoveredCellCoord.row]?.[hoveredCellCoord.col];
      if (cell && cell.type === 'empty') {
        return hoveredCellCoord;
      }
    }
    if (selectedTileCoord) return selectedTileCoord;
    return null;
  }, [showMovePreviews, previewTileCoord, hoveredCellCoord, selectedTileCoord, board]);

  // Compute move classification for active preview target
  const moveClassification: MoveClassification | null = useMemo(() => {
    if (!activePreviewTarget) return null;
    const cell = board[activePreviewTarget.row]?.[activePreviewTarget.col];
    if (cell && cell.type !== 'empty') return null;
    return classifyMove(board, activePreviewTarget, companies, safeThreshold);
  }, [board, activePreviewTarget, companies, safeThreshold]);

  // Compute highlighted cells for connected expansion / founding / merger
  const previewHighlights = useMemo(() => {
    const connectedKeys = new Set<string>();
    const mergerCompanyIds = new Set<CompanyId>();
    let expandingCompanyId: CompanyId | null = null;

    if (moveClassification) {
      if (moveClassification.type === 'expansion') {
        expandingCompanyId = moveClassification.companyId;
        for (const c of moveClassification.connectedCoords) {
          connectedKeys.add(`${c.row},${c.col}`);
        }
      } else if (moveClassification.type === 'founding') {
        for (const c of moveClassification.connectedCoords) {
          connectedKeys.add(`${c.row},${c.col}`);
        }
      } else if (moveClassification.type === 'merger' || moveClassification.type === 'illegal_safe_merger') {
        for (const id of moveClassification.participatingCompanyIds) {
          mergerCompanyIds.add(id);
        }
      }
    }

    return {
      connectedKeys,
      expandingCompanyId,
      mergerCompanyIds,
    };
  }, [moveClassification]);

  // Player hand tile coordinate lookup set
  const handLookup = useMemo(() => {
    const set = new Set<string>();
    for (const h of playerHand) {
      set.add(`${h.row},${h.col}`);
    }
    return set;
  }, [playerHand]);

  // Handle cell left-click
  const handleCellClick = (r: number, c: number, cell: CellState) => {
    const coord: TileCoord = { row: r, col: c };

    if (inspectModeActive) {
      playClick();
      handleSetInspectedCoord(coord);
      return;
    }

    if (onCellClick) {
      onCellClick(coord, cell);
    }

    if (cell.type === 'empty' && interactive && onPlaceTile) {
      playClick();
      onPlaceTile(coord);
    } else if (cell.type !== 'empty') {
      playClick();
      handleSetInspectedCoord(coord);
    }
  };

  // Handle cell right-click (Quick Peek inspector)
  const handleCellContextMenu = (
    e: React.MouseEvent,
    r: number,
    c: number
  ) => {
    e.preventDefault();
    playClick();
    handleSetInspectedCoord({ row: r, col: c });
  };

  // Cell hover handlers
  const handleCellMouseEnter = (
    e: React.MouseEvent,
    r: number,
    c: number
  ) => {
    setHoveredCellCoord({ row: r, col: c });
    if (hoverTooltipEnabled) {
      setTooltipPos({ x: e.clientX, y: e.clientY });
    }
  };

  const handleCellMouseLeave = () => {
    setHoveredCellCoord(null);
    setTooltipPos(null);
  };

  // Get human description for preview status strip
  const getPreviewDescription = () => {
    if (!moveClassification) {
      if (selectedTileCoord) {
        return t('board.selectedPrompt', { coord: formatCoord(selectedTileCoord) });
      }
      return t('board.hoverPrompt');
    }

    const coordStr = formatCoord(moveClassification.coord);

    switch (moveClassification.type) {
      case 'isolated':
        return t('board.previewIsolated', { coord: coordStr });
      case 'expansion': {
        const comp = companies[moveClassification.companyId];
        const name = comp?.name ?? moveClassification.companyId.toUpperCase();
        return t('board.previewExpansion', {
          coord: coordStr,
          name,
          count: moveClassification.connectedCoords.length,
        });
      }
      case 'founding':
        return t('board.previewFounding', {
          coord: coordStr,
          count: moveClassification.connectedCoords.length,
        });
      case 'merger': {
        const names = moveClassification.participatingCompanyIds
          .map((id) => companies[id]?.name ?? id.toUpperCase())
          .join(' vs ');
        return t('board.previewMerger', { coord: coordStr, names });
      }
      case 'illegal_safe_merger': {
        const safeNames = moveClassification.safeCompanyIds
          .map((id) => companies[id]?.name ?? id.toUpperCase())
          .join(' & ');
        return t('board.previewDead', { coord: coordStr, names: safeNames });
      }
    }
  };

  return (
    <div className={`board-container survey-map-container ${className}`} role="region" aria-label={`Oil Baron ${rows}x${cols} Game Board`}>
      {/* Board Top Toolbar */}
      <div className="board-toolbar">
        <div className="toolbar-left">
          <div className="board-title-group">
            <Grid size={16} className="text-gold" />
            <span className="board-title">{t('board.title')}</span>
            <span className="badge">{rows} × {cols} Grid</span>
          </div>
        </div>

        <div className="toolbar-right">
          {/* Inspect Mode Toggle */}
          <button
            type="button"
            className={`btn btn-sm ${inspectModeActive ? 'btn-primary' : 'btn-secondary'}`}
            onClick={() => {
              playClick();
              setInspectModeActive((prev) => !prev);
            }}
            title={t('board.inspectModeTooltip')}
          >
            <Eye size={13} />
            <span>{inspectModeActive ? t('board.inspecting') : t('board.inspectMode')}</span>
          </button>

          {/* Hover Tooltip Toggle */}
          <button
            type="button"
            className={`btn btn-sm ${hoverTooltipEnabled ? 'btn-secondary' : 'btn-ghost'}`}
            onClick={() => {
              playClick();
              setHoverTooltipEnabled((prev) => !prev);
            }}
            title={t('board.tooltipToggle')}
          >
            <Info size={13} />
            <span>{t('board.tooltip')} {hoverTooltipEnabled ? 'ON' : 'OFF'}</span>
          </button>

          {/* Color Legend Toggle */}
          <button
            type="button"
            className={`btn btn-sm ${showLegend ? 'btn-secondary' : 'btn-ghost'}`}
            onClick={() => {
              playClick();
              setShowLegend((prev) => !prev);
            }}
            title={t('board.legendToggle')}
          >
            <HelpCircle size={13} />
            <span>{t('board.legend')}</span>
          </button>
        </div>
      </div>

      {/* Live Move Preview Status Strip */}
      <div className="board-preview-strip">
        <div
          className={`preview-tag ${
            moveClassification ? `preview-tag-${moveClassification.type}` : 'preview-tag-idle'
          }`}
        >
          {moveClassification && (
            <span className="preview-badge-pill">
              {moveClassification.type === 'isolated' && 'ISOLATED'}
              {moveClassification.type === 'expansion' && 'EXPANSION'}
              {moveClassification.type === 'founding' && 'FOUNDING'}
              {moveClassification.type === 'merger' && 'MERGER'}
              {moveClassification.type === 'illegal_safe_merger' && 'DEAD TILE'}
            </span>
          )}
          <span className="preview-desc-text">{getPreviewDescription()}</span>
        </div>
      </div>

      {/* Optional Collapsible Move Classification Legend */}
      {showLegend && (
        <div className="board-legend">
          <div className="legend-item">
            <span className="legend-swatch swatch-isolated" />
            <span className="legend-label">Isolated</span>
            <span className="legend-desc">No neighbors</span>
          </div>
          <div className="legend-item">
            <span className="legend-swatch swatch-expansion" />
            <span className="legend-label">Expansion</span>
            <span className="legend-desc">Extends 1 company</span>
          </div>
          <div className="legend-item">
            <span className="legend-swatch swatch-founding" />
            <span className="legend-label">Founding</span>
            <span className="legend-desc">Connects chits to found</span>
          </div>
          <div className="legend-item">
            <span className="legend-swatch swatch-merger" />
            <span className="legend-label">Merger</span>
            <span className="legend-desc">Connects 2+ companies</span>
          </div>
          <div className="legend-item">
            <span className="legend-swatch swatch-illegal" />
            <span className="legend-label">Dead Tile</span>
            <span className="legend-desc">Illegal safe merger</span>
          </div>
        </div>
      )}

      {/* Responsive Board Grid Scroll Wrapper */}
      <div className="board-scroll-area">
        <div
          className="board-grid sector-grid"
          role="grid"
          aria-label={`${rows} by ${cols} Tiles Grid`}
          style={{
            gridTemplateColumns: `28px repeat(${cols}, minmax(32px, 52px))`,
            gridTemplateRows: `24px repeat(${rows}, minmax(32px, 52px))`,
          }}
        >
          {/* Top-Left Corner Header */}
          <div className="board-coord-header corner-header" role="columnheader">
            #
          </div>

          {/* Column Headers (1 to 12) */}
          {Array.from({ length: cols }).map((_, c) => (
            <div key={`col-${c}`} className="board-coord-header col-header" role="columnheader">
              {c + 1}
            </div>
          ))}

          {/* Board Rows (A through J) */}
          {Array.from({ length: rows }).map((_, r) => {
            const rowLabel = ROW_LABELS[r] ?? String.fromCharCode(65 + r);

            return (
              <React.Fragment key={`row-${r}`}>
                {/* Row Header (Letter) */}
                <div className="board-coord-header row-header" role="rowheader">
                  {rowLabel}
                </div>

                {/* Row Cells (12 cells) */}
                {Array.from({ length: cols }).map((_, c) => {
                  const cell = board[r]?.[c] ?? { type: 'empty' };
                  const key = `${r},${c}`;
                  const formatted = `${rowLabel}-${c + 1}`;

                  const isInHand = handLookup.has(key);
                  const isSelectedHand = coordEquals(selectedTileCoord, { row: r, col: c });
                  const isHoveredCell = coordEquals(hoveredCellCoord, { row: r, col: c });
                  const isPreviewTarget = coordEquals(activePreviewTarget, { row: r, col: c });

                  // Determine preview styling class for target cell
                  let previewClass = '';
                  if (isPreviewTarget && moveClassification) {
                    previewClass = `preview-${moveClassification.type}`;
                  }

                  // Determine if this cell is highlighted due to connected preview
                  let connectedHighlightClass = '';
                  if (previewHighlights.connectedKeys.has(key)) {
                    connectedHighlightClass = 'highlight-connected';
                  } else if (
                    cell.type === 'company' &&
                    previewHighlights.expandingCompanyId === cell.companyId
                  ) {
                    connectedHighlightClass = 'highlight-connected';
                  } else if (
                    cell.type === 'company' &&
                    previewHighlights.mergerCompanyIds.has(cell.companyId)
                  ) {
                    connectedHighlightClass = 'highlight-merger';
                  }

                  // Company visual metadata if occupied
                  const companyVisual =
                    cell.type === 'company'
                      ? getCompanyVisual(cell.companyId, definitions)
                      : null;
                  const companyState =
                    cell.type === 'company' ? companies[cell.companyId] : null;

                  const isSafeCompany =
                    cell.type === 'company' &&
                    Boolean(
                      companyState &&
                      (companyState.isSafe || companyState.size >= safeThreshold)
                    );

                  const isTarget = Boolean(
                    isPreviewTarget ||
                    isSelectedHand ||
                    (previewTileCoord && coordEquals(previewTileCoord, { row: r, col: c })) ||
                    (cell.type === 'empty' && isHoveredCell)
                  );

                  const cellClassName = [
                    'board-cell',
                    cell.type,
                    'parcel',
                    isSafeCompany ? 'well-safe' : '',
                    isTarget ? 'target' : '',
                    previewClass,
                    connectedHighlightClass,
                    isInHand ? 'is-in-hand' : '',
                    isSelectedHand ? 'is-selected-hand' : '',
                  ]
                    .filter(Boolean)
                    .join(' ');

                  return (
                    <div
                      key={`cell-${r}-${c}`}
                      className={cellClassName}
                      style={
                        cell.type === 'company' && companyVisual
                          ? {
                              backgroundColor: companyVisual.color,
                              color: companyVisual.textColor,
                              borderColor: companyVisual.color,
                            }
                          : undefined
                      }
                      onClick={() => handleCellClick(r, c, cell)}
                      onContextMenu={(e) => handleCellContextMenu(e, r, c)}
                      onMouseEnter={(e) => handleCellMouseEnter(e, r, c)}
                      onMouseLeave={handleCellMouseLeave}
                      role="gridcell"
                      aria-label={`Cell ${formatted}: ${cell.type}`}
                      tabIndex={0}
                      onKeyDown={(e) => {
                        if (e.key === 'Enter' || e.key === ' ') {
                          e.preventDefault();
                          handleCellClick(r, c, cell);
                        } else if (e.key === 'i' || e.key === 'I') {
                          e.preventDefault();
                          handleSetInspectedCoord({ row: r, col: c });
                        }
                      }}
                      data-row={r}
                      data-col={c}
                      data-coord={formatted}
                      data-type={cell.type}
                    >
                      {/* EMPTY CELL */}
                      {cell.type === 'empty' && (
                        <>
                          {showCoordinates ? (
                            <span className="cell-coord-label">{formatted}</span>
                          ) : (
                            !isTarget && <span className="parcel-guide-dot" aria-hidden="true" />
                          )}
                          {isInHand && <span className="cell-in-hand-indicator" title="In your hand" />}
                          {isTarget && (
                            <span className="cell-preview-overlay" data-testid="target-reticle">
                              {moveClassification?.type === 'illegal_safe_merger' ? (
                                <Ban size={16} color="#ef4444" />
                              ) : (
                                <Crosshair size={14} className="text-gold" />
                              )}
                            </span>
                          )}
                        </>
                      )}

                      {/* UNINCORPORATED PLACED CHIT */}
                      {cell.type === 'unincorporated' && (
                        <div className="chit-content">
                          <span className="chit-dot" />
                          <span className="chit-label">{formatted}</span>
                        </div>
                      )}

                      {/* COMPANY WELLHEAD TILE */}
                      {cell.type === 'company' && companyVisual && (() => {
                        const glyph =
                          COMPANY_GLYPHS[cell.companyId] ??
                          (companyState?.tier === 'mid' ? '◆' : '▲');
                        const initial = companyVisual.initials;

                        return (
                          <div
                            className={`well ${cell.companyId} ${isSafeCompany ? 'safe' : ''}`}
                            data-testid={`well-${cell.companyId}`}
                          >
                            <span className="well-glyph">{glyph}</span>
                            <span className="well-initial">{initial}</span>
                            {companyState && (
                              <span className="tile-size-badge" title={`Chain size: ${companyState.size}`}>
                                {companyState.size >= safeThreshold ? (
                                  <ShieldCheck size={8} />
                                ) : (
                                  companyState.size
                                )}
                              </span>
                            )}
                          </div>
                        );
                      })()}

                      {/* DISCARDED CELL */}
                      {cell.type === 'discarded' && (
                        <div className="discarded-tile-content" data-testid="discarded-marker">
                          <span className="tile-coord-micro">{formatted}</span>
                          <span className="discarded-x-mark">✕</span>
                        </div>
                      )}
                    </div>
                  );
                })}
              </React.Fragment>
            );
          })}
        </div>
      </div>

      {/* Floating Quick Peek Tooltip (when hover tooltip enabled) */}
      {hoverTooltipEnabled && hoveredCellCoord && (
        <TileInspector
          coord={hoveredCellCoord}
          board={board}
          companies={companies}
          definitions={definitions}
          safeThreshold={safeThreshold}
          mode="tooltip"
          anchorPosition={tooltipPos}
          isPlayableInHand={handLookup.has(`${hoveredCellCoord.row},${hoveredCellCoord.col}`)}
        />
      )}

      {/* Quick Peek Inspector Modal (on right-click, inspect button, or click) */}
      {activeInspectedCoord && (
        <TileInspector
          coord={activeInspectedCoord}
          board={board}
          companies={companies}
          definitions={definitions}
          safeThreshold={safeThreshold}
          mode="modal"
          isOpen={true}
          onClose={() => handleSetInspectedCoord(null)}
          onPlaceTile={onPlaceTile}
          isPlayableInHand={handLookup.has(`${activeInspectedCoord.row},${activeInspectedCoord.col}`)}
        />
      )}
    </div>
  );
};

export default GameBoard;
