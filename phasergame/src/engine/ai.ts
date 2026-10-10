import {
  classifyMove,
  isTileDead,
  getAdjacentCoords,
} from './board';
import { createInitialGameState, DEFAULT_CONFIG } from './config';
import {
  calculateSharePrice,
  getCompanyDefinition,
  validateSharePurchases,
} from './market';
import {
  calculateMergerBonuses,
  determineSurvivingCompany,
} from './merger';
import {
  gameReducer,
  hasPlayableTile,
  isGameOver,
  liquidateGame,
} from './reducer';
import type {
  CellState,
  CompanyId,
  CompanyState,
  GameConfig,
  GameState,
  PlayerState,
  ShareDispositionChoices,
  SharePurchase,
  TileCoord,
} from './types';

/**
 * Checks whether placing a specific coordinate is legally valid according to engine rules.
 */
function isTileLegalToPlay(
  tile: TileCoord,
  board: readonly (readonly CellState[])[],
  companies: Record<CompanyId, CompanyState>,
  config: GameConfig
): boolean {
  if (
    tile.row < 0 ||
    tile.row >= config.boardRows ||
    tile.col < 0 ||
    tile.col >= config.boardCols
  ) {
    return false;
  }
  const cell = board[tile.row]?.[tile.col];
  if (!cell || cell.type !== 'empty') {
    return false;
  }
  if (isTileDead(board, tile, companies, config.safeThreshold)) {
    return false;
  }
  const classification = classifyMove(board, tile, companies, config.safeThreshold);
  if (classification.type === 'illegal_safe_merger') {
    return false;
  }
  if (classification.type === 'founding') {
    const hasAvailable = config.companies.some((def) => !companies[def.id]?.isActive);
    if (!hasAvailable) {
      return false;
    }
  }
  return true;
}

/**
 * Scores a candidate tile placement based on expected strategic return:
 * - Founding: High priority due to free founder bonus shares and immediate ownership.
 * - Expansion: Evaluates share price growth for held companies vs opponent holdings.
 * - Merger: Evaluates net shareholder cash bonuses and share appreciation.
 * - Isolated: Neutral baseline.
 */
function evaluateTileMoveUtility(
  state: GameState,
  player: PlayerState,
  tile: TileCoord
): number {
  const classification = classifyMove(
    state.board,
    tile,
    state.companies,
    state.config.safeThreshold
  );

  switch (classification.type) {
    case 'founding': {
      // Free founder shares provide instant wealth and commanding lead
      const founderBonus = state.config.founderBonusShares;
      const connectedCount = classification.connectedCoords.length;
      return 5000 + founderBonus * 250 + connectedCount * 100;
    }

    case 'expansion': {
      const companyId = classification.companyId;
      const comp = state.companies[companyId]!;
      const def = getCompanyDefinition(state.config.companies, companyId);
      const priceDelta = def?.priceDelta ?? 100;
      const addedTiles = classification.connectedCoords.length;

      const myShares = player.shares[companyId] ?? 0;
      const totalOpponentShares = state.players.reduce((sum, p) => {
        if (p.id === player.id) return sum;
        return sum + (p.shares[companyId] ?? 0);
      }, 0);

      const myGain = myShares * priceDelta * addedTiles;
      const opponentGain = totalOpponentShares * priceDelta * addedTiles;

      let safeBonus = 0;
      if (
        !comp.isSafe &&
        comp.size + addedTiles >= state.config.safeThreshold &&
        myShares > 0
      ) {
        safeBonus = 1000;
      }

      return 1000 + myGain * 2 - opponentGain * 0.5 + safeBonus;
    }

    case 'merger': {
      let utility = 2000;
      const survivorInfo = determineSurvivingCompany(
        classification.participatingCompanyIds,
        state.companies
      );

      const likelySurvivorId = survivorInfo.hasTie
        ? survivorInfo.tiedCompanyIds[0]!
        : survivorInfo.survivorId!;

      const defSurvivor = getCompanyDefinition(state.config.companies, likelySurvivorId);
      const survivorPriceDelta = defSurvivor?.priceDelta ?? 100;

      let totalDefunctTiles = 0;

      for (const id of classification.participatingCompanyIds) {
        if (id === likelySurvivorId) continue;
        const comp = state.companies[id]!;
        totalDefunctTiles += comp.size;
        const def = getCompanyDefinition(state.config.companies, id);
        if (!def) continue;

        const prePrice = calculateSharePrice(def, comp.size);
        const bonuses = calculateMergerBonuses(id, prePrice, state.players);
        const myBonus = bonuses[player.id] ?? 0;
        const totalOppBonus = state.players.reduce((sum, p) => {
          if (p.id === player.id) return sum;
          return sum + (bonuses[p.id] ?? 0);
        }, 0);

        const myDefunctShares = player.shares[id] ?? 0;
        const defunctLiquidation = myDefunctShares * prePrice;

        utility += myBonus * 2 - totalOppBonus * 0.5 + defunctLiquidation;
      }

      const mySurvivorShares = player.shares[likelySurvivorId] ?? 0;
      const survivorAppreciation =
        mySurvivorShares * survivorPriceDelta * (totalDefunctTiles + 1);
      utility += survivorAppreciation;

      return utility;
    }

    case 'isolated': {
      const neighbors = getAdjacentCoords(
        tile,
        state.config.boardRows,
        state.config.boardCols
      );
      const emptyNeighbors = neighbors.filter(
        (n) => state.board[n.row]?.[n.col]?.type === 'empty'
      );
      return 100 + emptyNeighbors.length * 5;
    }

    default:
      return -10000;
  }
}

