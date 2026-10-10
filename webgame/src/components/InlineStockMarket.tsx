import React, { useState, useEffect, useRef } from 'react';
import { Plus, Minus, Check, ShoppingBag, ArrowDownRight, ArrowUpRight, DollarSign, Trash2 } from 'lucide-react';
import type { CompanyDefinition, CompanyId, CompanyState, SharePurchase, ShareSale } from '../engine/types';
import { calculateSharePrice } from '../engine/market';
import { playCashChime, playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

export interface InlineStockMarketProps {
  activePlayerCash: number;
  companies: Partial<Record<CompanyId, CompanyState & { stockPrice?: number }>>;
  definitions: readonly CompanyDefinition[];
  playerShares?: Partial<Record<CompanyId, number>>;
  onConfirmPurchases: (purchases: SharePurchase[]) => void;
  onConfirmSales?: (sales: ShareSale[]) => void;
  onSellAll?: () => void;
  onPass: () => void;
}

export const InlineStockMarket: React.FC<InlineStockMarketProps> = ({
  activePlayerCash,
  companies,
  definitions,
  playerShares = {},
  onConfirmPurchases,
  onConfirmSales,
  onSellAll,
  onPass,
}) => {
  const { t, language } = useTranslation();
  const [mode, setMode] = useState<'buy' | 'sell'>('buy');
  const [quantities, setQuantities] = useState<Record<CompanyId, number>>({});

  const activeCompanyList = definitions.filter((def) => {
    const c = companies[def.id];
    return c && c.isActive && c.availableShares > 0;
  });

  const getPrice = (def: CompanyDefinition): number => {
    const c = companies[def.id];
    if (!c) return 0;
    if (typeof c.stockPrice === 'number') {
      return c.stockPrice;
    }
    return calculateSharePrice(def, c.size);
  };

  const totalSelectedShares = Object.values(quantities).reduce((a, b) => a + b, 0);

  const totalCost = activeCompanyList.reduce((sum, def) => {
    const qty = quantities[def.id] || 0;
    const price = getPrice(def);
    return sum + qty * price;
  }, 0);

  // Owned companies for sell mode
  const ownedCompanies = definitions.filter((def) => (playerShares[def.id] ?? 0) > 0);
  const totalOwnedSharesCount = ownedCompanies.reduce(
    (sum, def) => sum + (playerShares[def.id] ?? 0),
    0
  );
  const totalPortfolioValue = ownedCompanies.reduce((sum, def) => {
    const count = playerShares[def.id] ?? 0;
    return sum + count * getPrice(def);
  }, 0);

  const handleIncrement = (id: CompanyId) => {
    const c = companies[id];
    const def = definitions.find((d) => d.id === id);
    if (!c || !def) return;
    const currentQty = quantities[id] || 0;
    const price = getPrice(def);
    if (totalSelectedShares >= 3) return;
    if (currentQty >= c.availableShares) return;
    if (totalCost + price > activePlayerCash) return;

    playClick();
    setQuantities((prev) => ({ ...prev, [id]: (prev[id] || 0) + 1 }));
  };

  const handleDecrement = (id: CompanyId) => {
    const currentQty = quantities[id] || 0;
    if (currentQty <= 0) return;

    playClick();
    setQuantities((prev) => {
      const next = { ...prev, [id]: currentQty - 1 };
      if (next[id] === 0) delete next[id];
      return next;
    });
  };

  const handleSellOne = (companyId: CompanyId) => {
    playCashChime();
    onConfirmSales?.([{ companyId, count: 1 }]);
  };

  const handleSellAllCompany = (companyId: CompanyId, count: number) => {
    playCashChime();
    onConfirmSales?.([{ companyId, count }]);
  };

  const handleLiquidateEntirePortfolio = () => {
    playCashChime();
    onSellAll?.();
  };

  const handleFinish = () => {
    if (mode === 'buy') {
      const purchases: SharePurchase[] = Object.entries(quantities)
        .filter(([_, count]) => count > 0)
        .map(([companyId, count]) => ({ companyId: companyId as CompanyId, count }));

      if (purchases.length > 0) {
        playCashChime();
        onConfirmPurchases(purchases);
      } else {
        playClick();
        onPass();
      }
    } else {
      playClick();
      onPass();
    }
  };

  const handleFinishRef = useRef(handleFinish);
  handleFinishRef.current = handleFinish;

  useEffect(() => {
    if (typeof window === 'undefined') return;

    const handleKeyDown = (e: KeyboardEvent) => {
      // Don't trigger shortcuts inside text inputs
      if (
        e.target &&
        typeof e.target === 'object' &&
        'tagName' in e.target
      ) {
        const tagName = (e.target as { tagName?: string }).tagName?.toLowerCase();
        if (tagName === 'input' || tagName === 'textarea' || tagName === 'select') {
          return;
        }
      }

      if (e.altKey && (e.key?.toLowerCase() === 'e' || e.code === 'KeyE')) {
        e.preventDefault();
        e.stopPropagation();
        e.stopImmediatePropagation?.();
        handleFinishRef.current();
      }
    };

    window.addEventListener('keydown', handleKeyDown, true);
    return () => {
      window.removeEventListener('keydown', handleKeyDown, true);
    };
  }, []);

  return (
    <div className="inline-stock-shelf">
      <div className="shelf-header">
        <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
          <span className="shelf-title">
            <ShoppingBag size={14} style={{ marginRight: '6px' }} />
            {t('shelf.title')}
          </span>
          <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <button
              type="button"
              className={`btn-mode-tab ${mode === 'buy' ? 'active' : ''}`}
              onClick={() => {
                playClick();
                setMode('buy');
              }}
              aria-label={t('shelf.tabAcquire')}
            >
              <ShoppingBag size={13} style={{ marginRight: '5px' }} />
              {t('shelf.tabAcquire')}
            </button>
            <button
              type="button"
              className={`btn-mode-tab ${mode === 'sell' ? 'active' : ''}`}
              onClick={() => {
                playClick();
                setMode('sell');
              }}
              aria-label={t('shelf.tabLiquidate')}
            >
              <DollarSign size={13} style={{ marginRight: '5px' }} />
              {t('shelf.tabLiquidate')}
              {totalOwnedSharesCount > 0 && (
                <span className="tab-shares-badge">{totalOwnedSharesCount}</span>
              )}
            </button>
          </div>
        </div>

        <span className="shelf-budget">
          {mode === 'buy' ? (
            language === 'de' ? (
              <>
                Ausgewählt: <strong>{totalSelectedShares} / 3 Aktien</strong> | Gesamt:{' '}
                <strong>${totalCost}</strong> | Bargeld: <strong>${activePlayerCash}</strong>
              </>
            ) : (
              <>
                Selected: <strong>{totalSelectedShares} / 3 shares</strong> | Total:{' '}
                <strong>${totalCost}</strong> | Cash: <strong>${activePlayerCash}</strong>
              </>
            )
          ) : (
            <>
              Portfolio:{' '}
              <strong>
                {totalOwnedSharesCount} {language === 'de' ? 'Aktien' : 'shares'}
              </strong>{' '}
              | Wert: <strong>${totalPortfolioValue}</strong> | Cash:{' '}
              <strong>${activePlayerCash}</strong>
            </>
          )}
        </span>
      </div>

      {mode === 'buy' ? (
        <div className="shelf-companies-row">
          {activeCompanyList.length === 0 ? (
            <div className="no-companies-note">{t('shelf.noCompanies')}</div>
          ) : (
            activeCompanyList.map((def) => {
              const cState = companies[def.id];
              const qty = quantities[def.id] || 0;
              const price = getPrice(def);
              const canAdd =
                totalSelectedShares < 3 &&
                qty < (cState?.availableShares || 0) &&
                totalCost + price <= activePlayerCash;

              return (
                <div
                  key={def.id}
                  className="inline-company-card"
                  style={{ borderLeftColor: def.color }}
                >
                  <div className="company-info">
                    <span className="company-name">{def.name}</span>
                    <span className="company-price">${price}</span>
                    <span className="company-avail">
                      {cState?.availableShares} {language === 'de' ? 'übrig' : 'left'}
                    </span>
                  </div>
                  <div className="company-counter">
                    <button
                      type="button"
                      className="counter-btn"
                      onClick={() => handleDecrement(def.id)}
                      disabled={qty <= 0}
                      aria-label={`decrease ${def.name}`}
                    >
                      <Minus size={12} />
                    </button>
                    <span className="counter-val">{qty}</span>
                    <button
                      type="button"
                      className="counter-btn"
                      onClick={() => handleIncrement(def.id)}
                      disabled={!canAdd}
                      aria-label={`increase ${def.name}`}
                    >
                      <Plus size={12} />
                    </button>
                  </div>
                </div>
              );
            })
          )}
        </div>
      ) : (
        <div className="shelf-companies-row">
          {ownedCompanies.length === 0 ? (
            <div className="no-companies-note" style={{ padding: '12px 4px' }}>
              {t('shelf.noSharesOwned')}
            </div>
          ) : (
            ownedCompanies.map((def) => {
              const ownedCount = playerShares[def.id] ?? 0;
              const price = getPrice(def);
              const totalVal = ownedCount * price;

              return (
                <div
                  key={def.id}
                  className="inline-company-card"
                  style={{ borderLeftColor: def.color }}
                >
                  <div className="company-info">
                    <span className="company-name">{def.name}</span>
                    <span className="company-price">${price}</span>
                    <span className="company-avail">
                      {t('shelf.sharesOwned', { count: ownedCount, val: totalVal })}
                    </span>
                  </div>
                  <div className="company-counter" style={{ gap: '6px' }}>
                    <button
                      type="button"
                      className="btn-sell-action"
                      onClick={() => handleSellOne(def.id)}
                      aria-label={`sell 1 share of ${def.name}`}
                    >
                      {t('shelf.sellOne')}
                    </button>
                    <button
                      type="button"
                      className="btn-sell-action is-all"
                      onClick={() => handleSellAllCompany(def.id, ownedCount)}
                      aria-label={`sell all shares of ${def.name}`}
                    >
                      {t('shelf.sellAllCompany')}
                    </button>
                  </div>
                </div>
              );
            })
          )}
        </div>
      )}

      <div className="shelf-footer-actions">
        {mode === 'sell' && (
          <button
            type="button"
            className="btn btn-secondary liquidate-all-portfolio-btn"
            onClick={handleLiquidateEntirePortfolio}
            disabled={totalOwnedSharesCount === 0}
            aria-label={t('shelf.liquidateEntire')}
            style={{ marginRight: 'auto' }}
          >
            <Trash2 size={13} style={{ marginRight: '5px' }} />
            {t('shelf.liquidateEntire')}
          </button>
        )}

        <button
          type="button"
          className="btn btn-primary finish-turn-btn"
          onClick={handleFinish}
          aria-label="finish turn"
        >
          <Check size={14} style={{ marginRight: '6px' }} />
          {mode === 'buy'
            ? totalSelectedShares > 0
              ? t('shelf.buyFinish')
              : t('shelf.passFinish')
            : t('shelf.finishTurn')}
        </button>
      </div>
    </div>
  );
};

