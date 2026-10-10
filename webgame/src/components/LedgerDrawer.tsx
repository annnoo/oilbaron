import React, { useState } from 'react';
import { X, BarChart2, PieChart, Building2 } from 'lucide-react';
import type { GameState } from '../engine/types';
import { calculateNetWorth, calculateSharePrice } from '../engine/market';
import { playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

export interface LedgerDrawerProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
}

type LedgerTab = 'wealth' | 'shares' | 'companies' | 'history';

export const LedgerDrawer: React.FC<LedgerDrawerProps> = ({ isOpen, onClose, gameState }) => {
  const { t } = useTranslation();
  const [activeTab, setActiveTab] = useState<LedgerTab>('wealth');

  if (!isOpen) return null;

  const { players, companies, config } = gameState;

  return (
    <div className="ledger-drawer-backdrop" onClick={onClose}>
      <div className="ledger-drawer-panel" onClick={(e) => e.stopPropagation()}>
        <div className="drawer-header">
          <div className="drawer-title-group">
            <span className="drawer-title">{t('drawer.title')}</span>
            <span className="drawer-subtitle">{t('drawer.subtitle')}</span>
          </div>
          <button
            type="button"
            className="drawer-close-btn"
            onClick={() => {
              playClick();
              onClose();
            }}
            aria-label="close ledger"
          >
            <X size={18} />
          </button>
        </div>

        <div className="drawer-tabs">
          <button
            type="button"
            className={`drawer-tab ${activeTab === 'wealth' ? 'active' : ''}`}
            onClick={() => setActiveTab('wealth')}
            aria-label="wealth"
          >
            <BarChart2 size={14} />
            <span>{t('drawer.tabWealth')}</span>
          </button>
          <button
            type="button"
            className={`drawer-tab ${activeTab === 'shares' ? 'active' : ''}`}
            onClick={() => setActiveTab('shares')}
            aria-label="shares"
          >
            <PieChart size={14} />
            <span>{t('drawer.tabShares')}</span>
          </button>
          <button
            type="button"
            className={`drawer-tab ${activeTab === 'companies' ? 'active' : ''}`}
            onClick={() => setActiveTab('companies')}
            aria-label="companies"
          >
            <Building2 size={14} />
            <span>{t('drawer.tabCompanies')}</span>
          </button>
        </div>

        <div className="drawer-body">
          {activeTab === 'wealth' && (
            <div className="tab-pane-wealth">
              <h4 className="pane-title">{t('drawer.netWorth')}</h4>
              <div className="wealth-list">
                {players.map((p) => {
                  const netWorth = calculateNetWorth(p, companies, config.companies);
                  const stockVal = netWorth - p.cash;
                  return (
                    <div key={p.id} className="wealth-card">
                      <div className="wealth-card-header">
                        <span className="player-name">{p.name}</span>
                        <span className="player-networth">${netWorth.toLocaleString()}</span>
                      </div>
                      <div className="wealth-progress-bar">
                        <div
                          className="bar-cash"
                          style={{ width: `${Math.min(100, (p.cash / (netWorth || 1)) * 100)}%` }}
                          title={`Cash: $${p.cash}`}
                        />
                        <div
                          className="bar-stocks"
                          style={{
                            width: `${Math.min(100, (stockVal / (netWorth || 1)) * 100)}%`,
                          }}
                          title={`Stock: $${stockVal}`}
                        />
                      </div>
                      <div className="wealth-breakdown">
                        <span>Cash: ${p.cash.toLocaleString()}</span>
                        <span>Stocks: ${stockVal.toLocaleString()}</span>
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          )}

          {activeTab === 'shares' && (
            <div className="tab-pane-shares">
              <h4 className="pane-title">{t('drawer.matrixTitle')}</h4>
              <div className="matrix-table-wrap">
                <table className="matrix-table">
                  <thead>
                    <tr>
                      <th>{t('drawer.playerCol')}</th>
                      {config.companies.map((c) => (
                        <th key={c.id} style={{ color: c.color }}>
                          {c.name.slice(0, 3)}
                        </th>
                      ))}
                    </tr>
                  </thead>
                  <tbody>
                    {players.map((p) => (
                      <tr key={p.id}>
                        <td className="row-player-name">{p.name}</td>
                        {config.companies.map((c) => (
                          <td key={c.id} className="row-shares-val">
                            {p.shares[c.id] || 0}
                          </td>
                        ))}
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          )}

          {activeTab === 'companies' && (
            <div className="tab-pane-companies">
              <h4 className="pane-title">{t('drawer.corpOverview')}</h4>
              <div className="companies-grid">
                {config.companies.map((def) => {
                  const cState = companies[def.id];
                  const active = cState?.isActive ?? false;
                  const price =
                    (cState as any)?.stockPrice !== undefined
                      ? (cState as any).stockPrice
                      : active && cState
                        ? calculateSharePrice(def, cState.size)
                        : 0;
                  return (
                    <div
                      key={def.id}
                      className={`company-stat-tile ${active ? 'active' : 'inactive'}`}
                      style={{ borderLeftColor: def.color }}
                    >
                      <div className="stat-name">{def.name}</div>
                      <div className="stat-details">
                        <span>Status: {active ? (cState?.isSafe ? t('drawer.safe') : t('drawer.active')) : t('drawer.dormant')}</span>
                        <span>Wells: {cState?.size ?? 0}</span>
                        <span>Price: ${price}</span>
                        <span>Bank Shares: {cState?.availableShares ?? config.totalSharesPerCompany}</span>
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
