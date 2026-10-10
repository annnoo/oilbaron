import React from 'react';
import {
  Volume2,
  VolumeX,
  BookOpen,
  BarChart3,
  RotateCcw,
  Languages,
  Monitor,
} from 'lucide-react';
import type { TurnPhase } from '../engine/types';
import { playClick } from '../assets/audio';

export interface GameHeaderProps {
  activePlayerName: string;
  activePlayerColor?: string;
  isAi: boolean;
  turnPhase: TurnPhase;
  soundMuted: boolean;
  language: 'en' | 'de';
  onToggleSound: () => void;
  onToggleLanguage: () => void;
  onToggleLedger: () => void;
  onOpenRules: () => void;
  onOpenNewGame: () => void;
  onSwitchClassic?: () => void;
}

export const GameHeader: React.FC<GameHeaderProps> = ({
  activePlayerName,
  activePlayerColor = '#f59e0b',
  isAi,
  turnPhase,
  soundMuted,
  language,
  onToggleSound,
  onToggleLanguage,
  onToggleLedger,
  onOpenRules,
  onOpenNewGame,
  onSwitchClassic,
}) => {
  const getStepLabel = () => {
    if (isAi) return 'AI Turn in Progress';
    switch (turnPhase) {
      case 'place_tile':
        return 'Step 1: Drill Well';
      case 'choose_founding_company':
        return 'Incorporate Company';
      case 'choose_merger_survivor':
        return 'Merger Tiebreak';
      case 'dispose_merger_shares':
        return 'Merger Disposition';
      case 'buy_shares':
        return 'Step 2: Invest in Stocks';
      case 'game_over':
        return 'Game Over';
      default:
        return 'Active Turn';
    }
  };

  return (
    <header className="game-header">
      <div className="header-brand">
        <div className="brand-logo-badge">🛢️</div>
        <div className="brand-title">
          <span className="brand-name">OIL BARON</span>
          <span className="brand-sub">STRATEGY</span>
        </div>
      </div>

      <div className="header-turn-status">
        <span
          className="player-color-dot"
          style={{ backgroundColor: activePlayerColor }}
        />
        <span className="player-turn-name">
          {activePlayerName} {isAi ? '(CPU)' : ''}
        </span>
        <span className="turn-step-pill">{getStepLabel()}</span>
      </div>

      <div className="header-actions">
        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onToggleLedger();
          }}
          title="Ledger & Strategic Analytics"
          aria-label="ledger"
        >
          <BarChart3 size={16} />
          <span className="btn-label">Ledger</span>
        </button>

        <button
          type="button"
          className="header-icon-btn"
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
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onToggleLanguage();
          }}
          title="Switch Language (EN / DE)"
          aria-label="language"
        >
          <Languages size={16} />
          <span className="btn-label">{language.toUpperCase()}</span>
        </button>

        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onOpenRules();
          }}
          title="Game Rules & Guide"
          aria-label="rules"
        >
          <BookOpen size={16} />
        </button>

        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onOpenNewGame();
          }}
          title="New Match"
          aria-label="new game"
        >
          <RotateCcw size={16} />
        </button>

        {onSwitchClassic && (
          <button
            type="button"
            className="header-icon-btn classic-switch-btn"
            onClick={() => {
              playClick();
              onSwitchClassic();
            }}
            title="Switch to 1993 Windows 3.1 Classic View"
            aria-label="classic view"
          >
            <Monitor size={16} />
            <span className="btn-label">1993 View</span>
          </button>
        )}
      </div>
    </header>
  );
};
