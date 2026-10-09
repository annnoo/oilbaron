import {
  applyTilePlacement,
  classifyMove,
  isTileDead,
} from './board';
import {
  buyShares,
  calculateSharePrice,
  getCompanyDefinition,
} from './market';
import {
  applyShareDisposition,
  calculateMergerBonuses,
  consolidateMergerTerritory,
  determineSurvivingCompany,
  getDefunctCompanyQueue,
  getShareholderQueue,
} from './merger';
import type {
  CellState,
  CompanyId,
  CompanyState,
  GameAction,
  GameConfig,
  GameState,
  PendingMergerState,
  PlayerState,
  TileCoord,
} from './types';

/**
 * Counts all grid cells belonging to a specific company on the board.
 */
function countCompanyTilesOnBoard(
  board: readonly (readonly CellState[])[],
  companyId: CompanyId
): number {
  let count = 0;
  for (let r = 0; r < board.length; r++) {
    const row = board[r]!;
    for (let c = 0; c < row.length; c++) {
      const cell = row[c]!;
      if (cell.type === 'company' && cell.companyId === companyId) {
        count++;
      }
    }
  }
  return count;
}

/**
 * Evaluates whether a player possesses at least one legally playable tile in hand.
 * A tile is not playable if:
 * - It is out of grid bounds or the target cell is not empty.
 * - Placing it would trigger an illegal merger between 2+ safe companies (dead tile).
 * - Placing it would found a company, but all companies are already active on the board.
 */
export function hasPlayableTile(
  player: PlayerState,
  board: readonly (readonly CellState[])[],
  companies: Record<CompanyId, CompanyState>,
  config: GameConfig
): boolean {
  if (player.hand.length === 0) return false;
  return player.hand.some((tile) => {
    if (
      tile.row < 0 ||
      tile.row >= config.boardRows ||
      tile.col < 0 ||
      tile.col >= config.boardCols
    ) {
      return false;
    }
    const cell = board[tile.row]?.[tile.col];
    if (!cell || cell.type !== 'empty') return false;
    if (isTileDead(board, tile, companies, config.safeThreshold)) return false;
    const classification = classifyMove(board, tile, companies, config.safeThreshold);
    if (classification.type === 'illegal_safe_merger') return false;
    if (classification.type === 'founding') {
      const anyAvailable = config.companies.some((def) => !companies[def.id]?.isActive);
      if (!anyAvailable) return false;
    }
    return true;
  });
}

/**
 * Determines whether the game has satisfied any victory or termination criteria:
 * 1. All active companies on the board have reached safe status (size >= safeThreshold).
 * 2. Any active company has grown to size >= 41.
 * 3. The tile pool is exhausted and no player possesses any playable (non-dead) tile.
 * 4. The entire board grid is completely occupied.
 */
export function isGameOver(state: GameState): boolean {
  const activeCompanies = Object.values(state.companies).filter((c) => c.isActive);

  if (activeCompanies.length > 0) {
    if (activeCompanies.every((c) => c.size >= state.config.safeThreshold)) {
      return true;
    }
    if (activeCompanies.some((c) => c.size >= 41)) {
      return true;
    }
  }

  if (state.tilePool.length === 0) {
    const anyPlayable = state.players.some((p) =>
      hasPlayableTile(p, state.board, state.companies, state.config)
    );
    if (!anyPlayable) {
      return true;
    }
  }

  let hasEmptyCell = false;
  for (let r = 0; r < state.board.length; r++) {
    const row = state.board[r]!;
    for (let c = 0; c < row.length; c++) {
      if (row[c]!.type === 'empty') {
        hasEmptyCell = true;
        break;
      }
    }
    if (hasEmptyCell) break;
  }
  if (!hasEmptyCell) {
    return true;
  }

  return false;
}

/**
 * Executes final end-game portfolio liquidation:
 * - Awards majority and minority bonuses for all active companies.
 * - Liquidates all active shares into cash at final share price.
 * - Liquidates inactive company shares at $0.
 * - Identifies winning player by highest cash balance.
 */
