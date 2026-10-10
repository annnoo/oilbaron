import React from 'react';
import {
  CLASSIC_COMPANY_BITMAPS,
  CLASSIC_CHIT_BITMAP,
  CLASSIC_NOT_PLAYABLE_BITMAP,
  CLASSIC_PLAYER_INFO_BITMAP,
  CLASSIC_DISCARD_SQUARE_BITMAP,
  CLASSIC_BLANK_BITMAP,
} from '../../assets/classicAssets';
import { ClassicWindow } from './ClassicWindow';
import { TurnGuidanceBar } from '../TurnGuidanceBar';
import { MenuBar, type MenuActionId } from '../MenuBar';
import { playClick } from '../../assets/audio';
import type { GameState, TileCoord } from '../../engine/types';
import { formatCoord, ROW_LABELS } from '../../engine/board';
import { calculateSharePrice, calculateNetWorth } from '../../engine/market';

export interface ClassicGameViewProps {
  gameState: GameState;
  selectedHandCoord: TileCoord | null;
  hoveredHandCoord: TileCoord | null;
  showHints: boolean;
  onToggleHints: (hints: boolean) => void;
  onSelectHandCoord: (coord: TileCoord | null) => void;
  onPlaceTile: (coord: TileCoord) => void;
  onDiscardDeadTile: (coord: TileCoord) => void;
  onPlayBlankTile?: (coord: TileCoord) => void;
  onMenuAction: (actionId: MenuActionId) => void;
  onSwitchToModern: () => void;
  onTogglePlayerAi: (playerIndex: number, isAi: boolean) => void;
  onOpenStockMarket: () => void;
  onEndTurn: () => void;
}

