import { describe, it, expect } from 'vitest';
import {
  calculateSharePrice,
  calculateNetWorth,
  canBuyShares,
  buyShares,
  validateShareSales,
  sellShares,
  sellAllShares,
  calculateCompanyDividend,
  calculatePlayerDividends,
} from '../market';
import type { CompanyDefinition, CompanyState, PlayerState, SharePurchase, ShareSale } from '../types';

describe('Stock Market & Pricing Formulas', () => {
  const lowDef: CompanyDefinition = {
    id: 'american',
    name: 'American',
    tier: 'low',
    basePrice: 200,
    priceDelta: 100,
    color: 'red',
  };
  const midDef: CompanyDefinition = {
    id: 'imperial',
    name: 'Imperial',
    tier: 'mid',
    basePrice: 300,
    priceDelta: 100,
    color: 'blue',
  };
  const highDef: CompanyDefinition = {
    id: 'royal',
    name: 'Royal',
    tier: 'high',
    basePrice: 400,
    priceDelta: 100,
    color: 'gold',
  };

  it('calculates stock price accurately across tiers and size bounds', () => {
    // Inactive company has price 0
    expect(calculateSharePrice(lowDef, 0)).toBe(0);
    expect(calculateSharePrice(lowDef, 1)).toBe(0);
    expect(calculateSharePrice(lowDef, -1)).toBe(0);

    // Size 2 (base price)
    expect(calculateSharePrice(lowDef, 2)).toBe(200);
    expect(calculateSharePrice(midDef, 2)).toBe(300);
    expect(calculateSharePrice(highDef, 2)).toBe(400);

    // Size 5 (+3 deltas)
    expect(calculateSharePrice(lowDef, 5)).toBe(500);

    // Size 11 (Safe threshold: +9 deltas)
    expect(calculateSharePrice(lowDef, 11)).toBe(200 + 9 * 100); // 1100
    expect(calculateSharePrice(midDef, 11)).toBe(300 + 9 * 100); // 1200
    expect(calculateSharePrice(highDef, 11)).toBe(400 + 9 * 100); // 1300

    // Size 41+ (Cap at 41 tiles: 2 + 39 = 41)
    expect(calculateSharePrice(lowDef, 41)).toBe(200 + 39 * 100); // 4100
    expect(calculateSharePrice(lowDef, 50)).toBe(4100); // capped
    expect(calculateSharePrice(highDef, 45)).toBe(400 + 39 * 100); // 4300 capped
  });

  it('calculates player net worth including cash and stock values', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 2500,
      shares: { american: 5, royal: 2, imperial: 10 },
      hand: [],
    };
    const companyStates: Record<string, CompanyState> = {
      american: { id: 'american', size: 11, isActive: true } as any,
      royal: { id: 'royal', size: 2, isActive: true } as any,
      imperial: { id: 'imperial', size: 0, isActive: false } as any, // inactive company worth $0
    };
    const defs = { american: lowDef, royal: highDef, imperial: midDef };

    // Net worth = 2500 + (5 * 1100) + (2 * 400) + (10 * 0) = 2500 + 5500 + 800 + 0 = 8800
    const netWorth = calculateNetWorth(player, companyStates, defs);
    expect(netWorth).toBe(8800);
  });

  it('calculates player net worth when player has only cash and no shares', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 6000,
      shares: {},
      hand: [],
    };
    expect(calculateNetWorth(player, {}, {})).toBe(6000);
  });

  it('executes share purchases deducting cash and bank inventory', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 3000,
      shares: { american: 0 },
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 5,
      isActive: true,
      availableShares: 20,
    } as any;

    const result = buyShares(
      player,
      { american: company },
      { american: lowDef },
      [{ companyId: 'american', count: 3 }],
      3
    );

    expect(result.success).toBe(true);
    // Price at size 5 = 200 + 3 * 100 = 500. Total cost = 1500.
    expect(result.player.cash).toBe(1500);
    expect(result.player.shares.american).toBe(3);
    expect(result.companies.american?.availableShares).toBe(17);
    expect(result.totalCost).toBe(1500);

    // Verify immutability of input objects
    expect(player.cash).toBe(3000);
    expect(player.shares.american).toBe(0);
    expect(company.availableShares).toBe(20);
  });

  it('allows purchasing 0 shares or empty purchases list', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 3000,
      shares: { american: 1 },
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 5,
      isActive: true,
      availableShares: 20,
    } as any;

    expect(canBuyShares(player, { american: company }, { american: lowDef }, [])).toBe(true);

    const result = buyShares(player, { american: company }, { american: lowDef }, []);
    expect(result.success).toBe(true);
    expect(result.player.cash).toBe(3000);
    expect(result.totalCost).toBe(0);
    expect(result.player.shares.american).toBe(1);
    expect(result.companies.american?.availableShares).toBe(20);
  });

  it('supports multi-company share purchases up to maxSharesPerPurchase limit', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 5000,
      shares: { american: 0, royal: 1 },
      hand: [],
    };
    const americanCompany: CompanyState = {
      id: 'american',
      size: 2, // Price = 200
      isActive: true,
      availableShares: 10,
    } as any;
    const royalCompany: CompanyState = {
      id: 'royal',
      size: 2, // Price = 400
      isActive: true,
      availableShares: 5,
    } as any;

    const companies = { american: americanCompany, royal: royalCompany };
    const defs = { american: lowDef, royal: highDef };
    const purchases: SharePurchase[] = [
      { companyId: 'american', count: 2 },
      { companyId: 'royal', count: 1 },
    ];

    expect(canBuyShares(player, companies, defs, purchases, 3)).toBe(true);

    const result = buyShares(player, companies, defs, purchases, 3);
    expect(result.success).toBe(true);
    // Cost = 2 * 200 + 1 * 400 = 800. Cash = 5000 - 800 = 4200.
    expect(result.totalCost).toBe(800);
    expect(result.player.cash).toBe(4200);
    expect(result.player.shares.american).toBe(2);
    expect(result.player.shares.royal).toBe(2);
    expect(result.companies.american?.availableShares).toBe(8);
    expect(result.companies.royal?.availableShares).toBe(4);
  });

  it('rejects purchase when requested shares exceed maxSharesPerPurchase', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 10000,
      shares: {},
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 2,
      isActive: true,
      availableShares: 20,
    } as any;

    const purchases: SharePurchase[] = [{ companyId: 'american', count: 4 }];
    expect(canBuyShares(player, { american: company }, { american: lowDef }, purchases, 3)).toBe(false);

    const result = buyShares(player, { american: company }, { american: lowDef }, purchases, 3);
    expect(result.success).toBe(false);
    expect(result.error).toMatch(/exceed/i);
    expect(result.player.cash).toBe(10000);
  });

  it('rejects purchase when player has insufficient cash', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 300,
      shares: {},
      hand: [],
    };
    const company: CompanyState = {
      id: 'royal',
      size: 2, // Price = 400
      isActive: true,
      availableShares: 10,
    } as any;

    const purchases: SharePurchase[] = [{ companyId: 'royal', count: 1 }];
    expect(canBuyShares(player, { royal: company }, { royal: highDef }, purchases, 3)).toBe(false);

    const result = buyShares(player, { royal: company }, { royal: highDef }, purchases, 3);
    expect(result.success).toBe(false);
    expect(result.error).toMatch(/cash/i);
    expect(result.player.cash).toBe(300);
  });

  it('rejects purchase when company has insufficient bank shares', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 5000,
      shares: {},
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 2,
      isActive: true,
      availableShares: 1,
    } as any;

    const purchases: SharePurchase[] = [{ companyId: 'american', count: 2 }];
    expect(canBuyShares(player, { american: company }, { american: lowDef }, purchases, 3)).toBe(false);

    const result = buyShares(player, { american: company }, { american: lowDef }, purchases, 3);
    expect(result.success).toBe(false);
    expect(result.error).toMatch(/available/i);
  });

  it('rejects purchase when company is inactive or not on board', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 5000,
      shares: {},
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 0,
      isActive: false,
      availableShares: 25,
    } as any;

    const purchases: SharePurchase[] = [{ companyId: 'american', count: 1 }];
    expect(canBuyShares(player, { american: company }, { american: lowDef }, purchases, 3)).toBe(false);

    const result = buyShares(player, { american: company }, { american: lowDef }, purchases, 3);
    expect(result.success).toBe(false);
    expect(result.error).toMatch(/active/i);
  });

  it('rejects purchase when share count is negative or invalid', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 5000,
      shares: {},
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 2,
      isActive: true,
      availableShares: 25,
    } as any;

    const purchases: SharePurchase[] = [{ companyId: 'american', count: -1 }];
    expect(canBuyShares(player, { american: company }, { american: lowDef }, purchases, 3)).toBe(false);

    const result = buyShares(player, { american: company }, { american: lowDef }, purchases, 3);
    expect(result.success).toBe(false);
  });

  it('aggregates multiple purchase items targeting the same company', () => {
    const player: PlayerState = {
      id: 'p1',
      name: 'Player 1',
      isAi: false,
      cash: 5000,
      shares: { american: 2 },
      hand: [],
    };
    const company: CompanyState = {
      id: 'american',
      size: 2, // Price = 200
      isActive: true,
      availableShares: 20,
    } as any;

    // 1 + 2 = 3 shares in same company
    const purchases: SharePurchase[] = [
      { companyId: 'american', count: 1 },
      { companyId: 'american', count: 2 },
    ];

    expect(canBuyShares(player, { american: company }, { american: lowDef }, purchases, 3)).toBe(true);

    const result = buyShares(player, { american: company }, { american: lowDef }, purchases, 3);
    expect(result.success).toBe(true);
    expect(result.totalCost).toBe(600); // 3 * 200
    expect(result.player.cash).toBe(4400);
    expect(result.player.shares.american).toBe(5); // 2 previous + 3
    expect(result.companies.american?.availableShares).toBe(17); // 20 - 3
  });

  describe('Selling Shares Mechanics', () => {
    it('validates selling shares: checks ownership and active status', () => {
      const player: PlayerState = {
        id: 'p1',
        name: 'Player 1',
        isAi: false,
        cash: 1000,
        shares: { american: 5, royal: 2, imperial: 10 },
        hand: [],
      };
      const companies: Record<string, CompanyState> = {
        american: { id: 'american', size: 5, isActive: true, availableShares: 15 } as any, // Price = 200 + 3*100 = 500
        royal: { id: 'royal', size: 2, isActive: true, availableShares: 20 } as any, // Price = 400
        imperial: { id: 'imperial', size: 0, isActive: false, availableShares: 25 } as any, // Inactive
      };
      const defs = { american: lowDef, royal: highDef, imperial: midDef };

      // Valid sale: 2 American ($1000) and 1 Royal ($400)
      const sales: ShareSale[] = [
        { companyId: 'american', count: 2 },
        { companyId: 'royal', count: 1 },
      ];
      const validRes = validateShareSales(player, companies, defs, sales);
      expect(validRes.valid).toBe(true);
      expect(validRes.totalRevenue).toBe(1400);

      // Invalid: selling more shares than owned
      const excessSale: ShareSale[] = [{ companyId: 'american', count: 6 }];
      const excessRes = validateShareSales(player, companies, defs, excessSale);
      expect(excessRes.valid).toBe(false);
      expect(excessRes.reason).toContain('exceeds owned shares');

      // Invalid: selling shares in an inactive company
      const inactiveSale: ShareSale[] = [{ companyId: 'imperial', count: 2 }];
      const inactiveRes = validateShareSales(player, companies, defs, inactiveSale);
      expect(inactiveRes.valid).toBe(false);
      expect(inactiveRes.reason).toContain('not currently active');

      // Invalid: negative count
      const negSale: ShareSale[] = [{ companyId: 'american', count: -1 }];
      expect(validateShareSales(player, companies, defs, negSale).valid).toBe(false);
    });

    it('executes sellShares immutably, crediting cash and replenishing bank inventory', () => {
      const player: PlayerState = {
        id: 'p1',
        name: 'Player 1',
        isAi: false,
        cash: 1000,
        shares: { american: 5 },
        hand: [],
      };
      const companies: Record<string, CompanyState> = {
        american: { id: 'american', size: 4, isActive: true, availableShares: 10 } as any, // Price = 200 + 2*100 = 400
      };
      const defs = { american: lowDef };

      const sales: ShareSale[] = [{ companyId: 'american', count: 3 }];
      const result = sellShares(player, companies, defs, sales);

      expect(result.success).toBe(true);
      expect(result.totalRevenue).toBe(1200); // 3 * 400
      expect(result.player.cash).toBe(2200); // 1000 + 1200
      expect(result.player.shares.american).toBe(2); // 5 - 3
      expect(result.companies.american?.availableShares).toBe(13); // 10 + 3
    });

    it('executes sellAllShares to fully liquidate all active holdings', () => {
      const player: PlayerState = {
        id: 'p1',
        name: 'Player 1',
        isAi: false,
        cash: 500,
        shares: { american: 4, royal: 2, imperial: 8 },
        hand: [],
      };
      const companies: Record<string, CompanyState> = {
        american: { id: 'american', size: 2, isActive: true, availableShares: 10 } as any, // Price = 200 => 4 * 200 = 800
        royal: { id: 'royal', size: 3, isActive: true, availableShares: 15 } as any, // Price = 400 + 100 = 500 => 2 * 500 = 1000
        imperial: { id: 'imperial', size: 0, isActive: false, availableShares: 20 } as any, // Inactive, not sold!
      };
      const defs = { american: lowDef, royal: highDef, imperial: midDef };

      const result = sellAllShares(player, companies, defs);
      expect(result.success).toBe(true);
      expect(result.totalRevenue).toBe(1800); // 800 + 1000
      expect(result.player.cash).toBe(2300); // 500 + 1800
      expect(result.player.shares.american).toBe(0);
      expect(result.player.shares.royal).toBe(0);
      expect(result.player.shares.imperial).toBe(8); // Inactive remains
      expect(result.companies.american?.availableShares).toBe(14); // 10 + 4
      expect(result.companies.royal?.availableShares).toBe(17); // 15 + 2
    });
  });

  describe('Dividends System', () => {
    it('calculates company dividend yield proportionally to share price', () => {
      const highWithYield: CompanyDefinition = {
        ...highDef,
        dividendYield: 0.125, // 12.5% for high tier
      };
      const lowWithYield: CompanyDefinition = {
        ...lowDef,
        dividendYield: 0.067, // 6.7% for low tier
      };

      // Royal at size 2 has price $400. 12.5% of 400 = $50 per share
      expect(calculateCompanyDividend(highWithYield, 2)).toBe(50);

      // American at size 2 has price $200. 6.7% of 200 = round(13.4) = $13 per share
      expect(calculateCompanyDividend(lowWithYield, 2)).toBe(13);

      // Inactive company (size 0) yields $0
      expect(calculateCompanyDividend(highWithYield, 0)).toBe(0);
    });

    it('calculates total dividends for a player across active companies', () => {
      const defs = {
        american: { ...lowDef, dividendYield: 0.067 },
        royal: { ...highDef, dividendYield: 0.125 },
      };
      const player: PlayerState = {
        id: 'p1',
        name: 'Player 1',
        isAi: false,
        cash: 1000,
        shares: { american: 10, royal: 4 },
        hand: [],
      };
      const companies: Record<string, CompanyState> = {
        american: { id: 'american', size: 2, isActive: true } as any, // $13 per share * 10 = $130
        royal: { id: 'royal', size: 2, isActive: true } as any, // $50 per share * 4 = $200
      };

      const result = calculatePlayerDividends(player, companies, defs);
      expect(result.totalDividend).toBe(330);
      expect(result.breakdown.american).toBe(130);
      expect(result.breakdown.royal).toBe(200);
    });
  });
});
