import React, { useState, useEffect } from 'react';
import { DollarSign, ArrowRightLeft, Shield, AlertCircle, Check } from 'lucide-react';
import { playClick, playCashChime } from '../../assets/audio';
import { CompanyId, GameState, ShareDispositionChoices } from '../../engine/types';
import { validateShareDisposition } from '../../engine/merger';

export interface ShareDispositionModalProps {
  isOpen: boolean;
  gameState: GameState;
  defunctCompanyId: CompanyId;
  survivorCompanyId: CompanyId;
  preMergerPrice: number;
  onConfirmDisposition: (choices: ShareDispositionChoices) => void;
}

export const ShareDispositionModal: React.FC<ShareDispositionModalProps> = ({
  isOpen,
  gameState,
  defunctCompanyId,
  survivorCompanyId,
  preMergerPrice,
  onConfirmDisposition,
}) => {
  const pendingMerger = gameState.pendingMerger;
  const currentShareholderId =
    pendingMerger?.shareholderQueue[pendingMerger?.currentShareholderIndex ?? 0];

  const shareholder = gameState.players.find((p) => p.id === currentShareholderId);
  const totalShares = shareholder?.shares[defunctCompanyId] || 0;

  const survivorCompany = gameState.companies[survivorCompanyId];
  const defunctDef = gameState.config.companies.find((c) => c.id === defunctCompanyId);
  const survivorDef = gameState.config.companies.find((c) => c.id === survivorCompanyId);

  const [sell, setSell] = useState<number>(0);
  const [trade, setTrade] = useState<number>(0);
  const [hold, setHold] = useState<number>(totalShares);

  // Reset counters when shareholder or defunct company changes
  useEffect(() => {
    if (isOpen) {
      setSell(0);
      setTrade(0);
      setHold(totalShares);
    }
  }, [isOpen, currentShareholderId, defunctCompanyId, totalShares]);

  if (!isOpen || !shareholder || !survivorCompany) return null;

  const maxTradeEven = Math.min(
    Math.floor((totalShares - sell) / 2) * 2,
    survivorCompany.availableShares * 2
  );

  const handleSetSell = (newSell: number) => {
    const clampedSell = Math.max(0, Math.min(newSell, totalShares));
    playClick();
    setSell(clampedSell);

    // Rebalance trade if sell + trade > totalShares
    let currentTrade = trade;
    if (clampedSell + currentTrade > totalShares) {
      currentTrade = Math.floor((totalShares - clampedSell) / 2) * 2;
      setTrade(currentTrade);
    }
    setHold(totalShares - clampedSell - currentTrade);
  };

  const handleSetTrade = (newTrade: number) => {
    // trade must be even
    const evenTrade = Math.max(0, Math.floor(newTrade / 2) * 2);
    const maxPossible = Math.min(
      totalShares - sell,
      survivorCompany.availableShares * 2
    );
    const clampedTrade = Math.min(evenTrade, maxPossible);
    playClick();
    setTrade(clampedTrade);
    setHold(totalShares - sell - clampedTrade);
  };

  const handleSellAll = () => {
    playClick();
    setSell(totalShares);
    setTrade(0);
    setHold(0);
  };

  const handleTradeMax = () => {
    playClick();
    const maxTrade = Math.min(
      Math.floor(totalShares / 2) * 2,
      survivorCompany.availableShares * 2
    );
    setTrade(maxTrade);
    setSell(0);
    setHold(totalShares - maxTrade);
  };

  const handleHoldAll = () => {
    playClick();
    setSell(0);
    setTrade(0);
    setHold(totalShares);
  };

  const validation = validateShareDisposition(shareholder, defunctCompanyId, survivorCompany, {
    sell,
    trade,
    hold,
  });

  const cashGained = sell * preMergerPrice;
  const survivorSharesGained = Math.floor(trade / 2);

  const handleConfirm = () => {
    if (!validation.valid) return;
    if (cashGained > 0) {
      playCashChime();
    } else {
      playClick();
    }
    onConfirmDisposition({ sell, trade, hold });
  };

  return (
    <div className="modal-backdrop" role="presentation">
      <div
        className="modal-dialog modal-md"
        role="dialog"
        aria-modal="true"
        aria-labelledby="disposition-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="disposition-title">
            <ArrowRightLeft size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Merger Share Disposition</span>
          </div>
        </div>

        <div className="modal-body">
          {/* Active Shareholder Card */}
          <div
            style={{
              padding: '10px 14px',
              background: 'var(--bg-surface-elevated)',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-sm)',
              marginBottom: '14px',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
            }}
          >
            <div>
              <div style={{ fontSize: '11px', color: 'var(--text-muted)' }}>Shareholder Disposition Turn</div>
              <div style={{ fontSize: '14px', fontWeight: 700, color: 'var(--text-primary)' }}>
                {shareholder.name}
              </div>
            </div>
            <div style={{ textAlign: 'right' }}>
              <div style={{ fontSize: '11px', color: 'var(--text-muted)' }}>
                {defunctDef?.name} Stock Held
              </div>
              <div
                style={{
                  fontFamily: 'var(--font-mono)',
                  fontSize: '16px',
                  fontWeight: 700,
                  color: 'var(--text-amber)',
                }}
              >
                {totalShares} Shares
              </div>
            </div>
          </div>

          <div style={{ fontSize: '12px', color: 'var(--text-secondary)', marginBottom: '12px' }}>
            Choose how to dispose of your {totalShares} shares of {defunctDef?.name} (pre-merger price: ${preMergerPrice}).
            Surviving company ({survivorDef?.name}) has {survivorCompany.availableShares} shares left in bank.
          </div>

          {/* Quick Buttons Toolbar */}
          <div className="preset-button-bar" style={{ marginBottom: '14px' }}>
            <button type="button" className="btn btn-secondary btn-sm" onClick={handleSellAll}>
              Sell All ({totalShares})
            </button>
            <button
              type="button"
              className="btn btn-secondary btn-sm"
              onClick={handleTradeMax}
              disabled={maxTradeEven === 0}
            >
              Trade Max ({maxTradeEven})
            </button>
            <button type="button" className="btn btn-secondary btn-sm" onClick={handleHoldAll}>
              Hold All ({totalShares})
            </button>
          </div>

          {/* 3 Disposition Rows: Sell, Trade, Hold */}
          <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
            {/* 1. SELL ROW */}
            <div
              style={{
                padding: '10px 12px',
                background: 'var(--bg-surface-elevated)',
                border: '1px solid var(--border-default)',
                borderRadius: 'var(--radius-sm)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'space-between',
              }}
            >
              <div>
                <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                  <DollarSign size={15} color="#22c55e" />
                  <span style={{ fontWeight: 700, fontSize: '13px' }}>Sell Shares</span>
                </div>
                <div style={{ fontSize: '11px', color: 'var(--text-muted)', marginTop: '2px' }}>
                  Receive ${preMergerPrice} cash per share
                </div>
              </div>

              <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  style={{ padding: '3px 8px' }}
                  disabled={sell <= 0}
                  onClick={() => handleSetSell(sell - 1)}
                >
                  -1
                </button>
                <span
                  style={{
                    fontFamily: 'var(--font-mono)',
                    fontSize: '14px',
                    fontWeight: 700,
                    minWidth: '24px',
                    textAlign: 'center',
                  }}
                >
                  {sell}
                </span>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  style={{ padding: '3px 8px' }}
                  disabled={sell >= totalShares}
                  onClick={() => handleSetSell(sell + 1)}
                >
                  +1
                </button>
              </div>
            </div>

            {/* 2. TRADE 2-FOR-1 ROW */}
            <div
              style={{
                padding: '10px 12px',
                background: 'var(--bg-surface-elevated)',
                border: '1px solid var(--border-default)',
                borderRadius: 'var(--radius-sm)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'space-between',
              }}
            >
              <div>
                <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                  <ArrowRightLeft size={15} color="#38bdf8" />
                  <span style={{ fontWeight: 700, fontSize: '13px' }}>Trade 2-for-1</span>
                </div>
                <div style={{ fontSize: '11px', color: 'var(--text-muted)', marginTop: '2px' }}>
                  Trade 2 {defunctDef?.name} for 1 {survivorDef?.name} share
                </div>
              </div>

              <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  style={{ padding: '3px 8px' }}
                  disabled={trade <= 0}
                  onClick={() => handleSetTrade(trade - 2)}
                >
                  -2
                </button>
                <span
                  style={{
                    fontFamily: 'var(--font-mono)',
                    fontSize: '14px',
                    fontWeight: 700,
                    minWidth: '24px',
                    textAlign: 'center',
                  }}
                >
                  {trade}
                </span>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  style={{ padding: '3px 8px' }}
                  disabled={trade + 2 > totalShares - sell || (trade / 2 + 1) > survivorCompany.availableShares}
                  onClick={() => handleSetTrade(trade + 2)}
                >
                  +2
                </button>
              </div>
            </div>

            {/* 3. HOLD ROW */}
            <div
              style={{
                padding: '10px 12px',
                background: 'var(--bg-surface-elevated)',
                border: '1px solid var(--border-default)',
                borderRadius: 'var(--radius-sm)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'space-between',
              }}
            >
              <div>
                <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                  <Shield size={15} color="#a855f7" />
                  <span style={{ fontWeight: 700, fontSize: '13px' }}>Hold Defunct Shares</span>
                </div>
                <div style={{ fontSize: '11px', color: 'var(--text-muted)', marginTop: '2px' }}>
                  Keep shares in dormant reserve for potential refounding
                </div>
              </div>

              <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                <span
                  style={{
                    fontFamily: 'var(--font-mono)',
                    fontSize: '14px',
                    fontWeight: 700,
                    minWidth: '24px',
                    textAlign: 'center',
                  }}
                >
                  {hold}
                </span>
              </div>
            </div>
          </div>

          {/* Real-Time Outcome Summary */}
          <div
            style={{
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              padding: '10px 14px',
              background: 'rgba(245, 158, 11, 0.08)',
              border: '1px solid var(--border-accent)',
              borderRadius: 'var(--radius-sm)',
              marginTop: '14px',
              fontSize: '12px',
            }}
          >
            <div>
              Cash Gained: <strong style={{ color: 'var(--text-green)' }}>+${cashGained.toLocaleString()}</strong>
            </div>
            <div>
              {survivorDef?.name} Shares: <strong style={{ color: '#38bdf8' }}>+{survivorSharesGained}</strong>
            </div>
            <div>
              Defunct Kept: <strong>{hold}</strong>
            </div>
          </div>

          {!validation.valid && (
            <div style={{ display: 'flex', alignItems: 'center', gap: '6px', marginTop: '8px', color: '#f87171', fontSize: '12px' }}>
              <AlertCircle size={14} />
              <span>{validation.reason}</span>
            </div>
          )}
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end', gap: '8px' }}>
          <button
            type="button"
            className="btn btn-primary"
            disabled={!validation.valid}
            onClick={handleConfirm}
          >
            Confirm Share Disposition
          </button>
        </div>
      </div>
    </div>
  );
};