/**
 * Selects the optimal legal tile to place from the specified player's hand.
 * Supports 'smart' (deterministic optimal utility) and 'dumb' (authentic 1993
 * OILBARON.EXE Random(100) noise injection resulting in suboptimal plays).
 * Returns null if no legal moves exist.
 */
export function getBestTileMove(
  state: GameState,
  playerIndex: number,
  difficulty?: 'dumb' | 'smart',
  rng: ((candidate: TileCoord) => number) = Math.random
): TileCoord | null {
  const player = state.players[playerIndex];
  if (!player || player.hand.length === 0) {
    return null;
  }

  const playableTiles = player.hand.filter((tile) =>
    isTileLegalToPlay(tile, state.board, state.companies, state.config)
  );

  if (playableTiles.length === 0) {
    return null;
  }

  const effectiveDifficulty = difficulty ?? player.aiDifficulty ?? 'smart';

  if (effectiveDifficulty === 'smart') {
    let bestTile = playableTiles[0]!;
    let bestScore = evaluateTileMoveUtility(state, player, bestTile);

    for (let i = 1; i < playableTiles.length; i++) {
      const candidate = playableTiles[i]!;
      const score = evaluateTileMoveUtility(state, player, candidate);
      if (score > bestScore) {
        bestScore = score;
        bestTile = candidate;
      } else if (score === bestScore) {
        // Deterministic tiebreak
        if (
          candidate.row * 1000 + candidate.col <
          bestTile.row * 1000 + bestTile.col
        ) {
          bestTile = candidate;
        }
      }
    }

    return bestTile;
  }

  // Dumb AI: In 1993 OILBARON.EXE (ai.c:710-732), uniform random noise is added
  // to candidate scores, causing erratic tile choices and suboptimal plays.
  const getCandidateScore = (t: TileCoord) => {
    const baseScore = evaluateTileMoveUtility(state, player, t);
    const noise = rng(t) * 10000;
    return baseScore + noise;
  };

  let bestTile = playableTiles[0]!;
  let bestScore = getCandidateScore(bestTile);

  for (let i = 1; i < playableTiles.length; i++) {
    const candidate = playableTiles[i]!;
    const score = getCandidateScore(candidate);
    if (score > bestScore) {
      bestScore = score;
      bestTile = candidate;
    }
  }

  return bestTile;
}

/**
 * Chooses the best company to found from the available inactive companies.
 * Prefers low-cost tiers for cheap entry and rapid growth, companies where the
 * player holds legacy shares from previous mergers, and companies with ample bank stock.
 */
