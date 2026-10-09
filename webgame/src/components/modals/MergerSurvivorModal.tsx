import React, { useState } from 'react';
import { GitMerge, Check, AlertCircle } from 'lucide-react';
import { playClick, playMergerAlert } from '../../assets/audio';
import { CompanyId, GameState } from '../../engine/types';
import { calculateSharePrice } from '../../engine/market';

export interface MergerSurvivorModalProps {
  isOpen: boolean;
  gameState: GameState;
  tiedCompanyIds: readonly CompanyId[];
  onChooseSurvivor: (survivorCompanyId: CompanyId) => void;
}

export const MergerSurvivorModal: React.FC<MergerSurvivorModalProps> = ({
  isOpen,
  gameState,
  tiedCompanyIds,
  onChooseSurvivor,
}) => {
  const [selectedId, setSelectedId] = useState<CompanyId | null>(null);

  if (!isOpen) return null;

  const activePlayer = gameState.players[gameState.activePlayerIndex];
  const tiedDefs = gameState.config.companies.filter((c) => tiedCompanyIds.includes(c.id));

  const handleSelect = (id: CompanyId) => {
    playClick();
    setSelectedId(id);
  };

  const handleConfirm = () => {
    if (!selectedId) return;
    playMergerAlert();
    onChooseSurvivor(selectedId);
  };

  return (
    <div className="modal-backdrop" role="presentation">
      <div
        className="modal-dialog modal-md"
        role="dialog"
        aria-modal="true"
        aria-labelledby="merger-survivor-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="merger-survivor-title">
            <GitMerge size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Merger Conflict — Tie-Break Decision</span>
          </div>
        </div>

        <div className="modal-body">
          <div className="founding-banner" style={{ borderColor: 'rgba(239, 68, 68, 0.4)' }}>
            <div>
              <div style={{ fontSize: '13px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Multiple Corporations Tie for Largest Size!
              </div>
              <div style={{ fontSize: '11px', color: 'var(--text-secondary)', marginTop: '2px' }}>
                {activePlayer ? activePlayer.name : 'The active player'} must choose which corporation survives and absorbs the rival companies.
              </div>
            </div>
          </div>

          <div style={{ margin: '14px 0 8px 0', fontSize: '12px', fontWeight: 600, color: 'var(--text-secondary)' }}>
            Select the Surviving Corporation:
          </div>

          <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
            {tiedDefs.map((def) => {
              const compState = gameState.companies[def.id]!;
              const isSelected = selectedId === def.id;
              const price = calculateSharePrice(def, compState.size);

              return (
                <div
                  key={def.id}
                  role="button"
                  tabIndex={0}
                  className={`founding-company-card ${isSelected ? 'is-selected' : ''}`}
                  style={{
                    borderLeftWidth: '5px',
                    borderLeftColor: def.color,
                  }}
                  onClick={() => handleSelect(def.id)}
                  onKeyDown={(e) => {
                    if (e.key === 'Enter' || e.key === ' ') {
                      e.preventDefault();
                      handleSelect(def.id);
                    }
                  }}
                >
                  <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <span
                        style={{
                          width: '12px',
                          height: '12px',
                          borderRadius: '2px',
                          backgroundColor: def.color,
                          display: 'inline-block',
                        }}
                      />
                      <span style={{ fontWeight: 700, fontSize: '14px', color: 'var(--text-primary)' }}>
                        {def.name}
                      </span>
                    </div>
                    {isSelected && <Check size={18} color="var(--accent-gold)" />}
                  </div>

                  <div style={{ display: 'flex', gap: '14px', fontSize: '12px', color: 'var(--text-muted)', marginTop: '4px' }}>
                    <span>Size: <strong>{compState.size} tiles</strong></span>
                    <span>Share Price: <strong style={{ color: 'var(--text-amber)' }}>${price}</strong></span>
                    <span>Bank Shares: <strong>{compState.availableShares}</strong></span>
                  </div>
                </div>
              );
            })}
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end', gap: '8px' }}>
          <button
            type="button"
            className="btn btn-primary"
            disabled={!selectedId}
            onClick={handleConfirm}
          >
            Confirm Survivor
          </button>
        </div>
      </div>
    </div>
  );
};
