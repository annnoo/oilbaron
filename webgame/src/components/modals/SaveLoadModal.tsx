import React, { useState, useEffect } from 'react';
import { Save, FolderOpen, Download, Upload, Trash2, X, Check, Clock } from 'lucide-react';
import { playClick } from '../../assets/audio';
import type { GameState } from '../../engine/types';
import {
  saveToLocalStorage,
  loadFromLocalStorage,
  listLocalStorageSaveSlots,
  deleteLocalStorageSaveSlot,
  exportGameStateToJson,
  importGameStateFromJson,
  type SaveSlotMetadata,
} from '../../engine/persistence';
import { useTranslation } from '../../i18n';

export interface SaveLoadModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  onLoadGame: (state: GameState) => void;
  initialMode?: 'save' | 'load';
}

const PRESET_SLOTS = ['Slot 1', 'Slot 2', 'Slot 3', 'Slot 4', 'Slot 5'];

export const SaveLoadModal: React.FC<SaveLoadModalProps> = ({
  isOpen,
  onClose,
  gameState,
  onLoadGame,
  initialMode = 'save',
}) => {
  const { t } = useTranslation();
  const [activeTab, setActiveTab] = useState<'save' | 'load'>(initialMode);
  const [slots, setSlots] = useState<SaveSlotMetadata[]>(() => {
    try {
      return listLocalStorageSaveSlots();
    } catch {
      return [];
    }
  });
  const [slotName, setSlotName] = useState<string>('Slot 1');
  const [successMsg, setSuccessMsg] = useState<string | null>(null);
  const [errorMsg, setErrorMsg] = useState<string | null>(null);

  const refreshSlots = () => {
    try {
      const updated = listLocalStorageSaveSlots();
      setSlots(updated);
    } catch {
      setSlots([]);
    }
  };

  useEffect(() => {
    if (isOpen) {
      setActiveTab(initialMode);
      refreshSlots();
      setSuccessMsg(null);
      setErrorMsg(null);
    }
  }, [isOpen, initialMode]);

  if (!isOpen) return null;

  const handleSave = (targetSlot?: string) => {
    playClick();
    setErrorMsg(null);
    const chosenSlot = (targetSlot || slotName).trim() || 'Slot 1';
    const success = saveToLocalStorage(gameState, chosenSlot);
    if (success) {
      refreshSlots();
      setSuccessMsg(t('saveLoad.matchSavedSuccess', { name: chosenSlot }));
    } else {
      setErrorMsg('Failed to save game to browser storage.');
    }
  };

  const handleLoad = (slotKey: string) => {
    playClick();
    setErrorMsg(null);
    const loadedState = loadFromLocalStorage(slotKey);
    if (loadedState) {
      onLoadGame(loadedState);
      onClose();
    } else {
      setErrorMsg(`Could not load save "${slotKey}". Data may be corrupted.`);
    }
  };

  const handleDelete = (slotKey: string) => {
    playClick();
    deleteLocalStorageSaveSlot(slotKey);
    refreshSlots();
    setSuccessMsg(null);
  };

  const handleExportJson = () => {
    playClick();
    exportGameStateToJson(gameState);
  };

  const handleImportJson = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (event) => {
      try {
        const jsonContent = event.target?.result as string;
        const loadedState = importGameStateFromJson(jsonContent);
        playClick();
        onLoadGame(loadedState);
        onClose();
      } catch (err) {
        setErrorMsg('Invalid or corrupted Oil Baron JSON save file.');
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
            <span>{t('saveLoad.title')}</span>
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

        {/* Tab switchers */}
        <div style={{ display: 'flex', borderBottom: '1px solid var(--border-default)', padding: '0 16px' }}>
          <button
            type="button"
            className={`btn btn-ghost ${activeTab === 'save' ? 'active' : ''}`}
            style={{
              borderRadius: 0,
              borderBottom: activeTab === 'save' ? '2px solid var(--accent-gold)' : '2px solid transparent',
              fontWeight: activeTab === 'save' ? 700 : 400,
              padding: '10px 16px',
            }}
            onClick={() => {
              playClick();
              setActiveTab('save');
            }}
          >
            <Save size={14} style={{ marginRight: '6px' }} />
            {t('saveLoad.saveTab')}
          </button>
          <button
            type="button"
            className={`btn btn-ghost ${activeTab === 'load' ? 'active' : ''}`}
            style={{
              borderRadius: 0,
              borderBottom: activeTab === 'load' ? '2px solid var(--accent-gold)' : '2px solid transparent',
              fontWeight: activeTab === 'load' ? 700 : 400,
              padding: '10px 16px',
            }}
            onClick={() => {
              playClick();
              setActiveTab('load');
            }}
          >
            <FolderOpen size={14} style={{ marginRight: '6px' }} />
            {t('saveLoad.loadTab')}
          </button>
        </div>

        <div className="modal-body">
          {/* Messages */}
          {successMsg && (
            <div
              style={{
                display: 'flex',
                alignItems: 'center',
                gap: '6px',
                color: '#22c55e',
                fontSize: '12px',
                marginBottom: '12px',
                padding: '8px 12px',
                background: 'rgba(34, 197, 94, 0.1)',
                borderRadius: 'var(--radius-sm)',
              }}
            >
              <Check size={14} />
              <span>{successMsg}</span>
            </div>
          )}
          {errorMsg && (
            <div
              style={{
                display: 'flex',
                alignItems: 'center',
                gap: '6px',
                color: '#ef4444',
                fontSize: '12px',
                marginBottom: '12px',
                padding: '8px 12px',
                background: 'rgba(239, 68, 68, 0.1)',
                borderRadius: 'var(--radius-sm)',
              }}
            >
              <X size={14} />
              <span>{errorMsg}</span>
            </div>
          )}

          {/* Quick Preset Slot Buttons */}
          <div style={{ marginBottom: '16px' }}>
            <span className="form-label" style={{ fontSize: '11px', textTransform: 'uppercase' }}>
              Quick Slots
            </span>
            <div style={{ display: 'flex', gap: '6px', flexWrap: 'wrap' }}>
              {PRESET_SLOTS.map((preset) => {
                const isSelected = slotName === preset;
                const slotData = slots.find((s) => s.slot === preset);
                return (
                  <button
                    key={preset}
                    type="button"
                    className={`btn btn-sm ${isSelected ? 'btn-primary' : 'btn-secondary'}`}
                    style={{ flex: 1, minWidth: '70px', fontSize: '11px' }}
                    onClick={() => {
                      playClick();
                      setSlotName(preset);
                    }}
                    title={slotData ? `Saved on ${slotData.date}` : 'Empty slot'}
                  >
                    {preset}
                    {slotData ? ' *' : ''}
                  </button>
                );
              })}
            </div>
          </div>

          {/* Save Match Input */}
          <div style={{ marginBottom: '16px' }}>
            <span className="form-label">{t('saveLoad.saveCurrent')}</span>
            <div style={{ display: 'flex', gap: '8px' }}>
              <input
                type="text"
                className="form-input"
                style={{ flex: 1 }}
                placeholder={t('saveLoad.descPlaceholder')}
                value={slotName}
                onChange={(e) => setSlotName(e.target.value)}
              />
              <button
                type="button"
                className="btn btn-primary"
                onClick={() => handleSave()}
              >
                <Save size={14} style={{ marginRight: '6px' }} />
                {t('saveLoad.saveTab')}
              </button>
            </div>
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
              {t('saveLoad.exportJson')}
            </button>
            <label
              className="btn btn-secondary btn-sm"
              style={{ flex: 1, cursor: 'pointer', textAlign: 'center' }}
            >
              <Upload size={13} style={{ marginRight: '6px' }} />
              {t('saveLoad.importJson')}
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
            <span className="form-label">{t('saveLoad.slotsCount', { count: slots.length })}</span>
            {slots.length === 0 ? (
              <div
                style={{
                  padding: '24px',
                  textAlign: 'center',
                  color: 'var(--text-muted)',
                  background: 'var(--bg-surface-elevated)',
                  borderRadius: 'var(--radius-sm)',
                }}
              >
                {t('saveLoad.noSaves')}
              </div>
            ) : (
              <div
                style={{
                  display: 'flex',
                  flexDirection: 'column',
                  gap: '8px',
                  maxHeight: '220px',
                  overflowY: 'auto',
                }}
              >
                {slots.map((item) => (
                  <div
                    key={item.slot}
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
                        {item.slot}
                      </div>
                      <div
                        style={{
                          display: 'flex',
                          gap: '10px',
                          fontSize: '11px',
                          color: 'var(--text-muted)',
                          marginTop: '2px',
                        }}
                      >
                        {item.date && (
                          <span>
                            <Clock size={11} style={{ verticalAlign: 'middle', marginRight: '3px' }} />
                            {item.date.includes('T') ? new Date(item.date).toLocaleDateString() : item.date}
                          </span>
                        )}
                        <span>{t('saveLoad.playersCount', { count: item.players.length })}</span>
                        <span>{item.players.join(', ')}</span>
                      </div>
                    </div>

                    <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                      <button
                        type="button"
                        className="btn btn-primary btn-sm"
                        onClick={() => handleLoad(item.slot)}
                      >
                        <FolderOpen size={13} style={{ marginRight: '4px' }} />
                        {t('saveLoad.loadBtn')}
                      </button>
                      <button
                        type="button"
                        className="btn btn-ghost btn-sm"
                        style={{ padding: '5px', color: 'var(--text-muted)' }}
                        onClick={() => handleDelete(item.slot)}
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
            {t('common.close')}
          </button>
        </div>
      </div>
    </div>
  );
};
