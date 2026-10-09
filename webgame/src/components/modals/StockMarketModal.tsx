import React, { useState, useEffect, useMemo } from 'react';
import { TrendingUp, Plus, Minus, DollarSign, AlertCircle, ShoppingCart, X } from 'lucide-react';
import { playClick, playCashChime } from '../../assets/audio';
import { CompanyId, GameState, SharePurchase } from '../../engine/types';
import { calculateSharePrice, validateSharePurchases } from '../../engine/market';
import { useTranslation } from '../../i18n';

export interface StockMarketModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  onConfirmPurchase: (purchases: SharePurchase[]) => void;
}

export const StockMarketModal: React.FC<StockMarketModalProps> = ({
  isOpen,
  onClose,
  gameState,
  onConfirmPurchase,
}) => {
  const { t } = useTranslation();
  const [counts, setCounts] = useState<Record<CompanyId, number>>({});

  useEffect(() => {
    if (isOpen) {
      setCounts({});
    }
  }, [isOpen]);

  if (!isOpen) return null;

  const activePlayer = gameState.players[gameState.activePlayerIndex];
  const maxShares = gameState.config.maxSharesPerPurchase; // default 3

  // Active companies that have been founded on board (size >= 2)
  const activeCompanies = gameState.config.companies.filter((def) => {
    const comp = gameState.companies[def.id];
    return comp && comp.isActive && comp.size >= 2;
  });

  const totalSelectedCount = Object.values(counts).reduce((acc, c) => acc + c, 0);

  // Convert counts to purchases array
  const purchases: SharePurchase[] = Object.entries(counts)
    .filter(([_, count]) => count > 0)
    .map(([companyId, count]) => ({ companyId, count }));

  const validation = activePlayer
    ? validateSharePurchases(
        activePlayer,
        gameState.companies,
        gameState.config.companies,
        purchases,
        maxShares
      )
    : { valid: false, totalCost: 0, reason: 'No active player' };

  const handleIncrement = (companyId: CompanyId) => {
    const current = counts[companyId] || 0;
    const company = gameState.companies[companyId];
    const def = gameState.config.companies.find((c) => c.id === companyId);
    if (!company || !def || !activePlayer) return;

    if (totalSelectedCount >= maxShares) return;
    if (current >= company.availableShares) return;

    const price = calculateSharePrice(def, company.size);
    if (validation.totalCost + price > activePlayer.cash) return;

    playClick();
    setCounts((prev) => ({
      ...prev,
      [companyId]: (prev[companyId] || 0) + 1,
    }));
  };

  const handleDecrement = (companyId: CompanyId) => {
    const current = counts[companyId] || 0;
    if (current <= 0) return;
    playClick();
    setCounts((prev) => {
      const next = { ...prev, [companyId]: current - 1 };
      if (next[companyId] === 0) delete next[companyId];
      return next;
    });
  };

  const handleConfirm = () => {
    if (!validation.valid || purchases.length === 0) return;
    playCashChime();
    onConfirmPurchase(purchases);
  };

  const handlePass = () => {
    playClick();
    onConfirmPurchase([]);
  };

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
        aria-labelledby="stock-market-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="stock-market-title">
            <TrendingUp size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>{t('market.title')}</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close stock market modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Player Summary Header */}
          <div
            style={{
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              padding: '10px 14px',
              background: 'var(--bg-surface-elevated)',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-sm)',
              marginBottom: '14px',
            }}
          >
            <div>
              <div style={{ fontSize: '12px', color: 'var(--text-muted)' }}>{t('market.buyerPortfolio')}</div>
              <div style={{ fontSize: '14px', fontWeight: 700, color: 'var(--text-primary)' }}>
                {activePlayer ? activePlayer.name : 'Unknown Player'}
              </div>
            </div>
            <div style={{ textAlign: 'right' }}>
              <div style={{ fontSize: '11px', color: 'var(--text-muted)' }}>{t('market.availableCash')}</div>
              <div
                style={{
                  fontFamily: 'var(--font-mono)',
                  fontSize: '16px',
                  fontWeight: 700,
                  color: 'var(--text-green)',
                }}
              >
                ${activePlayer ? activePlayer.cash.toLocaleString() : 0}
              </div>
            </div>
          </div>

          {/* Quota Banner */}
          <div
            style={{
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              padding: '8px 12px',
              background: 'rgba(245, 158, 11, 0.08)',
              border: '1px solid var(--border-accent)',
              borderRadius: 'var(--radius-sm)',
              marginBottom: '14px',
            }}
          >
            <span style={{ fontSize: '12px', color: 'var(--text-primary)' }}>
              {t('market.purchaseQuota')} <strong>{totalSelectedCount} / {maxShares}</strong> {t('common.shares')} {t('market.quotaMax', { count: totalSelectedCount }).replace(/^[0-9]+\s*(of\s*3\s*shares|von\s*3\s*Aktien)\s*/i, '')}
            </span>
            <span
              style={{
                fontFamily: 'var(--font-mono)',
                fontSize: '11px',
                fontWeight: 600,
                color: 'var(--text-amber)',
              }}
            >
              {t('market.remainingCount', { count: maxShares - totalSelectedCount })}
            </span>
          </div>

          {/* Active Companies List */}
          {activeCompanies.length === 0 ? (
            <div style={{ padding: '28px', textAlign: 'center', color: 'var(--text-muted)' }}>
              {t('market.noActive')}
            </div>
          ) : (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '8px', maxHeight: '340px', overflowY: 'auto' }}>
              {activeCompanies.map((def) => {
                const comp = gameState.companies[def.id]!;
                const count = counts[def.id] || 0;
                const price = calculateSharePrice(def, comp.size);
                const playerShares = activePlayer ? activePlayer.shares[def.id] || 0 : 0;
                const canAdd =
                  totalSelectedCount < maxShares &&
                  count < comp.availableShares &&
                  (activePlayer?.cash ?? 0) >= validation.totalCost + price;

                return (
                  <div
                    key={def.id}
                    style={{
                      display: 'flex',
                      alignItems: 'center',
                      justifyContent: 'space-between',
                      padding: '10px 12px',
                      background: 'var(--bg-surface-elevated)',
                      border: '1px solid var(--border-default)',
                      borderLeft: `5px solid ${def.color}`,
                      borderRadius: 'var(--radius-sm)',
                    }}
                  >
                    <div>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                        <span style={{ fontWeight: 700, fontSize: '13px', color: 'var(--text-primary)' }}>
                          {def.name}
                        </span>
                        {comp.isSafe && (
                          <span
                            style={{
                              fontSize: '10px',
                              fontWeight: 700,
                              color: '#22c55e',
                              background: 'rgba(34, 197, 94, 0.15)',
                              padding: '1px 5px',
                              borderRadius: '3px',
                            }}
                          >
                            {t('common.safe')}
                          </span>
                        )}
                      </div>
                      <div style={{ display: 'flex', gap: '10px', fontSize: '11px', color: 'var(--text-muted)', marginTop: '3px' }}>
                        <span>{t('common.size')}: <strong>{comp.size}</strong></span>
                        <span>{t('common.bank')}: <strong>{comp.availableShares}</strong> {t('market.left')}</span>
                        <span>{t('market.youOwn', { count: playerShares })}</span>
                      </div>
                    </div>

                    <div style={{ display: 'flex', alignItems: 'center', gap: '14px' }}>
                      <div style={{ textAlign: 'right' }}>
                        <div style={{ fontFamily: 'var(--font-mono)', fontSize: '14px', fontWeight: 700, color: 'var(--text-amber)' }}>
                          ${price}
                        </div>
                        <div style={{ fontSize: '10px', color: 'var(--text-muted)' }}>per share</div>
                      </div>

                      {/* Increment / Decrement Counter */}
                      <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                        <button
                          type="button"
                          className="btn btn-secondary btn-sm"
                          style={{ padding: '4px 8px', minWidth: '28px' }}
                          disabled={count <= 0}
                          onClick={() => handleDecrement(def.id)}
                          aria-label={`Decrease ${def.name} shares`}
                        >
                          <Minus size={13} />
                        </button>
                        <span
                          style={{
                            fontFamily: 'var(--font-mono)',
                            fontSize: '14px',
                            fontWeight: 700,
                            minWidth: '22px',
                            textAlign: 'center',
                            color: count > 0 ? 'var(--accent-gold)' : 'var(--text-primary)',
                          }}
                        >
                          {count}
                        </span>
                        <button
                          type="button"
                          className="btn btn-secondary btn-sm"
                          style={{ padding: '4px 8px', minWidth: '28px' }}
                          disabled={!canAdd}
                          onClick={() => handleIncrement(def.id)}
                          aria-label={`Increase ${def.name} shares`}
                        >
                          <Plus size={13} />
                        </button>
                      </div>
                    </div>
                  </div>
                );
              })}
            </div>
          )}

          {/* Total Cost Breakdown */}
          {purchases.length > 0 && (
            <div
              style={{
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'space-between',
                padding: '10px 14px',
                background: 'var(--bg-surface-hover)',
                border: '1px solid var(--border-accent)',
                borderRadius: 'var(--radius-sm)',
                marginTop: '14px',
              }}
            >
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                <ShoppingCart size={16} color="var(--accent-gold)" />
                <span style={{ fontSize: '13px', fontWeight: 600 }}>{t('market.totalPurchaseCost')}</span>
              </div>
              <div
                style={{
                  fontFamily: 'var(--font-mono)',
                  fontSize: '18px',
                  fontWeight: 700,
                  color: validation.valid ? 'var(--text-amber)' : 'var(--text-red)',
                }}
              >
                ${validation.totalCost.toLocaleString()}
              </div>
            </div>
          )}

          {!validation.valid && purchases.length > 0 && (
            <div style={{ display: 'flex', alignItems: 'center', gap: '6px', marginTop: '8px', color: '#f87171', fontSize: '12px' }}>
              <AlertCircle size={14} />
              <span>{validation.reason}</span>
            </div>
          )}
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'space-between' }}>
          <button
            type="button"
            className="btn btn-secondary"
            onClick={handlePass}
          >
            {t('market.skipPass')}
          </button>
          <div style={{ display: 'flex', gap: '8px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={onClose}
            >
              {t('common.cancel')}
            </button>
            <button
              type="button"
              className="btn btn-primary"
              disabled={purchases.length === 0 || !validation.valid}
              onClick={handleConfirm}
            >
              {t('market.confirmPurchaseCost', { cost: validation.totalCost })}
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};
