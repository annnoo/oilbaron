import React from 'react';
import { Award, DollarSign, X, CheckCircle, TrendingUp } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { CompanyId, GameState } from '../../engine/types';
import { calculateMergerBonuses } from '../../engine/merger';
import { useTranslation } from '../../i18n';

export interface MergerStatsModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  defunctCompanyId: CompanyId;
  survivorCompanyId: CompanyId;
  preMergerPrice: number;
}

export const MergerStatsModal: React.FC<MergerStatsModalProps> = ({
  isOpen,
  onClose,
  gameState,
  defunctCompanyId,
  survivorCompanyId,
  preMergerPrice,
}) => {
  const { t } = useTranslation();
  if (!isOpen) return null;

  const defunctDef = gameState.config.companies.find((c) => c.id === defunctCompanyId);
  const survivorDef = gameState.config.companies.find((c) => c.id === survivorCompanyId);
  const defunctState = gameState.companies[defunctCompanyId];

  const bonuses = calculateMergerBonuses(defunctCompanyId, preMergerPrice, gameState.players);
  const majorityPool = 10 * preMergerPrice;
  const minorityPool = 5 * preMergerPrice;

  const shareholders = gameState.players
    .map((p) => ({
      id: p.id,
      name: p.name,
      shares: p.shares[defunctCompanyId] || 0,
      bonus: bonuses[p.id] || 0,
      color: p.color,
    }))
    .filter((s) => s.shares > 0)
    .sort((a, b) => b.shares - a.shares);

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
        aria-labelledby="merger-stats-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="merger-stats-title">
            <Award size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>{t('mergerStats.title')}</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close merger payouts dialog"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Header Summary */}
          <div
            style={{
              padding: '12px 14px',
              background: 'var(--bg-surface-elevated)',
              border: '1px solid var(--border-default)',
              borderRadius: 'var(--radius-sm)',
              marginBottom: '14px',
            }}
          >
            <div style={{ fontSize: '13px', fontWeight: 700, color: 'var(--text-primary)' }}>
              <span style={{ color: defunctDef?.color || '#f87171' }}>{defunctDef?.name || defunctCompanyId}</span>
              {' '}{t('mergerStats.acquiredNotice', { defunct: '', survivor: '' }).replace('{defunct}', '').replace('{survivor}', '').trim()}{' '}
              <span style={{ color: survivorDef?.color || '#38bdf8' }}>{survivorDef?.name || survivorCompanyId}</span>!
            </div>
            <div style={{ display: 'flex', gap: '16px', fontSize: '11px', color: 'var(--text-secondary)', marginTop: '4px' }}>
              <span>{t('mergerStats.preMergerPrice', { price: `$${preMergerPrice}` })}</span>
              <span>{t('common.size')}: <strong>{defunctState?.size ?? 0} {t('common.tiles')}</strong></span>
            </div>
          </div>

          {/* Bonus Pool Metrics */}
          <div
            style={{
              display: 'grid',
              gridTemplateColumns: '1fr 1fr',
              gap: '10px',
              marginBottom: '14px',
            }}
          >
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: 'var(--text-amber)' }}>
                ${majorityPool.toLocaleString()}
              </span>
              <span className="discarded-stat-label">{t('mergerStats.majorityBonusPool')}</span>
            </div>
            <div className="discarded-stat-card">
              <span className="discarded-stat-value" style={{ color: '#38bdf8' }}>
                ${minorityPool.toLocaleString()}
              </span>
              <span className="discarded-stat-label">{t('mergerStats.minorityBonusPool')}</span>
            </div>
          </div>

          {/* Shareholder Payouts Table */}
          <div style={{ border: '1px solid var(--border-default)', borderRadius: 'var(--radius-sm)', overflow: 'hidden' }}>
            <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '12px' }}>
              <thead>
                <tr style={{ background: 'var(--bg-surface-elevated)', borderBottom: '1px solid var(--border-subtle)', textAlign: 'left' }}>
                  <th style={{ padding: '8px 10px' }}>{t('stats.shareholder')}</th>
                  <th style={{ padding: '8px 10px', textAlign: 'right' }}>{t('common.shares')}</th>
                  <th style={{ padding: '8px 10px', textAlign: 'right' }}>{t('mergerStats.cashPayout')}</th>
                </tr>
              </thead>
              <tbody>
                {shareholders.length === 0 ? (
                  <tr>
                    <td colSpan={3} style={{ padding: '16px', textAlign: 'center', color: 'var(--text-muted)' }}>
                      {t('mergerStats.noSharesHeld', { name: defunctDef?.name || defunctCompanyId })}
                    </td>
                  </tr>
                ) : (
                  shareholders.map((s) => (
                    <tr key={s.id} style={{ borderBottom: '1px solid var(--border-subtle)' }}>
                      <td style={{ padding: '8px 10px', fontWeight: 600 }}>
                        <span style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}>
                          {s.color && (
                            <span
                              style={{
                                width: '8px',
                                height: '8px',
                                borderRadius: '50%',
                                backgroundColor: s.color,
                                display: 'inline-block',
                              }}
                            />
                          )}
                          <span>{s.name}</span>
                        </span>
                      </td>
                      <td style={{ padding: '8px 10px', textAlign: 'right', fontFamily: 'var(--font-mono)' }}>
                        {s.shares}
                      </td>
                      <td
                        style={{
                          padding: '8px 10px',
                          textAlign: 'right',
                          fontFamily: 'var(--font-mono)',
                          fontWeight: 700,
                          color: s.bonus > 0 ? 'var(--text-green)' : 'var(--text-muted)',
                        }}
                      >
                        {s.bonus > 0 ? `+$${s.bonus.toLocaleString()}` : '$0'}
                      </td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end' }}>
          <button
            type="button"
            className="btn btn-primary"
            onClick={() => {
              playClick();
              onClose();
            }}
          >
            {t('mergerStats.proceed')}
          </button>
        </div>
      </div>
    </div>
  );
};
