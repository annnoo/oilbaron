import React, { useState, useMemo } from 'react';
import { HelpCircle, AlertCircle, CheckCircle, Sparkles, X } from 'lucide-react';
import { playClick } from '../../assets/audio';
import type { GameState, TileCoord, CompanyId } from '../../engine/types';
import {
  classifyMove,
  formatCoord,
  isTileDead,
  ROW_LABELS,
  type MoveClassification,
} from '../../engine/board';
import { useTranslation } from '../../i18n';

export interface BlankPieceModalProps {
  isOpen: boolean;
  onClose: () => void;
  gameState: GameState;
  blankTileCoord: TileCoord;
  initialCoord?: TileCoord;
  onConfirmPlacement: (blankTileCoord: TileCoord, targetCoord: TileCoord) => void;
}

/**
 * Parses user input into a TileCoord within board dimensions.
 * Accepts formats like: "A12", "A-12", "a12", "a-12", "C5", "c-5".
 */
function parseCoordinate(
  text: string,
  rows: number,
  cols: number
): TileCoord | null {
  const trimmed = text.trim().toUpperCase();
  const match = trimmed.match(/^([A-Z])-?(\d{1,2})$/);
  if (!match) return null;
  const row = match[1]!.charCodeAt(0) - 65;
  const col = parseInt(match[2]!, 10) - 1;
  if (row < 0 || row >= rows || col < 0 || col >= cols) return null;
  return { row, col };
}

