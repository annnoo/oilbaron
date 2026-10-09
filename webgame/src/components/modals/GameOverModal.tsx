import React, { useEffect } from 'react';
import { Trophy, Crown, RotateCcw, X, DollarSign, Award, Users } from 'lucide-react';
import { playVictoryFanfare, playClick } from '../../assets/audio';
import { GameState } from '../../engine/types';
import { calculateSharePrice } from '../../engine/market';

export interface GameOverModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  onNewGame: () => void;
}

export const GameOverModal: React.FC<GameOverModalProps> = ({
  isOpen,
  onClose,
  gameState,
  onNewGame,
}) => {
  useEffect(() => {
    if (isOpen) {
      playVictoryFanfare();
    }
  }, [isOpen]);

  if (!isOpen) return null;

  // Compute final liquidated net worth
  const rankedPlayers = gameState.players
    .map((p) => {
      let stockValue = 0;
      for (const [companyId, count] of Object.entries(p.shares)) {
        if (count > 0) {
          const comp = gameState.companies[companyId];
          const def = gameState.config.companies.find((c) => c.id === companyId);
          if (comp && comp.isActive && comp.size >= 2 && def) {
            stockValue += count * calculateSharePrice(def, comp.size);
          }
        }
      }
      return {
        id: p.id,
        name: p.name,
        color: p.color,
        isAi: p.isAi,
        cash: p.cash,
        stockValue,
        totalNetWorth: p.cash + stockValue,
      };
    })
    .sort((a, b) => b.totalNetWorth - a.totalNetWorth);

  const winner = rankedPlayers[0];

  return (
    <div className="modal-backdrop" role="presentation">
      <div
        className="modal-dialog modal-lg"
        role="dialog"
        aria-modal="true"
        aria-labelledby="gameover-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="gameover-title">
            <Trophy size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Match Concluded — The Oil Baron Winner!</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close game over modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Winner Celebration Banner */}
          {winner && (
            <div
              style={{
                textAlign: 'center',
                padding: '20px 16px',
                background: 'linear-gradient(135deg, rgba(245, 158, 11, 0.2), rgba(217, 119, 6, 0.08))',
                border: '2px solid var(--accent-gold)',
                borderRadius: 'var(--radius-sm)',
                marginBottom: '18px',
              }}
            >
              <Crown size={36} color="var(--accent-gold)" style={{ margin: '0 auto 8px auto' }} />
              <div style={{ fontSize: '13px', textTransform: 'uppercase', letterSpacing: '0.08em', color: 'var(--text-amber)', fontWeight: 700 }}>
                Grand Petroleum Baron
              </div>
              <div style={{ fontSize: '24px', fontWeight: 800, color: 'var(--text-primary)', margin: '4px 0' }}>
                {winner.name} {winner.isAi && '(CPU)'}
              </div>
              <div style={{ fontFamily: 'var(--font-mono)', fontSize: '20px', fontWeight: 700, color: 'var(--text-green)' }}>
                ${winner.totalNetWorth.toLocaleString()} Final Net Worth
              </div>
            </div>
          )}

          {/* Final Standings Leaderboard */}
          <div>
            <div style={{ fontSize: '12px', fontWeight: 700, textTransform: 'uppercase', color: 'var(--text-secondary)', marginBottom: '8px' }}>
              Final Portfolio & Liquidated Wealth Leaderboard
            </div>

            <div style={{ border: '1px solid var(--border-default)', borderRadius: 'var(--radius-sm)', overflow: 'hidden' }}>
              <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
                <thead>
                  <tr style={{ background: 'var(--bg-surface-elevated)', borderBottom: '1px solid var(--border-subtle)', textAlign: 'left' }}>
                    <th style={{ padding: '8px 12px' }}>Rank</th>
                    <th style={{ padding: '8px 12px' }}>Oil Baron</th>
                    <th style={{ padding: '8px 12px', textAlign: 'right' }}>Liquid Cash</th>
                    <th style={{ padding: '8px 12px', textAlign: 'right' }}>Stock Assets</th>
                    <th style={{ padding: '8px 12px', textAlign: 'right' }}>Total Net Worth</th>
                  </tr>
                </thead>
                <tbody>
                  {rankedPlayers.map((player, idx) => (
                    <tr
                      key={player.id}
                      style={{
                        borderBottom: '1px solid var(--border-subtle)',
                        background: idx === 0 ? 'rgba(245, 158, 11, 0.06)' : 'transparent',
                      }}
                    >
                      <td style={{ padding: '8px 12px', fontFamily: 'var(--font-mono)', fontWeight: 700 }}>
                        {idx === 0 ? '👑 #1' : `#${idx + 1}`}
                      </td>
                      <td style={{ padding: '8px 12px', fontWeight: 600 }}>
                        <span style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}>
                          {player.color && (
                            <span
                              style={{
                                width: '8px',
                                height: '8px',
                                borderRadius: '50%',
                                backgroundColor: player.color,
                                display: 'inline-block',
                              }}
                            />
                          )}
                          <span>{player.name}</span>
                          {player.isAi && <span style={{ fontSize: '10px', color: 'var(--text-muted)' }}>(CPU)</span>}
                        </span>
                      </td>
                      <td style={{ padding: '8px 12px', textAlign: 'right', fontFamily: 'var(--font-mono)', color: '#22c55e' }}>
                        ${player.cash.toLocaleString()}
                      </td>
                      <td style={{ padding: '8px 12px', textAlign: 'right', fontFamily: 'var(--font-mono)', color: '#f59e0b' }}>
                        ${player.stockValue.toLocaleString()}
                      </td>
                      <td
                        style={{
                          padding: '8px 12px',
                          textAlign: 'right',
                          fontFamily: 'var(--font-mono)',
                          fontWeight: 700,
                          fontSize: '14px',
                          color: idx === 0 ? 'var(--text-amber)' : 'var(--text-primary)',
                        }}
                      >
                        ${player.totalNetWorth.toLocaleString()}
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'space-between' }}>
          <button type="button" className="btn btn-secondary" onClick={onClose}>
            Inspect Board State
          </button>
          <button
            type="button"
            className="btn btn-primary"
            onClick={() => {
              playClick();
              onNewGame();
            }}
          >
            <RotateCcw size={14} style={{ marginRight: '6px' }} />
            Start New Game
          </button>
        </div>
      </div>
    </div>
  );
};
