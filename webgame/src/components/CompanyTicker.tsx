import React from 'react';
import { Shield, TrendingUp, ChevronRight, Layers } from 'lucide-react';
import type { CompanyDefinition, CompanyId, CompanyState } from '../engine/types';
import { CLASSIC_COMPANIES, DEFAULT_CONFIG } from '../engine/config';
import { calculateSharePrice, getCompanyDefinition } from '../engine/market';
import { useTranslation } from '../i18n';

export interface CompanyTickerProps {
  /**
   * Current company states, either as a record mapping (GameState.companies)
   * or an array of CompanyState.
   */
  companies: Record<CompanyId, CompanyState> | CompanyState[];

  /**
   * Template definitions for calculating share price curves.
   * Defaults to CLASSIC_COMPANIES.
   */
  definitions?: CompanyDefinition[] | Record<CompanyId, CompanyDefinition>;

  /**
   * Safe size threshold (defaults to 11).
   */
  safeThreshold?: number;

  /**
   * Total share supply per company (defaults to 25).
   */
  totalShares?: number;

  /**
   * Optional callback when an active company card is clicked.
   */
  onSelectCompany?: (companyId: CompanyId) => void;

  /**
   * Optional custom CSS class name.
   */
  className?: string;
}

export const CompanyTicker: React.FC<CompanyTickerProps> = ({
  companies,
  definitions = CLASSIC_COMPANIES,
  safeThreshold = DEFAULT_CONFIG.safeThreshold,
  totalShares = DEFAULT_CONFIG.totalSharesPerCompany,
  onSelectCompany,
  className = '',
}) => {
  const { t } = useTranslation();

  // Normalize companies record/array
  const companyList: CompanyState[] = Array.isArray(companies)
    ? companies
    : Object.values(companies);

  const activeCompanies = companyList.filter((c) => c.isActive && c.size > 0);
  const unformedCompanies = companyList.filter((c) => !c.isActive || c.size === 0);

  return (
    <div
      className={`company-ticker ${className}`}
      role="region"
      aria-label="Stock Market Company Ticker"
    >
      <div className="ticker-header">
        <TrendingUp size={14} className="ticker-header-icon" />
        <span className="ticker-header-title">{t('ticker.market')}</span>
        {activeCompanies.length > 0 && (
          <span className="ticker-counter-badge">
            {activeCompanies.length} {t('ticker.active')}
          </span>
        )}
      </div>

      <div className="ticker-scroll-container">
        {activeCompanies.length === 0 ? (
          <div className="ticker-empty-wrapper">
            <span className="ticker-empty-msg">
              {t('ticker.emptyMsg')}
            </span>
            <div className="ticker-unformed-pills">
              <span className="ticker-unformed-heading">{t('ticker.available')}</span>
              {unformedCompanies.map((c) => {
                const def = getCompanyDefinition(definitions, c.id);
                const basePrice = def?.basePrice ?? 200;
                return (
                  <span
                    key={c.id}
                    className="ticker-unformed-pill"
                    title={`${c.name} (${c.tier} tier) - Base Price $${basePrice}`}
                  >
                    <span
                      className="ticker-dot"
                      style={{ backgroundColor: c.color }}
                    />
                    <span className="ticker-unformed-name">{c.name}</span>
                    <span className="ticker-unformed-price">${basePrice}</span>
                  </span>
                );
              })}
            </div>
          </div>
        ) : (
          <div className="ticker-cards-row">
            {activeCompanies.map((company) => {
              const def = getCompanyDefinition(definitions, company.id);
              const fallbackDef: CompanyDefinition = {
                id: company.id,
                name: company.name,
                tier: company.tier,
                basePrice: 200,
                priceDelta: 100,
                color: company.color,
              };
              const sharePrice = calculateSharePrice(def ?? fallbackDef, company.size);
              const isSafe = company.isSafe || company.size >= safeThreshold;

              return (
                <div
                  key={company.id}
                  className={`company-ticker-card ${isSafe ? 'is-safe' : ''} ${
                    onSelectCompany ? 'clickable' : ''
                  }`}
                  style={{
                    borderLeftColor: company.color,
                  }}
                  onClick={() => onSelectCompany?.(company.id)}
                  role={onSelectCompany ? 'button' : undefined}
                  tabIndex={onSelectCompany ? 0 : undefined}
                  onKeyDown={
                    onSelectCompany
                      ? (e) => {
                          if (e.key === 'Enter' || e.key === ' ') {
                            e.preventDefault();
                            onSelectCompany(company.id);
                          }
                        }
                      : undefined
                  }
                  title={`${company.name} • ${company.size} tiles • $${sharePrice}/share`}
                >
                  <div className="card-top">
                    <span
                      className="ticker-dot"
                      style={{ backgroundColor: company.color }}
                    />
                    <span className="company-title">{company.name}</span>
                    {isSafe && (
                      <span
                        className="badge badge-safe ticker-safe-badge"
                        title={t('ticker.safeBadgeTitle', { threshold: safeThreshold })}
                      >
                        <Shield size={10} />
                        <span>{t('common.safe')}</span>
                      </span>
                    )}
                  </div>

                  <div className="card-stats">
                    <div className="stat-group">
                      <span className="stat-label">{t('common.size')}</span>
                      <span className="stat-value num">{company.size}</span>
                    </div>

                    <div className="stat-group">
                      <span className="stat-label">{t('common.price')}</span>
                      <span className="stat-value money text-gold">
                        ${sharePrice.toLocaleString()}
                      </span>
                    </div>

                    <div className="stat-group">
                      <span className="stat-label">{t('common.bank')}</span>
                      <span className="stat-value shares">
                        {company.availableShares}
                        <span className="shares-max">/{totalShares}</span>
                      </span>
                    </div>
                  </div>
                </div>
              );
            })}

            {/* Compact summary of remaining unformed companies */}
            {unformedCompanies.length > 0 && (
              <div
                className="ticker-unformed-summary-card"
                title={`${unformedCompanies.length} companies available to be founded`}
              >
                <div className="unformed-summary-header">
                  <Layers size={12} className="text-secondary" />
                  <span>{unformedCompanies.length} {t('ticker.unformed')}</span>
                </div>
                <div className="unformed-dots">
                  {unformedCompanies.map((c) => (
                    <span
                      key={c.id}
                      className="unformed-dot"
                      style={{ backgroundColor: c.color }}
                      title={`${c.name} (${t('ticker.unformed')})`}
                    />
                  ))}
                </div>
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
};

export default CompanyTicker;
