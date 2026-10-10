import { DEFAULT_CONFIG } from './config';
import type {
  CompanyDefinition,
  CompanyId,
  CompanyState,
  PlayerState,
  SharePurchase,
  ShareSale,
} from './types';

export type CompanyDefinitionsInput =
  | Record<CompanyId, CompanyDefinition>
  | Partial<Record<CompanyId, CompanyDefinition>>
  | readonly CompanyDefinition[];

export interface SharePurchaseValidation {
  valid: boolean;
  reason?: string;
  totalCost: number;
  aggregatedCounts: Map<CompanyId, number>;
}

export interface BuySharesResult {
  success: boolean;
  error?: string;
  player: PlayerState;
  companies: Record<CompanyId, CompanyState>;
  totalCost: number;
}

export interface ShareSaleValidation {
  valid: boolean;
  reason?: string;
  totalRevenue: number;
  aggregatedCounts: Map<CompanyId, number>;
}

export interface SellSharesResult {
  success: boolean;
  error?: string;
  player: PlayerState;
  companies: Record<CompanyId, CompanyState>;
  totalRevenue: number;
  soldCounts: Record<CompanyId, number>;
}

export interface PlayerDividendsResult {
  totalDividend: number;
  breakdown: Record<CompanyId, number>;
}

/**
 * Resolves a company definition from either an array or a record mapping.
 */
export function getCompanyDefinition(
  definitions: CompanyDefinitionsInput,
  companyId: CompanyId
): CompanyDefinition | undefined {
  if (Array.isArray(definitions)) {
    return definitions.find((d) => d.id === companyId);
  }
  return (definitions as Record<CompanyId, CompanyDefinition>)[companyId];
}

/**
 * Calculates the market price per share of a company given its definition and board size.
 *
 * Price curve:
 * - Inactive or unformed companies (size < 2) have a share price of $0.
 * - Active companies (size >= 2) start at basePrice and gain priceDelta for each tile
 *   above size 2, capped at size 41 (basePrice + 39 * priceDelta).
 *
 * Formula: P(S) = basePrice + min(max(0, S - 2), 39) * priceDelta
 */
export function calculateSharePrice(
  definition: CompanyDefinition,
  size: number
): number {
  if (size < 2) {
    return 0;
  }
  const deltaSteps = Math.min(Math.max(0, size - 2), 39);
  return definition.basePrice + deltaSteps * definition.priceDelta;
}

/**
 * Calculates a player's total net worth:
 * Total cash + current market value of all held company stocks.
 */
export function calculateNetWorth(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput
): number {
  let netWorth = player.cash;

  for (const [companyId, shareCount] of Object.entries(player.shares)) {
    if (shareCount > 0) {
      const company = companyStates[companyId];
      if (company && company.isActive && company.size >= 2) {
        const def = getCompanyDefinition(definitions, companyId);
        if (def) {
          const price = calculateSharePrice(def, company.size);
          netWorth += shareCount * price;
        }
      }
    }
  }

  return netWorth;
}

/**
 * Validates whether a player can legally execute the requested share purchases.
 */