export function getBestFoundedCompany(
  state: GameState,
  availableCompanyIds: string[],
  difficulty?: 'dumb' | 'smart',
  rng: () => number = Math.random
): string {
  if (availableCompanyIds.length === 0) {
    throw new Error('No available companies to found');
  }

  const activePlayer = state.players[state.activePlayerIndex];
  const effectiveDifficulty = difficulty ?? activePlayer?.aiDifficulty ?? 'smart';

  if (effectiveDifficulty === 'dumb') {
    const idx = Math.min(
      availableCompanyIds.length - 1,
      Math.floor(rng() * availableCompanyIds.length)
    );
    return availableCompanyIds[idx]!;
  }

  let bestId = availableCompanyIds[0]!;
  let bestScore = -Infinity;

  for (const id of availableCompanyIds) {
    const def = getCompanyDefinition(state.config.companies, id);
    const comp = state.companies[id];
    const basePrice = def?.basePrice ?? 300;
    const availableShares = comp?.availableShares ?? state.config.totalSharesPerCompany;

    const legacySharesHeld = activePlayer?.shares[id] ?? 0;

    // 1. Resurrecting legacy held shares is massively valuable
    // 2. Lower base price allows cheaper majority acquisition (500 - basePrice)
    // 3. Ample bank shares ensure availability to buy
    const score =
      legacySharesHeld * 2000 +
      (1000 - basePrice) +
      availableShares * 10;

    if (score > bestScore) {
      bestScore = score;
      bestId = id;
    } else if (score === bestScore && id < bestId) {
      bestId = id;
    }
  }

  return bestId;
}

/**
 * Evaluates tied candidate companies in a merger and selects the survivor that
 * optimizes the player's stock appreciation and cash bonus payouts.
 */
export function getBestMergerSurvivor(
  state: GameState,
  tiedCompanyIds: string[],
  difficulty?: 'dumb' | 'smart',
  rng: () => number = Math.random
): string {
  if (tiedCompanyIds.length === 0) {
    throw new Error('No tied company ids provided');
  }
  if (tiedCompanyIds.length === 1) {
    return tiedCompanyIds[0]!;
  }

  const activePlayer = state.players[state.activePlayerIndex];
  if (!activePlayer) {
    return tiedCompanyIds[0]!;
  }

  const effectiveDifficulty = difficulty ?? activePlayer.aiDifficulty ?? 'smart';

  if (effectiveDifficulty === 'dumb') {
    const idx = Math.min(
      tiedCompanyIds.length - 1,
      Math.floor(rng() * tiedCompanyIds.length)
    );
    return tiedCompanyIds[idx]!;
  }

  let bestSurvivor = tiedCompanyIds[0]!;
  let bestScore = -Infinity;

  for (const candidateId of tiedCompanyIds) {
    const candidateComp = state.companies[candidateId];
    const defCandidate = getCompanyDefinition(state.config.companies, candidateId);
    if (!candidateComp || !defCandidate) continue;

    let score = 0;
    let totalOtherSize = 0;

    for (const otherId of tiedCompanyIds) {
      if (otherId === candidateId) continue;
      const otherComp = state.companies[otherId];
      const defOther = getCompanyDefinition(state.config.companies, otherId);
      if (!otherComp || !defOther) continue;

      totalOtherSize += otherComp.size;
      const prePrice = calculateSharePrice(defOther, otherComp.size);
      const bonuses = calculateMergerBonuses(otherId, prePrice, state.players);

      const myBonus = bonuses[activePlayer.id] ?? 0;
      const totalOppBonus = state.players.reduce((sum, p) => {
        if (p.id === activePlayer.id) return sum;
        return sum + (bonuses[p.id] ?? 0);
      }, 0);

      const heldDefunct = activePlayer.shares[otherId] ?? 0;
      score += myBonus * 2 - totalOppBonus * 0.5 + heldDefunct * prePrice;
    }

    const mySurvivorShares = activePlayer.shares[candidateId] ?? 0;
    const appreciation =
      mySurvivorShares * defCandidate.priceDelta * (totalOtherSize + 1);
    score += appreciation;

    if (score > bestScore) {
      bestScore = score;
      bestSurvivor = candidateId;
    } else if (score === bestScore && candidateId < bestSurvivor) {
      bestSurvivor = candidateId;
    }
  }

  return bestSurvivor;
}

