import React from 'react';
import { Volume2, VolumeX, BarChart2, Settings, Monitor } from 'lucide-react';
import type { CompanyDefinition, CompanyId, CompanyState, TurnPhase } from '../engine/types';
import { calculateSharePrice } from '../engine/market';
import { playClick } from '../assets/audio';

export interface TacticalOpsBarProps {
  activePlayerName: string;
  activePlayerCash: number;
  activePlayerNetWorth?: number;
  activePlayerColor?: string;
  isAi: boolean;
  turnPhase: TurnPhase;
  directiveText: string;
  directivePhase: string;
  companies: Partial<Record<CompanyId, CompanyState | (Partial<CompanyState> & { stockPrice?: number; [key: string]: any })>>;
  definitions: readonly CompanyDefinition[];
  soundMuted: boolean;
  language: 'en' | 'de';
  onToggleSound: () => void;
  onToggleLedger: () => void;
  onOpenOptions: () => void;
  onSwitchClassic?: () => void;
}

export const TacticalOpsBar: React.FC<TacticalOpsBarProps> = ({
  activePlayerName,
  activePlayerCash,
  activePlayerNetWorth,
  activePlayerColor = '#38bdf8',
  isAi,
  directiveText,
  directivePhase,
  companies,
  definitions,
  soundMuted,
  onToggleSound,
  onToggleLedger,
  onOpenOptions,
  onSwitchClassic,
}) => {
  const activeCompanies = definitions.filter((def) => {
    const c = companies[def.id];
    return c && c.isActive;
  });

  return (
    <header className="ops-bar-container">
      <div className="ops-bar">
        {/* Concessionaire Profile */}
        <div className="baron-profile">
          <div className="badge-icon" style={{ borderColor: activePlayerColor }}>
            {activePlayerName.slice(0, 2).toUpperCase()}
          </div>
          <div className="baron-meta">
            <div className="title">
              {activePlayerName} {isAi ? '(CPU)' : ''}
            </div>
            <div className="funds">
              ${activePlayerCash.toLocaleString()}
              {activePlayerNetWorth !== undefined && (
                <span className="networth"> / ${activePlayerNetWorth.toLocaleString()}</span>
              )}
            </div>
          </div>
        </div>

        {/* Turn Directive */}
        <div className="turn-directive">
          <div className="phase">{directivePhase}</div>
          <div className="text">{directiveText}</div>
        </div>

        {/* Tactical Controls */}
        <div className="ops-controls">
          <button
            type="button"
            className="btn-hud"
            onClick={() => {
              playClick();
              onToggleLedger();
            }}
            title="Strategic Ledger & Financials (Alt+L)"
            aria-label="ledger"
          >
            <BarChart2 size={16} />
          </button>

          <button
            type="button"
            className="btn-hud"
            onClick={() => {
              playClick();
              onToggleSound();
            }}
            title={soundMuted ? 'Unmute Audio' : 'Mute Audio'}
            aria-label="sound"
          >
            {soundMuted ? <VolumeX size={16} /> : <Volume2 size={16} />}
          </button>

          <button
            type="button"
            className="btn-hud"
            onClick={() => {
              playClick();
              onOpenOptions();
            }}
            title="Operations Settings"
            aria-label="settings"
          >
            <Settings size={16} />
          </button>

          {onSwitchClassic && (
            <button
              type="button"
              className="btn-hud"
              onClick={() => {
                playClick();
                onSwitchClassic();
              }}
              title="Switch to 1993 View"
              aria-label="classic view"
            >
              <Monitor size={16} />
            </button>
          )}
        </div>
      </div>

      {/* Active Pipeline Network Strip */}
      <div className="network-status-bar">
        {activeCompanies.length === 0 ? (
          <div className="network-idle">No active corporate pipeline networks.</div>
        ) : (
          activeCompanies.map((def) => {
            const cState = companies[def.id];
            const isSafe = cState?.isSafe ?? false;
            const price = typeof (cState as any)?.stockPrice === 'number'
              ? (cState as any).stockPrice
              : (cState?.size ? calculateSharePrice(def, cState.size) : 0);
            return (
              <div
                key={def.id}
                className={`conglomerate-badge ${isSafe ? 'safe' : ''}`}
                style={{ borderLeftColor: def.color }}
              >
                <span className="company-name" style={{ color: def.color }}>
                  {def.name}
                </span>
                {isSafe && <span className="safe-pill">REFINERY HUB</span>}
                <span className="company-details">
                  ${price} · {cState?.size ?? 0} wells
                </span>
              </div>
            );
          })
        )}
      </div>
    </header>
  );
};