export function validateSharePurchases(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput,
  purchases: readonly SharePurchase[],
  maxSharesPerPurchase: number = DEFAULT_CONFIG.maxSharesPerPurchase
): SharePurchaseValidation {
  const aggregatedCounts = new Map<CompanyId, number>();

  for (const p of purchases) {
    if (p.count < 0 || !Number.isInteger(p.count)) {
      return {
        valid: false,
        reason: `Invalid share count requested for company ${p.companyId}: ${p.count}`,
        totalCost: 0,
        aggregatedCounts,
      };
    }
    if (p.count === 0) continue;
    aggregatedCounts.set(
      p.companyId,
      (aggregatedCounts.get(p.companyId) ?? 0) + p.count
    );
  }

  let totalShares = 0;
  for (const count of aggregatedCounts.values()) {
    totalShares += count;
  }

  if (totalShares > maxSharesPerPurchase) {
    return {
      valid: false,
      reason: `Requested share purchases (${totalShares}) exceed maximum allowed per purchase (${maxSharesPerPurchase})`,
      totalCost: 0,
      aggregatedCounts,
    };
  }

  let totalCost = 0;

  for (const [companyId, count] of aggregatedCounts.entries()) {
    const company = companyStates[companyId];
    if (!company || !company.isActive || company.size < 2) {
      return {
        valid: false,
        reason: `Company ${companyId} is not active on the board`,
        totalCost: 0,
        aggregatedCounts,
      };
    }

    const def = getCompanyDefinition(definitions, companyId);
    if (!def) {
      return {
        valid: false,
        reason: `Company definition not found for ${companyId}`,
        totalCost: 0,
        aggregatedCounts,
      };
    }

    if (company.availableShares < count) {
      return {
        valid: false,
        reason: `Insufficient shares available in bank for ${companyId} (requested ${count}, available ${company.availableShares})`,
        totalCost: 0,
        aggregatedCounts,
      };
    }

    const sharePrice = calculateSharePrice(def, company.size);
    totalCost += count * sharePrice;
  }

  if (player.cash < totalCost) {
    return {
      valid: false,
      reason: `Insufficient player cash (required $${totalCost}, player has $${player.cash})`,
      totalCost,
      aggregatedCounts,
    };
  }

  return {
    valid: true,
    totalCost,
    aggregatedCounts,
  };
}

/**
 * Evaluates whether a player can legally execute the requested share purchases.
 */
export function canBuyShares(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput,
  purchases: readonly SharePurchase[],
  maxSharesPerPurchase: number = DEFAULT_CONFIG.maxSharesPerPurchase
): boolean {
  return validateSharePurchases(
    player,
    companyStates,
    definitions,
    purchases,
    maxSharesPerPurchase
  ).valid;
}

/**
 * Immutably executes a share purchase transaction for a player:
 * - Deducts total cost from player cash
 * - Adds shares to player portfolio
 * - Decrements available bank shares for each purchased company
 */
export function buyShares(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput,
  purchases: readonly SharePurchase[],
  maxSharesPerPurchase: number = DEFAULT_CONFIG.maxSharesPerPurchase
): BuySharesResult {
  const validation = validateSharePurchases(
    player,
    companyStates,
    definitions,
    purchases,
    maxSharesPerPurchase
  );

  if (!validation.valid) {
    return {
      success: false,
      error: validation.reason,
      player,
      companies: { ...(companyStates as Record<CompanyId, CompanyState>) },
      totalCost: 0,
    };
  }

  const { totalCost, aggregatedCounts } = validation;

  const newShares: Record<CompanyId, number> = { ...player.shares };
  const newCompanies: Record<CompanyId, CompanyState> = {
    ...(companyStates as Record<CompanyId, CompanyState>),
  };

  for (const [companyId, count] of aggregatedCounts.entries()) {
    newShares[companyId] = (newShares[companyId] ?? 0) + count;
    const company = companyStates[companyId]!;
    newCompanies[companyId] = {
      ...company,
      availableShares: company.availableShares - count,
    };
  }

  const newPlayer: PlayerState = {
    ...player,
    cash: player.cash - totalCost,
    shares: newShares,
    hand: [...player.hand],
  };

  return {
    success: true,
    player: newPlayer,
    companies: newCompanies,
    totalCost,
  };
}

/**
 * Validates whether a player can legally execute the requested share sales.
 */