/**
 * Determines optimal stock purchases up to turn limits:
 * - Prioritizes establishing or defending majority holdings.
 * - Targets undervalued companies with high growth potential while preserving liquidity.
 */
export function getBestSharePurchases(
  state: GameState,
  playerIndex: number,
  difficulty?: 'dumb' | 'smart',
  rng: () => number = Math.random
): { companyId: string; count: number }[] {
  const player = state.players[playerIndex];
  if (!player || player.cash <= 0) {
    return [];
  }

  const remainingAllowed =
    state.config.maxSharesPerPurchase - state.turnPurchasedSharesCount;
  if (remainingAllowed <= 0) {
    return [];
  }

  const activeCompanies = Object.values(state.companies).filter(
    (c) => c.isActive && c.availableShares > 0
  );
  if (activeCompanies.length === 0) {
    return [];
  }

  const effectiveDifficulty = difficulty ?? player.aiDifficulty ?? 'smart';

  if (effectiveDifficulty === 'dumb') {
    let remainingCash = player.cash;
    const simulatedAvailable: Record<CompanyId, number> = {};
    for (const c of activeCompanies) {
      simulatedAvailable[c.id] = c.availableShares;
    }

    const purchaseMap: Record<CompanyId, number> = {};
    let totalPurchased = 0;

    while (totalPurchased < remainingAllowed) {
      const affordable = activeCompanies.filter((c) => {
        const avail = simulatedAvailable[c.id] ?? 0;
        if (avail <= 0) return false;
        const def = getCompanyDefinition(state.config.companies, c.id);
        if (!def) return false;
        const price = calculateSharePrice(def, c.size);
        return price <= remainingCash;
      });

      if (affordable.length === 0) {
        break;
      }

      const selectedIndex = Math.min(
        affordable.length - 1,
        Math.floor(rng() * affordable.length)
      );
      const chosenComp = affordable[selectedIndex]!;
      const chosenDef = getCompanyDefinition(state.config.companies, chosenComp.id)!;
      const price = calculateSharePrice(chosenDef, chosenComp.size);

      purchaseMap[chosenComp.id] = (purchaseMap[chosenComp.id] ?? 0) + 1;
      simulatedAvailable[chosenComp.id]!--;
      remainingCash -= price;
      totalPurchased++;
    }

    const purchases: SharePurchase[] = Object.entries(purchaseMap).map(
      ([companyId, count]) => ({ companyId, count })
    );

    const validation = validateSharePurchases(
      player,
      state.companies,
      state.config.companies,
      purchases,
      remainingAllowed
    );

    return validation.valid ? purchases : [];
  }

  let remainingCash = player.cash;
  const simulatedAvailable: Record<CompanyId, number> = {};
  const simulatedMyShares: Record<CompanyId, number> = { ...player.shares };

  for (const c of activeCompanies) {
    simulatedAvailable[c.id] = c.availableShares;
  }

  const purchaseMap: Record<CompanyId, number> = {};
  let totalPurchased = 0;

  while (totalPurchased < remainingAllowed) {
    let bestCompId: CompanyId | null = null;
    let bestScore = -Infinity;

    for (const c of activeCompanies) {
      const avail = simulatedAvailable[c.id] ?? 0;
      if (avail <= 0) continue;

      const def = getCompanyDefinition(state.config.companies, c.id);
      if (!def) continue;

      const price = calculateSharePrice(def, c.size);
      if (price > remainingCash) continue;

      const myShares = simulatedMyShares[c.id] ?? 0;
      const maxOppShares = state.players.reduce((max, p, idx) => {
        if (idx === playerIndex) return max;
        const shares = p.shares[c.id] ?? 0;
        return shares > max ? shares : max;
      }, 0);

      let score = 0;

      // 1. Majority / minority race
      if (myShares < maxOppShares) {
        if (myShares + 1 >= maxOppShares) {
          score += 1500; // Contesting majority
        } else {
          score += 800; // Catching up for minority
        }
      } else if (myShares === maxOppShares) {
        score += 1400; // Taking outright lead
      } else {
        const leadMargin = myShares - maxOppShares;
        if (leadMargin < 3) {
          score += 900; // Defending slender lead
        } else {
          score += 250; // Lead already secure
        }
      }

      // 2. Growth potential
      if (!c.isSafe) {
        score += 400;
      } else {
        score += 150;
      }

      // 3. Cheaper entry cost
      score += (1000 - price) / 10;

      if (score > bestScore) {
        bestScore = score;
        bestCompId = c.id;
      }
    }

    if (!bestCompId) {
      break;
    }

    const def = getCompanyDefinition(state.config.companies, bestCompId)!;
    const price = calculateSharePrice(def, state.companies[bestCompId]!.size);

    purchaseMap[bestCompId] = (purchaseMap[bestCompId] ?? 0) + 1;
    simulatedAvailable[bestCompId]!--;
    simulatedMyShares[bestCompId] = (simulatedMyShares[bestCompId] ?? 0) + 1;
    remainingCash -= price;
    totalPurchased++;
  }

  const purchases: SharePurchase[] = Object.entries(purchaseMap).map(
    ([companyId, count]) => ({ companyId, count })
  );

  const validation = validateSharePurchases(
    player,
    state.companies,
    state.config.companies,
    purchases,
    remainingAllowed
  );

  return validation.valid ? purchases : [];
}

