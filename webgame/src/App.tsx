import React, { useState } from 'react';
import {
  Volume2,
  VolumeX,
  Flame,
  Coins,
  AlertTriangle,
  Trophy,
  Sparkles,
  TrendingUp,
  ShieldCheck,
  RotateCcw,
} from 'lucide-react';
import {
  soundEffects,
  playClick,
  playFoundCompany,
  playMergerAlert,
  playCashChime,
  playVictoryFanfare,
} from './assets/audio';
import { CLASSIC_COMPANIES, DEFAULT_CONFIG, createInitialGameState } from './engine/config';
import type { CompanyId, GameState } from './engine/types';
import { MenuBar, type MenuActionId } from './components/MenuBar';
import { CompanyTicker } from './components/CompanyTicker';

export const App: React.FC = () => {
  const [muted, setMuted] = useState<boolean>(soundEffects.isMuted());
  const [slowCpuTurns, setSlowCpuTurns] = useState<boolean>(false);
  const [hideChits, setHideChits] = useState<boolean>(false);
  const [activePlayerIsAi, setActivePlayerIsAi] = useState<boolean>(false);
  const [lastAction, setLastAction] = useState<string | null>(null);
  const [selectedCompanyId, setSelectedCompanyId] = useState<CompanyId | null>(null);

  // Initialize game state with standard 2-player match
  const [gameState, setGameState] = useState<GameState>(() =>
    createInitialGameState(DEFAULT_CONFIG, ['Player 1', 'Computer 1 (CPU)'])
  );

  const handleToggleMute = () => {
    const nextMuted = soundEffects.toggleMute();
    setMuted(nextMuted);
    setLastAction(`Sound toggled: ${nextMuted ? 'Muted' : 'Enabled'}`);
  };

  const handleMenuAction = (actionId: MenuActionId) => {
    switch (actionId) {
      case 'toggle_sound':
        handleToggleMute();
        break;
      case 'toggle_slow_cpu':
        setSlowCpuTurns((prev) => !prev);
        setLastAction(`Slow CPU turns: ${!slowCpuTurns ? 'ON' : 'OFF'}`);
        break;
      case 'toggle_hide_chits':
        setHideChits((prev) => !prev);
        setLastAction(`Hide chits (hotseat privacy): ${!hideChits ? 'ON' : 'OFF'}`);
        break;
      case 'toggle_player_ai':
        setActivePlayerIsAi((prev) => !prev);
        setLastAction(`Active player type changed to ${!activePlayerIsAi ? 'CPU' : 'Human'}`);
        break;
      case 'new_game_same':
        setGameState(createInitialGameState(DEFAULT_CONFIG, ['Player 1', 'Computer 1 (CPU)']));
        setLastAction('New Game started (Same Options)');
        playFoundCompany();
        break;
      default:
        setLastAction(`Menu Action: ${actionId.replace(/_/g, ' ').toUpperCase()}`);
        break;
    }
  };

  // Helper interactive simulation controls to verify CompanyTicker live updates
  const simulateFoundCompany = (companyId: CompanyId, initialSize = 2) => {
    playFoundCompany();
    setGameState((prev) => {
      const target = prev.companies[companyId];
      if (!target) return prev;
      return {
        ...prev,
        companies: {
          ...prev.companies,
          [companyId]: {
            ...target,
            isActive: true,
            size: initialSize,
            isSafe: initialSize >= prev.config.safeThreshold,
            availableShares: Math.max(0, target.availableShares - 1),
          },
        },
      };
    });
    setLastAction(`Founded ${companyId.toUpperCase()} with ${initialSize} tiles`);
  };

  const simulateGrowCompany = (companyId: CompanyId, delta = 3) => {
    playClick();
    setGameState((prev) => {
      const target = prev.companies[companyId];
      if (!target || !target.isActive) return prev;
      const nextSize = target.size + delta;
      const isSafe = nextSize >= prev.config.safeThreshold;
      if (isSafe && !target.isSafe) {
        playCashChime();
      }
      return {
        ...prev,
        companies: {
          ...prev.companies,
          [companyId]: {
            ...target,
            size: nextSize,
            isSafe,
          },
        },
      };
    });
    setLastAction(`Expanded ${companyId.toUpperCase()} by +${delta} tiles`);
  };

  const simulateResetCompanies = () => {
    playClick();
    setGameState(createInitialGameState(DEFAULT_CONFIG, ['Player 1', 'Computer 1 (CPU)']));
    setLastAction('Reset all company board sizes to unformed state');
  };

  return (
    <div className="app-container">
      {/* Authentic Windows 3.1 & Modern Top Menu Bar */}
      <MenuBar
        soundMuted={muted}
        slowCpuTurns={slowCpuTurns}
        hideChits={hideChits}
        activePlayerIsAi={activePlayerIsAi}
        activePlayerName={activePlayerIsAi ? 'Computer 1' : 'Player 1'}
        onAction={handleMenuAction}
        onToggleSound={handleToggleMute}
        onToggleSlowCpu={() => setSlowCpuTurns((prev) => !prev)}
        onToggleHideChits={() => setHideChits((prev) => !prev)}
        onTogglePlayerAi={() => setActivePlayerIsAi((prev) => !prev)}
        onNewGameSame={() => handleMenuAction('new_game_same')}
      />

      {/* Live Company Stock Ticker */}
      <CompanyTicker
        companies={gameState.companies}
        definitions={gameState.config.companies}
        safeThreshold={gameState.config.safeThreshold}
        totalShares={gameState.config.totalSharesPerCompany}
        onSelectCompany={(id) => {
          setSelectedCompanyId(id);
          playClick();
          setLastAction(`Selected company in ticker: ${id.toUpperCase()}`);
        }}
      />

      {/* Main Container */}
      <main className="game-main">
        {/* Action feedback alert banner */}
        {lastAction && (
          <div
            style={{
              padding: '8px 14px',
              backgroundColor: 'var(--bg-surface-elevated)',
              border: '1px solid var(--border-accent)',
              borderRadius: 'var(--radius-sm)',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              color: 'var(--text-amber)',
              fontSize: '12px',
              fontWeight: 500,
            }}
          >
            <span>▶ {lastAction}</span>
            <button
              type="button"
              className="btn btn-ghost btn-sm"
              style={{ padding: '2px 6px', fontSize: '11px' }}
              onClick={() => setLastAction(null)}
            >
              Dismiss
            </button>
          </div>
        )}

        {/* Task 2 Interactive Verification Panel */}
        <section className="panel">
          <div className="panel-header">
            <span>Task 2: Menu Bar & Company Ticker Controls</span>
            <span className="badge badge-safe">Active</span>
          </div>

          <div className="panel-body" style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
            <p style={{ color: 'var(--text-secondary)' }}>
              Top menu bar provides authentic 1993 dropdown navigation (Game, Actions, Views, Settings, Help)
              and keyboard shortcuts (e.g. <kbd className="menu-item-shortcut">Alt+B</kbd> Buy Shares,{' '}
              <kbd className="menu-item-shortcut">Alt+P</kbd> Place Chit,{' '}
              <kbd className="menu-item-shortcut">Alt+E</kbd> End Turn). The Company Ticker displays live active
              oil corporations, sizes, market share prices, bank supply, and Safe badges (
              <span className="badge badge-safe" style={{ display: 'inline-flex' }}>
                <ShieldCheck size={10} /> SAFE (11+)
              </span>
              ).
            </p>

            {/* Live Ticker State Simulator */}
            <div>
              <h2 style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-amber)', marginBottom: '8px' }}>
                Company Ticker Simulation Controls
              </h2>
              <div style={{ display: 'flex', flexWrap: 'wrap', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => simulateFoundCompany('american', 2)}
                >
                  <Flame size={14} color="#e63946" /> Found American (2 tiles, $200)
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => simulateGrowCompany('american', 3)}
                >
                  <TrendingUp size={14} /> Expand American (+3 tiles)
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => simulateFoundCompany('american', 11)}
                >
                  <ShieldCheck size={14} color="#4ade80" /> Make American Safe (11 tiles)
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => simulateFoundCompany('imperial', 4)}
                >
                  <Flame size={14} color="#2a9d8f" /> Found Imperial (4 tiles, $500)
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => simulateFoundCompany('royal', 12)}
                >
                  <ShieldCheck size={14} color="#1d3557" /> Found Royal (12 tiles - Safe)
                </button>
                <button
                  type="button"
                  className="btn btn-danger btn-sm"
                  onClick={simulateResetCompanies}
                >
                  <RotateCcw size={14} /> Reset Companies
                </button>
              </div>
            </div>

            {/* Audio Testing Controls */}
            <div>
              <h2 style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: '8px' }}>
                Audio Synthesizer Triggers
              </h2>
              <div style={{ display: 'flex', flexWrap: 'wrap', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playClick()}
                >
                  <Sparkles size={14} /> Click
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playCashChime()}
                >
                  <Coins size={14} /> Cash Chime
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playFoundCompany()}
                >
                  <Flame size={14} /> Found Company
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playMergerAlert()}
                >
                  <AlertTriangle size={14} /> Merger Siren
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playVictoryFanfare()}
                >
                  <Trophy size={14} /> Victory Fanfare
                </button>
              </div>
            </div>

            {/* Company Tokens Palette */}
            <div>
              <h2 style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: '8px' }}>
                All 8 Oil Corporations
              </h2>
              <div style={{ display: 'flex', flexWrap: 'wrap', gap: '8px' }}>
                {CLASSIC_COMPANIES.map((company) => (
                  <div
                    key={company.id}
                    style={{
                      display: 'flex',
                      alignItems: 'center',
                      gap: '8px',
                      padding: '4px 10px',
                      backgroundColor: 'var(--bg-surface-elevated)',
                      border: '1px solid var(--border-subtle)',
                      borderRadius: 'var(--radius-sm)',
                    }}
                  >
                    <span
                      style={{
                        width: '12px',
                        height: '12px',
                        borderRadius: '50%',
                        backgroundColor: company.color,
                        display: 'inline-block',
                      }}
                    />
                    <span style={{ fontSize: '12px', fontWeight: 500 }}>{company.name}</span>
                    <span style={{ fontSize: '11px', color: 'var(--text-muted)' }}>
                      (${company.basePrice})
                    </span>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </section>
      </main>
    </div>
  );
};

export default App;
