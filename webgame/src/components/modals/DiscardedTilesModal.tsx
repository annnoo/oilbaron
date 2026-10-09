import React, { useState } from 'react';
import { Trash2, AlertTriangle, X, Layers } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { GameState, TileCoord } from '../../engine/types';
import { isTileDead, classifyMove, formatCoord } from '../../engine/board';
import { useTranslation } from '../../i18n';

export interface DiscardedTilesModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

export const DiscardedTilesModal: React.FC<DiscardedTilesModalProps> = ({
  isOpen,
  onClose,
  gameState,
}) => {
  const { t } = useTranslation();
  const [activeTab, setActiveTab] = useState<'discarded' | 'hand_dead' | 'all_dead'>('discarded');

  if (!isOpen) return null;

  const discardedTiles: TileCoord[] = (gameState.history || [])
    .filter((a): a is { type: 'DISCARD_DEAD_TILE'; coord: TileCoord } => a.type === 'DISCARD_DEAD_TILE')
    .map((a) => a.coord);
  const safeThreshold = gameState.config.safeThreshold;
  const companies = gameState.companies;

  // Scan all player hands for dead tiles
  const deadTilesInHands: { playerIndex: number; playerName: string; tile: TileCoord }[] = [];
  gameState.players.forEach((player, idx) => {
    player.hand.forEach((tile) => {
      if (isTileDead(gameState.board, tile, companies, safeThreshold)) {
        deadTilesInHands.push({ playerIndex: idx, playerName: player.name, tile });
      }
    });
  });

  // Calculate unplayed dead tiles on the entire board grid
  const allDeadOnGrid: TileCoord[] = [];
  const rows = gameState.board.length;
  const cols = gameState.board[0]?.length ?? 0;
  for (let r = 0; r < rows; r++) {
    for (let c = 0; c < cols; c++) {
      const cell = gameState.board[r]?.[c];
      if (cell && cell.type === 'empty') {
        const coord: TileCoord = { row: r, col: c };
        const cl = classifyMove(gameState.board, coord, companies, safeThreshold);
        if (cl.type === 'illegal_safe_merger') {
          allDeadOnGrid.push(coord);
        }
      }
    }
  }

  return (
    <div
      className="modal-backdrop"
      role="presentation"
      onClick={(e) => {
        if (e.target === e.currentTarget) onClose();
      }}
    >
      <div
        className="modal-dialog modal-md"
        role="dialog"
        aria-modal="true"
        aria-labelledby="discarded-tiles-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="discarded-tiles-title">
            <Trash2 size={18} style={{ color: '#f87171' }} />
            <span>{t('discarded.title')}</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close discarded pieces dialog"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Quick Metrics Bar */}
          <div className="discarded-summary-grid">
            <div className="discarded-stat-card">
              <span className="discarded-stat-value">{discardedTiles.length}</span>
              <span className="discarded-stat-label">{t('discarded.discardedHistory')}</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#ef4444' }}>
                {deadTilesInHands.length}
              </span>
              <span className="discarded-stat-label">{t('discarded.deadInHands')}</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#38bdf8' }}>
                {gameState.tilePool.length}
              </span>
              <span className="discarded-stat-label">{t('discarded.remainingInBag')}</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#fbbf24' }}>
                {allDeadOnGrid.length}
              </span>
              <span className="discarded-stat-label">{t('discarded.deadUnplacedGrid')}</span>
            </div>
          </div>

          {/* Navigation Tabs */}
          <div className="discarded-tabs-bar">
            <button
              type="button"
              className={`discarded-tab-btn ${activeTab === 'discarded' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveTab('discarded');
              }}
            >
              {t('discarded.permanentlyDiscarded')} ({discardedTiles.length})
            </button>
            <button
              type="button"
              className={`discarded-tab-btn ${activeTab === 'hand_dead' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveTab('hand_dead');
              }}
            >
              {t('discarded.deadTilesInHands')} ({deadTilesInHands.length})
            </button>
            <button
              type="button"
              className={`discarded-tab-btn ${activeTab === 'all_dead' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveTab('all_dead');
              }}
            >
              {t('discarded.allDeadOnBoard', { count: allDeadOnGrid.length })}
            </button>
          </div>

          {/* Tab 1: Permanently Discarded */}
          {activeTab === 'discarded' && (
            <div>
              {discardedTiles.length === 0 ? (
                <div style={{ padding: '24px', textAlign: 'center', color: 'var(--text-muted)' }}>
                  {t('discarded.noDiscardedPermanently')}
                </div>
              ) : (
                <div className="dead-tiles-grid">
                  {discardedTiles.map((coord, idx) => (
                    <div key={`${coord.row}-${coord.col}-${idx}`} className="dead-tile-card">
                      <div className="dead-tile-header">
                        <span className="dead-tile-coord">{formatCoord(coord)}</span>
                        <Trash2 size={12} color="#f87171" />
                      </div>
                      <span className="dead-tile-reason">{t('discarded.discardedReason')}</span>
                    </div>
                  ))}
                </div>
              )}
            </div>
          )}

          {/* Tab 2: Dead Tiles in Hands */}
          {activeTab === 'hand_dead' && (
            <div>
              {deadTilesInHands.length === 0 ? (
                <div style={{ padding: '24px', textAlign: 'center', color: 'var(--text-muted)' }}>
                  {t('discarded.noDeadInHands')}
                </div>
              ) : (
                <div className="dead-tiles-grid">
                  {deadTilesInHands.map((item, idx) => (
                    <div key={`${item.tile.row}-${item.tile.col}-${idx}`} className="dead-tile-card">
                      <div className="dead-tile-header">
                        <span className="dead-tile-coord">{formatCoord(item.tile)}</span>
                        <AlertTriangle size={12} color="#f59e0b" />
                      </div>
                      <span className="dead-tile-reason" style={{ color: 'var(--text-secondary)' }}>
                        {t('discarded.heldBy', { name: item.playerName })}
                      </span>
                    </div>
                  ))}
                </div>
              )}
            </div>
          )}

          {/* Tab 3: All Dead on Board */}
          {activeTab === 'all_dead' && (
            <div>
              {allDeadOnGrid.length === 0 ? (
                <div style={{ padding: '24px', textAlign: 'center', color: 'var(--text-muted)' }}>
                  {t('discarded.noDeadOnGrid')}
                </div>
              ) : (
                <div className="dead-tiles-grid">
                  {allDeadOnGrid.map((coord) => (
                    <div key={`${coord.row}-${coord.col}`} className="dead-tile-card">
                      <div className="dead-tile-header">
                        <span className="dead-tile-coord">{formatCoord(coord)}</span>
                        <AlertTriangle size={12} color="#ef4444" />
                      </div>
                      <span className="dead-tile-reason">
                        {t('discarded.connectsSafe')}
                      </span>
                    </div>
                  ))}
                </div>
              )}
            </div>
          )}
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end' }}>
          <button type="button" className="btn btn-secondary" onClick={onClose}>
            {t('common.close')}
          </button>
        </div>
      </div>
    </div>
  );
};
