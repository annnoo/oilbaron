import { DEFAULT_CONFIG } from './config';
import type {
  CompanyDefinition,
  CompanyId,
  CompanyState,
  PlayerState,
  SharePurchase,
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
