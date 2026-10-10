import React, { useState, useEffect } from 'react';
import { Users, User, Bot, Plus, Trash2, X, Play, Palette } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { DEFAULT_PLAYER_COLORS } from '../../engine/config';
import type { GameConfig } from '../../engine/types';
import { useTranslation } from '../../i18n';

export interface PlayerConfig {
  id: string;
  name: string;
  color: string;
  isAi: boolean;
  aiDifficulty?: 'dumb' | 'smart';
}

export interface NewGameModalProps {
  isOpen: boolean;
  onClose: () => void;
  onStartGame: (players: PlayerConfig[], options?: Partial<Pick<GameConfig, 'enableEighthCompany' | 'randomSharePrices' | 'randomDividends'>>) => void;
  initialPlayers?: readonly PlayerConfig[];
  initialConfig?: GameConfig;
}

export const COLOR_PALETTE: readonly string[] = [
  '#f59e0b', // Gold / Amber
  '#38bdf8', // Cyan / Sky Blue
  '#22c55e', // Emerald Green
  '#a855f7', // Violet / Purple
  '#f97316', // Coral / Orange
  '#ef4444', // Crimson / Red
  '#ec4899', // Hot Pink
  '#14b8a6', // Teal
];

const DEFAULT_ROSTER: readonly PlayerConfig[] = [
  { id: 'p1', name: 'Player 1', color: '#f59e0b', isAi: false },
  { id: 'p2', name: 'Computer 1 (CPU)', color: '#38bdf8', isAi: true, aiDifficulty: 'smart' },
];