export const ClassicGameView: React.FC<ClassicGameViewProps> = ({
  gameState,
  selectedHandCoord,
  hoveredHandCoord,
  showHints,
  onToggleHints,
  onSelectHandCoord,
  onPlaceTile,
  onDiscardDeadTile,
  onPlayBlankTile,
  onMenuAction,
  onSwitchToModern,
  onTogglePlayerAi,
  onOpenStockMarket,
  onEndTurn,
}) => {
  const activePlayer = gameState.players[gameState.activePlayerIndex];
  const activePlayerIsAi = activePlayer?.isAi ?? false;

  const rows = gameState.board.length;
  const cols = gameState.board[0]?.length ?? 12;

  const turnSoldAllowance = gameState.turnSoldShares
    ? Object.values(gameState.turnSoldShares).reduce((acc, c) => acc + c, 0)
    : 0;

  return (
    <div className="win31-root">
      {/* Top Retro Switcher Toolbar */}
      <div
        style={{
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'space-between',
          marginBottom: '8px',
          padding: '4px 8px',
          backgroundColor: '#c0c0c0',
          border: '2px solid #808080',
        }}
      >
        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
          <img
            src={CLASSIC_PLAYER_INFO_BITMAP}
            alt="Oil Baron"
            style={{ width: '24px', height: '24px', imageRendering: 'pixelated' }}
          />
          <strong style={{ fontSize: '13px' }}>Oil Baron (1993 Windows 3.1 Classic Edition)</strong>
        </div>

        <button
          type="button"
          className="win31-btn"
          onClick={() => {
            playClick();
            onSwitchToModern();
          }}
          title="Switch to Modern Studio UI"
        >
          <span>Switch to Modern Studio UI ▶</span>
        </button>
      </div>

      {/* Main Window */}
      <ClassicWindow title={`OILBARON.EXE - [Petroleum Oil Board: ${rows}x${cols}]`}>
        {/* Top Menu Bar */}
        <MenuBar
          activePlayerIsAi={activePlayerIsAi}
          activePlayerName={activePlayer?.name ?? 'Player 1'}
          onAction={onMenuAction}
        />

        {/* Turn Guidance & Onboarding Bar */}
        <div style={{ padding: '8px' }}>
          <TurnGuidanceBar
            phase={gameState.phase}
            activePlayerName={activePlayer?.name ?? 'Player 1'}
            activePlayerColor={activePlayer?.color}
            isAi={activePlayerIsAi}
            selectedCoord={selectedHandCoord}
            cash={activePlayer?.cash}
            showHints={showHints}
            repurchaseAllowance={turnSoldAllowance}
            lastDividendPayout={gameState.lastDividendPayout}
            onToggleHints={onToggleHints}
            onPlaceSelected={() => {
              if (selectedHandCoord) {
                if (selectedHandCoord.isBlank) {
                  onPlayBlankTile?.(selectedHandCoord);
                } else {
                  onPlaceTile(selectedHandCoord);
                }
              }
            }}
            onOpenStockMarket={onOpenStockMarket}
            onEndTurn={onEndTurn}
          />

          {/* Company Ticker Bar in Classic 1993 Style */}
          <div
            className="win31-sunken-grey"
            style={{
              padding: '6px',
              display: 'flex',
              alignItems: 'center',
              gap: '12px',
              overflowX: 'auto',
              marginBottom: '8px',
            }}
          >
            {gameState.config.companies
              .filter((c) => !c.isDormant)
              .map((def) => {
                const comp = gameState.companies[def.id];
                const bmp = CLASSIC_COMPANY_BITMAPS[def.id];
                const price = comp ? calculateSharePrice(def, comp.size) : 0;

                return (
                  <div
                    key={def.id}
                    className="win31-window"
                    style={{
                      padding: '4px 8px',
                      display: 'flex',
                      alignItems: 'center',
                      gap: '6px',
                      minWidth: '110px',
                      cursor: 'pointer',
                    }}
                    onClick={() => {
                      playClick();
                      if (gameState.phase === 'buy_shares') {
                        onOpenStockMarket();
                      }
                    }}
                    title={
                      gameState.phase === 'buy_shares'
                        ? `Click to Buy/Sell shares in ${def.name}`
                        : `${def.name} • Size: ${comp?.size ?? 0} • $${price}`
                    }
                  >
                    {bmp && <img src={bmp} alt={def.name} className="win31-cell-bmp" />}
                    <div>
                      <div style={{ fontWeight: 'bold', fontSize: '11px' }}>{def.name}</div>
                      <div style={{ fontSize: '10px' }}>
                        Size: <strong>{comp?.size ?? 0}</strong> | ${price}
                      </div>
                    </div>
                  </div>
                );
              })}
          </div>

          {/* Layout: Board on Left, Scoreboard on Right */}
          <div style={{ display: 'flex', gap: '10px', alignItems: 'flex-start' }}>
            {/* Board */}
            <div
              className="win31-window"
              style={{ padding: '6px', maxWidth: '100%', overflow: 'auto' }}
              data-testid="classic-board"
            >
              <div
                className="win31-board-grid"
                style={{
                  gridTemplateColumns: `26px repeat(${cols}, 38px)`,
                  gridTemplateRows: `20px repeat(${rows}, 38px)`,
                }}
              >
                {/* Column Headers */}
                <div className="win31-coord-hdr"></div>
                {Array.from({ length: cols }, (_, c) => (
                  <div key={`col-${c}`} className="win31-coord-hdr">
                    {c + 1}
                  </div>
                ))}

                {/* Rows */}
                {Array.from({ length: rows }, (_, r) => (
                  <React.Fragment key={`row-${r}`}>
                    <div className="win31-coord-hdr">{ROW_LABELS[r]}</div>
                    {Array.from({ length: cols }, (_, c) => {
                      const coord: TileCoord = { row: r, col: c };
                      const cell = gameState.board[r]?.[c];
                      const isSelectedTarget =
                        selectedHandCoord &&
                        selectedHandCoord.row === r &&
                        selectedHandCoord.col === c;

                      const isInHand = activePlayer?.hand.some(
                        (t) => t.row === r && t.col === c
                      );

                      return (
                        <div
                          key={`cell-${r}-${c}`}
                          className={`win31-board-cell ${
                            isSelectedTarget ? 'is-target' : ''
                          }`}
                          onClick={() => {
                            if (isInHand) {
                              playClick();
                              onSelectHandCoord(coord);
                            } else if (selectedHandCoord) {
                              onPlaceTile(selectedHandCoord);
                            }
                          }}
                          title={`Tile ${formatCoord(coord)}`}
                        >
                          {cell?.type === 'company' && (
                            <img
                              src={CLASSIC_COMPANY_BITMAPS[cell.companyId]}
                              alt={cell.companyId}
                              className="win31-cell-bmp"
                            />
                          )}

                          {cell?.type === 'unincorporated' && (
                            <img
                              src={CLASSIC_CHIT_BITMAP}
                              alt="Oil Derrick"
                              className="win31-cell-bmp"
                            />
                          )}

                          {cell?.type === 'discarded' && (
                            <img
                              src={CLASSIC_DISCARD_SQUARE_BITMAP}
                              alt="Discarded X"
                              className="win31-cell-bmp"
                              data-testid="classic-discarded-marker"
                            />
                          )}

                          {cell?.type === 'empty' && (
                            <span className="win31-cell-label">
                              {ROW_LABELS[r]}{c + 1}
                            </span>
                          )}
                        </div>
                      );
                    })}
                  </React.Fragment>
                ))}
              </div>

              {/* Player Hand Tray */}
              <div style={{ marginTop: '8px' }}>
                <div style={{ fontSize: '11px', fontWeight: 'bold', marginBottom: '4px' }}>
                  {activePlayer?.name}'s Rack ({activePlayerIsAi ? 'CPU' : 'Human'}):
                </div>

                <div className="win31-hand-tray">
                  {activePlayer?.hand.map((tile, idx) => {
                    const isBlank = Boolean(tile.isBlank);
                    const isSelected =
                      selectedHandCoord &&
                      (isBlank && selectedHandCoord.isBlank
                        ? true
                        : selectedHandCoord.row === tile.row && selectedHandCoord.col === tile.col);

                    return (
                      <div
                        key={isBlank ? `hand-blank-${idx}` : `hand-${tile.row}-${tile.col}-${idx}`}
                        className={`win31-chit-card ${isSelected ? 'is-selected' : ''}`}
                        title={isBlank ? 'Wildcard Blank Piece: Click to place on any vacant square' : undefined}
                        onClick={() => {
                          playClick();
                          if (isBlank && onPlayBlankTile) {
                            onPlayBlankTile(tile);
                          }
                          onSelectHandCoord(isSelected ? null : tile);
                        }}
                      >
                        <img
                          src={isBlank ? CLASSIC_BLANK_BITMAP : CLASSIC_CHIT_BITMAP}
                          alt={isBlank ? 'Blank Piece' : 'Chit'}
                          style={{ width: '24px', height: '24px', imageRendering: 'pixelated' }}
                        />
                        <span style={{ fontSize: '11px', fontWeight: 'bold', marginTop: '2px' }}>
                          {isBlank ? 'BLANK' : formatCoord(tile)}
                        </span>
                      </div>
                    );
                  })}
                </div>
              </div>
            </div>

            {/* Scoreboard Sidebar Window */}
            <div className="win31-window" style={{ minWidth: '220px', padding: '6px' }}>
              <div className="win31-titlebar" style={{ marginBottom: '6px' }}>
                <span>Player Standings</span>
              </div>

              <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
                {gameState.players.map((p, idx) => {
                  const isActive = idx === gameState.activePlayerIndex;
                  const nw = calculateNetWorth(p, gameState.companies, gameState.config.companies);

                  return (
                    <div
                      key={p.id}
                      className="win31-sunken-grey"
                      style={{
                        padding: '6px',
                        border: isActive ? '2px solid #000080' : '1px solid #808080',
                      }}
                    >
                      <div style={{ display: 'flex', justifyContent: 'space-between', fontWeight: 'bold' }}>
                        <span>
                          {isActive && '▶ '}{p.name}
                        </span>
                        <span>{p.isAi ? '[CPU]' : '[Human]'}</span>
                      </div>
                      <div style={{ fontSize: '10px', marginTop: '3px' }}>
                        Cash: <strong>${p.cash.toLocaleString()}</strong> | Net Worth: <strong>${nw.toLocaleString()}</strong>
                      </div>
                      <div style={{ marginTop: '4px' }}>
                        <button
                          type="button"
                          className="win31-btn"
                          style={{ fontSize: '9px', padding: '2px 6px' }}
                          onClick={() => onTogglePlayerAi(idx, !p.isAi)}
                        >
                          Change to {p.isAi ? 'Human' : 'CPU'}
                        </button>
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          </div>
        </div>
      </ClassicWindow>
    </div>
  );
};