/**
 * Calculates optimal share disposition choices for a shareholder during merger resolution:
 * - Executes 2-for-1 trades when survivor share price >= 2 * defunct share price.
 * - Sells shares when trade is disadvantageous or survivor bank stock is exhausted.
 * - Sells any leftover odd share for immediate liquidity.
 */
export function getBestMergerDisposition(
  state: GameState,
  playerIndex: number,
  defunctCompanyId: string,
  difficulty?: 'dumb' | 'smart',
  rng: () => number = Math.random
): ShareDispositionChoices {
  const player = state.players[playerIndex];
  if (!player) {
    return { sell: 0, trade: 0, hold: 0 };
  }

  const heldShares = player.shares[defunctCompanyId] ?? 0;
  if (heldShares <= 0) {
    return { sell: 0, trade: 0, hold: 0 };
  }

  const effectiveDifficulty = difficulty ?? player.aiDifficulty ?? 'smart';

  const survivorId = state.pendingMerger?.survivingCompanyId;
  if (!survivorId) {
    return { sell: heldShares, trade: 0, hold: 0 };
  }

  const survivorComp = state.companies[survivorId];
  const defSurvivor = getCompanyDefinition(state.config.companies, survivorId);
  const defDefunct = getCompanyDefinition(state.config.companies, defunctCompanyId);

  const defunctPrice =
    state.pendingMerger?.preMergerPrices[defunctCompanyId] ??
    (defDefunct
      ? calculateSharePrice(defDefunct, state.companies[defunctCompanyId]?.size ?? 0)
      : 0);

  const survivorPrice =
    state.pendingMerger?.preMergerPrices[survivorId] ??
    (defSurvivor && survivorComp
      ? calculateSharePrice(defSurvivor, survivorComp.size)
      : 0);

  const availableSurvivorShares = survivorComp?.availableShares ?? 0;
  const maxPossibleTradePairs = Math.min(
    Math.floor(heldShares / 2),
    availableSurvivorShares
  );

  if (effectiveDifficulty === 'smart') {
    // 2-for-1 trade is favorable if 1 survivor share >= 2 defunct shares
    if (maxPossibleTradePairs > 0 && survivorPrice >= 2 * defunctPrice) {
      const tradeShares = maxPossibleTradePairs * 2;
      const remainingShares = heldShares - tradeShares;
      return {
        sell: remainingShares,
        trade: tradeShares,
        hold: 0,
      };
    }

    // Otherwise, liquidate all held shares into cash
    return {
      sell: heldShares,
      trade: 0,
      hold: 0,
    };
  }

  // Dumb AI: Can choose suboptimal hold/sell/trade decisions or random choices.
  const roll = rng();
  if (roll < 0.33) {
    // Suboptimally hold all shares
    return {
      sell: 0,
      trade: 0,
      hold: heldShares,
    };
  } else if (roll < 0.66) {
    // Liquidate all shares into cash (even if trade was 2-for-1 profitable)
    return {
      sell: heldShares,
      trade: 0,
      hold: 0,
    };
  } else {
    // Trade as many as possible (even if trade was unfavorable)
    const tradeShares = maxPossibleTradePairs * 2;
    const remaining = heldShares - tradeShares;
    return {
      sell: 0,
      trade: tradeShares,
      hold: remaining,
    };
  }
}

