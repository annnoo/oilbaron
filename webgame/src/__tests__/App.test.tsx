import { describe, it, expect } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import App from '../App';

describe('App Integration', () => {
  it('renders App shell containing MenuBar and CompanyTicker', () => {
    const html = renderToStaticMarkup(<App />);

    // Top menu bar
    expect(html).toContain('role="menubar"');
    expect(html).toContain('<span class="mnemonic">G</span>ame');
    expect(html).toContain('<span class="mnemonic">A</span>ctions');
    expect(html).toContain('<span class="mnemonic">V</span>iews');
    expect(html).toContain('<span class="mnemonic">S</span>ettings');
    expect(html).toContain('<span class="mnemonic">H</span>elp');

    // Company ticker
    expect(html).toContain('role="region"');
    expect(html).toContain('MARKET');
    expect(html).toContain('Available:');

    // App panel verification
    expect(html).toContain('Company Ticker Controls');
    expect(html).toContain('Found American');
    expect(html).toContain('Make American Safe (11 tiles)');
  });
});
