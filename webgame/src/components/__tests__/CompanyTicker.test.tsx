import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { CompanyTicker } from '../CompanyTicker';
import { CLASSIC_COMPANIES } from '../../engine/config';
import type { CompanyState } from '../../engine/types';

describe('CompanyTicker Component', () => {
  const mockInitialCompanies: Record<string, CompanyState> = {
    american: {
      id: 'american',
      name: 'American Natural Gas',
      tier: 'low',
      color: '#e63946',
      size: 0,
      isActive: false,
      isSafe: false,
      availableShares: 25,
    },
    arabian: {
      id: 'arabian',
      name: 'Arabian Oil',
      tier: 'low',
      color: '#f4a261',
      size: 0,
      isActive: false,
      isSafe: false,
      availableShares: 25,
    },
    imperial: {
      id: 'imperial',
      name: 'Imperial Petroleum',
      tier: 'mid',
      color: '#2a9d8f',
      size: 0,
      isActive: false,
      isSafe: false,
      availableShares: 25,
    },
  };

  it('renders empty notice and available unformed companies when no company is active', () => {
    const html = renderToStaticMarkup(
      <CompanyTicker companies={mockInitialCompanies} />
    );

    expect(html).toContain('No active companies on board yet');
    expect(html).toContain('Available:');
    expect(html).toContain('American Natural Gas');
    expect(html).toContain('$200'); // Low tier base price
    expect(html).toContain('Imperial Petroleum');
    expect(html).toContain('$300'); // Mid tier base price
  });

  it('renders active company card with size, live share price, and bank inventory', () => {
    const activeCompanies: Record<string, CompanyState> = {
      ...mockInitialCompanies,
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 5, // size 5 -> basePrice $200 + 3 * $100 = $500
        isActive: true,
        isSafe: false,
        availableShares: 18,
      },
    };

    const html = renderToStaticMarkup(
      <CompanyTicker companies={activeCompanies} totalShares={25} />
    );

    expect(html).toContain('1 Active');
    expect(html).toContain('American Natural Gas');
    expect(html).toContain('5'); // size
    expect(html).toContain('$500'); // calculated price
    expect(html).toContain('18'); // bank shares
    expect(html).toContain('/25'); // total shares
    expect(html).not.toContain('SAFE'); // Not safe yet (< 11 tiles)
  });

  it('renders SAFE badge when company size reaches safeThreshold (>= 11 tiles)', () => {
    const activeCompanies: Record<string, CompanyState> = {
      ...mockInitialCompanies,
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 11,
        isActive: true,
        isSafe: false, // size >= 11 triggers safe badge
        availableShares: 14,
      },
    };

    const html = renderToStaticMarkup(
      <CompanyTicker companies={activeCompanies} safeThreshold={11} />
    );

    expect(html).toContain('SAFE');
    expect(html).toContain('badge-safe');
    expect(html).toContain('11'); // size
    // Price for size 11 low tier: $200 + (11-2)*100 = $1,100
    expect(html).toContain('$1,100');
  });

  it('does NOT render SAFE badge when company size is 10 tiles (< safeThreshold)', () => {
    const activeCompanies: Record<string, CompanyState> = {
      ...mockInitialCompanies,
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 10,
        isActive: true,
        isSafe: false,
        availableShares: 15,
      },
    };

    const html = renderToStaticMarkup(
      <CompanyTicker companies={activeCompanies} safeThreshold={11} />
    );

    expect(html).not.toContain('SAFE');
  });

  it('renders SAFE badge when isSafe property is explicitly true regardless of size', () => {
    const activeCompanies: Record<string, CompanyState> = {
      ...mockInitialCompanies,
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 8,
        isActive: true,
        isSafe: true,
        availableShares: 10,
      },
    };

    const html = renderToStaticMarkup(
      <CompanyTicker companies={activeCompanies} safeThreshold={11} />
    );

    expect(html).toContain('SAFE');
  });

  it('handles multiple active companies and remaining unformed summary', () => {
    const activeCompanies: Record<string, CompanyState> = {
      ...mockInitialCompanies,
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 6,
        isActive: true,
        isSafe: false,
        availableShares: 15,
      },
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        size: 12,
        isActive: true,
        isSafe: true,
        availableShares: 10,
      },
    };

    const html = renderToStaticMarkup(
      <CompanyTicker companies={activeCompanies} />
    );

    expect(html).toContain('2 Active');
    expect(html).toContain('American Natural Gas');
    expect(html).toContain('Imperial Petroleum');
    expect(html).toContain('1 Unformed');
  });

  it('renders interactive button attributes when onSelectCompany callback is provided', () => {
    const activeCompanies: Record<string, CompanyState> = {
      ...mockInitialCompanies,
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        size: 4,
        isActive: true,
        isSafe: false,
        availableShares: 20,
      },
    };

    const onSelect = vi.fn();
    const html = renderToStaticMarkup(
      <CompanyTicker companies={activeCompanies} onSelectCompany={onSelect} />
    );

    expect(html).toContain('role="button"');
    expect(html).toContain('tabindex="0"');
  });
});