/**
 * Simulates one complete turn for the active AI player, executing all intermediate
 * actions (tile placement, founding choices, merger survivor selection, share disposition,
 * stock purchasing, and turn ending) until the turn completes or the game ends.
 */
export function simulateAiTurn(
  state: GameState,
  difficulty?: 'dumb' | 'smart'
): GameState {
  if (state.phase === 'game_over') {
    return state;
  }

  const startingActiveIndex = state.activePlayerIndex;
  let currentState = state;
  let safetyStepCount = 0;
  const MAX_TURN_STEPS = 500;

  while (
    currentState.phase !== 'game_over' &&
    currentState.activePlayerIndex === startingActiveIndex &&
    safetyStepCount < MAX_TURN_STEPS
  ) {
    safetyStepCount++;
    const activePlayer = currentState.players[currentState.activePlayerIndex];
    const currentDifficulty = difficulty ?? activePlayer?.aiDifficulty ?? 'smart';

    switch (currentState.phase) {
      case 'place_tile': {
        if (!activePlayer) {
          throw new Error(`Player not found at index ${currentState.activePlayerIndex}`);
        }

        // 1. Discard any dead tiles in hand first
        const deadTile = activePlayer.hand.find((t) =>
          isTileDead(
            currentState.board,
            t,
            currentState.companies,
            currentState.config.safeThreshold
          )
        );

        if (deadTile) {
          currentState = gameReducer(currentState, {
            type: 'DISCARD_DEAD_TILE',
            coord: deadTile,
          });
          break;
        }

        // 2. Select best legal playable tile
        const bestMove = getBestTileMove(
          currentState,
          currentState.activePlayerIndex,
          currentDifficulty
        );
        if (bestMove) {
          currentState = gameReducer(currentState, {
            type: 'PLACE_TILE',
            coord: bestMove,
          });
          break;
        }

        // 2b. If active AI holds a wildcard blank piece, find the highest utility legal target
        const blankTileInHand = activePlayer.hand.find((t) => t.isBlank);
        if (blankTileInHand) {
          let bestTarget: TileCoord | null = null;
          let bestTargetScore = -Infinity;
          for (let r = 0; r < currentState.config.boardRows; r++) {
            for (let c = 0; c < currentState.config.boardCols; c++) {
              const target: TileCoord = { row: r, col: c };
              if (isTileLegalToPlay(target, currentState.board, currentState.companies, currentState.config)) {
                const baseScore = evaluateTileMoveUtility(currentState, activePlayer, target);
                const score = currentDifficulty === 'dumb'
                  ? baseScore + Math.random() * 10000
                  : baseScore;
                if (score > bestTargetScore) {
                  bestTargetScore = score;
                  bestTarget = target;
                }
              }
            }
          }
          if (bestTarget) {
            currentState = gameReducer(currentState, {
              type: 'PLACE_BLANK_TILE',
              blankTileCoord: blankTileInHand,
              targetCoord: bestTarget,
            });
            break;
          }
        }

        // 3. No playable tiles and no dead tiles
        if (isGameOver(currentState)) {
          const liquidation = liquidateGame(currentState);
          return {
            ...currentState,
            players: liquidation.players,
            phase: 'game_over',
            winnerPlayerId: liquidation.winnerPlayerId,
          };
        }

        // Advance to share purchase phase
        currentState = {
          ...currentState,
          phase: 'buy_shares',
        };
        break;
      }

      case 'choose_founding_company': {
        const available = currentState.pendingFounding?.availableCompanyIds ?? [];
        const companyId = getBestFoundedCompany(
          currentState,
          available,
          currentDifficulty
        );
        currentState = gameReducer(currentState, {
          type: 'CHOOSE_FOUNDED_COMPANY',
          companyId,
        });
        break;
      }

      case 'choose_merger_survivor': {
        const participating =
          currentState.pendingMerger?.participatingCompanyIds ?? [];
        const survivorCheck = determineSurvivingCompany(
          participating,
          currentState.companies
        );

        const validSurvivorIds = survivorCheck.hasTie
          ? survivorCheck.tiedCompanyIds
          : survivorCheck.survivorId
            ? [survivorCheck.survivorId]
            : participating;

        const survivorCompanyId = getBestMergerSurvivor(
          currentState,
          validSurvivorIds,
          currentDifficulty
        );
        currentState = gameReducer(currentState, {
          type: 'CHOOSE_MERGER_SURVIVOR',
          survivorCompanyId,
        });
        break;
      }

      case 'dispose_merger_shares': {
        const pm = currentState.pendingMerger;
        if (!pm || !pm.currentDefunctCompanyId) {
          throw new Error('Corrupt merger state during share disposition');
        }

        const shareholderId = pm.shareholderQueue[pm.currentShareholderIndex];
        const shareholderIndex = currentState.players.findIndex(
          (p) => p.id === shareholderId
        );
        if (shareholderIndex === -1) {
          throw new Error(`Shareholder ${shareholderId} not found`);
        }

        const shareholder = currentState.players[shareholderIndex];
        const shareholderDifficulty =
          difficulty ?? shareholder?.aiDifficulty ?? 'smart';

        const choices = getBestMergerDisposition(
          currentState,
          shareholderIndex,
          pm.currentDefunctCompanyId,
          shareholderDifficulty
        );

        currentState = gameReducer(currentState, {
          type: 'DISPOSE_MERGER_SHARES',
          choices,
        });
        break;
      }

      case 'buy_shares': {
        const purchases = getBestSharePurchases(
          currentState,
          currentState.activePlayerIndex,
          currentDifficulty
        );
        if (purchases.length > 0) {
          currentState = gameReducer(currentState, {
            type: 'BUY_SHARES',
            purchases,
          });
        }

        currentState = gameReducer(currentState, { type: 'END_TURN' });
        break;
      }

      default:
        throw new Error(`Unhandled turn phase in AI turn: ${currentState.phase}`);
    }
  }

  if (safetyStepCount >= MAX_TURN_STEPS) {
    throw new Error(
      `simulateAiTurn exceeded ${MAX_TURN_STEPS} steps in phase '${currentState.phase}'`
    );
  }

  return currentState;
}

/**
 * Simulates a complete multi-player bot game from initialization to game over.
 * Executes turns deterministically without deadlocks or unhandled exceptions.
 */
export function playFullGameAi(
  config: GameConfig,
  playerCount: number,
  seed?: number,
  maxTurns: number = 1000,
  defaultDifficulty: 'dumb' | 'smart' = 'smart'
): GameState {
  const players = Array.from({ length: playerCount }, (_, i) => ({
    id: `p${i + 1}`,
    name: `Bot ${i + 1}`,
    isAi: true,
    aiDifficulty: defaultDifficulty,
  }));

  let state = createInitialGameState(config, players, seed);
  let turns = 0;

  while (state.phase !== 'game_over' && turns < maxTurns) {
    state = simulateAiTurn(state);
    turns++;
  }

  if (state.phase !== 'game_over' && isGameOver(state)) {
    const liquidation = liquidateGame(state);
    return {
      ...state,
      players: liquidation.players,
      phase: 'game_over',
      winnerPlayerId: liquidation.winnerPlayerId,
    };
  }

  return state;
}
