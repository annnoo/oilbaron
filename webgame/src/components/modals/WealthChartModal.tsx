import React from 'react';
import { BarChart3, X, DollarSign, TrendingUp, Crown } from 'lucide-react';
import { GameState } from '../../engine/types';
import { calculateNetWorth, calculateSharePrice } from '../../engine/market';
import { useTranslation } from '../../i18n';

export interface WealthChartModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

export const WealthChartModal: React.FC<WealthChartModalProps> = ({
  isOpen,
  onClose,
  gameState,
}) => {
  const { t } = useTranslation();
  if (!isOpen) return null;

  const playerWealth = gameState.players.map((p) => {
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
    const cash = p.cash;
    const netWorth = cash + stockValue;

    return {
      id: p.id,
      name: p.name,
      color: p.color,
      isAi: p.isAi,
      cash,
      stockValue,
      netWorth,
    };
  });

  const maxNetWorth = Math.max(...playerWealth.map((p) => p.netWorth), 1);
  const sortedPlayers = [...playerWealth].sort((a, b) => b.netWorth - a.netWorth);

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
        aria-labelledby="wealth-chart-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="wealth-chart-title">
            <BarChart3 size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>{t('wealth.title')}</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close wealth chart modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px' }}>
            <span style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
              {t('wealth.balanceDesc')}
            </span>
            <div style={{ display: 'flex', gap: '14px', fontSize: '11px' }}>
              <span style={{ display: 'inline-flex', alignItems: 'center', gap: '4px' }}>
                <span style={{ width: '8px', height: '8px', borderRadius: '2px', backgroundColor: '#22c55e' }} />
                <span>{t('wealth.liquidCash')}</span>
              </span>
              <span style={{ display: 'inline-flex', alignItems: 'center', gap: '4px' }}>
                <span style={{ width: '8px', height: '8px', borderRadius: '2px', backgroundColor: '#f59e0b' }} />
                <span>{t('wealth.stockPortfolio')}</span>
              </span>
            </div>
          </div>

          <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
            {sortedPlayers.map((p, idx) => {
              const isLeader = idx === 0;
              const totalPct = Math.round((p.netWorth / maxNetWorth) * 100);
              const cashPct = p.netWorth > 0 ? (p.cash / p.netWorth) * 100 : 0;
              const stockPct = p.netWorth > 0 ? (p.stockValue / p.netWorth) * 100 : 0;

              return (
                <div
                  key={p.id}
                  style={{
                    padding: '12px 14px',
                    background: 'var(--bg-surface-elevated)',
                    border: isLeader
                      ? '1px solid var(--accent-gold)'
                      : '1px solid var(--border-subtle)',
                    borderRadius: 'var(--radius-sm)',
                    display: 'flex',
                    flexDirection: 'column',
                    gap: '8px',
                  }}
                >
                  <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      {isLeader && <Crown size={14} color="var(--accent-gold)" />}
                      <span
                        style={{
                          width: '10px',
                          height: '10px',
                          borderRadius: '50%',
                          backgroundColor: p.color || '#f59e0b',
                          display: 'inline-block',
                        }}
                      />
                      <span style={{ fontWeight: 700, fontSize: '14px', color: 'var(--text-primary)' }}>
                        {p.name}
                      </span>
                      {p.isAi && (
                        <span style={{ fontSize: '10px', color: 'var(--text-muted)' }}>({t('common.cpu')})</span>
                      )}
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '14px' }}>
                      <span style={{ fontSize: '11px', color: 'var(--text-muted)' }}>
                        {t('common.cash')}: <strong style={{ color: '#22c55e' }}>${p.cash.toLocaleString()}</strong> | {t('scoreboard.stocks')}: <strong style={{ color: '#f59e0b' }}>${p.stockValue.toLocaleString()}</strong>
                      </span>
                      <span
                        style={{
                          fontFamily: 'var(--font-mono)',
                          fontSize: '15px',
                          fontWeight: 700,
                          color: isLeader ? 'var(--text-amber)' : 'var(--text-primary)',
                        }}
                      >
                        ${p.netWorth.toLocaleString()}
                      </span>
                    </div>
                  </div>

                  {/* Wealth Bar */}
                  <div
                    style={{
                      height: '12px',
                      background: 'rgba(255, 255, 255, 0.05)',
                      borderRadius: '6px',
                      overflow: 'hidden',
                      display: 'flex',
                      width: `${Math.max(totalPct, 12)}%`,
                      minWidth: '40px',
                      transition: 'width var(--transition-normal)',
                    }}
                  >
                    <div
                      style={{
                        width: `${cashPct}%`,
                        backgroundColor: '#22c55e',
                        height: '100%',
                      }}
                      title={`${t('common.cash')}: $${p.cash.toLocaleString()}`}
                    />
                    <div
                      style={{
                        width: `${stockPct}%`,
                        backgroundColor: '#f59e0b',
                        height: '100%',
                      }}
                      title={`${t('wealth.stockPortfolio')}: $${p.stockValue.toLocaleString()}`}
                    />
                  </div>
                </div>
              );
            })}
          </div>
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
