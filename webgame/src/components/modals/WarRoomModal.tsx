import React from 'react';
import { Compass, X, ShieldCheck, MapPin, Layers, Award } from 'lucide-react';
import { GameState } from '../../engine/types';
import { calculateSharePrice } from '../../engine/market';

export interface WarRoomModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

export const WarRoomModal: React.FC<WarRoomModalProps> = ({
  isOpen,
  onClose,
  gameState,
}) => {
  if (!isOpen) return null;

  const totalGridCells = 120; // 10 x 12
  let placedTilesCount = 0;
  let unincorporatedCount = 0;

  for (let r = 0; r < gameState.board.length; r++) {
    for (let c = 0; c < (gameState.board[r]?.length ?? 0); c++) {
      const cell = gameState.board[r]![c]!;
      if (cell.type !== 'empty') placedTilesCount++;
      if (cell.type === 'unincorporated') unincorporatedCount++;
    }
  }

  const activeCompanies = gameState.config.companies.filter((def) => {
    const comp = gameState.companies[def.id];
    return comp && comp.isActive;
  });

  const safeCompanies = activeCompanies.filter((def) => {
    const comp = gameState.companies[def.id];
    return comp && comp.isSafe;
  });

  const remainingBag = gameState.tilePool.length;

  return (
    <div
      className="modal-backdrop"
      role="presentation"
      onClick={(e) => {
        if (e.target === e.currentTarget) onClose();
      }}
    >
      <div
        className="modal-dialog modal-lg"
        role="dialog"
        aria-modal="true"
        aria-labelledby="war-room-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="war-room-title">
            <Compass size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>War Room — Strategic Intelligence</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close war room modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Key Intelligence Metrics */}
          <div className="discarded-summary-grid">
            <div className="discarded-stat-card">
              <span className="discarded-stat-value">{placedTilesCount} / {totalGridCells}</span>
              <span className="discarded-stat-label">Grid Occupancy ({((placedTilesCount / totalGridCells) * 100).toFixed(0)}%)</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#22c55e' }}>
                {safeCompanies.length} / {activeCompanies.length}
              </span>
              <span className="discarded-stat-label">Safe Corporations (≥{gameState.config.safeThreshold})</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#38bdf8' }}>
                {remainingBag}
              </span>
              <span className="discarded-stat-label">Tiles in Draw Bag</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#fbbf24' }}>
                {unincorporatedCount}
              </span>
              <span className="discarded-stat-label">Unincorporated Reserves</span>
            </div>
          </div>

          {/* Territory Dominance Breakdown */}
          <div style={{ marginTop: '16px' }}>
            <div style={{ fontSize: '12px', fontWeight: 700, textTransform: 'uppercase', color: 'var(--text-secondary)', marginBottom: '8px' }}>
              Active Corporation Territory Control
            </div>

            {activeCompanies.length === 0 ? (
              <div style={{ padding: '24px', textAlign: 'center', color: 'var(--text-muted)' }}>
                No active companies on the board yet.
              </div>
            ) : (
              <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
                {activeCompanies.map((def) => {
                  const comp = gameState.companies[def.id]!;
                  const price = calculateSharePrice(def, comp.size);
                  const pct = ((comp.size / totalGridCells) * 100).toFixed(1);

                  return (
                    <div
                      key={def.id}
                      style={{
                        padding: '10px 14px',
                        background: 'var(--bg-surface-elevated)',
                        border: '1px solid var(--border-default)',
                        borderLeft: `5px solid ${def.color}`,
                        borderRadius: 'var(--radius-sm)',
                      }}
                    >
                      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '6px' }}>
                        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                          <span style={{ fontWeight: 700, fontSize: '13px' }}>{def.name}</span>
                          {comp.isSafe ? (
                            <span style={{ fontSize: '10px', fontWeight: 700, color: '#22c55e', background: 'rgba(34, 197, 94, 0.15)', padding: '1px 5px', borderRadius: '3px' }}>
                              SAFE MONOPOLY
                            </span>
                          ) : (
                            <span style={{ fontSize: '10px', color: 'var(--text-muted)' }}>
                              Vulnerable ({gameState.config.safeThreshold - comp.size} more to safe)
                            </span>
                          )}
                        </div>
                        <div style={{ fontSize: '12px', fontFamily: 'var(--font-mono)' }}>
                          <strong>{comp.size} tiles</strong> ({pct}%) | <span style={{ color: 'var(--text-amber)' }}>${price}/share</span>
                        </div>
                      </div>

                      {/* Progress bar towards safety */}
                      <div style={{ height: '6px', background: 'rgba(255, 255, 255, 0.05)', borderRadius: '3px', overflow: 'hidden' }}>
                        <div
                          style={{
                            height: '100%',
                            width: `${Math.min(100, (comp.size / gameState.config.safeThreshold) * 100)}%`,
                            backgroundColor: def.color,
                          }}
                        />
                      </div>
                    </div>
                  );
                })}
              </div>
            )}
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end' }}>
          <button type="button" className="btn btn-secondary" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
};
