import React, { useState } from 'react';
import { Building2, Award, Check } from 'lucide-react';
import { playClick, playFoundCompany } from '../../assets/audio';
import { CompanyDefinition, CompanyId, GameState } from '../../engine/types';
import { useTranslation } from '../../i18n';

export interface FoundingCompanyModalProps {
  isOpen: boolean;
  gameState: GameState;
  availableCompanyIds: readonly CompanyId[];
  onSelectCompany: (companyId: CompanyId) => void;
}

export const FoundingCompanyModal: React.FC<FoundingCompanyModalProps> = ({
  isOpen,
  gameState,
  availableCompanyIds,
  onSelectCompany,
}) => {
  const { t } = useTranslation();
  const [selectedId, setSelectedId] = useState<CompanyId | null>(null);

  if (!isOpen) return null;

  const activePlayer = gameState.players[gameState.activePlayerIndex];
  const founderBonusShares = gameState.config.founderBonusShares;

  const availableDefs = gameState.config.companies.filter(
    (c) => !c.isDormant && availableCompanyIds.includes(c.id)
  );

  const handleConfirm = () => {
    if (!selectedId) return;
    playFoundCompany();
    onSelectCompany(selectedId);
  };

  return (
    <div className="modal-backdrop" role="presentation">
      <div
        className="modal-dialog modal-md"
        role="dialog"
        aria-modal="true"
        aria-labelledby="founding-modal-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="founding-modal-title">
            <Building2 size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>{t('founding.title')}</span>
          </div>
        </div>

        <div className="modal-body">
          <div className="founding-banner">
            <div>
              <div style={{ fontSize: '13px', fontWeight: 700, color: 'var(--text-primary)' }}>
                {t('founding.playerNotice', { name: activePlayer ? activePlayer.name : t('common.player') })}
              </div>
              <div style={{ fontSize: '11px', color: 'var(--text-secondary)', marginTop: '2px' }}>
                {t('founding.chooseCorpSubtitle')}
              </div>
            </div>
            <div className="founder-bonus-badge" title="Founder bonus awarded to the active player">
              <Award size={16} color="var(--accent-gold)" />
              <span>{t('founding.bonusShares', { count: founderBonusShares })}</span>
            </div>
          </div>

          <div className="founding-company-grid">
            {availableDefs.map((comp: CompanyDefinition) => {
              const isSelected = selectedId === comp.id;
              const tierBadgeColor =
                comp.tier === 'low'
                  ? '#38bdf8'
                  : comp.tier === 'mid'
                  ? '#eab308'
                  : '#a855f7';

              return (
                <div
                  key={comp.id}
                  role="button"
                  tabIndex={0}
                  className={`founding-company-card ${isSelected ? 'is-selected' : ''}`}
                  onClick={() => {
                    playClick();
                    setSelectedId(comp.id);
                  }}
                  onKeyDown={(e) => {
                    if (e.key === 'Enter' || e.key === ' ') {
                      e.preventDefault();
                      playClick();
                      setSelectedId(comp.id);
                    }
                  }}
                  style={{
                    borderLeftWidth: '5px',
                    borderLeftColor: comp.color,
                  }}
                >
                  <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <span
                        style={{
                          width: '14px',
                          height: '14px',
                          borderRadius: '3px',
                          backgroundColor: comp.color,
                          display: 'inline-block',
                        }}
                      />
                      <span style={{ fontWeight: 700, fontSize: '14px', color: 'var(--text-primary)' }}>
                        {comp.name}
                      </span>
                    </div>
                    {isSelected && <Check size={18} color="var(--accent-gold)" />}
                  </div>

                  <div style={{ display: 'flex', alignItems: 'center', gap: '6px', marginTop: '4px' }}>
                    <span
                      style={{
                        fontSize: '10px',
                        fontWeight: 700,
                        textTransform: 'uppercase',
                        padding: '2px 6px',
                        borderRadius: '3px',
                        background: 'rgba(255, 255, 255, 0.08)',
                        color: tierBadgeColor,
                      }}
                    >
                      {t('founding.tier', { tier: comp.tier })}
                    </span>
                    <span style={{ fontSize: '11px', color: 'var(--text-muted)' }}>
                      {t('founding.basePrice', { price: comp.basePrice })}
                    </span>
                  </div>
                </div>
              );
            })}
          </div>

          {availableDefs.length === 0 && (
            <div style={{ padding: '20px', textAlign: 'center', color: 'var(--text-muted)' }}>
              {t('founding.noneAvailable')}
            </div>
          )}
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end', gap: '8px' }}>
          <button
            type="button"
            className="btn btn-primary"
            disabled={!selectedId}
            onClick={handleConfirm}
          >
            {t('founding.incorporateBtn')}
          </button>
        </div>
      </div>
    </div>
  );
};
