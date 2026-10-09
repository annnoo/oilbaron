import React, { useState, useEffect } from 'react';
import { TrendingUp, Plus, Minus, AlertCircle, ShoppingCart, DollarSign, ArrowDownRight, ArrowUpRight, X } from 'lucide-react';
import { playClick, playCashChime } from '../../assets/audio';
import { CompanyId, GameState, SharePurchase, ShareSale } from '../../engine/types';
import { calculateSharePrice, validateSharePurchases, validateShareSales } from '../../engine/market';

export interface StockMarketModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  onConfirmPurchase: (purchases: SharePurchase[]) => void;
  onConfirmSale?: (sales: ShareSale[]) => void;
  onSellAll?: () => void;
  initialMode?: 'buy' | 'sell';
  initialCompanyId?: CompanyId | null;
}

export const StockMarketModal: React.FC<StockMarketModalProps> = ({
  isOpen,
  onClose,
  gameState,
  onConfirmPurchase,
  onConfirmSale,
  onSellAll,
  initialMode = 'buy',
  initialCompanyId,
}) => {
  const [mode, setMode] = useState<'buy' | 'sell'>(initialMode);
  const [buyCounts, setBuyCounts] = useState<Record<CompanyId, number>>({});
  const [sellCounts, setSellCounts] = useState<Record<CompanyId, number>>({});

  useEffect(() => {
    if (isOpen) {
      setBuyCounts({});
      setSellCounts({});
      setMode(initialMode);
    }
  }, [isOpen, initialMode]);

  if (!isOpen) return null;

  const activePlayer = gameState.players[gameState.activePlayerIndex];
  const maxBaseShares = gameState.config.maxSharesPerPurchase; // default 3

  // Repurchase allowance: shares sold this turn increase the purchase limit
  const totalSoldThisTurn = Object.values(gameState.turnSoldShares ?? {}).reduce(
    (sum, c) => sum + c,
    0
  );
  const effectiveMaxShares = maxBaseShares + totalSoldThisTurn;

  // Active companies that have been founded on board (size >= 2)
  const activeCompanies = gameState.config.companies.filter((def) => {
    const comp = gameState.companies[def.id];
    return comp && comp.isActive && comp.size >= 2;
  });

  // Total buy count
  const totalSelectedBuyCount = Object.values(buyCounts).reduce((acc, c) => acc + c, 0);

  // Convert buy counts to purchases array
  const purchases: SharePurchase[] = Object.entries(buyCounts)
    .filter(([_, count]) => count > 0)
    .map(([companyId, count]) => ({ companyId, count }));

  // Convert sell counts to sales array
  const sales: ShareSale[] = Object.entries(sellCounts)
    .filter(([_, count]) => count > 0)
    .map(([companyId, count]) => ({ companyId, count }));

  const buyValidation = activePlayer
    ? validateSharePurchases(
        activePlayer,
        gameState.companies,
        gameState.config.companies,
        purchases,
        effectiveMaxShares
      )
    : { valid: false, totalCost: 0, reason: 'No active player' };

  const sellValidation = activePlayer
    ? validateShareSales(
        activePlayer,
        gameState.companies,
        gameState.config.companies,
        sales
      )
    : { valid: false, totalRevenue: 0, reason: 'No active player' };

  const handleBuyIncrement = (companyId: CompanyId) => {
    const current = buyCounts[companyId] || 0;
    const company = gameState.companies[companyId];
    const def = gameState.config.companies.find((c) => c.id === companyId);
    if (!company || !def || !activePlayer) return;

    if (totalSelectedBuyCount >= effectiveMaxShares) return;
    if (current >= company.availableShares) return;

    const price = calculateSharePrice(def, company.size);
    if (buyValidation.totalCost + price > activePlayer.cash) return;

    playClick();
    setBuyCounts((prev) => ({
      ...prev,
      [companyId]: (prev[companyId] || 0) + 1,
    }));
  };

  const handleBuyDecrement = (companyId: CompanyId) => {
    const current = buyCounts[companyId] || 0;
    if (current <= 0) return;
    playClick();
    setBuyCounts((prev) => {
      const next = { ...prev, [companyId]: current - 1 };
      if (next[companyId] === 0) delete next[companyId];
      return next;
    });
  };

  const handleSellIncrement = (companyId: CompanyId) => {
    const current = sellCounts[companyId] || 0;
    const owned = activePlayer ? activePlayer.shares[companyId] ?? 0 : 0;
    if (current >= owned) return;

    playClick();
    setSellCounts((prev) => ({
      ...prev,
      [companyId]: (prev[companyId] || 0) + 1,
    }));
  };

  const handleSellDecrement = (companyId: CompanyId) => {
    const current = sellCounts[companyId] || 0;
    if (current <= 0) return;
    playClick();
    setSellCounts((prev) => {
      const next = { ...prev, [companyId]: current - 1 };
      if (next[companyId] === 0) delete next[companyId];
      return next;
    });
  };

  const handleConfirmPurchase = () => {
    if (!buyValidation.valid || purchases.length === 0) return;
    playCashChime();
    onConfirmPurchase(purchases);
  };

  const handleConfirmSale = () => {
    if (!sellValidation.valid || sales.length === 0) return;
    playCashChime();
    if (onConfirmSale) {
      onConfirmSale(sales);
    }
  };

  const handleSellAll = () => {
    playCashChime();
    if (onSellAll) {
      onSellAll();
    } else {
      const maxSales: Record<CompanyId, number> = {};
      for (const comp of activeCompanies) {
        const owned = activePlayer?.shares[comp.id] ?? 0;
        if (owned > 0) {
          maxSales[comp.id] = owned;
        }
      }
      setSellCounts(maxSales);
    }
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
            <span>
              {mode === 'buy'
                ? 'Stock Exchange — Buy Shares'
                : 'Stock Exchange — Sell Shares'}
            </span>
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
          {/* Mode Switcher Tabs (Classic "To Buy" / "To Sell") */}
          <div
            style={{
              display: 'flex',
              gap: '6px',
              padding: '4px',
              background: 'var(--bg-surface-elevated)',
              borderRadius: 'var(--radius-sm)',
              marginBottom: '12px',
              border: '1px solid var(--border-subtle)',
            }}
          >
            <button
              type="button"
              className={`btn btn-sm ${mode === 'buy' ? 'btn-primary' : 'btn-secondary'}`}
              style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', gap: '6px' }}
              onClick={() => {
                playClick();
                setMode('buy');
              }}
            >
              <ArrowDownRight size={14} />
              <span>To Buy</span>
            </button>
            <button
              type="button"
              className={`btn btn-sm ${mode === 'sell' ? 'btn-primary' : 'btn-secondary'}`}
              style={{ flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center', gap: '6px' }}
              onClick={() => {
                playClick();
                setMode('sell');
              }}
            >
              <ArrowUpRight size={14} />
              <span>To Sell</span>
            </button>
          </div>

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
              <div style={{ fontSize: '12px', color: 'var(--text-muted)' }}>
                {mode === 'buy' ? 'Buyer Portfolio' : 'Seller Portfolio'}
              </div>
              <div style={{ fontSize: '14px', fontWeight: 700, color: 'var(--text-primary)' }}>
                {activePlayer ? activePlayer.name : 'Unknown Player'}
              </div>
            </div>
            <div style={{ textAlign: 'right' }}>
              <div style={{ fontSize: '11px', color: 'var(--text-muted)' }}>Available Cash</div>
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

          {mode === 'buy' ? (
            <>
              {/* Buy Quota Banner */}
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
                  Purchase quota: <strong>{totalSelectedBuyCount} / {effectiveMaxShares}</strong> shares selected this turn
                  {totalSoldThisTurn > 0 && (
                    <span style={{ color: 'var(--text-green)', marginLeft: '6px', fontSize: '11px' }}>
                      (+{totalSoldThisTurn} repurchase allowance)
                    </span>
                  )}
                </span>
                <span
                  style={{
                    fontFamily: 'var(--font-mono)',
                    fontSize: '11px',
                    fontWeight: 600,
                    color: 'var(--text-amber)',
                  }}
                >
                  {Math.max(0, effectiveMaxShares - totalSelectedBuyCount)} remaining
                </span>
              </div>

              {/* Active Companies List for Buying */}
              <div style={{ display: 'flex', flexDirection: 'column', gap: '8px', maxHeight: '320px', overflowY: 'auto' }}>
                {activeCompanies.length === 0 ? (
                  <div style={{ textAlign: 'center', padding: '24px 0', color: 'var(--text-muted)', fontSize: '13px' }}>
                    No corporate entities have been founded yet. Place tiles adjacent to unincorporated wells to establish companies!
                  </div>
                ) : (
                  activeCompanies.map((def) => {
                    const company = gameState.companies[def.id]!;
                    const price = calculateSharePrice(def, company.size);
                    const selected = buyCounts[def.id] || 0;
                    const owned = activePlayer ? activePlayer.shares[def.id] || 0 : 0;
                    const canAddMore =
                      totalSelectedBuyCount < effectiveMaxShares &&
                      selected < company.availableShares &&
                      activePlayer &&
                      buyValidation.totalCost + price <= activePlayer.cash;

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
                            {company.isSafe && (
                              <span
                                style={{
                                  fontSize: '9px',
                                  textTransform: 'uppercase',
                                  padding: '1px 5px',
                                  borderRadius: '3px',
                                  background: 'rgba(56, 189, 248, 0.15)',
                                  color: '#38bdf8',
                                  fontWeight: 700,
                                }}
                              >
                                Safe
                              </span>
                            )}
                          </div>
                          <div style={{ display: 'flex', gap: '10px', fontSize: '11px', color: 'var(--text-muted)', margin: '3px 0 0 0' }}>
                            <span>Size: <strong>{company.size}</strong></span>
                            <span>Bank: <strong>{company.availableShares}</strong> left</span>
                            <span>You own: <strong>{owned}</strong></span>
                          </div>
                        </div>

                        <div style={{ display: 'flex', alignItems: 'center', gap: '14px' }}>
                          <div style={{ textAlign: 'right' }}>
                            <div
                              style={{
                                fontFamily: 'var(--font-mono)',
                                fontSize: '14px',
                                fontWeight: 700,
                                color: 'var(--text-amber)',
                              }}
                            >
                              ${price}
                            </div>
                            <div style={{ fontSize: '10px', color: 'var(--text-muted)' }}>per share</div>
                          </div>

                          <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                            <button
                              type="button"
                              className="btn btn-secondary btn-sm"
                              style={{ padding: '4px 8px', minWidth: '28px' }}
                              disabled={selected <= 0}
                              onClick={() => handleBuyDecrement(def.id)}
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
                                color: selected > 0 ? 'var(--accent-gold)' : 'var(--text-primary)',
                              }}
                            >
                              {selected}
                            </span>
                            <button
                              type="button"
                              className="btn btn-secondary btn-sm"
                              style={{ padding: '4px 8px', minWidth: '28px' }}
                              disabled={!canAddMore}
                              onClick={() => handleBuyIncrement(def.id)}
                              aria-label={`Increase ${def.name} shares`}
                            >
                              <Plus size={13} />
                            </button>
                          </div>
                        </div>
                      </div>
                    );
                  })
                )}
              </div>

              {/* Total Buy Cost summary */}
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
                    <span style={{ fontSize: '13px', fontWeight: 600 }}>Total Purchase Cost:</span>
                  </div>
                  <div
                    style={{
                      fontFamily: 'var(--font-mono)',
                      fontSize: '18px',
                      fontWeight: 700,
                      color: buyValidation.valid ? 'var(--text-amber)' : 'var(--text-red)',
                    }}
                  >
                    ${buyValidation.totalCost.toLocaleString()}
                  </div>
                </div>
              )}

              {!buyValidation.valid && purchases.length > 0 && (
                <div style={{ display: 'flex', alignItems: 'center', gap: '6px', marginTop: '8px', color: '#f87171', fontSize: '12px' }}>
                  <AlertCircle size={14} />
                  <span>{buyValidation.reason}</span>
                </div>
              )}
            </>
          ) : (
            <>
              {/* Sell Banner & Sell All Action */}
              <div
                style={{
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'space-between',
                  padding: '8px 12px',
                  background: 'rgba(34, 197, 94, 0.08)',
                  border: '1px solid var(--border-success, #22c55e)',
                  borderRadius: 'var(--radius-sm)',
                  marginBottom: '14px',
                }}
              >
                <div>
                  <span style={{ fontSize: '12px', color: 'var(--text-primary)' }}>
                    Liquidate shares into cash at current market valuation.
                  </span>
                </div>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={handleSellAll}
                  style={{ padding: '4px 10px', fontSize: '11px', fontWeight: 600 }}
                >
                  Sell All Shares
                </button>
              </div>

              {/* Active Companies List for Selling */}
              <div style={{ display: 'flex', flexDirection: 'column', gap: '8px', maxHeight: '320px', overflowY: 'auto' }}>
                {activeCompanies.length === 0 ? (
                  <div style={{ textAlign: 'center', padding: '24px 0', color: 'var(--text-muted)', fontSize: '13px' }}>
                    No active companies available for share sales.
                  </div>
                ) : (
                  activeCompanies.map((def) => {
                    const company = gameState.companies[def.id]!;
                    const price = calculateSharePrice(def, company.size);
                    const selected = sellCounts[def.id] || 0;
                    const owned = activePlayer ? activePlayer.shares[def.id] || 0 : 0;
                    const canSellMore = selected < owned;

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
                          opacity: owned === 0 ? 0.6 : 1,
                        }}
                      >
                        <div>
                          <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                            <span style={{ fontWeight: 700, fontSize: '13px', color: 'var(--text-primary)' }}>
                              {def.name}
                            </span>
                            {company.isSafe && (
                              <span
                                style={{
                                  fontSize: '9px',
                                  textTransform: 'uppercase',
                                  padding: '1px 5px',
                                  borderRadius: '3px',
                                  background: 'rgba(56, 189, 248, 0.15)',
                                  color: '#38bdf8',
                                  fontWeight: 700,
                                }}
                              >
                                Safe
                              </span>
                            )}
                          </div>
                          <div style={{ display: 'flex', gap: '10px', fontSize: '11px', color: 'var(--text-muted)', margin: '3px 0 0 0' }}>
                            <span>Size: <strong>{company.size}</strong></span>
                            <span>Bank: <strong>{company.availableShares}</strong> left</span>
                            <span>Owned: <strong>{owned}</strong></span>
                          </div>
                        </div>

                        <div style={{ display: 'flex', alignItems: 'center', gap: '14px' }}>
                          <div style={{ textAlign: 'right' }}>
                            <div
                              style={{
                                fontFamily: 'var(--font-mono)',
                                fontSize: '14px',
                                fontWeight: 700,
                                color: 'var(--text-green)',
                              }}
                            >
                              +${price}
                            </div>
                            <div style={{ fontSize: '10px', color: 'var(--text-muted)' }}>per share</div>
                          </div>

                          <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                            <button
                              type="button"
                              className="btn btn-secondary btn-sm"
                              style={{ padding: '4px 8px', minWidth: '28px' }}
                              disabled={selected <= 0}
                              onClick={() => handleSellDecrement(def.id)}
                              aria-label={`Decrease selling ${def.name} shares`}
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
                                color: selected > 0 ? 'var(--text-green)' : 'var(--text-primary)',
                              }}
                            >
                              {selected}
                            </span>
                            <button
                              type="button"
                              className="btn btn-secondary btn-sm"
                              style={{ padding: '4px 8px', minWidth: '28px' }}
                              disabled={!canSellMore}
                              onClick={() => handleSellIncrement(def.id)}
                              aria-label={`Increase selling ${def.name} shares`}
                            >
                              <Plus size={13} />
                            </button>
                          </div>
                        </div>
                      </div>
                    );
                  })
                )}
              </div>

              {/* Total Sale Proceeds summary */}
              {sales.length > 0 && (
                <div
                  style={{
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'space-between',
                    padding: '10px 14px',
                    background: 'var(--bg-surface-hover)',
                    border: '1px solid var(--border-success, #22c55e)',
                    borderRadius: 'var(--radius-sm)',
                    marginTop: '14px',
                  }}
                >
                  <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                    <DollarSign size={16} color="var(--text-green)" />
                    <span style={{ fontSize: '13px', fontWeight: 600 }}>Total Sale Proceeds:</span>
                  </div>
                  <div
                    style={{
                      fontFamily: 'var(--font-mono)',
                      fontSize: '18px',
                      fontWeight: 700,
                      color: 'var(--text-green)',
                    }}
                  >
                    +${sellValidation.totalRevenue.toLocaleString()}
                  </div>
                </div>
              )}

              {!sellValidation.valid && sales.length > 0 && (
                <div style={{ display: 'flex', alignItems: 'center', gap: '6px', marginTop: '8px', color: '#f87171', fontSize: '12px' }}>
                  <AlertCircle size={14} />
                  <span>{sellValidation.reason}</span>
                </div>
              )}
            </>
          )}
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'space-between' }}>
          {mode === 'buy' ? (
            <>
              <button
                type="button"
                className="btn btn-secondary"
                onClick={handlePass}
              >
                Skip / Pass Turn
              </button>
              <div style={{ display: 'flex', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary"
                  onClick={onClose}
                >
                  Cancel
                </button>
                <button
                  type="button"
                  className="btn btn-primary"
                  disabled={purchases.length === 0 || !buyValidation.valid}
                  onClick={handleConfirmPurchase}
                >
                  Confirm Purchase (${buyValidation.totalCost})
                </button>
              </div>
            </>
          ) : (
            <>
              <button
                type="button"
                className="btn btn-secondary"
                onClick={handleSellAll}
              >
                Sell All Shares
              </button>
              <div style={{ display: 'flex', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary"
                  onClick={onClose}
                >
                  Cancel
                </button>
                <button
                  type="button"
                  className="btn btn-primary"
                  disabled={sales.length === 0 || !sellValidation.valid}
                  onClick={handleConfirmSale}
                >
                  Confirm Sale (+${sellValidation.totalRevenue})
                </button>
              </div>
            </>
          )}
        </div>
      </div>
    </div>
  );
};