export function liquidateGame(state: GameState): {
  players: PlayerState[];
  winnerPlayerId: string | null;
} {
  let players = state.players.map((p) => ({
    ...p,
    shares: { ...p.shares },
  }));

  const activeCompanies = Object.values(state.companies).filter((c) => c.isActive);

  for (const company of activeCompanies) {
    const def = getCompanyDefinition(state.config.companies, company.id);
    if (!def) continue;

    const sharePrice = calculateSharePrice(def, company.size);

    // 1. Award majority and minority bonuses
    const bonuses = calculateMergerBonuses(company.id, sharePrice, players);
    players = players.map((p) => ({
      ...p,
      cash: p.cash + (bonuses[p.id] ?? 0),
    }));

    // 2. Liquidate held shares at final market price
    players = players.map((p) => {
      const heldShares = p.shares[company.id] ?? 0;
      const liquidationCash = heldShares * sharePrice;
      return {
        ...p,
        cash: p.cash + liquidationCash,
        shares: {
          ...p.shares,
          [company.id]: 0,
        },
      };
    });
  }

  // Clear shares of inactive companies
  for (const def of state.config.companies) {
    if (!state.companies[def.id]?.isActive) {
      players = players.map((p) => ({
        ...p,
        shares: {
          ...p.shares,
          [def.id]: 0,
        },
      }));
    }
  }

  // Identify winner
  let winnerPlayerId: string | null = null;
  let maxCash = -1;
  for (const p of players) {
    if (p.cash > maxCash) {
      maxCash = p.cash;
      winnerPlayerId = p.id;
    }
  }

  return { players, winnerPlayerId };
}

/**
 * Advances through the merger defunct queue:
 * - Sequentially awards bonuses for each defunct company.
 * - If shareholders exist, transitions to 'dispose_merger_shares'.
 * - If no shareholders exist, consolidates territory immediately and advances to next.
 * - When queue is empty, transitions back to 'buy_shares'.
 */
function processNextDefunctCompany(
  board: CellState[][],
  companies: Record<CompanyId, CompanyState>,
  players: PlayerState[],
  pendingMerger: PendingMergerState,
  activePlayerIndex: number,
  config: GameConfig
): {
  board: CellState[][];
  companies: Record<CompanyId, CompanyState>;
  players: PlayerState[];
  pendingMerger: PendingMergerState | null;
  phase: GameState['phase'];
} {
  let currentBoard = board;
  let currentCompanies = { ...companies };
  let currentPlayers = [...players];
  let remainingDefunctQueue = [...pendingMerger.defunctCompanyQueue];
  const survivorCompanyId = pendingMerger.survivingCompanyId!;

  while (remainingDefunctQueue.length > 0) {
    const currentDefunctId = remainingDefunctQueue[0]!;
    remainingDefunctQueue = remainingDefunctQueue.slice(1);

    const price = pendingMerger.preMergerPrices[currentDefunctId] ?? 0;

    // 1. Award majority and minority bonuses
    const bonuses = calculateMergerBonuses(currentDefunctId, price, currentPlayers);
    currentPlayers = currentPlayers.map((p) => ({
      ...p,
      cash: p.cash + (bonuses[p.id] ?? 0),
    }));

    // 2. Build shareholder disposition queue
    const shareholderQueue = getShareholderQueue(
      currentDefunctId,
      currentPlayers,
      activePlayerIndex
    );

    if (shareholderQueue.length > 0) {
      return {
        board: currentBoard,
        companies: currentCompanies,
        players: currentPlayers,
        pendingMerger: {
          ...pendingMerger,
          defunctCompanyQueue: remainingDefunctQueue,
          currentDefunctCompanyId: currentDefunctId,
          shareholderQueue,
          currentShareholderIndex: 0,
        },
        phase: 'dispose_merger_shares',
      };
    }

    // No shareholders: consolidate territory immediately
    currentBoard = consolidateMergerTerritory(
      currentBoard,
      pendingMerger.coord,
      currentDefunctId,
      survivorCompanyId
    );

    const survivorSize = countCompanyTilesOnBoard(currentBoard, survivorCompanyId);
    currentCompanies[survivorCompanyId] = {
      ...currentCompanies[survivorCompanyId]!,
      size: survivorSize,
      isSafe: survivorSize >= config.safeThreshold,
    };

    currentCompanies[currentDefunctId] = {
      ...currentCompanies[currentDefunctId]!,
      size: 0,
      isActive: false,
      isSafe: false,
      availableShares: config.totalSharesPerCompany,
    };
  }

  // All defunct companies resolved
  return {
    board: currentBoard,
    companies: currentCompanies,
    players: currentPlayers,
    pendingMerger: null,
    phase: 'buy_shares',
  };
}

/**
 * Pure immutable game state reducer for Oil Baron.
 * Handles all player actions, phase transitions, merger resolution, and game-over evaluation.
 */
