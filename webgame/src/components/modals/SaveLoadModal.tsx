import React, { useState, useEffect } from 'react';
import { Save, FolderOpen, Download, Upload, Trash2, X, Check, Clock } from 'lucide-react';
import { playClick } from '../../assets/audio';
import { GameState } from '../../engine/types';

export interface SaveSlot {
  id: string;
  timestamp: number;
  dateStr: string;
  name: string;
  playerCount: number;
  activePlayer: string;
  state: GameState;
}

export interface SaveLoadModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  onLoadGame: (state: GameState) => void;
}

const STORAGE_KEY = 'oilbaron_saved_games';

export const SaveLoadModal: React.FC<SaveLoadModalProps> = ({
  isOpen,
  onClose,
  gameState,
  onLoadGame,
}) => {
  const [saves, setSaves] = useState<SaveSlot[]>([]);
  const [saveName, setSaveName] = useState<string>('');
  const [successMsg, setSuccessMsg] = useState<string | null>(null);

  useEffect(() => {
    if (isOpen) {
      try {
        const raw = localStorage.getItem(STORAGE_KEY);
        if (raw) {
          const parsed = JSON.parse(raw);
          if (Array.isArray(parsed)) {
            setSaves(parsed);
          }
        }
      } catch (err) {
        console.error('Failed to parse saved games:', err);
      }
      setSuccessMsg(null);
    }
  }, [isOpen]);

  if (!isOpen) return null;

  const handleSaveToBrowser = () => {
    playClick();
    const activePlayer = gameState.players[gameState.activePlayerIndex];
    const defaultName = saveName.trim() || `Turn ${gameState.history.length + 1} (${activePlayer ? activePlayer.name : 'Match'})`;

    const newSlot: SaveSlot = {
      id: `save_${Date.now()}`,
      timestamp: Date.now(),
      dateStr: new Date().toLocaleString(),
      name: defaultName,
      playerCount: gameState.players.length,
      activePlayer: activePlayer ? activePlayer.name : 'Unknown',
      state: gameState,
    };

    const updated = [newSlot, ...saves.slice(0, 9)]; // keep max 10 slots
    setSaves(updated);
    try {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(updated));
      setSuccessMsg(`Match saved as "${defaultName}"!`);
      setSaveName('');
    } catch (err) {
      console.error('Failed to save to localStorage:', err);
    }
  };

  const handleLoadSlot = (slot: SaveSlot) => {
    playClick();
    onLoadGame(slot.state);
    onClose();
  };

  const handleDeleteSlot = (id: string) => {
    playClick();
    const updated = saves.filter((s) => s.id !== id);
    setSaves(updated);
    localStorage.setItem(STORAGE_KEY, JSON.stringify(updated));
  };

  const handleExportJson = () => {
    playClick();
    const dataStr = 'data:text/json;charset=utf-8,' + encodeURIComponent(JSON.stringify(gameState, null, 2));
    const downloadAnchor = document.createElement('a');
    downloadAnchor.setAttribute('href', dataStr);
    downloadAnchor.setAttribute('download', `oilbaron-save-${Date.now()}.json`);
    document.body.appendChild(downloadAnchor);
    downloadAnchor.click();
    downloadAnchor.remove();
  };

  const handleImportJson = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (event) => {
      try {
        const parsed = JSON.parse(event.target?.result as string);
        if (parsed.board && parsed.players && parsed.config) {
          playClick();
          onLoadGame(parsed as GameState);
          onClose();
        } else {
          alert('Invalid Oil Baron match save file.');
        }
      } catch (err) {
        alert('Could not parse JSON save file.');
      }
    };
    reader.readAsText(file);
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
        aria-labelledby="saveload-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="saveload-title">
            <Save size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Save & Load Match State</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close save/load modal"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Save Match Input */}
          <div style={{ marginBottom: '16px' }}>
            <span className="form-label">Save Current Match</span>
            <div style={{ display: 'flex', gap: '8px' }}>
              <input
                type="text"
                className="form-input"
                style={{ flex: 1 }}
                placeholder="Match description (e.g. Turn 12 Mid-Game)"
                value={saveName}
                onChange={(e) => setSaveName(e.target.value)}
              />
              <button
                type="button"
                className="btn btn-primary"
                onClick={handleSaveToBrowser}
              >
                <Save size={14} style={{ marginRight: '6px' }} />
                Save Game
              </button>
            </div>
            {successMsg && (
              <div style={{ display: 'flex', alignItems: 'center', gap: '6px', color: '#22c55e', fontSize: '12px', marginTop: '6px' }}>
                <Check size={14} />
                <span>{successMsg}</span>
              </div>
            )}
          </div>

          {/* JSON Export / Import */}
          <div style={{ display: 'flex', gap: '8px', marginBottom: '18px' }}>
            <button
              type="button"
              className="btn btn-secondary btn-sm"
              style={{ flex: 1 }}
              onClick={handleExportJson}
            >
              <Download size={13} style={{ marginRight: '6px' }} />
              Export Save as JSON
            </button>
            <label
              className="btn btn-secondary btn-sm"
              style={{ flex: 1, cursor: 'pointer', textAlign: 'center' }}
            >
              <Upload size={13} style={{ marginRight: '6px' }} />
              Import JSON Save
              <input
                type="file"
                accept=".json"
                style={{ display: 'none' }}
                onChange={handleImportJson}
              />
            </label>
          </div>

          {/* Saved Games List */}
          <div>
            <span className="form-label">Saved Matches ({saves.length} / 10 slots)</span>
            {saves.length === 0 ? (
              <div style={{ padding: '24px', textAlign: 'center', color: 'var(--text-muted)', background: 'var(--bg-surface-elevated)', borderRadius: 'var(--radius-sm)' }}>
                No browser-saved matches found yet.
              </div>
            ) : (
              <div style={{ display: 'flex', flexDirection: 'column', gap: '8px', maxHeight: '240px', overflowY: 'auto' }}>
                {saves.map((slot) => (
                  <div
                    key={slot.id}
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
                      <div style={{ fontWeight: 700, fontSize: '13px', color: 'var(--text-primary)' }}>
                        {slot.name}
                      </div>
                      <div style={{ display: 'flex', gap: '10px', fontSize: '11px', color: 'var(--text-muted)', marginTop: '2px' }}>
                        <span><Clock size={11} style={{ verticalAlign: 'middle', marginRight: '3px' }} />{slot.dateStr}</span>
                        <span>{slot.playerCount} Players</span>
                        <span>Active: {slot.activePlayer}</span>
                      </div>
                    </div>

                    <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                      <button
                        type="button"
                        className="btn btn-primary btn-sm"
                        onClick={() => handleLoadSlot(slot)}
                      >
                        <FolderOpen size={13} style={{ marginRight: '4px' }} />
                        Load
                      </button>
                      <button
                        type="button"
                        className="btn btn-ghost btn-sm"
                        style={{ padding: '5px', color: 'var(--text-muted)' }}
                        onClick={() => handleDeleteSlot(slot.id)}
                        aria-label="Delete save"
                      >
                        <Trash2 size={13} />
                      </button>
                    </div>
                  </div>
                ))}
              </div>
            )}
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end' }}>
          <button type="button" className="btn btn-secondary" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
};
