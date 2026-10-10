import React, { useState } from 'react';
import { Plus, Minus, Check, ShoppingBag } from 'lucide-react';
import type { CompanyDefinition, CompanyId, CompanyState, SharePurchase } from '../engine/types';
import { calculateSharePrice } from '../engine/market';
import { playCashChime, playClick } from '../assets/audio';

export interface InlineStockMarketProps {
  activePlayerCash: number;
  companies: Partial<Record<CompanyId, CompanyState & { stockPrice?: number }>>;
  definitions: readonly CompanyDefinition[];
  onConfirmPurchases: (purchases: SharePurchase[]) => void;
  onPass: () => void;
}

export const InlineStockMarket: React.FC<InlineStockMarketProps> = ({
  activePlayerCash,
  companies,
  definitions,
  onConfirmPurchases,
  onPass,
}) => {
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

  const handleFinish = () => {
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
  };

  return (
    <div className="inline-stock-shelf">
      <div className="shelf-header">
        <span className="shelf-title">
          <ShoppingBag size={14} style={{ marginRight: '6px' }} />
          Stock Exchange: Invest in up to 3 shares
        </span>
        <span className="shelf-budget">
          Selected: <strong>{totalSelectedShares} / 3 shares</strong> | Total:{' '}
          <strong>${totalCost}</strong> | Cash: <strong>${activePlayerCash}</strong>
        </span>
      </div>

      <div className="shelf-companies-row">
        {activeCompanyList.length === 0 ? (
          <div className="no-companies-note">No active companies with available shares.</div>
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
                  <span className="company-avail">{cState?.availableShares} left</span>
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

      <div className="shelf-footer-actions">
        <button
          type="button"
          className="btn btn-primary finish-turn-btn"
          onClick={handleFinish}
          aria-label="finish turn"
        >
          <Check size={14} style={{ marginRight: '6px' }} />
          {totalSelectedShares > 0 ? `Buy Shares & Finish Turn (Alt+E)` : `Pass & Finish Turn (Alt+E)`}
        </button>
      </div>
    </div>
  );
};
