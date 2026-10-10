import React from 'react';
import {
  Volume2,
  VolumeX,
  BookOpen,
  BarChart3,
  RotateCcw,
  Languages,
  Monitor,
  Settings,
  FolderOpen,
} from 'lucide-react';
import type { TurnPhase } from '../engine/types';
import { playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

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
  onOpenOptions?: () => void;
  onOpenSaveLoad?: () => void;
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
  onOpenOptions,
  onOpenSaveLoad,
  onSwitchClassic,
}) => {
  const { t } = useTranslation();

  const getStepLabel = () => {
    if (isAi) return t('header.aiTurn');
    switch (turnPhase) {
      case 'place_tile':
        return t('header.step1');
      case 'choose_founding_company':
        return t('header.incorporate');
      case 'choose_merger_survivor':
        return t('header.mergerTiebreak');
      case 'dispose_merger_shares':
        return t('header.mergerDisposition');
      case 'buy_shares':
        return t('header.step2');
      case 'game_over':
        return t('header.gameOver');
      default:
        return t('header.activeTurn');
    }
  };

  return (
    <header className="game-header">
      <div className="header-brand">
        <div className="brand-logo-badge">🛢️</div>
        <div className="brand-title">
          <span className="brand-name">OIL BARON</span>
          <span className="brand-sub">{t('header.brandSub')}</span>
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
          title={t('header.ledgerTooltip')}
          aria-label="ledger"
        >
          <BarChart3 size={16} />
          <span className="btn-label">{t('header.ledger')}</span>
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

        {onOpenSaveLoad && (
          <button
            type="button"
            className="header-icon-btn"
            onClick={() => {
              playClick();
              onOpenSaveLoad();
            }}
            title={t('header.saveLoad')}
            aria-label="save load"
          >
            <FolderOpen size={16} />
          </button>
        )}

        {onOpenOptions && (
          <button
            type="button"
            className="header-icon-btn"
            onClick={() => {
              playClick();
              onOpenOptions();
            }}
            title={t('header.options')}
            aria-label="options"
          >
            <Settings size={16} />
          </button>
        )}

        <button
          type="button"
          className="header-icon-btn"
          onClick={() => {
            playClick();
            onOpenRules();
          }}
          title={t('header.rules')}
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
          title={t('header.newMatch')}
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
            title={t('header.classicViewTooltip')}
            aria-label="classic view"
          >
            <Monitor size={16} />
            <span className="btn-label">{t('header.classicView')}</span>
          </button>
        )}
      </div>
    </header>
  );
};
