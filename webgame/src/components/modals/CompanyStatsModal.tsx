import React, { useState } from 'react';
import { Building2, X, Award, ShieldCheck, PieChart, Users } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { CompanyId, GameState } from '../../engine/types';
import { calculateSharePrice } from '../../engine/market';
import { useTranslation } from '../../i18n';

export interface CompanyStatsModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

export const CompanyStatsModal: React.FC<CompanyStatsModalProps> = ({
  isOpen,
  onClose,
  gameState,
}) => {
  const { t } = useTranslation();
  const [selectedCompanyId, setSelectedCompanyId] = useState<CompanyId | null>(null);

  if (!isOpen) return null;

  const companiesList = gameState.config.companies.filter((c) => !c.isDormant);
  const activeCompanyId = selectedCompanyId || companiesList[0]?.id || '';
  const selectedDef = gameState.config.companies.find((c) => c.id === activeCompanyId);
  const selectedState = selectedDef ? gameState.companies[selectedDef.id] : null;

  const currentPrice = selectedDef && selectedState ? calculateSharePrice(selectedDef, selectedState.size) : 0;
  const majorityBonus = currentPrice * (gameState.config.majorityBonusMultiplier || 10);
  const minorityBonus = currentPrice * (gameState.config.minorityBonusMultiplier || 5);

  // Compute shareholder standings for selected company
  const shareholders = gameState.players
    .map((p) => ({
      id: p.id,
      name: p.name,
      shares: p.shares[activeCompanyId] || 0,
      color: p.color,
    }))
    .filter((s) => s.shares > 0)
    .sort((a, b) => b.shares - a.shares);

  const totalIssuedShares = gameState.players.reduce(
    (sum, p) => sum + (p.shares[activeCompanyId] || 0),
    0
  );

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
        aria-labelledby="company-stats-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="company-stats-title">
            <Building2 size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>{t('stats.title')}</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close company stats modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Company Selector Tab Bar */}
          <div
            style={{
              display: 'flex',
              gap: '6px',
              overflowX: 'auto',
              paddingBottom: '10px',
              borderBottom: '1px solid var(--border-subtle)',
              marginBottom: '16px',
            }}
          >
            {companiesList.map((def) => {
              const compState = gameState.companies[def.id];
              const isActive = compState && compState.isActive;
              const isSelected = def.id === activeCompanyId;

              return (
                <button
                  key={def.id}
                  type="button"
                  onClick={() => {
                    playClick();
                    setSelectedCompanyId(def.id);
                  }}
                  style={{
                    display: 'flex',
                    alignItems: 'center',
                    gap: '6px',
                    padding: '6px 12px',
                    borderRadius: 'var(--radius-sm)',
                    border: isSelected
                      ? '1px solid var(--accent-gold)'
                      : '1px solid var(--border-default)',
                    background: isSelected
                      ? 'rgba(245, 158, 11, 0.12)'
                      : 'var(--bg-surface-elevated)',
                    color: isSelected ? 'var(--text-amber)' : 'var(--text-primary)',
                    cursor: 'pointer',
                    fontSize: '12px',
                    fontWeight: isSelected ? 700 : 500,
                    whiteSpace: 'nowrap',
                    transition: 'all var(--transition-fast)',
                  }}
                >
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
                  {compState?.isSafe && (
                    <span style={{ fontSize: '9px', color: '#22c55e', fontWeight: 700 }}>
                      [{t('common.safe')}]
                    </span>
                  )}
                  {!isActive && (
                    <span style={{ fontSize: '10px', color: 'var(--text-muted)' }}>{t('stats.inactive')}</span>
                  )}
                </button>
              );
            })}
          </div>

          {/* Selected Company Details Card */}
          {selectedDef && selectedState && (
            <div>
              <div
                style={{
                  display: 'grid',
                  gridTemplateColumns: 'repeat(auto-fit, minmax(130px, 1fr))',
                  gap: '10px',
                  marginBottom: '16px',
                }}
              >
                <div className="discarded-stat-card" style={{ borderLeft: `4px solid ${selectedDef.color}` }}>
                  <span className="discarded-stat-value">{selectedState.isActive ? t('common.active') : t('stats.unfounded')}</span>
                  <span className="discarded-stat-label">{t('stats.status')}</span>
                </div>

                <div className="discarded-stat-card">
                  <span className="discarded-stat-value">{selectedState.size}</span>
                  <span className="discarded-stat-label">{t('stats.gridSize')}</span>
                </div>

                <div className="discarded-stat-card">
                  <span className="discarded-stat-value" style={{ color: 'var(--text-amber)' }}>
                    ${currentPrice}
                  </span>
                  <span className="discarded-stat-label">{t('stats.stockPrice')}</span>
                </div>

                <div className="discarded-stat-card">
                  <span className="discarded-stat-value" style={{ color: '#38bdf8' }}>
                    {selectedState.availableShares}
                  </span>
                  <span className="discarded-stat-label">{t('stats.bankShares')}</span>
                </div>

                <div className="discarded-stat-card">
                  <span
                    className="discarded-stat-value"
                    style={{ color: selectedState.isSafe ? '#22c55e' : 'var(--text-muted)' }}
                  >
                    {selectedState.isSafe ? t('common.protected') : t('common.acquirable')}
                  </span>
                  <span className="discarded-stat-label">{t('stats.safeStatus')}</span>
                </div>
              </div>

              {/* Merger Bonuses Preview */}
              <div
                style={{
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'space-between',
                  padding: '10px 14px',
                  background: 'rgba(245, 158, 11, 0.07)',
                  border: '1px solid var(--border-accent)',
                  borderRadius: 'var(--radius-sm)',
                  marginBottom: '16px',
                }}
              >
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                  <Award size={16} color="var(--accent-gold)" />
                  <span style={{ fontSize: '12px', fontWeight: 600 }}>{t('stats.prospectiveBonuses')}:</span>
                </div>
                <div style={{ display: 'flex', gap: '16px', fontSize: '13px' }}>
                  <span>
                    <strong style={{ color: 'var(--text-amber)' }}>{t('stats.firstMajority', { amount: `$${majorityBonus.toLocaleString()}` })}</strong>
                  </span>
                  <span>
                    <strong style={{ color: '#38bdf8' }}>{t('stats.secondMinority', { amount: `$${minorityBonus.toLocaleString()}` })}</strong>
                  </span>
                </div>
              </div>

              {/* Shareholder Standings Table */}
              <div style={{ marginTop: '12px' }}>
                <div
                  style={{
                    fontSize: '12px',
                    fontWeight: 700,
                    textTransform: 'uppercase',
                    color: 'var(--text-secondary)',
                    marginBottom: '8px',
                    display: 'flex',
                    alignItems: 'center',
                    gap: '6px',
                  }}
                >
                  <Users size={14} />
                  <span>{t('stats.shareholderDist')} ({totalIssuedShares} {t('common.shares')})</span>
                </div>

                {shareholders.length === 0 ? (
                  <div style={{ padding: '24px', textAlign: 'center', color: 'var(--text-muted)' }}>
                    {t('stats.noSharesOwned', { name: selectedDef.name })}
                  </div>
                ) : (
                  <div
                    style={{
                      border: '1px solid var(--border-default)',
                      borderRadius: 'var(--radius-sm)',
                      overflow: 'hidden',
                    }}
                  >
                    <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
                      <thead>
                        <tr style={{ background: 'var(--bg-surface-elevated)', borderBottom: '1px solid var(--border-subtle)', textAlign: 'left' }}>
                          <th style={{ padding: '8px 12px' }}>{t('stats.rank')}</th>
                          <th style={{ padding: '8px 12px' }}>{t('stats.shareholder')}</th>
                          <th style={{ padding: '8px 12px', textAlign: 'right' }}>{t('common.shares')}</th>
                          <th style={{ padding: '8px 12px', textAlign: 'right' }}>{t('stats.ownership')}</th>
                          <th style={{ padding: '8px 12px', textAlign: 'right' }}>{t('stats.bonusRole')}</th>
                        </tr>
                      </thead>
                      <tbody>
                        {shareholders.map((holder, idx) => {
                          const pct = totalIssuedShares > 0 ? ((holder.shares / totalIssuedShares) * 100).toFixed(1) : '0';
                          const isMajority = idx === 0;
                          const isMinority = idx === 1;

                          return (
                            <tr
                              key={holder.id}
                              style={{
                                borderBottom: '1px solid var(--border-subtle)',
                                background: isMajority ? 'rgba(245, 158, 11, 0.05)' : 'transparent',
                              }}
                            >
                              <td style={{ padding: '8px 12px', fontFamily: 'var(--font-mono)' }}>
                                #{idx + 1}
                              </td>
                              <td style={{ padding: '8px 12px', fontWeight: 600 }}>
                                <span style={{ display: 'inline-flex', alignItems: 'center', gap: '6px' }}>
                                  {holder.color && (
                                    <span
                                      style={{
                                        width: '8px',
                                        height: '8px',
                                        borderRadius: '50%',
                                        backgroundColor: holder.color,
                                        display: 'inline-block',
                                      }}
                                    />
                                  )}
                                  <span>{holder.name}</span>
                                </span>
                              </td>
                              <td style={{ padding: '8px 12px', textAlign: 'right', fontFamily: 'var(--font-mono)', fontWeight: 700 }}>
                                {holder.shares}
                              </td>
                              <td style={{ padding: '8px 12px', textAlign: 'right', fontFamily: 'var(--font-mono)', color: 'var(--text-muted)' }}>
                                {pct}%
                              </td>
                              <td style={{ padding: '8px 12px', textAlign: 'right' }}>
                                {isMajority ? (
                                  <span style={{ fontSize: '11px', fontWeight: 700, color: 'var(--text-amber)' }}>
                                    {t('stats.majorityHolder')} (${majorityBonus.toLocaleString()})
                                  </span>
                                ) : isMinority ? (
                                  <span style={{ fontSize: '11px', fontWeight: 700, color: '#38bdf8' }}>
                                    {t('stats.minorityHolder')} (${minorityBonus.toLocaleString()})
                                  </span>
                                ) : (
                                  <span style={{ fontSize: '11px', color: 'var(--text-muted)' }}>{t('common.none')}</span>
                                )}
                              </td>
                            </tr>
                          );
                        })}
                      </tbody>
                    </table>
                  </div>
                )}
              </div>
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
