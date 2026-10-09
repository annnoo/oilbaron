import React, { useState, useEffect } from 'react';
import { Sliders, X, Check, RotateCcw } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { GameConfig } from '../../engine/types';
import { CLASSIC_CONFIG, MODERN_BALANCED_CONFIG } from '../../engine/config';

export interface OptionsModalProps {
  isOpen: boolean;
  onClose: () => void;
  config: GameConfig;
  onSaveConfig: (newConfig: GameConfig, slowCpu: boolean) => void;
  slowCpu: boolean;
}

export const OptionsModal: React.FC<OptionsModalProps> = ({
  isOpen,
  onClose,
  config,
  onSaveConfig,
  slowCpu,
}) => {
  const [ruleset, setRuleset] = useState<GameConfig['ruleset']>(config.ruleset);
  const [handSize, setHandSize] = useState<number>(config.handSize);
  const [safeThreshold, setSafeThreshold] = useState<number>(config.safeThreshold);
  const [startingCash, setStartingCash] = useState<number>(config.startingCash);
  const [isSlowCpu, setIsSlowCpu] = useState<boolean>(slowCpu);

  useEffect(() => {
    if (isOpen) {
      setRuleset(config.ruleset);
      setHandSize(config.handSize);
      setSafeThreshold(config.safeThreshold);
      setStartingCash(config.startingCash);
      setIsSlowCpu(slowCpu);
    }
  }, [isOpen, config, slowCpu]);

  if (!isOpen) return null;

  const handleRulesetSelect = (selected: GameConfig['ruleset']) => {
    playClick();
    setRuleset(selected);
    if (selected === 'oilbaron_classic') {
      setHandSize(CLASSIC_CONFIG.handSize);
      setSafeThreshold(CLASSIC_CONFIG.safeThreshold);
      setStartingCash(CLASSIC_CONFIG.startingCash);
    } else {
      setHandSize(MODERN_BALANCED_CONFIG.handSize);
      setSafeThreshold(MODERN_BALANCED_CONFIG.safeThreshold);
      setStartingCash(MODERN_BALANCED_CONFIG.startingCash);
    }
  };

  const handleResetDefaults = () => {
    playClick();
    const defaults = ruleset === 'oilbaron_classic' ? CLASSIC_CONFIG : MODERN_BALANCED_CONFIG;
    setHandSize(defaults.handSize);
    setSafeThreshold(defaults.safeThreshold);
    setStartingCash(defaults.startingCash);
    setIsSlowCpu(false);
  };

  const handleSave = () => {
    playClick();
    const base = ruleset === 'oilbaron_classic' ? CLASSIC_CONFIG : MODERN_BALANCED_CONFIG;
    const newConfig: GameConfig = {
      ...base,
      ruleset,
      handSize,
      safeThreshold,
      startingCash,
      founderBonusShares: ruleset === 'oilbaron_classic' ? 10 : 1,
    };
    onSaveConfig(newConfig, isSlowCpu);
    onClose();
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
        className="modal-dialog modal-md"
        role="dialog"
        aria-modal="true"
        aria-labelledby="options-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="options-title">
            <Sliders size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Game Options & Ruleset</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close options modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Ruleset Selection */}
          <div className="form-group" style={{ marginBottom: '16px' }}>
            <span className="form-label">Ruleset Paradigm</span>
            <div className="ruleset-cards-grid">
              <button
                type="button"
                className={`ruleset-card ${ruleset === 'oilbaron_classic' ? 'is-selected' : ''}`}
                onClick={() => handleRulesetSelect('oilbaron_classic')}
              >
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                  <span style={{ fontWeight: 700, color: 'var(--accent-gold)' }}>1993 Oil Baron Classic</span>
                  {ruleset === 'oilbaron_classic' && <Check size={16} color="var(--accent-gold)" />}
                </div>
                <span className="form-help">
                  Authentic 10 founder bonus shares. High-stakes early founding advantages and classic Windows 3.1 economy.
                </span>
              </button>

              <button
                type="button"
                className={`ruleset-card ${ruleset === 'modern_balanced' ? 'is-selected' : ''}`}
                onClick={() => handleRulesetSelect('modern_balanced')}
              >
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                  <span style={{ fontWeight: 700, color: '#38bdf8' }}>Modern Balanced</span>
                  {ruleset === 'modern_balanced' && <Check size={16} color="#38bdf8" />}
                </div>
                <span className="form-help">
                  Standard 1 founder bonus share. Smooth competitive curve with tighter mid-game control.
                </span>
              </button>
            </div>
          </div>

          {/* Hand Size Slider */}
          <div className="form-group" style={{ marginBottom: '14px' }}>
            <div className="form-label">
              <span>Tiles in Player Hand</span>
              <span className="slider-value-badge">{handSize} tiles</span>
            </div>
            <div className="slider-container">
              <input
                type="range"
                min={4}
                max={10}
                value={handSize}
                onChange={(e) => setHandSize(Number(e.target.value))}
                className="slider-input"
              />
            </div>
            <span className="form-help">Default is 6 tiles. Larger hands grant broader tactical foresight.</span>
          </div>

          {/* Safe Threshold Slider */}
          <div className="form-group" style={{ marginBottom: '14px' }}>
            <div className="form-label">
              <span>Safe Company Threshold</span>
              <span className="slider-value-badge">{safeThreshold} tiles</span>
            </div>
            <div className="slider-container">
              <input
                type="range"
                min={8}
                max={20}
                value={safeThreshold}
                onChange={(e) => setSafeThreshold(Number(e.target.value))}
                className="slider-input"
              />
            </div>
            <span className="form-help">
              Companies with this size or greater cannot be acquired in mergers (Default: 11).
            </span>
          </div>

          {/* Starting Cash */}
          <div className="form-group" style={{ marginBottom: '14px' }}>
            <div className="form-label">
              <span>Starting Cash</span>
              <span className="slider-value-badge">${startingCash.toLocaleString()}</span>
            </div>
            <div className="slider-container">
              <input
                type="range"
                min={2000}
                max={12000}
                step={500}
                value={startingCash}
                onChange={(e) => setStartingCash(Number(e.target.value))}
                className="slider-input"
              />
            </div>
            <span className="form-help">Capital allocated to each player at match start (Default: $6,000).</span>
          </div>

          {/* Slow Computer Turns Toggle */}
          <div className="form-group" style={{ marginTop: '16px' }}>
            <label style={{ display: 'flex', alignItems: 'center', gap: '10px', cursor: 'pointer' }}>
              <input
                type="checkbox"
                checked={isSlowCpu}
                onChange={(e) => {
                  playClick();
                  setIsSlowCpu(e.target.checked);
                }}
                style={{ width: '16px', height: '16px', accentColor: 'var(--accent-gold)' }}
              />
              <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-primary)' }}>
                Slow Computer Turns (Display AI thinking pause)
              </span>
            </label>
            <span className="form-help" style={{ marginLeft: '26px' }}>
              Adds intentional delays between CPU actions so you can follow their placements and purchases.
            </span>
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'space-between' }}>
          <button
            type="button"
            className="btn btn-secondary"
            onClick={handleResetDefaults}
          >
            <RotateCcw size={14} style={{ marginRight: '6px' }} />
            Reset Defaults
          </button>
          <div style={{ display: 'flex', gap: '8px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={onClose}
            >
              Cancel
            </button>
            <button
              type="button"
              className="btn btn-primary"
              onClick={handleSave}
            >
              Apply Options
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};
