import React, { useState, useEffect, useCallback } from 'react';
import {
  soundEffects,
  playClick,
  playFoundCompany,
  playMergerAlert,
  playCashChime,
  playVictoryFanfare,
} from './assets/audio';
import { DEFAULT_CONFIG, createInitialGameState } from './engine/config';
import type {
  CompanyId,
  GameAction,
  GameConfig,
  GameState,
  ShareDispositionChoices,
  SharePurchase,
  ShareSale,
  TileCoord,
} from './engine/types';
import { formatCoord } from './engine/board';
import { gameReducer } from './engine/reducer';
import { simulateAiTurn, getBestMergerDisposition } from './engine/ai';
import { useTranslation } from './i18n';

// Core UI Components
import type { MenuActionId } from './components/MenuBar';
import { GameHeader } from './components/GameHeader';
import { TurnAdvisor } from './components/TurnAdvisor';
import { ActionDock } from './components/ActionDock';
import { LedgerDrawer } from './components/LedgerDrawer';
import { CompanyTicker } from './components/CompanyTicker';
import { GameBoard } from './components/GameBoard';
import { PlayerScoreboard } from './components/PlayerScoreboard';
import { ClassicGameView } from './components/classic/ClassicGameView';

// Modal Dialogs
import { NewGameModal, type PlayerConfig } from './components/modals/NewGameModal';
import { OptionsModal } from './components/modals/OptionsModal';
import { FoundingCompanyModal } from './components/modals/FoundingCompanyModal';
import { DiscardedTilesModal } from './components/modals/DiscardedTilesModal';
import { StockMarketModal } from './components/modals/StockMarketModal';
import { CompanyStatsModal } from './components/modals/CompanyStatsModal';
import { MergerSurvivorModal } from './components/modals/MergerSurvivorModal';
import { MergerStatsModal } from './components/modals/MergerStatsModal';
import { ShareDispositionModal } from './components/modals/ShareDispositionModal';
import { WealthChartModal } from './components/modals/WealthChartModal';
import { ShareGraphModal } from './components/modals/ShareGraphModal';
import { WarRoomModal } from './components/modals/WarRoomModal';
import { SaveLoadModal } from './components/modals/SaveLoadModal';
import { HelpModal } from './components/modals/HelpModal';
import { GameOverModal } from './components/modals/GameOverModal';
import { BlankPieceModal } from './components/modals/BlankPieceModal';

export interface AppProps {
  initialViewMode?: 'modern' | 'classic';
}