export function gameReducer(state: GameState, action: GameAction): GameState {
  if (state.phase === 'game_over') {
    throw new Error('Game is already over');
  }

  switch (action.type) {
    case 'PLACE_TILE': {
      if (state.phase !== 'place_tile') {
        throw new Error(`Cannot place tile during phase '${state.phase}'`);
      }

      const { coord } = action;
      if (
        coord.row < 0 ||
        coord.row >= state.config.boardRows ||
        coord.col < 0 ||
        coord.col >= state.config.boardCols
      ) {
        throw new Error(`Coordinate (${coord.row}, ${coord.col}) out of bounds`);
      }

      const activePlayer = state.players[state.activePlayerIndex]!;
      const inHandIndex = activePlayer.hand.findIndex(
        (t) => t.row === coord.row && t.col === coord.col
      );
      if (inHandIndex === -1) {
        throw new Error(`Tile (${coord.row}, ${coord.col}) is not in active player's hand`);
      }

      const targetCell = state.board[coord.row]?.[coord.col];
      if (!targetCell || targetCell.type !== 'empty') {
        throw new Error(`Cell (${coord.row}, ${coord.col}) is already occupied`);
      }

      if (isTileDead(state.board, coord, state.companies, state.config.safeThreshold)) {
        throw new Error('Cannot place dead tile');
      }

      const updatedHand = activePlayer.hand.filter((_, idx) => idx !== inHandIndex);
      const updatedPlayers = state.players.map((p, idx) =>
        idx === state.activePlayerIndex ? { ...p, hand: updatedHand } : p
      );

      const classification = classifyMove(
        state.board,
        coord,
        state.companies,
        state.config.safeThreshold
      );

      if (classification.type === 'illegal_safe_merger') {
        throw new Error('Cannot place dead tile');
      }

      if (classification.type === 'isolated') {
        const newBoard = applyTilePlacement(state.board, coord, { type: 'unincorporated' });
        return {
          ...state,
          board: newBoard,
          players: updatedPlayers,
          phase: 'buy_shares',
          history: [...state.history, action],
        };
      }

      if (classification.type === 'expansion') {
        const newBoard = state.board.map((r) => [...r]);
        for (const c of classification.connectedCoords) {
          newBoard[c.row]![c.col] = { type: 'company', companyId: classification.companyId };
        }

        const company = state.companies[classification.companyId]!;
        const newSize = company.size + classification.connectedCoords.length;
        const isSafe = newSize >= state.config.safeThreshold;

        const newCompanies = {
          ...state.companies,
          [classification.companyId]: {
            ...company,
            size: newSize,
            isSafe,
          },
        };

        return {
          ...state,
          board: newBoard,
          companies: newCompanies,
          players: updatedPlayers,
          phase: 'buy_shares',
          history: [...state.history, action],
        };
      }

      if (classification.type === 'founding') {
        const newBoard = applyTilePlacement(state.board, coord, { type: 'unincorporated' });
        const availableCompanyIds = state.config.companies
          .filter((def) => !state.companies[def.id]?.isActive)
          .map((def) => def.id);

        if (availableCompanyIds.length === 0) {
          throw new Error('No available companies to found');
        }

        return {
          ...state,
          board: newBoard,
          players: updatedPlayers,
          phase: 'choose_founding_company',
          pendingFounding: {
            coord,
            connectedCoords: classification.connectedCoords,
            availableCompanyIds,
          },
          history: [...state.history, action],
        };
      }

      if (classification.type === 'merger') {
        const newBoard = applyTilePlacement(state.board, coord, { type: 'unincorporated' });
        const preMergerPrices: Record<CompanyId, number> = {};
        for (const id of classification.participatingCompanyIds) {
          const comp = state.companies[id]!;
          const def = getCompanyDefinition(state.config.companies, id)!;
          preMergerPrices[id] = calculateSharePrice(def, comp.size);
        }

        const survivorResult = determineSurvivingCompany(
          classification.participatingCompanyIds,
          state.companies
        );

        if (survivorResult.hasTie) {
          return {
            ...state,
            board: newBoard,
            players: updatedPlayers,
            phase: 'choose_merger_survivor',
            pendingMerger: {
              coord,
              participatingCompanyIds: classification.participatingCompanyIds,
              survivingCompanyId: null,
              defunctCompanyQueue: [],
              currentDefunctCompanyId: null,
              shareholderQueue: [],
              currentShareholderIndex: 0,
              preMergerPrices,
            },
            history: [...state.history, action],
          };
        }

        const survivingCompanyId = survivorResult.survivorId!;
        const defunctQueue = getDefunctCompanyQueue(
          classification.participatingCompanyIds,
          survivingCompanyId,
          state.companies
        );

        const initialPendingMerger: PendingMergerState = {
          coord,
          participatingCompanyIds: classification.participatingCompanyIds,
          survivingCompanyId,
          defunctCompanyQueue: defunctQueue,
          currentDefunctCompanyId: null,
          shareholderQueue: [],
          currentShareholderIndex: 0,
          preMergerPrices,
        };

        const stepResult = processNextDefunctCompany(
          newBoard,
          state.companies,
          updatedPlayers,
          initialPendingMerger,
          state.activePlayerIndex,
          state.config
        );

        return {
          ...state,
          board: stepResult.board,
          companies: stepResult.companies,
          players: stepResult.players,
          pendingMerger: stepResult.pendingMerger,
          phase: stepResult.phase,
          history: [...state.history, action],
        };
      }

      throw new Error(`Unhandled classification: ${(classification as any).type}`);
    }

    case 'CHOOSE_FOUNDED_COMPANY': {
      if (state.phase !== 'choose_founding_company' || !state.pendingFounding) {
        throw new Error(`Cannot choose founded company during phase '${state.phase}'`);
      }

      const { connectedCoords, availableCompanyIds } = state.pendingFounding;
      if (!availableCompanyIds.includes(action.companyId)) {
        throw new Error(`Company ${action.companyId} is not available to be founded`);
      }

      const company = state.companies[action.companyId];
      if (!company || company.isActive) {
        throw new Error(`Company ${action.companyId} is already active or invalid`);
      }

      const bonusShares = state.config.founderBonusShares;
      const sharesAwarded = Math.min(bonusShares, company.availableShares);

      const newBoard = state.board.map((r) => [...r]);
      for (const c of connectedCoords) {
        newBoard[c.row]![c.col] = { type: 'company', companyId: action.companyId };
      }

      const newSize = connectedCoords.length;
      const isSafe = newSize >= state.config.safeThreshold;

      const newCompanies: Record<CompanyId, CompanyState> = {
        ...state.companies,
        [action.companyId]: {
          ...company,
          isActive: true,
          size: newSize,
          isSafe,
          availableShares: company.availableShares - sharesAwarded,
        },
      };

      const updatedPlayers = state.players.map((p, idx) =>
        idx === state.activePlayerIndex
          ? {
              ...p,
              shares: {
                ...p.shares,
                [action.companyId]: (p.shares[action.companyId] ?? 0) + sharesAwarded,
              },
            }
          : p
      );

      return {
        ...state,
        board: newBoard,
        companies: newCompanies,
        players: updatedPlayers,
        pendingFounding: null,
        phase: 'buy_shares',
        history: [...state.history, action],
      };
    }

    case 'CHOOSE_MERGER_SURVIVOR': {
      if (state.phase !== 'choose_merger_survivor' || !state.pendingMerger) {
        throw new Error(`Cannot choose merger survivor during phase '${state.phase}'`);
      }

      const survivorCheck = determineSurvivingCompany(
        state.pendingMerger.participatingCompanyIds,
        state.companies
      );

      const validSurvivorIds = survivorCheck.hasTie
        ? survivorCheck.tiedCompanyIds
        : survivorCheck.survivorId
          ? [survivorCheck.survivorId]
          : state.pendingMerger.participatingCompanyIds;

      if (!validSurvivorIds.includes(action.survivorCompanyId)) {
        throw new Error(
          `Company ${action.survivorCompanyId} is not an eligible survivor among tied contenders (${validSurvivorIds.join(', ')})`
        );
      }

      const survivingCompanyId = action.survivorCompanyId;
      const defunctQueue = getDefunctCompanyQueue(
        state.pendingMerger.participatingCompanyIds,
        survivingCompanyId,
        state.companies
      );

      const updatedPendingMerger: PendingMergerState = {
        ...state.pendingMerger,
        survivingCompanyId,
        defunctCompanyQueue: defunctQueue,
      };

      const stepResult = processNextDefunctCompany(
        state.board,
        state.companies,
        state.players,
        updatedPendingMerger,
        state.activePlayerIndex,
        state.config
      );

      return {
        ...state,
        board: stepResult.board,
        companies: stepResult.companies,
        players: stepResult.players,
        pendingMerger: stepResult.pendingMerger,
        phase: stepResult.phase,
        history: [...state.history, action],
      };
    }

    case 'DISPOSE_MERGER_SHARES': {
      if (state.phase !== 'dispose_merger_shares' || !state.pendingMerger) {
        throw new Error(`Cannot dispose merger shares during phase '${state.phase}'`);
      }

      const {
        currentDefunctCompanyId,
        survivingCompanyId,
        shareholderQueue,
        currentShareholderIndex,
        preMergerPrices,
      } = state.pendingMerger;

      if (!currentDefunctCompanyId || !survivingCompanyId) {
        throw new Error('Corrupt merger state: missing defunct or survivor company');
      }

      const shareholderId = shareholderQueue[currentShareholderIndex]!;
      const playerIndex = state.players.findIndex((p) => p.id === shareholderId);
      if (playerIndex === -1) {
        throw new Error(`Shareholder ${shareholderId} not found in players`);
      }

      const player = state.players[playerIndex]!;
      const defunctPrice = preMergerPrices[currentDefunctCompanyId]!;
      const survivorCompany = state.companies[survivingCompanyId]!;

      const dispositionResult = applyShareDisposition(
        player,
        currentDefunctCompanyId,
        defunctPrice,
        survivorCompany,
        action.choices
      );

      const updatedPlayers = state.players.map((p, idx) =>
        idx === playerIndex ? dispositionResult.player : p
      );

      const updatedCompanies: Record<CompanyId, CompanyState> = {
        ...state.companies,
        [survivingCompanyId]: dispositionResult.survivorCompany,
      };

      const nextShareholderIndex = currentShareholderIndex + 1;

      if (nextShareholderIndex < shareholderQueue.length) {
        return {
          ...state,
          players: updatedPlayers,
          companies: updatedCompanies,
          pendingMerger: {
            ...state.pendingMerger,
            currentShareholderIndex: nextShareholderIndex,
          },
          history: [...state.history, action],
        };
      }

      // All shareholders for currentDefunctCompanyId have disposed shares
      const updatedBoard = consolidateMergerTerritory(
        state.board,
        state.pendingMerger.coord,
        currentDefunctCompanyId,
        survivingCompanyId
      );

      const survivorSize = countCompanyTilesOnBoard(updatedBoard, survivingCompanyId);
      updatedCompanies[survivingCompanyId] = {
        ...updatedCompanies[survivingCompanyId]!,
        size: survivorSize,
        isSafe: survivorSize >= state.config.safeThreshold,
      };

      const heldDefunctShares = updatedPlayers.reduce(
        (sum, p) => sum + (p.shares[currentDefunctCompanyId] ?? 0),
        0
      );
      updatedCompanies[currentDefunctCompanyId] = {
        ...updatedCompanies[currentDefunctCompanyId]!,
        size: 0,
        isActive: false,
        isSafe: false,
        availableShares: state.config.totalSharesPerCompany - heldDefunctShares,
      };

      const nextStep = processNextDefunctCompany(
        updatedBoard,
        updatedCompanies,
        updatedPlayers,
        state.pendingMerger,
        state.activePlayerIndex,
        state.config
      );

      return {
        ...state,
        board: nextStep.board,
        companies: nextStep.companies,
        players: nextStep.players,
        pendingMerger: nextStep.pendingMerger,
        phase: nextStep.phase,
        history: [...state.history, action],
      };
    }

    case 'BUY_SHARES': {
      if (state.phase !== 'buy_shares') {
        throw new Error(`Cannot buy shares during phase '${state.phase}'`);
      }

      if (action.purchases.length === 0) {
        return {
          ...state,
          history: [...state.history, action],
        };
      }

      const totalRequested = action.purchases.reduce((sum, p) => sum + p.count, 0);
      const remainingPurchases =
        state.config.maxSharesPerPurchase - state.turnPurchasedSharesCount;

      if (totalRequested > remainingPurchases) {
        throw new Error(
          `Requested share purchases (${totalRequested}) exceed remaining allowed (${remainingPurchases})`
        );
      }

      const activePlayer = state.players[state.activePlayerIndex]!;
      const result = buyShares(
        activePlayer,
        state.companies,
        state.config.companies,
        action.purchases,
        remainingPurchases
      );

      if (!result.success) {
        throw new Error(result.error);
      }

      const updatedPlayers = state.players.map((p, idx) =>
        idx === state.activePlayerIndex ? result.player : p
      );

      return {
        ...state,
        players: updatedPlayers,
        companies: result.companies,
        turnPurchasedSharesCount: state.turnPurchasedSharesCount + totalRequested,
        history: [...state.history, action],
      };
    }

    case 'DISCARD_DEAD_TILE': {
      if (state.phase !== 'place_tile') {
        throw new Error(`Cannot discard dead tile during phase '${state.phase}'`);
      }

      const activePlayer = state.players[state.activePlayerIndex]!;
      const inHandIndex = activePlayer.hand.findIndex(
        (t) => t.row === action.coord.row && t.col === action.coord.col
      );

      if (inHandIndex === -1) {
        throw new Error(`Tile (${action.coord.row}, ${action.coord.col}) not found in player hand`);
      }

      if (!isTileDead(state.board, action.coord, state.companies, state.config.safeThreshold)) {
        throw new Error(`Tile at (${action.coord.row}, ${action.coord.col}) is not dead`);
      }

      let updatedHand = activePlayer.hand.filter((_, idx) => idx !== inHandIndex);
      let updatedTilePool = [...state.tilePool];

      if (updatedTilePool.length > 0) {
        const drawn = updatedTilePool[0]!;
        updatedTilePool = updatedTilePool.slice(1);
        updatedHand.push(drawn);
      }

      const updatedPlayers = state.players.map((p, idx) =>
        idx === state.activePlayerIndex ? { ...p, hand: updatedHand } : p
      );

      const candidateState: GameState = {
        ...state,
        players: updatedPlayers,
        tilePool: updatedTilePool,
        history: [...state.history, action],
      };

      const activePlayerUpdated = updatedPlayers[state.activePlayerIndex]!;
      if (
        updatedTilePool.length === 0 &&
        !hasPlayableTile(activePlayerUpdated, state.board, state.companies, state.config)
      ) {
        if (isGameOver(candidateState)) {
          const liquidation = liquidateGame(candidateState);
          return {
            ...candidateState,
            players: liquidation.players,
            phase: 'game_over',
            winnerPlayerId: liquidation.winnerPlayerId,
          };
        }
        return {
          ...candidateState,
          phase: 'buy_shares',
        };
      }

      return candidateState;
    }

    case 'END_TURN': {
      if (state.phase !== 'buy_shares') {
        throw new Error(`Cannot end turn during phase '${state.phase}'`);
      }

      const activePlayer = state.players[state.activePlayerIndex]!;
      let updatedTilePool = [...state.tilePool];
      let updatedHand = [...activePlayer.hand];

      // 1. Replenish hand
      const needed = state.config.handSize - updatedHand.length;
      if (needed > 0 && updatedTilePool.length > 0) {
        const drawCount = Math.min(needed, updatedTilePool.length);
        updatedHand.push(...updatedTilePool.slice(0, drawCount));
        updatedTilePool = updatedTilePool.slice(drawCount);
      }

      // 2. Dead tile cleanup
      while (updatedTilePool.length > 0) {
        const deadIndex = updatedHand.findIndex((t) =>
          isTileDead(state.board, t, state.companies, state.config.safeThreshold)
        );
        if (deadIndex === -1) break;
        updatedHand.splice(deadIndex, 1, updatedTilePool[0]!);
        updatedTilePool = updatedTilePool.slice(1);
      }

      const nextPlayerIndex = (state.activePlayerIndex + 1) % state.players.length;

      const updatedPlayers = state.players.map((p, idx) =>
        idx === state.activePlayerIndex ? { ...p, hand: updatedHand } : p
      );

      const candidateState: GameState = {
        ...state,
        players: updatedPlayers,
        tilePool: updatedTilePool,
        activePlayerIndex: nextPlayerIndex,
        phase: 'place_tile',
        turnPurchasedSharesCount: 0,
        history: [...state.history, action],
      };

      if (isGameOver(candidateState)) {
        const liquidation = liquidateGame(candidateState);
        return {
          ...candidateState,
          players: liquidation.players,
          phase: 'game_over',
          winnerPlayerId: liquidation.winnerPlayerId,
        };
      }

      const nextPlayer = candidateState.players[nextPlayerIndex]!;
      if (
        !hasPlayableTile(
          nextPlayer,
          candidateState.board,
          candidateState.companies,
          candidateState.config
        )
      ) {
        return {
          ...candidateState,
          phase: 'buy_shares',
        };
      }

      return candidateState;
    }

    default: {
      const _exhaustive: never = action;
      throw new Error(`Unhandled action type: ${(action as any).type}`);
    }
  }
}