export function validateShareSales(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput,
  sales: readonly ShareSale[]
): ShareSaleValidation {
  const aggregatedCounts = new Map<CompanyId, number>();

  for (const s of sales) {
    if (typeof s.count !== 'number' || isNaN(s.count) || s.count < 0) {
      return {
        valid: false,
        reason: 'Share sale count must be a non-negative number',
        totalRevenue: 0,
        aggregatedCounts: new Map(),
      };
    }
    if (s.count === 0) continue;
    aggregatedCounts.set(s.companyId, (aggregatedCounts.get(s.companyId) ?? 0) + s.count);
  }

  let totalRevenue = 0;

  for (const [companyId, requestedCount] of aggregatedCounts.entries()) {
    const company = companyStates[companyId];
    const def = getCompanyDefinition(definitions, companyId);

    if (!company || !def || !company.isActive || company.size < 2) {
      return {
        valid: false,
        reason: `Company ${def?.name ?? companyId} is not currently active on the board`,
        totalRevenue: 0,
        aggregatedCounts,
      };
    }

    const ownedShares = player.shares[companyId] ?? 0;
    if (requestedCount > ownedShares) {
      return {
        valid: false,
        reason: `Requested sale of ${requestedCount} shares in ${def.name} exceeds owned shares (${ownedShares})`,
        totalRevenue: 0,
        aggregatedCounts,
      };
    }

    const price = calculateSharePrice(def, company.size);
    totalRevenue += requestedCount * price;
  }

  return {
    valid: true,
    totalRevenue,
    aggregatedCounts,
  };
}

/**
 * Immutably executes a share sale transaction:
 * - Deducts shares from player portfolio
 * - Adds proceeds to player cash
 * - Returns sold shares to company bank inventory
 */
export function sellShares(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput,
  sales: readonly ShareSale[]
): SellSharesResult {
  const validation = validateShareSales(player, companyStates, definitions, sales);

  if (!validation.valid) {
    return {
      success: false,
      error: validation.reason,
      player,
      companies: { ...(companyStates as Record<CompanyId, CompanyState>) },
      totalRevenue: 0,
      soldCounts: {},
    };
  }

  const { totalRevenue, aggregatedCounts } = validation;
  const newShares: Record<CompanyId, number> = { ...player.shares };
  const newCompanies: Record<CompanyId, CompanyState> = {
    ...(companyStates as Record<CompanyId, CompanyState>),
  };
  const soldCounts: Record<CompanyId, number> = {};

  for (const [companyId, count] of aggregatedCounts.entries()) {
    newShares[companyId] = (newShares[companyId] ?? 0) - count;
    soldCounts[companyId] = count;
    const company = companyStates[companyId]!;
    newCompanies[companyId] = {
      ...company,
      availableShares: company.availableShares + count,
    };
  }

  const newPlayer: PlayerState = {
    ...player,
    cash: player.cash + totalRevenue,
    shares: newShares,
    hand: [...player.hand],
  };

  return {
    success: true,
    player: newPlayer,
    companies: newCompanies,
    totalRevenue,
    soldCounts,
  };
}

/**
 * Sells all shares held by a player in all currently active companies.
 */
export function sellAllShares(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput
): SellSharesResult {
  const sales: ShareSale[] = [];

  for (const [companyId, count] of Object.entries(player.shares)) {
    if (count > 0) {
      const company = companyStates[companyId];
      if (company && company.isActive && company.size >= 2) {
        sales.push({ companyId, count });
      }
    }
  }

  return sellShares(player, companyStates, definitions, sales);
}

/**
 * Calculates the dividend per share for an active company.
 */
export function calculateCompanyDividend(
  definition: CompanyDefinition,
  size: number
): number {
  if (size < 2) return 0;
  const price = calculateSharePrice(definition, size);
  const yieldRate =
    definition.dividendYield ??
    (definition.tier === 'high' ? 0.125 : definition.tier === 'mid' ? 0.10 : 0.067);
  return Math.round(price * yieldRate);
}

/**
 * Calculates total dividend earnings for a player across all active holdings.
 */
export function calculatePlayerDividends(
  player: PlayerState,
  companyStates: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>,
  definitions: CompanyDefinitionsInput
): PlayerDividendsResult {
  let totalDividend = 0;
  const breakdown: Record<CompanyId, number> = {};

  for (const [companyId, count] of Object.entries(player.shares)) {
    if (count > 0) {
      const company = companyStates[companyId];
      if (company && company.isActive && company.size >= 2) {
        const def = getCompanyDefinition(definitions, companyId);
        if (def) {
          const divPerShare = calculateCompanyDividend(def, company.size);
          const compTotal = count * divPerShare;
          if (compTotal > 0) {
            breakdown[companyId] = compTotal;
            totalDividend += compTotal;
          }
        }
      }
    }
  }

  return { totalDividend, breakdown };
}
