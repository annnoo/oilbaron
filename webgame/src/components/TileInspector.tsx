import React, { useEffect } from 'react';
import {
  Eye,
  X,
  ShieldCheck,
  ShieldAlert,
  Compass,
  Layers,
  DollarSign,
  TrendingUp,
  Coins,
  AlertTriangle,
  Flame,
  CheckCircle2,
  Ban,
  ArrowUp,
  ArrowDown,
  ArrowLeft,
  ArrowRight,
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
  getContiguousCoordinates,
  getNeighborDetails,
  isCompanySafe,
  type NeighborDetail,
} from '../engine/board';
import { calculateSharePrice, getCompanyDefinition } from '../engine/market';
import { playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

export interface CompanyVisual {
  id: string;
  name: string;
  shortCode: string;
  initials: string;
  color: string;
  textColor: string;
}

export const COMPANY_VISUALS: Record<string, CompanyVisual> = {
  american: { id: 'american', name: 'American Natural Gas', shortCode: 'AM', initials: 'A', color: '#e63946', textColor: '#ffffff' },
  arabian:  { id: 'arabian',  name: 'Arabian Oil',           shortCode: 'AB', initials: 'Ar', color: '#f4a261', textColor: '#090d12' },
  imperial: { id: 'imperial', name: 'Imperial Petroleum',    shortCode: 'IM', initials: 'I', color: '#2a9d8f', textColor: '#ffffff' },
  supra:    { id: 'supra',    name: 'Supra Oil',             shortCode: 'SU', initials: 'S', color: '#e76f51', textColor: '#ffffff' },
  tower:    { id: 'tower',    name: 'Tower Drilling',        shortCode: 'TW', initials: 'T', color: '#457b9d', textColor: '#ffffff' },
  royal:    { id: 'royal',    name: 'Royal Dutch',           shortCode: 'RD', initials: 'R', color: '#1d3557', textColor: '#ffffff' },
  weemoco:  { id: 'weemoco',  name: 'Weemoco Oil',           shortCode: 'WM', initials: 'W', color: '#38b000', textColor: '#ffffff' },
  purple:   { id: 'purple',   name: 'Purple Oil Company, Int.', shortCode: 'PR', initials: 'P', color: '#8b5cf6', textColor: '#ffffff' },
};

export function getCompanyVisual(companyId: CompanyId, definitions?: readonly CompanyDefinition[]): CompanyVisual {
  if (COMPANY_VISUALS[companyId]) {
    return COMPANY_VISUALS[companyId]!;
  }
  const def = definitions?.find((d) => d.id === companyId);
  if (def) {
    return {
      id: def.id,
      name: def.name,
      shortCode: def.name.slice(0, 2).toUpperCase(),
      initials: def.name[0]?.toUpperCase() ?? def.id[0]?.toUpperCase() ?? '?',
      color: def.color,
      textColor: '#ffffff',
    };
  }
  return {
    id: companyId,
    name: companyId,
    shortCode: companyId.slice(0, 2).toUpperCase(),
    initials: companyId[0]?.toUpperCase() ?? '?',
    color: '#38bdf8',
    textColor: '#ffffff',
  };
}

export interface TileInspectorProps {
  /**
   * The board coordinate to inspect. If null, the inspector will not render.
   */
  coord: TileCoord | null;

  /**
   * Current 2D board state.
   */
  board: readonly (readonly CellState[])[];

  /**
   * Company states indexed by companyId.
   */
  companies?: Partial<Record<CompanyId, CompanyState>>;

  /**
   * Company template definitions. Defaults to CLASSIC_COMPANIES.
   */
  definitions?: readonly CompanyDefinition[];

  /**
   * Safe size threshold (defaults to 11).
   */
  safeThreshold?: number;

  /**
   * Whether the inspector is visibly open (defaults to true when coord is provided).
   */
  isOpen?: boolean;

  /**
   * Display mode:
   * - 'modal': Authentic Windows 3.1 Quick Peek modal dialog with backdrop
   * - 'tooltip': Lightweight floating tooltip near cursor or anchor
   * - 'card': Embedded inline inspection panel
   */
  mode?: 'modal' | 'tooltip' | 'card';

  /**
   * Optional screen coordinates for floating tooltip mode.
   */
  anchorPosition?: { x: number; y: number } | null;

  /**
   * Callback invoked when closing the inspector.
   */
  onClose?: () => void;

  /**
   * Optional callback to place a chit at this coordinate (if empty & playable).
   */
  onPlaceTile?: (coord: TileCoord) => void;

  /**
   * Whether this coordinate is currently held in the active player's hand.
   */
  isPlayableInHand?: boolean;

  /**
   * Optional extra CSS class name.
   */
  className?: string;
}

export const TileInspector: React.FC<TileInspectorProps> = ({
  coord,
  board,
  companies = {},
  definitions = CLASSIC_COMPANIES,
  safeThreshold = DEFAULT_CONFIG.safeThreshold,
  isOpen = true,
  mode = 'modal',
  anchorPosition,
  onClose,
  onPlaceTile,
  isPlayableInHand = false,
  className = '',
}) => {
  // Listen for Escape key in modal mode
  useEffect(() => {
    if (!isOpen || !coord || mode !== 'modal') return;

    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === 'Escape') {
        e.preventDefault();
        onClose?.();
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [isOpen, coord, mode, onClose]);

  if (!isOpen || !coord) {
    return null;
  }

  const rows = board.length;
  const cols = board[0]?.length ?? 0;

  if (coord.row < 0 || coord.row >= rows || coord.col < 0 || coord.col >= cols) {
    return null;
  }

  const { t, language } = useTranslation();
  const cell = board[coord.row]?.[coord.col] ?? { type: 'empty' };
  const formattedCoord = formatCoord(coord);
  const neighbors = getNeighborDetails(board, coord);

  // Compute cell-specific details
  let companyDef: CompanyDefinition | undefined;
  let companyState: CompanyState | undefined;
  let companyVisual: CompanyVisual | undefined;
  let chainCoords: TileCoord[] = [];
  let isSafe = false;
  let sharePrice = 0;
  let majorityBonus = 0;
  let minorityBonus = 0;
  let movePreview: ReturnType<typeof classifyMove> | null = null;

  if (cell.type === 'company') {
    companyDef = getCompanyDefinition(definitions, cell.companyId);
    companyState = companies[cell.companyId];
    companyVisual = getCompanyVisual(cell.companyId, definitions);
    chainCoords = getContiguousCoordinates(board, coord);
    const chainSize = companyState?.size ?? chainCoords.length;
    isSafe = isCompanySafe(companyState, safeThreshold);
    if (companyDef) {
      sharePrice = calculateSharePrice(companyDef, chainSize);
      majorityBonus = 10 * sharePrice;
      minorityBonus = 5 * sharePrice;
    }
  } else if (cell.type === 'unincorporated') {
    chainCoords = getContiguousCoordinates(board, coord);
  } else if (cell.type === 'empty') {
    movePreview = classifyMove(board, coord, companies, safeThreshold);
  }

  const handlePlace = () => {
    if (onPlaceTile && cell.type === 'empty') {
      playClick();
      onPlaceTile(coord);
      onClose?.();
    }
  };

  // Helper for rendering directional icon
  const getDirectionIcon = (dir: NeighborDetail['direction']) => {
    switch (dir) {
      case 'north': return <ArrowUp size={12} />;
      case 'south': return <ArrowDown size={12} />;
      case 'west':  return <ArrowLeft size={12} />;
      case 'east':  return <ArrowRight size={12} />;
    }
  };

  // Content for Quick Peek Tooltip mode
  if (mode === 'tooltip') {
    return (
      <div
        className={`tile-inspector-tooltip ${className}`}
        style={
          anchorPosition
            ? {
                position: 'fixed',
                left: `${anchorPosition.x + 12}px`,
                top: `${anchorPosition.y + 12}px`,
                zIndex: 1100,
                pointerEvents: 'none',
              }
            : undefined
        }
        role="tooltip"
      >
        <div className="tooltip-header">
          <span className="tooltip-coord">{formattedCoord}</span>
          {cell.type === 'company' && companyVisual && (
            <span
              className="tooltip-company-badge"
              style={{ backgroundColor: companyVisual.color, color: companyVisual.textColor }}
            >
              {companyVisual.shortCode}
            </span>
          )}
        </div>

        <div className="tooltip-body">
          {cell.type === 'empty' && movePreview && (
            <div className="tooltip-preview">
              <span className={`preview-pill preview-pill-${movePreview.type}`}>
                {movePreview.type.toUpperCase().replace(/_/g, ' ')}
              </span>
              <span className="tooltip-sub">Empty Cell</span>
            </div>
          )}

          {cell.type === 'unincorporated' && (
            <div className="tooltip-chit">
              <span className="badge badge-gold">Wildcat Chit</span>
              <span className="tooltip-sub">{chainCoords.length} Connected</span>
            </div>
          )}

          {cell.type === 'company' && companyState && (
            <div className="tooltip-company">
              <div className="tooltip-title">{companyState.name}</div>
              <div className="tooltip-meta">
                <span>Size: {companyState.size}</span>
                <span>${sharePrice.toLocaleString()}</span>
                {isSafe ? (
                  <span className="badge badge-safe">SAFE</span>
                ) : (
                  <span className="tooltip-sub">Vulnerable</span>
                )}
              </div>
            </div>
          )}

          {cell.type === 'discarded' && (
            <div className="tooltip-chit">
              <span className="badge badge-danger">Discarded Tile</span>
              <span className="tooltip-sub">Permanently Blocked ('X')</span>
            </div>
          )}
        </div>
      </div>
    );
  }

  // Full Quick Peek Inspector Dialog (Windows 3.1 Modal Style)
  return (
    <div
      className={`modal-backdrop ${className}`}
      onClick={(e) => {
        if (e.target === e.currentTarget) {
          onClose?.();
        }
      }}
      role="dialog"
      aria-modal="true"
      aria-label={`Tile Inspector Quick Peek: ${formattedCoord}`}
    >
      <div className="modal-dialog tile-inspector-dialog">
        {/* Modal Title Header */}
        <div className="modal-header">
          <div className="modal-title">
            <Eye size={18} className="text-gold" />
            <span>{language === 'de' ? `Schnellansicht — Feld ${formattedCoord}` : `Quick Peek — Cell ${formattedCoord}`}</span>
            <span className="badge badge-gold" style={{ marginLeft: '6px' }}>
              {language === 'de'
                ? `Reihe ${String.fromCharCode(65 + coord.row)} • Spalte ${coord.col + 1}`
                : `Row ${String.fromCharCode(65 + coord.row)} • Col ${coord.col + 1}`}
            </span>
          </div>

          <button
            type="button"
            className="btn btn-ghost btn-sm"
            onClick={onClose}
            aria-label="Close Inspector"
          >
            <X size={16} />
          </button>
        </div>

        {/* Modal Main Body */}
        <div className="modal-body tile-inspector-body">
          {/* Section 1: Current Cell State */}
          <div className="inspector-card">
            <div className="inspector-card-header">
              <Layers size={14} />
              <span>{language === 'de' ? 'Feld-Topologie & Besitz' : 'Cell Topology & Ownership'}</span>
            </div>

            <div className="inspector-card-content">
              {/* EMPTY CELL */}
              {cell.type === 'empty' && (
                <div className="inspector-empty-state">
                  <div className="state-badge-row">
                    <span className="badge" style={{ backgroundColor: 'var(--bg-deep)', border: '1px solid var(--border-default)' }}>
                      {language === 'de' ? 'Freies Rasterfeld' : 'Empty Grid Slot'}
                    </span>
                    {isPlayableInHand && (
                      <span className="badge badge-safe">
                        <CheckCircle2 size={12} /> {language === 'de' ? 'Auf deiner Hand' : 'In Your Hand'}
                      </span>
                    )}
                  </div>

                  {movePreview && (
                    <div className={`preview-info-box preview-box-${movePreview.type}`}>
                      <div className="preview-info-title">
                        {movePreview.type === 'isolated' && (language === 'de' ? 'Einzelbohrung' : 'Isolated Wildcat Placement')}
                        {movePreview.type === 'expansion' && (language === 'de' ? 'Konzern-Erweiterung' : 'Corporate Expansion')}
                        {movePreview.type === 'founding' && (language === 'de' ? 'Konzern-Neugründung' : 'New Corporation Founding')}
                        {movePreview.type === 'merger' && (language === 'de' ? 'Konzernfusion' : 'Multi-Company Merger')}
                        {movePreview.type === 'illegal_safe_merger' && (language === 'de' ? 'Ungesetzliche Fusion (Totes Feld)' : 'Illegal Safe Merger (Dead Tile)')}
                      </div>

                      <div className="preview-info-desc">
                        {movePreview.type === 'isolated' && (
                          <p>
                            Placing a chit here touches no other tiles. It will become an isolated wildcat chit.
                          </p>
                        )}
                        {movePreview.type === 'expansion' && (
                          <p>
                            Expands <strong>{movePreview.companyId.toUpperCase()}</strong> by adding this tile plus{' '}
                            {movePreview.connectedCoords.length - 1} connected unincorporated chit(s).
                          </p>
                        )}
                        {movePreview.type === 'founding' && (
                          <p>
                            Connects {movePreview.connectedCoords.length} adjacent unincorporated chits. Triggering founding of a new oil company!
                          </p>
                        )}
                        {movePreview.type === 'merger' && (
                          <p>
                            Triggers a corporate takeover merger between:{' '}
                            <strong>{movePreview.participatingCompanyIds.join(', ').toUpperCase()}</strong>.
                          </p>
                        )}
                        {movePreview.type === 'illegal_safe_merger' && (
                          <p style={{ color: '#f87171' }}>
                            <Ban size={14} style={{ display: 'inline', verticalAlign: 'middle', marginRight: '4px' }} />
                            This placement would merge two safe corporations ({movePreview.safeCompanyIds.join(' & ').toUpperCase()}).
                            This move is illegal under Oil Baron rules.
                          </p>
                        )}
                      </div>
                    </div>
                  )}
                </div>
              )}

              {/* UNINCORPORATED PLACED CHIT */}
              {cell.type === 'unincorporated' && (
                <div className="inspector-uninc-state">
                  <div className="state-badge-row">
                    <span className="badge badge-gold">Unincorporated Chit</span>
                    <span className="badge">Wildcat Drill Site</span>
                  </div>
                  <p style={{ color: 'var(--text-secondary)', fontSize: '13px', marginTop: '6px' }}>
                    This tile has been drilled as an independent wildcat strike. It is not currently owned by any oil corporation.
                  </p>
                  <div style={{ marginTop: '8px', fontSize: '12px', color: 'var(--text-muted)' }}>
                    Contiguous Wildcat Cluster: <strong>{chainCoords.length} chit(s)</strong>
                  </div>
                </div>
              )}

              {/* DISCARDED CELL */}
              {cell.type === 'discarded' && (
                <div className="inspector-uninc-state">
                  <div className="state-badge-row">
                    <span className="badge badge-danger">Discarded Tile ('X')</span>
                    <span className="badge">Permanently Blocked</span>
                  </div>
                  <p style={{ color: 'var(--text-secondary)', fontSize: '13px', marginTop: '6px' }}>
                    {language === 'de'
                      ? 'Dieses Feld wurde beim Ablegen eines unspielbaren toten Plättchens markiert und ist dauerhaft blockiert.'
                      : 'This tile was marked upon discarding an unplayable dead tile and is permanently blocked.'}
                  </p>
                </div>
              )}

              {/* COMPANY TILE */}
              {cell.type === 'company' && companyState && companyVisual && (
                <div className="inspector-company-state">
                  <div className="company-header-banner" style={{ borderLeft: `4px solid ${companyVisual.color}` }}>
                    <div
                      className="company-logo-avatar"
                      style={{ backgroundColor: companyVisual.color, color: companyVisual.textColor }}
                    >
                      {companyVisual.shortCode}
                    </div>

                    <div className="company-details">
                      <div className="company-name">{companyState.name}</div>
                      <div className="company-meta-tags">
                        <span className="badge">{companyState.tier.toUpperCase()} TIER</span>
                        {isSafe ? (
                          <span className="badge badge-safe">
                            <ShieldCheck size={12} /> SAFE ({companyState.size} tiles)
                          </span>
                        ) : (
                          <span className="badge badge-gold">
                            <ShieldAlert size={12} /> VULNERABLE ({safeThreshold - companyState.size} to safe)
                          </span>
                        )}
                      </div>
                    </div>
                  </div>

                  {/* Financial & Corporate Metrics */}
                  <div className="company-metrics-grid">
                    <div className="metric-cell">
                      <span className="metric-label">Chain Size</span>
                      <span className="metric-value">{companyState.size} Tiles</span>
                    </div>

                    <div className="metric-cell">
                      <span className="metric-label">Share Price</span>
                      <span className="metric-value font-mono text-gold">${sharePrice.toLocaleString()}</span>
                    </div>

                    <div className="metric-cell">
                      <span className="metric-label">Bank Shares</span>
                      <span className="metric-value">
                        {companyState.availableShares} / {DEFAULT_CONFIG.totalSharesPerCompany}
                      </span>
                    </div>

                    <div className="metric-cell">
                      <span className="metric-label">Majority Bonus (1st)</span>
                      <span className="metric-value text-green">${majorityBonus.toLocaleString()}</span>
                    </div>

                    <div className="metric-cell">
                      <span className="metric-label">Minority Bonus (2nd)</span>
                      <span className="metric-value text-blue">${minorityBonus.toLocaleString()}</span>
                    </div>

                    <div className="metric-cell">
                      <span className="metric-label">Status</span>
                      <span className="metric-value">
                        {isSafe ? 'Permanent Safe' : 'Can Be Merged'}
                      </span>
                    </div>
                  </div>
                </div>
              )}
            </div>
          </div>

          {/* Section 2: Cardinal Neighbors & Connectivity */}
          <div className="inspector-card">
            <div className="inspector-card-header">
              <Compass size={14} />
              <span>{language === 'de' ? 'Kardinale Nachbarschaft (Angrenzende Felder)' : 'Cardinal Connectivity (Adjacent Neighbors)'}</span>
            </div>

            <div className="inspector-card-content">
              <div className="neighbors-grid">
                {neighbors.map((n) => {
                  let neighborLabel = language === 'de' ? 'Frei' : 'Empty';
                  let neighborColor = 'var(--text-muted)';
                  let neighborBadgeClass = '';

                  if (n.isEdge) {
                    neighborLabel = language === 'de' ? 'Spielfeldrand' : 'Board Border';
                    neighborColor = 'var(--text-muted)';
                  } else if (n.cell?.type === 'unincorporated') {
                    neighborLabel = language === 'de' ? 'Wilde Bohrung' : 'Unincorporated Chit';
                    neighborColor = 'var(--accent-gold)';
                    neighborBadgeClass = 'badge-gold';
                  } else if (n.cell?.type === 'company') {
                    const comp = companies[n.cell.companyId];
                    neighborLabel = comp ? comp.name : n.cell.companyId.toUpperCase();
                    neighborColor = comp?.color ?? 'var(--accent-blue)';
                    neighborBadgeClass = 'badge-active';
                  } else if (n.cell?.type === 'discarded') {
                    neighborLabel = language === 'de' ? 'Abgelegt (Blockiert)' : 'Discarded (Blocked)';
                    neighborColor = '#ef4444';
                    neighborBadgeClass = 'badge-danger';
                  }

                  return (
                    <div key={n.direction} className="neighbor-tile">
                      <div className="neighbor-direction">
                        {getDirectionIcon(n.direction)}
                        <span>{language === 'de' ? (n.direction === 'north' ? 'NORDEN' : n.direction === 'south' ? 'SÜDEN' : n.direction === 'west' ? 'WESTEN' : 'OSTEN') : n.direction.toUpperCase()}</span>
                      </div>

                      <div className="neighbor-coord">
                        {n.isEdge ? '—' : formatCoord(n.coord)}
                      </div>

                      <div className="neighbor-status" style={{ color: neighborColor }}>
                        {neighborLabel}
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          </div>

          {/* Section 3: Chain Coordinates (if company or unincorporated) */}
          {chainCoords.length > 0 && (
            <div className="inspector-card">
              <div className="inspector-card-header">
                <TrendingUp size={14} />
                <span>Connected Chain Tiles ({chainCoords.length})</span>
              </div>

              <div className="inspector-card-content">
                <div className="chain-chips-container">
                  {chainCoords.map((c) => (
                    <span
                      key={`${c.row}-${c.col}`}
                      className={`chain-chip ${c.row === coord.row && c.col === coord.col ? 'current' : ''}`}
                    >
                      {formatCoord(c)}
                    </span>
                  ))}
                </div>
              </div>
            </div>
          )}
        </div>

        {/* Modal Action Footer */}
        <div className="modal-footer">
          {cell.type === 'empty' && onPlaceTile && (
            <button
              type="button"
              className="btn btn-primary"
              onClick={handlePlace}
            >
              <CheckCircle2 size={14} /> Place Chit at {formattedCoord}
            </button>
          )}

          <button
            type="button"
            className="btn btn-secondary"
            onClick={onClose}
          >
            Close
          </button>
        </div>
      </div>
    </div>
  );
};

export default TileInspector;