export const NewGameModal: React.FC<NewGameModalProps> = ({
  isOpen,
  onClose,
  onStartGame,
  initialPlayers,
  initialConfig,
}) => {
  const { t, language } = useTranslation();
  const [players, setPlayers] = useState<PlayerConfig[]>(() => {
    if (initialPlayers && initialPlayers.length >= 2) {
      return initialPlayers.map((p, idx) => ({
        id: p.id || `p${idx + 1}`,
        name: p.name,
        color: p.color || COLOR_PALETTE[idx % COLOR_PALETTE.length]!,
        isAi: Boolean(p.isAi),
        aiDifficulty: Boolean(p.isAi) ? (p.aiDifficulty ?? 'smart') : undefined,
      }));
    }
    return DEFAULT_ROSTER.map((p) => ({ ...p }));
  });

  const [activeColorPickerIdx, setActiveColorPickerIdx] = useState<number | null>(null);
  const [enableEighthCompany, setEnableEighthCompany] = useState<boolean>(
    initialConfig?.enableEighthCompany ?? false
  );
  const [randomSharePrices, setRandomSharePrices] = useState<boolean>(
    initialConfig?.randomSharePrices ?? false
  );
  const [randomDividends, setRandomDividends] = useState<boolean>(
    initialConfig?.randomDividends ?? false
  );

  // Sync state if modal reopens or initialPlayers/initialConfig changes
  useEffect(() => {
    if (isOpen) {
      if (initialPlayers && initialPlayers.length >= 2) {
        setPlayers(
          initialPlayers.map((p, idx) => ({
            id: p.id || `p${idx + 1}`,
            name: p.name,
            color: p.color || COLOR_PALETTE[idx % COLOR_PALETTE.length]!,
            isAi: Boolean(p.isAi),
            aiDifficulty: Boolean(p.isAi) ? (p.aiDifficulty ?? 'smart') : undefined,
          }))
        );
      } else {
        setPlayers(DEFAULT_ROSTER.map((p) => ({ ...p })));
      }
      setEnableEighthCompany(initialConfig?.enableEighthCompany ?? false);
      setRandomSharePrices(initialConfig?.randomSharePrices ?? false);
      setRandomDividends(initialConfig?.randomDividends ?? false);
      setActiveColorPickerIdx(null);
    }
  }, [isOpen, initialPlayers, initialConfig]);

  if (!isOpen) return null;

  const handleAddPlayer = () => {
    if (players.length >= 6) return;
    playClick();
    const nextIdx = players.length + 1;
    const isAi = players.length > 0;
    const defaultName = isAi ? `Computer ${nextIdx - 1} (CPU)` : `Player ${nextIdx}`;
    const nextColor = COLOR_PALETTE[players.length % COLOR_PALETTE.length]!;

    setPlayers((prev) => [
      ...prev,
      {
        id: `p${nextIdx}`,
        name: defaultName,
        color: nextColor,
        isAi,
        aiDifficulty: isAi ? 'smart' : undefined,
      },
    ]);
  };

  const handleRemovePlayer = (index: number) => {
    if (players.length <= 2) return;
    playClick();
    setPlayers((prev) => prev.filter((_, idx) => idx !== index));
    if (activeColorPickerIdx === index) {
      setActiveColorPickerIdx(null);
    }
  };

  const handleNameChange = (index: number, nextName: string) => {
    setPlayers((prev) =>
      prev.map((p, idx) => (idx === index ? { ...p, name: nextName } : p))
    );
  };

  const handleToggleAi = (index: number) => {
    playClick();
    setPlayers((prev) =>
      prev.map((p, idx) => {
        if (idx !== index) return p;
        const nextAi = !p.isAi;
        // Optionally update default name if it was untouched
        let updatedName = p.name;
        if (nextAi && p.name === `Player ${idx + 1}`) {
          updatedName = `Computer ${idx} (CPU)`;
        } else if (!nextAi && p.name.includes('(CPU)')) {
          updatedName = `Player ${idx + 1}`;
        }
        return {
          ...p,
          isAi: nextAi,
          aiDifficulty: nextAi ? (p.aiDifficulty ?? 'smart') : undefined,
          name: updatedName,
        };
      })
    );
  };

  const handleDifficultyChange = (index: number, difficulty: 'dumb' | 'smart') => {
    playClick();
    setPlayers((prev) =>
      prev.map((p, idx) => (idx === index ? { ...p, aiDifficulty: difficulty } : p))
    );
  };

  const handleSelectColor = (index: number, color: string) => {
    playClick();
    setPlayers((prev) =>
      prev.map((p, idx) => (idx === index ? { ...p, color } : p))
    );
    setActiveColorPickerIdx(null);
  };

  const handleApplyPreset = (presetType: '2p' | '3p' | '4p_cpu' | '4p_human') => {
    playClick();
    switch (presetType) {
      case '2p':
        setPlayers([
          { id: 'p1', name: 'Player 1', color: COLOR_PALETTE[0]!, isAi: false },
          { id: 'p2', name: 'Computer 1 (CPU)', color: COLOR_PALETTE[1]!, isAi: true, aiDifficulty: 'smart' },
        ]);
        break;
      case '3p':
        setPlayers([
          { id: 'p1', name: 'Player 1', color: COLOR_PALETTE[0]!, isAi: false },
          { id: 'p2', name: 'Computer 1 (CPU)', color: COLOR_PALETTE[1]!, isAi: true, aiDifficulty: 'smart' },
          { id: 'p3', name: 'Computer 2 (CPU)', color: COLOR_PALETTE[2]!, isAi: true, aiDifficulty: 'smart' },
        ]);
        break;
      case '4p_cpu':
        setPlayers([
          { id: 'p1', name: 'Player 1', color: COLOR_PALETTE[0]!, isAi: false },
          { id: 'p2', name: 'Computer 1 (CPU)', color: COLOR_PALETTE[1]!, isAi: true, aiDifficulty: 'smart' },
          { id: 'p3', name: 'Computer 2 (CPU)', color: COLOR_PALETTE[2]!, isAi: true, aiDifficulty: 'smart' },
          { id: 'p4', name: 'Computer 3 (CPU)', color: COLOR_PALETTE[3]!, isAi: true, aiDifficulty: 'smart' },
        ]);
        break;
      case '4p_human':
        setPlayers([
          { id: 'p1', name: 'Player 1', color: COLOR_PALETTE[0]!, isAi: false },
          { id: 'p2', name: 'Player 2', color: COLOR_PALETTE[1]!, isAi: false },
          { id: 'p3', name: 'Player 3', color: COLOR_PALETTE[2]!, isAi: false },
          { id: 'p4', name: 'Player 4', color: COLOR_PALETTE[3]!, isAi: false },
        ]);
        break;
    }
  };

  const handleStartGame = () => {
    playClick();
    const sanitized = players.map((p, idx) => ({
      ...p,
      name: p.name.trim() || `Player ${idx + 1}`,
      aiDifficulty: p.isAi ? (p.aiDifficulty ?? 'smart') : undefined,
    }));
    onStartGame(sanitized, {
      enableEighthCompany,
      randomSharePrices,
      randomDividends,
    });
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
        className="modal-dialog modal-lg"
        role="dialog"
        aria-modal="true"
        aria-labelledby="new-game-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="new-game-title">
            <Users size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>{t('newGame.title')}</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close setup modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Quick Presets Toolbar */}
          <div className="form-group" style={{ marginBottom: '14px' }}>
            <span className="form-label">
              <span>{t('newGame.presets')}</span>
              <span style={{ color: 'var(--text-muted)', fontWeight: 400 }}>
                {t('newGame.configuredCount', { count: players.length })}
              </span>
            </span>
            <div className="preset-button-bar">
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={() => handleApplyPreset('2p')}
              >
                {t('newGame.preset2p')}
              </button>
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={() => handleApplyPreset('3p')}
              >
                {t('newGame.preset3p')}
              </button>
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={() => handleApplyPreset('4p_cpu')}
              >
                {t('newGame.preset4pCpu')}
              </button>
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={() => handleApplyPreset('4p_human')}
              >
                {t('newGame.preset4pHuman')}
              </button>
            </div>
          </div>

          {/* Configured Players Roster */}
          <div className="form-group">
            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
              <span className="form-label" style={{ margin: 0 }}>
                {t('newGame.rosterTitle')}
              </span>
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={handleAddPlayer}
                disabled={players.length >= 6}
                title={players.length >= 6 ? t('newGame.maxPlayers') : t('newGame.addPlayer')}
              >
                <Plus size={14} />
                <span>{t('newGame.addPlayer')}</span>
              </button>
            </div>

            <div className="player-config-list" role="list">
              {players.map((player, index) => (
                <div
                  key={player.id || index}
                  className="player-config-row"
                  role="listitem"
                  data-testid={`player-row-${index}`}
                >
                  <span className="player-index-tag">#{index + 1}</span>

                  {/* Player Color Swatch */}
                  <div style={{ position: 'relative' }}>
                    <button
                      type="button"
                      className="player-color-swatch"
                      style={{ backgroundColor: player.color }}
                      onClick={() =>
                        setActiveColorPickerIdx(
                          activeColorPickerIdx === index ? null : index
                        )
                      }
                      title={`Change color for ${player.name}`}
                      aria-label={`Color for ${player.name}`}
                    />

                    {/* Popover Color Picker */}
                    {activeColorPickerIdx === index && (
                      <div
                        style={{
                          position: 'absolute',
                          top: '32px',
                          left: '0',
                          zIndex: 10,
                          backgroundColor: 'var(--bg-surface-elevated)',
                          border: '1px solid var(--border-default)',
                          borderRadius: 'var(--radius-sm)',
                          boxShadow: 'var(--shadow-modal)',
                          padding: '8px',
                          display: 'grid',
                          gridTemplateColumns: 'repeat(4, 1fr)',
                          gap: '6px',
                          width: '130px',
                        }}
                      >
                        {COLOR_PALETTE.map((c) => (
                          <button
                            key={c}
                            type="button"
                            style={{
                              width: '22px',
                              height: '22px',
                              borderRadius: '50%',
                              backgroundColor: c,
                              border:
                                player.color === c
                                  ? '2px solid #ffffff'
                                  : '1px solid rgba(255,255,255,0.2)',
                              cursor: 'pointer',
                              transform: player.color === c ? 'scale(1.15)' : 'none',
                            }}
                            onClick={() => handleSelectColor(index, c)}
                            aria-label={`Select color ${c}`}
                          />
                        ))}
                      </div>
                    )}
                  </div>

                  {/* Player Name Input */}
                  <input
                    type="text"
                    className="form-input"
                    style={{ flex: 1 }}
                    value={player.name}
                    onChange={(e) => handleNameChange(index, e.target.value)}
                    placeholder={`Player ${index + 1}`}
                    maxLength={24}
                    aria-label={`Name for player ${index + 1}`}
                  />

                  {/* Human vs CPU Toggle */}
                  <button
                    type="button"
                    className={`player-type-toggle-btn ${
                      player.isAi ? 'is-cpu' : 'is-human'
                    }`}
                    onClick={() => handleToggleAi(index)}
                    title={`Switch ${player.name} between Human and Computer AI`}
                    aria-label={`Toggle AI for ${player.name}`}
                  >
                    {player.isAi ? <Bot size={13} /> : <User size={13} />}
                    <span>{player.isAi ? t('common.cpu') : t('common.human')}</span>
                  </button>

                  {/* AI Difficulty Selector (CPU only) */}
                  {player.isAi && (
                    <select
                      className="form-select ai-difficulty-select"
                      style={{
                        width: 'auto',
                        minWidth: '95px',
                        padding: '4px 8px',
                        fontSize: '11px',
                        height: '28px',
                      }}
                      value={player.aiDifficulty ?? 'smart'}
                      onChange={(e) =>
                        handleDifficultyChange(index, e.target.value as 'dumb' | 'smart')
                      }
                      title={`AI Difficulty for ${player.name}`}
                      aria-label={`AI Difficulty for ${player.name}`}
                      data-testid={`ai-difficulty-${index}`}
                    >
                      <option value="smart">{t('newGame.smartAi')}</option>
                      <option value="dumb">{t('newGame.dumbAi')}</option>
                    </select>
                  )}

                  {/* Remove Player Button */}
                  <button
                    type="button"
                    className="btn btn-ghost btn-sm"
                    style={{ padding: '6px', color: 'var(--text-muted)' }}
                    onClick={() => handleRemovePlayer(index)}
                    disabled={players.length <= 2}
                    title={
                      players.length <= 2
                        ? t('newGame.minPlayers')
                        : `Remove ${player.name}`
                    }
                    aria-label={`Remove player ${index + 1}`}
                  >
                    <Trash2 size={14} />
                  </button>
                </div>
              ))}
            </div>
            <p className="form-help">
              {t('newGame.helpText')}
            </p>
          </div>

          {/* Gameplay Rule Toggles: 8th Company & Economy Randomizers */}
          <div style={{ display: 'flex', flexDirection: 'column', gap: '10px', marginTop: '16px', padding: '12px', background: 'var(--bg-surface-elevated)', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-subtle)' }}>
            <label style={{ display: 'flex', alignItems: 'center', gap: '10px', cursor: 'pointer' }}>
              <input
                type="checkbox"
                checked={enableEighthCompany}
                onChange={(e) => {
                  playClick();
                  setEnableEighthCompany(e.target.checked);
                }}
                style={{ width: '16px', height: '16px', accentColor: 'var(--accent-gold)' }}
              />
              <div>
                <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-primary)' }}>
                  Enable 8th Company ("Purple Oil Company, Int.")
                </span>
                <span className="form-help" style={{ display: 'block', margin: 0 }}>
                  Include the bonus 8th corporation (Purple) alongside the classic 7 oil companies.
                </span>
              </div>
            </label>

            <label style={{ display: 'flex', alignItems: 'center', gap: '10px', cursor: 'pointer' }}>
              <input
                type="checkbox"
                checked={randomSharePrices}
                onChange={(e) => {
                  playClick();
                  setRandomSharePrices(e.target.checked);
                }}
                style={{ width: '16px', height: '16px', accentColor: 'var(--accent-gold)' }}
              />
              <div>
                <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-primary)' }}>
                  Random Share Prices
                </span>
                <span className="form-help" style={{ display: 'block', margin: 0 }}>
                  Randomize base company share prices across tiers at match start.
                </span>
              </div>
            </label>

            <label style={{ display: 'flex', alignItems: 'center', gap: '10px', cursor: 'pointer' }}>
              <input
                type="checkbox"
                checked={randomDividends}
                onChange={(e) => {
                  playClick();
                  setRandomDividends(e.target.checked);
                }}
                style={{ width: '16px', height: '16px', accentColor: 'var(--accent-gold)' }}
              />
              <div>
                <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-primary)' }}>
                  Random Dividends
                </span>
                <span className="form-help" style={{ display: 'block', margin: 0 }}>
                  Randomize dividend yield payout rates across corporations.
                </span>
              </div>
            </label>
          </div>
        </div>

        <div className="modal-footer">
          <button type="button" className="btn btn-secondary" onClick={onClose}>
            {t('common.cancel')}
          </button>
          <button
            type="button"
            className="btn btn-primary"
            onClick={handleStartGame}
            disabled={players.length < 2 || players.length > 6}
          >
            <Play size={14} />
            <span>{t('newGame.startWithCount', { count: players.length })}</span>
          </button>
        </div>
      </div>
    </div>
  );
};
