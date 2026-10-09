import React, { useState } from 'react';
import { PieChart, X, Percent, Hash } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { GameState } from '../../engine/types';

export interface ShareGraphModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

export const ShareGraphModal: React.FC<ShareGraphModalProps> = ({
  isOpen,
  onClose,
  gameState,
}) => {
  const [viewMode, setViewMode] = useState<'totals' | 'percentages'>('totals');

  if (!isOpen) return null;

  const totalSharesPerCompany = gameState.config.totalSharesPerCompany || 25;
  const companies = gameState.config.companies.filter((c) => !c.isDormant);

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
        aria-labelledby="share-graph-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="share-graph-title">
            <PieChart size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Share Graph — Stock Ownership Matrix</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close share graph modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Header & Mode Switcher */}
          <div
            style={{
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              marginBottom: '16px',
            }}
          >
            <span style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
              Comprehensive share distribution across all players and bank reserves.
            </span>

            {/* Percentages vs Totals Toggle */}
            <div style={{ display: 'flex', gap: '4px', background: 'var(--bg-surface-elevated)', padding: '3px', borderRadius: 'var(--radius-sm)' }}>
              <button
                type="button"
                className={`discarded-tab-btn ${viewMode === 'totals' ? 'is-active' : ''}`}
                onClick={() => {
                  playClick();
                  setViewMode('totals');
                }}
              >
                <Hash size={13} style={{ marginRight: '4px', verticalAlign: 'middle' }} />
                Totals
              </button>
              <button
                type="button"
                className={`discarded-tab-btn ${viewMode === 'percentages' ? 'is-active' : ''}`}
                onClick={() => {
                  playClick();
                  setViewMode('percentages');
                }}
              >
                <Percent size={13} style={{ marginRight: '4px', verticalAlign: 'middle' }} />
                Percentages (%)
              </button>
            </div>
          </div>

          {/* Matrix Table */}
          <div style={{ border: '1px solid var(--border-default)', borderRadius: 'var(--radius-sm)', overflowX: 'auto' }}>
            <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '12px' }}>
              <thead>
                <tr style={{ background: 'var(--bg-surface-elevated)', borderBottom: '1px solid var(--border-subtle)' }}>
                  <th style={{ padding: '8px 12px', textAlign: 'left' }}>Company</th>
                  {gameState.players.map((p) => (
                    <th key={p.id} style={{ padding: '8px 10px', textAlign: 'center' }}>
                      <span style={{ display: 'inline-flex', alignItems: 'center', gap: '4px' }}>
                        {p.color && (
                          <span
                            style={{
                              width: '7px',
                              height: '7px',
                              borderRadius: '50%',
                              backgroundColor: p.color,
                              display: 'inline-block',
                            }}
                          />
                        )}
                        <span>{p.name}</span>
                      </span>
                    </th>
                  ))}
                  <th style={{ padding: '8px 10px', textAlign: 'center', color: 'var(--text-muted)' }}>
                    Bank Reserve
                  </th>
                </tr>
              </thead>
              <tbody>
                {companies.map((def) => {
                  const compState = gameState.companies[def.id];
                  const bankShares = compState ? compState.availableShares : totalSharesPerCompany;

                  return (
                    <tr key={def.id} style={{ borderBottom: '1px solid var(--border-subtle)' }}>
                      <td style={{ padding: '8px 12px', fontWeight: 600 }}>
                        <span style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}>
                          <span
                            style={{
                              width: '10px',
                              height: '10px',
                              borderRadius: '2px',
                              backgroundColor: def.color,
                              display: 'inline-block',
                            }}
                          />
                          <span>{def.name}</span>
                        </span>
                      </td>

                      {gameState.players.map((p) => {
                        const shares = p.shares[def.id] || 0;
                        const displayVal =
                          viewMode === 'totals'
                            ? shares
                            : `${((shares / totalSharesPerCompany) * 100).toFixed(0)}%`;

                        return (
                          <td
                            key={p.id}
                            style={{
                              padding: '8px 10px',
                              textAlign: 'center',
                              fontFamily: 'var(--font-mono)',
                              fontWeight: shares > 0 ? 700 : 400,
                              color: shares > 0 ? 'var(--text-amber)' : 'var(--text-muted)',
                            }}
                          >
                            {displayVal}
                          </td>
                        );
                      })}

                      <td
                        style={{
                          padding: '8px 10px',
                          textAlign: 'center',
                          fontFamily: 'var(--font-mono)',
                          color: 'var(--text-secondary)',
                        }}
                      >
                        {viewMode === 'totals'
                          ? bankShares
                          : `${((bankShares / totalSharesPerCompany) * 100).toFixed(0)}%`}
                      </td>
                    </tr>
                  );
                })}
              </tbody>
            </table>
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