export const BlankPieceModal: React.FC<BlankPieceModalProps> = ({
  isOpen,
  onClose,
  gameState,
  blankTileCoord,
  initialCoord,
  onConfirmPlacement,
}) => {
  const { t, language } = useTranslation();

  const rows = gameState.board.length;
  const cols = gameState.board[0]?.length ?? 12;

  // Find first vacant cell on board as initial selection if none provided
  const defaultCoord = useMemo(() => {
    if (initialCoord) return initialCoord;
    for (let r = 0; r < rows; r++) {
      for (let c = 0; c < cols; c++) {
        if (gameState.board[r]?.[c]?.type === 'empty') {
          return { row: r, col: c };
        }
      }
    }
    return { row: 0, col: 0 };
  }, [initialCoord, rows, cols, gameState.board]);

  const [inputVal, setInputVal] = useState<string>(() => {
    const rLabel = ROW_LABELS[defaultCoord.row] ?? String.fromCharCode(65 + defaultCoord.row);
    return `${rLabel}${defaultCoord.col + 1}`;
  });

  const [selectedCoord, setSelectedCoord] = useState<TileCoord>(defaultCoord);

  if (!isOpen) return null;

  const rowLetters = Array.from({ length: rows }, (_, idx) =>
    ROW_LABELS[idx] ?? String.fromCharCode(65 + idx)
  );
  const colNumbers = Array.from({ length: cols }, (_, idx) => idx + 1);

  // Validate current coordinate
  const cell = gameState.board[selectedCoord.row]?.[selectedCoord.col];
  const isOccupied = !cell || cell.type !== 'empty';
  const isDead = isTileDead(
    gameState.board,
    selectedCoord,
    gameState.companies,
    gameState.config.safeThreshold
  );

  let classification: MoveClassification | null = null;
  if (!isOccupied) {
    classification = classifyMove(
      gameState.board,
      selectedCoord,
      gameState.companies,
      gameState.config.safeThreshold
    );
  }

  const isValid = !isOccupied && !isDead && classification?.type !== 'illegal_safe_merger';

  const handleTextChange = (val: string) => {
    setInputVal(val);
    const parsed = parseCoordinate(val, rows, cols);
    if (parsed) {
      setSelectedCoord(parsed);
    }
  };

  const handleRowChange = (rIdx: number) => {
    const next: TileCoord = { row: rIdx, col: selectedCoord.col };
    setSelectedCoord(next);
    const rLabel = ROW_LABELS[next.row] ?? String.fromCharCode(65 + next.row);
    setInputVal(`${rLabel}${next.col + 1}`);
  };

  const handleColChange = (cNum: number) => {
    const next: TileCoord = { row: selectedCoord.row, col: cNum - 1 };
    setSelectedCoord(next);
    const rLabel = ROW_LABELS[next.row] ?? String.fromCharCode(65 + next.row);
    setInputVal(`${rLabel}${next.col + 1}`);
  };

  const handleConfirm = () => {
    if (!isValid) return;
    playClick();
    onConfirmPlacement(blankTileCoord, selectedCoord);
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
        className="modal-dialog modal-sm"
        role="dialog"
        aria-modal="true"
        aria-labelledby="blank-piece-title"
        style={{ maxWidth: '420px' }}
      >
        <div className="modal-header">
          <div className="modal-title" id="blank-piece-title">
            <Sparkles size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Move Blank: Keyboard</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label={t('common.close') || 'Close'}
          >
            <X size={16} />
          </button>
        </div>

        <div className="modal-body" style={{ padding: '16px' }}>
          <p
            style={{
              fontSize: '13px',
              fontWeight: 600,
              color: 'var(--text-primary)',
              marginBottom: '4px',
              textAlign: 'center',
            }}
          >
            Please Choose Square to Place Blank:
          </p>
          <p
            style={{
              fontSize: '11px',
              color: 'var(--text-secondary)',
              marginBottom: '16px',
              textAlign: 'center',
            }}
          >
            Format Xn (example A12)
          </p>

          {/* Coordinate input & selectors row */}
          <div
            style={{
              display: 'flex',
              gap: '8px',
              justifyContent: 'center',
              alignItems: 'center',
              marginBottom: '16px',
            }}
          >
            <div>
              <label
                htmlFor="blank-coord-input"
                style={{ fontSize: '10px', display: 'block', color: 'var(--text-secondary)' }}
              >
                Square:
              </label>
              <input
                id="blank-coord-input"
                data-testid="coord-input"
                type="text"
                className="input input-sm"
                value={inputVal}
                onChange={(e) => handleTextChange(e.target.value)}
                onKeyDown={(e) => {
                  if (e.key === 'Enter' && isValid) {
                    handleConfirm();
                  }
                }}
                maxLength={4}
                style={{
                  width: '70px',
                  textAlign: 'center',
                  fontWeight: 700,
                  fontSize: '14px',
                  textTransform: 'uppercase',
                }}
              />
            </div>

            <div>
              <label
                htmlFor="blank-row-select"
                style={{ fontSize: '10px', display: 'block', color: 'var(--text-secondary)' }}
              >
                Row:
              </label>
              <select
                id="blank-row-select"
                data-testid="row-select"
                className="input input-sm"
                value={selectedCoord.row}
                onChange={(e) => handleRowChange(parseInt(e.target.value, 10))}
                style={{ width: '60px' }}
              >
                {rowLetters.map((letter, idx) => (
                  <option key={letter} value={idx}>
                    {letter}
                  </option>
                ))}
              </select>
            </div>

            <div>
              <label
                htmlFor="blank-col-select"
                style={{ fontSize: '10px', display: 'block', color: 'var(--text-secondary)' }}
              >
                Col:
              </label>
              <select
                id="blank-col-select"
                data-testid="col-select"
                className="input input-sm"
                value={selectedCoord.col + 1}
                onChange={(e) => handleColChange(parseInt(e.target.value, 10))}
                style={{ width: '60px' }}
              >
                {colNumbers.map((num) => (
                  <option key={num} value={num}>
                    {num}
                  </option>
                ))}
              </select>
            </div>
          </div>

          {/* Placement outcome preview banner */}
          <div
            className="win31-sunken-grey"
            style={{
              padding: '10px',
              borderRadius: '4px',
              backgroundColor: isDead || isOccupied ? 'rgba(239, 68, 68, 0.1)' : 'var(--bg-secondary)',
              border: isDead || isOccupied ? '1px solid #ef4444' : '1px solid var(--border-color)',
            }}
          >
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
              {isOccupied ? (
                <>
                  <AlertCircle size={16} color="#ef4444" />
                  <span style={{ fontSize: '12px', color: '#ef4444', fontWeight: 600 }}>
                    Cell {formatCoord(selectedCoord)} is already occupied
                  </span>
                </>
              ) : isDead ? (
                <>
                  <AlertCircle size={16} color="#ef4444" />
                  <span style={{ fontSize: '12px', color: '#ef4444', fontWeight: 600 }}>
                    Illegal Move: Merges 2+ safe corporations
                  </span>
                </>
              ) : (
                <>
                  <CheckCircle size={16} color="var(--accent-green, #22c55e)" />
                  <span style={{ fontSize: '12px', color: 'var(--text-primary)', fontWeight: 600 }}>
                    Target: {formatCoord(selectedCoord)} —{' '}
                    {classification?.type === 'isolated'
                      ? 'Wildcat Chit (Unincorporated)'
                      : classification?.type === 'founding'
                      ? 'Founds a New Corporation'
                      : classification?.type === 'expansion'
                      ? `Expands ${gameState.companies[classification.companyId]?.name ?? classification.companyId}`
                      : classification?.type === 'merger'
                      ? `Merger between ${classification.participatingCompanyIds.map((id) => gameState.companies[id]?.name ?? id).join(' & ')}`
                      : 'Valid cell'}
                  </span>
                </>
              )}
            </div>
          </div>
        </div>

        <div className="modal-footer" style={{ justifyContent: 'center', gap: '16px' }}>
          <button
            type="button"
            className="btn btn-primary"
            style={{ minWidth: '80px' }}
            disabled={!isValid}
            onClick={handleConfirm}
          >
            OK
          </button>
          <button
            type="button"
            className="btn btn-secondary"
            style={{ minWidth: '80px' }}
            onClick={onClose}
          >
            Cancel
          </button>
        </div>
      </div>
    </div>
  );
};

export default BlankPieceModal;