export const App: React.FC<AppProps> = ({ initialViewMode }) => {
  const { t, language, setLanguage } = useTranslation();

  // Config & Audio State
  const [config, setConfig] = useState<GameConfig>(DEFAULT_CONFIG);
  const [muted, setMuted] = useState<boolean>(soundEffects.isMuted());
  const [slowCpuTurns, setSlowCpuTurns] = useState<boolean>(false);
  const [hideChits, setHideChits] = useState<boolean>(false);
  const [lastAction, setLastAction] = useState<string | null>(null);

  // Board & Hand Interactions
  const [selectedHandCoord, setSelectedHandCoord] = useState<TileCoord | null>(null);
  const [hoveredHandCoord, setHoveredHandCoord] = useState<TileCoord | null>(null);

  // Modals Visibility State
  const [newGameModalOpen, setNewGameModalOpen] = useState<boolean>(false);
  const [optionsModalOpen, setOptionsModalOpen] = useState<boolean>(false);
  const [stockMarketModalOpen, setStockMarketModalOpen] = useState<boolean>(false);
  const [discardedTilesModalOpen, setDiscardedTilesModalOpen] = useState<boolean>(false);
  const [wealthChartModalOpen, setWealthChartModalOpen] = useState<boolean>(false);
  const [shareGraphModalOpen, setShareGraphModalOpen] = useState<boolean>(false);
  const [companyStatsModalOpen, setCompanyStatsModalOpen] = useState<boolean>(false);
  const [warRoomModalOpen, setWarRoomModalOpen] = useState<boolean>(false);
  const [saveLoadModalOpen, setSaveLoadModalOpen] = useState<boolean>(false);
  const [saveLoadModalMode, setSaveLoadModalMode] = useState<'save' | 'load'>('save');
  const [helpModalOpen, setHelpModalOpen] = useState<boolean>(false);
  const [ledgerDrawerOpen, setLedgerDrawerOpen] = useState<boolean>(false);
  const [mergerStatsDismissed, setMergerStatsDismissed] = useState<boolean>(false);
  const [blankPieceModalOpen, setBlankPieceModalOpen] = useState<boolean>(false);
  const [activeBlankCoord, setActiveBlankCoord] = useState<TileCoord | null>(null);

  // View Mode & Turn Guidance Coaching
  const [viewMode, setViewMode] = useState<'modern' | 'classic'>(() => {
    if (initialViewMode) return initialViewMode;
    if (typeof window !== 'undefined') {
      if (
        window.location.hash === '#classic' ||
        window.location.pathname.endsWith('/classic')
      ) {
        return 'classic';
      }
    }
    return 'modern';
  });
  const [showBeginnerHints, setShowBeginnerHints] = useState<boolean>(true);

  // Sync viewMode with URL hash
  useEffect(() => {
    if (typeof window === 'undefined') return;
    if (viewMode === 'classic') {
      if (window.location.hash !== '#classic') {
        window.location.hash = 'classic';
      }
    } else {
      if (window.location.hash === '#classic') {
        window.history.replaceState(null, '', window.location.pathname + window.location.search);
      }
    }
  }, [viewMode]);

  // Handle external hash changes (e.g. browser back/forward)
  useEffect(() => {
    if (typeof window === 'undefined') return;
    const handleHashChange = () => {
      if (
        window.location.hash === '#classic' ||
        window.location.pathname.endsWith('/classic')
      ) {
        setViewMode('classic');
      } else {
        setViewMode('modern');
      }
    };
    window.addEventListener('hashchange', handleHashChange);
    return () => window.removeEventListener('hashchange', handleHashChange);
  }, []);

  // Initialize Game State
  const [gameState, setGameState] = useState<GameState>(() =>
    createInitialGameState(DEFAULT_CONFIG, [
      { name: 'Player 1', isAi: false },
      { name: 'Computer 1 (CPU)', isAi: true },
    ])
  );

  const activePlayer = gameState.players[gameState.activePlayerIndex];
  const activePlayerIsAi = activePlayer?.isAi ?? false;

  // Single dispatch reducer wrapper
  const dispatch = useCallback((action: GameAction) => {
    setGameState((prev) => {
      try {
        const next = gameReducer(prev, action);
        return next;
      } catch (err: any) {
        console.error('Dispatch error:', err);
        setLastAction(`Action Error: ${err.message}`);
        return prev;
      }
    });
  }, []);

  // CPU Turn Execution Loop
  useEffect(() => {
    if (gameState.phase === 'game_over') return;

    // 1. If currently in dispose_merger_shares, check if current shareholder is CPU
    if (gameState.phase === 'dispose_merger_shares' && gameState.pendingMerger) {
      const pm = gameState.pendingMerger;
      const shareholderId = pm.shareholderQueue[pm.currentShareholderIndex];
      const shareholder = gameState.players.find((p) => p.id === shareholderId);
      const shareholderIdx = gameState.players.findIndex((p) => p.id === shareholderId);

      if (shareholder?.isAi && pm.currentDefunctCompanyId && shareholderIdx !== -1) {
        const timer = setTimeout(() => {
          try {
            const choices = getBestMergerDisposition(
              gameState,
              shareholderIdx,
              pm.currentDefunctCompanyId!
            );
            dispatch({ type: 'DISPOSE_MERGER_SHARES', choices });
            setLastAction(`${shareholder.name} (CPU) disposed merger shares`);
          } catch (err: any) {
            console.error('AI merger disposition error:', err);
          }
        }, slowCpuTurns ? 700 : 200);
        return () => clearTimeout(timer);
      }
      return;
    }

    // 2. Main active player CPU turn loop
    if (activePlayerIsAi) {
      const delayMs = slowCpuTurns ? 800 : 220;
      const timer = setTimeout(() => {
        try {
          const nextState = simulateAiTurn(gameState);
          setGameState(nextState);
          setLastAction(`${activePlayer?.name} (CPU) executed turn actions`);
        } catch (err: any) {
          console.error('AI turn simulation error:', err);
        }
      }, delayMs);

      return () => clearTimeout(timer);
    }
  }, [gameState, activePlayerIsAi, slowCpuTurns, dispatch]);

  // Audio cues on phase transitions
  useEffect(() => {
    if (gameState.phase === 'choose_founding_company') {
      playFoundCompany();
    } else if (
      gameState.phase === 'choose_merger_survivor' ||
      gameState.phase === 'dispose_merger_shares'
    ) {
      playMergerAlert();
    } else if (gameState.phase === 'game_over') {
      playVictoryFanfare();
    }
  }, [gameState.phase]);

  // Reset merger stats dismissed status on new merger
  useEffect(() => {
    if (gameState.phase !== 'dispose_merger_shares') {
      setMergerStatsDismissed(false);
    }
  }, [gameState.phase]);

  // Sound toggle handler
  const handleToggleMute = () => {
    const nextMuted = soundEffects.toggleMute();
    setMuted(nextMuted);
    setLastAction(`Audio: ${nextMuted ? 'Muted' : 'Enabled'}`);
  };

  // Top Menu Bar Dispatcher
  const handleMenuAction = (actionId: MenuActionId) => {
    switch (actionId) {
      case 'new_game_same': {
        playClick();
        const roster = gameState.players.map((p) => ({
          name: p.name,
          isAi: p.isAi,
          color: p.color,
        }));
        setGameState(createInitialGameState(config, roster));
        setSelectedHandCoord(null);
        setLastAction('Started New Match with current player roster');
        break;
      }
      case 'new_game_new':
        setNewGameModalOpen(true);
        break;
      case 'open_options':
        setOptionsModalOpen(true);
        break;
      case 'save_game':
        setSaveLoadModalMode('save');
        setSaveLoadModalOpen(true);
        break;
      case 'load_game':
        setSaveLoadModalMode('load');
        setSaveLoadModalOpen(true);
        break;
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
      case 'toggle_player_ai': {
        const nextAi = !activePlayerIsAi;
        setGameState((prev) => ({
          ...prev,
          players: prev.players.map((p, idx) =>
            idx === prev.activePlayerIndex ? { ...p, isAi: nextAi } : p
          ),
        }));
        setLastAction(`Active player type changed to ${nextAi ? 'CPU' : 'Human'}`);
        break;
      }
      case 'buy_shares':
        setStockMarketModalOpen(true);
        break;
      case 'place_chit':
        if (selectedHandCoord) {
          handlePlaceTile(selectedHandCoord);
        } else {
          setLastAction('Select a chit from your hand tray first, then click Place Chit');
        }
        break;
      case 'discard_chit':
        if (selectedHandCoord) {
          handleDiscardDeadTile(selectedHandCoord);
        } else {
          setLastAction('Select a dead chit from your hand tray first');
        }
        break;
      case 'end_turn':
        if (gameState.phase === 'buy_shares') {
          dispatch({ type: 'END_TURN' });
          playClick();
          setLastAction('Ended turn and passed share purchases');
        } else {
          setLastAction('Cannot end turn: Must place a tile first');
        }
        break;
      case 'view_wealth':
        setWealthChartModalOpen(true);
        break;
      case 'view_shares':
        setShareGraphModalOpen(true);
        break;
      case 'view_companies':
        setCompanyStatsModalOpen(true);
        break;
      case 'view_warroom':
        setWarRoomModalOpen(true);
        break;
      case 'view_discarded':
        setDiscardedTilesModalOpen(true);
        break;
      case 'view_classic':
        setViewMode('classic');
        setLastAction('Switched to 1993 Windows 3.1 Classic View');
        break;
      case 'help_rules':
      case 'help_about':
        setHelpModalOpen(true);
        break;
      default:
        setLastAction(`Action: ${actionId}`);
        break;
    }
  };

  // Global Keyboard Shortcuts
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      // Don't trigger shortcuts inside text inputs
      if (
        e.target instanceof HTMLInputElement ||
        e.target instanceof HTMLTextAreaElement ||
        e.target instanceof HTMLSelectElement
      ) {
        return;
      }

      if (e.key === 'Escape') {
        if (ledgerDrawerOpen) {
          setLedgerDrawerOpen(false);
        }
      }

      if (e.altKey) {
        const key = e.key.toLowerCase();
        if (key === 'l') {
          e.preventDefault();
          setLedgerDrawerOpen((prev) => !prev);
        } else if (key === 'b') {
          e.preventDefault();
          setStockMarketModalOpen(true);
        } else if (key === 'p') {
          e.preventDefault();
          if (selectedHandCoord) handlePlaceTile(selectedHandCoord);
        } else if (key === 'd') {
          e.preventDefault();
          if (selectedHandCoord) handleDiscardDeadTile(selectedHandCoord);
        } else if (key === 'e') {
          e.preventDefault();
          if (gameState.phase === 'buy_shares') {
            dispatch({ type: 'END_TURN' });
            playClick();
            setLastAction('Ended turn and passed share purchases');
          }
        } else if (key === 'n') {
          e.preventDefault();
          setWealthChartModalOpen(true);
        } else if (key === 's') {
          e.preventDefault();
          setShareGraphModalOpen(true);
        } else if (key === 'c') {
          e.preventDefault();
          setCompanyStatsModalOpen(true);
        } else if (key === 'w') {
          e.preventDefault();
          setWarRoomModalOpen(true);
        }
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [selectedHandCoord, gameState.phase, ledgerDrawerOpen, dispatch]);

  // Wildcard Blank Piece Handlers
  const handleOpenBlankModal = (blankCoord: TileCoord) => {
    setActiveBlankCoord(blankCoord);
    setBlankPieceModalOpen(true);
  };

  const handleConfirmBlankPlacement = (blankTileCoord: TileCoord, targetCoord: TileCoord) => {
    try {
      dispatch({ type: 'PLACE_BLANK_TILE', blankTileCoord, targetCoord });
      setBlankPieceModalOpen(false);
      setActiveBlankCoord(null);
      setSelectedHandCoord(null);
      playClick();
      setLastAction(`Placed wildcard blank piece at ${formatCoord(targetCoord)}`);
    } catch (err: any) {
      setLastAction(`Blank placement error: ${err.message}`);
    }
  };

  // Tile Placement Handler
  const handlePlaceTile = (coord: TileCoord) => {
    if (coord.isBlank) {
      handleOpenBlankModal(coord);
      return;
    }
    try {
      dispatch({ type: 'PLACE_TILE', coord });
      setSelectedHandCoord(null);
      playClick();
      setLastAction(`Placed tile at ${formatCoord(coord)}`);
    } catch (err: any) {
      setLastAction(`Placement error: ${err.message}`);
    }
  };

  // Discard Dead Tile Handler
  const handleDiscardDeadTile = (coord: TileCoord) => {
    try {
      dispatch({ type: 'DISCARD_DEAD_TILE', coord });
      setSelectedHandCoord(null);
      playClick();
      setLastAction(`Discarded dead tile at ${formatCoord(coord)}`);
    } catch (err: any) {
      setLastAction(`Discard error: ${err.message}`);
    }
  };

  // Inline Player AI/Human Toggle
  const handleTogglePlayerAi = (playerIndex: number, nextIsAi: boolean) => {
    setGameState((prev) => {
      const nextPlayers = prev.players.map((p, idx) =>
        idx === playerIndex ? { ...p, isAi: nextIsAi } : p
      );
      return { ...prev, players: nextPlayers };
    });
    const pName = gameState.players[playerIndex]?.name ?? `Player ${playerIndex + 1}`;
    setLastAction(`${pName} type changed to ${nextIsAi ? 'CPU' : 'Human'}`);
  };

  // New Game Setup Confirmation
  const handleStartCustomGame = (
    players: PlayerConfig[],
    options?: Partial<Pick<GameConfig, 'boardRows' | 'boardCols' | 'enableEighthCompany' | 'randomSharePrices' | 'randomDividends'>>
  ) => {
    const effectiveGameConfig: GameConfig = options
      ? { ...config, ...options }
      : config;
    if (options) {
      setConfig(effectiveGameConfig);
    }
    const nextState = createInitialGameState(effectiveGameConfig, players);
    setGameState(nextState);
    setSelectedHandCoord(null);
    setNewGameModalOpen(false);
    setLastAction(`Started new match with ${players.length} players!`);
  };

  // Options Applied
  const handleSaveOptions = (newConfig: GameConfig, slowCpu: boolean) => {
    setConfig(newConfig);
    setSlowCpuTurns(slowCpu);
    setGameState((prev) => ({
      ...prev,
      config: newConfig,
    }));
    setLastAction('Applied updated game ruleset and options');
  };

  // Stock Market Purchases Confirmed
  const handleConfirmPurchases = (purchases: SharePurchase[]) => {
    if (purchases.length > 0) {
      dispatch({ type: 'BUY_SHARES', purchases });
      setLastAction(`Purchased ${purchases.reduce((acc, p) => acc + p.count, 0)} corporate shares`);
    }
    dispatch({ type: 'END_TURN' });
    setStockMarketModalOpen(false);
  };

  // Stock Market Sales Confirmed
  const handleConfirmSales = (sales: ShareSale[]) => {
    if (sales.length > 0) {
      dispatch({ type: 'SELL_SHARES', sales });
      const totalCount = sales.reduce((acc, s) => acc + s.count, 0);
      setLastAction(`Liquidated ${totalCount} shares across ${sales.length} companies for cash`);
    }
  };

  const handleSellAllShares = () => {
    dispatch({ type: 'SELL_ALL_SHARES' });
    setLastAction('Liquidated all active stock holdings for immediate cash');
  };

  // Merger Survivor Selected
  const handleSelectMergerSurvivor = (survivorCompanyId: CompanyId) => {
    dispatch({ type: 'CHOOSE_MERGER_SURVIVOR', survivorCompanyId });
    setLastAction(`Selected ${survivorCompanyId.toUpperCase()} as merger survivor`);
  };

  // Merger Share Disposition Confirmed
  const handleConfirmDisposition = (choices: ShareDispositionChoices) => {
    dispatch({ type: 'DISPOSE_MERGER_SHARES', choices });
    setLastAction(`Merger shares disposition confirmed`);
  };

  // Founding Company Selected
  const handleSelectFoundedCompany = (companyId: CompanyId) => {
    dispatch({ type: 'CHOOSE_FOUNDED_COMPANY', companyId });
    setLastAction(`Founded ${companyId.toUpperCase()}! Founder bonus awarded.`);
  };

  // Current Shareholder for Disposition Modal
  const pendingMerger = gameState.pendingMerger;
  const currentShareholderId =
    pendingMerger?.shareholderQueue[pendingMerger?.currentShareholderIndex ?? 0];
  const currentShareholder = gameState.players.find((p) => p.id === currentShareholderId);

  // Available dormant companies for founding
  const availableCompanyIds = gameState.config.companies
    .filter((c) => !c.isDormant && !gameState.companies[c.id]?.isActive)
    .map((c) => c.id);

  // Tied companies for merger survivor selection
  const tiedSurvivorIds = pendingMerger
    ? pendingMerger.participatingCompanyIds.filter(
        (id) =>
          gameState.companies[id]?.size ===
          Math.max(
            ...pendingMerger.participatingCompanyIds.map(
              (cid) => gameState.companies[cid]?.size ?? 0
            )
          )
      )
    : [];

  return (
    <div className={`app-container ${viewMode === 'classic' ? 'classic-mode' : ''}`}>
      {viewMode === 'classic' ? (
        <ClassicGameView
          gameState={gameState}
          selectedHandCoord={selectedHandCoord}
          hoveredHandCoord={hoveredHandCoord}
          showHints={showBeginnerHints}
          onToggleHints={setShowBeginnerHints}
          onSelectHandCoord={setSelectedHandCoord}
          onPlaceTile={handlePlaceTile}
          onDiscardDeadTile={handleDiscardDeadTile}
          onPlayBlankTile={handleOpenBlankModal}
          onMenuAction={handleMenuAction}
          onSwitchToModern={() => setViewMode('modern')}
          onTogglePlayerAi={handleTogglePlayerAi}
          onOpenStockMarket={() => setStockMarketModalOpen(true)}
          onEndTurn={() => {
            if (gameState.phase === 'buy_shares') {
              dispatch({ type: 'END_TURN' });
              playClick();
              setLastAction('Ended turn and passed share purchases');
            }
          }}
        />
      ) : (
        <>
          {/* Modern Strategy Game Header */}
          <GameHeader
            activePlayerName={activePlayer?.name ?? 'Player 1'}
            activePlayerColor={activePlayer?.color}
            isAi={activePlayerIsAi}
            turnPhase={gameState.phase}
            soundMuted={muted}
            language={language as 'en' | 'de'}
            onToggleSound={handleToggleMute}
            onToggleLanguage={() => setLanguage(language === 'en' ? 'de' : 'en')}
            onToggleLedger={() => setLedgerDrawerOpen((prev) => !prev)}
            onOpenRules={() => setHelpModalOpen(true)}
            onOpenNewGame={() => setNewGameModalOpen(true)}
            onSwitchClassic={() => setViewMode('classic')}
          />

          {/* Live Company Stock Ticker */}
          <CompanyTicker
            companies={gameState.companies}
            definitions={gameState.config.companies}
            safeThreshold={gameState.config.safeThreshold}
            totalShares={gameState.config.totalSharesPerCompany}
            onSelectCompany={() => {
              if (gameState.phase === 'buy_shares') {
                setStockMarketModalOpen(true);
              } else {
                setCompanyStatsModalOpen(true);
              }
            }}
          />

          {/* Main Workspace Layout */}
          <main className="game-main">
            <div className="game-workspace-layout">
              <div className="game-main-content">
                <TurnAdvisor
                  phase={gameState.phase}
                  isAi={activePlayerIsAi}
                  activePlayerName={activePlayer?.name ?? 'Player 1'}
                  selectedCoord={selectedHandCoord}
                  hoveredCoord={hoveredHandCoord}
                  board={gameState.board}
                  companies={gameState.companies}
                  definitions={gameState.config.companies}
                  safeThreshold={gameState.config.safeThreshold}
                />

                <GameBoard
                  board={gameState.board}
                  companies={gameState.companies}
                  definitions={gameState.config.companies}
                  safeThreshold={gameState.config.safeThreshold}
                  selectedTileCoord={selectedHandCoord}
                  previewTileCoord={hoveredHandCoord}
                  playerHand={gameState.players[gameState.activePlayerIndex]?.hand}
                  onPlaceTile={handlePlaceTile}
                />

                <ActionDock
                  phase={gameState.phase}
                  isCurrentTurn={!activePlayerIsAi}
                  hand={gameState.players[gameState.activePlayerIndex]?.hand ?? []}
                  selectedCoord={selectedHandCoord}
                  hoveredCoord={hoveredHandCoord}
                  activePlayerCash={activePlayer?.cash ?? 0}
                  activePlayerName={activePlayer?.name}
                  board={gameState.board}
                  companies={gameState.companies}
                  definitions={gameState.config.companies}
                  safeThreshold={gameState.config.safeThreshold}
                  hideChits={hideChits}
                  onSelectTile={setSelectedHandCoord}
                  onHoverTile={setHoveredHandCoord}
                  onPlaceTile={handlePlaceTile}
                  onDiscardDeadTile={handleDiscardDeadTile}
                  onConfirmPurchases={handleConfirmPurchases}
                  onPassTurn={() => {
                    if (gameState.phase === 'buy_shares') {
                      dispatch({ type: 'END_TURN' });
                      playClick();
                      setLastAction('Ended turn and passed share purchases');
                    }
                  }}
                />
              </div>

              <aside className="game-sidebar-content">
                <PlayerScoreboard
                  players={gameState.players}
                  activePlayerIndex={gameState.activePlayerIndex}
                  companies={gameState.companies}
                  definitions={gameState.config.companies}
                  onTogglePlayerAi={handleTogglePlayerAi}
                />
              </aside>
            </div>
          </main>

          <LedgerDrawer
            isOpen={ledgerDrawerOpen}
            onClose={() => setLedgerDrawerOpen(false)}
            gameState={gameState}
          />
        </>
      )}

      {/* =========================================================================
          MODALS & DIALOGS LAYER
         ========================================================================= */}

      {/* 1. Setup & Options Modals */}
      <NewGameModal
        isOpen={newGameModalOpen}
        onClose={() => setNewGameModalOpen(false)}
        onStartGame={handleStartCustomGame}
        initialPlayers={gameState.players.map((p) => ({
          id: p.id,
          name: p.name,
          color: p.color || '#f59e0b',
          isAi: p.isAi ?? false,
        }))}
        initialConfig={config}
      />

      <OptionsModal
        isOpen={optionsModalOpen}
        onClose={() => setOptionsModalOpen(false)}
        config={config}
        onSaveConfig={handleSaveOptions}
        slowCpu={slowCpuTurns}
      />

      {/* 2. Company Founding Modal */}
      <FoundingCompanyModal
        isOpen={gameState.phase === 'choose_founding_company' && !activePlayerIsAi}
        gameState={gameState}
        availableCompanyIds={availableCompanyIds}
        onSelectCompany={handleSelectFoundedCompany}
      />

      {/* 3. Merger Workflow Modals */}
      <MergerSurvivorModal
        isOpen={gameState.phase === 'choose_merger_survivor' && !activePlayerIsAi}
        gameState={gameState}
        tiedCompanyIds={tiedSurvivorIds}
        onChooseSurvivor={handleSelectMergerSurvivor}
      />

      {/* Merger Payouts Announcement Banner/Modal */}
      {gameState.phase === 'dispose_merger_shares' &&
        pendingMerger &&
        pendingMerger.currentDefunctCompanyId &&
        pendingMerger.survivingCompanyId &&
        !mergerStatsDismissed && (
          <MergerStatsModal
            isOpen={true}
            onClose={() => setMergerStatsDismissed(true)}
            gameState={gameState}
            defunctCompanyId={pendingMerger.currentDefunctCompanyId}
            survivorCompanyId={pendingMerger.survivingCompanyId}
            preMergerPrice={
              pendingMerger.preMergerPrices[pendingMerger.currentDefunctCompanyId] || 200
            }
          />
        )}

      {/* Share Disposition Step-by-Step Modal */}
      <ShareDispositionModal
        isOpen={
          gameState.phase === 'dispose_merger_shares' &&
          mergerStatsDismissed &&
          Boolean(currentShareholder && !currentShareholder.isAi)
        }
        gameState={gameState}
        defunctCompanyId={pendingMerger?.currentDefunctCompanyId || ''}
        survivorCompanyId={pendingMerger?.survivingCompanyId || ''}
        preMergerPrice={
          pendingMerger?.currentDefunctCompanyId
            ? pendingMerger.preMergerPrices[pendingMerger.currentDefunctCompanyId] || 200
            : 200
        }
        onConfirmDisposition={handleConfirmDisposition}
      />

      {/* 4. Stock Market Purchasing Drawer */}
      <StockMarketModal
        isOpen={stockMarketModalOpen}
        onClose={() => setStockMarketModalOpen(false)}
        gameState={gameState}
        onConfirmPurchase={handleConfirmPurchases}
        onConfirmSale={handleConfirmSales}
        onSellAll={handleSellAllShares}
      />

      {/* 5. Inspection & Strategic Views */}
      <CompanyStatsModal
        isOpen={companyStatsModalOpen}
        onClose={() => setCompanyStatsModalOpen(false)}
        gameState={gameState}
      />

      <WealthChartModal
        isOpen={wealthChartModalOpen}
        onClose={() => setWealthChartModalOpen(false)}
        gameState={gameState}
      />

      <ShareGraphModal
        isOpen={shareGraphModalOpen}
        onClose={() => setShareGraphModalOpen(false)}
        gameState={gameState}
      />

      <WarRoomModal
        isOpen={warRoomModalOpen}
        onClose={() => setWarRoomModalOpen(false)}
        gameState={gameState}
      />

      <DiscardedTilesModal
        isOpen={discardedTilesModalOpen}
        onClose={() => setDiscardedTilesModalOpen(false)}
        gameState={gameState}
      />

      {/* Wildcard Blank Piece (BLANK_MOVE) Modal */}
      {blankPieceModalOpen && activeBlankCoord && (
        <BlankPieceModal
          isOpen={blankPieceModalOpen}
          onClose={() => {
            setBlankPieceModalOpen(false);
            setActiveBlankCoord(null);
          }}
          gameState={gameState}
          blankTileCoord={activeBlankCoord}
          onConfirmPlacement={handleConfirmBlankPlacement}
        />
      )}

      {/* 6. System Utility Modals */}
      <SaveLoadModal
        isOpen={saveLoadModalOpen}
        initialMode={saveLoadModalMode}
        onClose={() => setSaveLoadModalOpen(false)}
        gameState={gameState}
        onLoadGame={(loadedState) => {
          setSelectedHandCoord(null);
          setGameState(loadedState);
          setLastAction('Loaded match state successfully!');
        }}
      />

      <HelpModal isOpen={helpModalOpen} onClose={() => setHelpModalOpen(false)} />

      {/* 7. Game Over Leaderboard */}
      <GameOverModal
        isOpen={gameState.phase === 'game_over'}
        onClose={() => {}}
        gameState={gameState}
        onNewGame={() => {
          setGameState(createInitialGameState(config, gameState.players));
          setLastAction('Started fresh match');
        }}
      />
    </div>
  );
};
export default App;
