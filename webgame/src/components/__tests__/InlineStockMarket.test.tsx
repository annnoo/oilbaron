import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest';
import { InlineStockMarket } from '../InlineStockMarket';
import { CLASSIC_COMPANIES } from '../../engine/config';
import type { CompanyState } from '../../engine/types';
import * as audioModule from '../../assets/audio';

// Register toBeInTheDocument matcher for vitest Chai
expect.extend({
  toBeInTheDocument(received) {
    const pass = Boolean(received);
    return {
      pass,
      message: () =>
        pass
          ? 'expected element not to be in document'
          : 'expected element to be in document',
    };
  },
});

declare module 'vitest' {
  interface Assertion<T = any> {
    toBeInTheDocument(): T;
  }
}

// Lightweight stateful render and screen harness for Vitest node environment
const internals =
  (React as any).__CLIENT_INTERNALS_DO_NOT_USE_OR_WARN_USERS_THEY_CANNOT_UPGRADE ||
  (React as any).__SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED;

let hookStates: any[] = [];
let hookIdx = 0;
let currentTree: any = null;
let currentHtml = '';
let rootElement: React.ReactElement | null = null;
let activeEffects: (() => void | (() => void))[] = [];
let activeCleanups: (() => void)[] = [];

const windowListeners: Record<string, { handler: (e: any) => void; useCapture?: boolean }[]> = {};

if (typeof (globalThis as any).window === 'undefined') {
  (globalThis as any).window = globalThis;
}

(globalThis as any).window.addEventListener = (
  type: string,
  handler: (e: any) => void,
  options?: boolean | AddEventListenerOptions
) => {
  const useCapture = typeof options === 'boolean' ? options : options?.capture;
  windowListeners[type] = windowListeners[type] || [];
  windowListeners[type].push({ handler, useCapture });
};

(globalThis as any).window.removeEventListener = (
  type: string,
  handler: (e: any) => void,
  options?: boolean | EventListenerOptions
) => {
  if (windowListeners[type]) {
    windowListeners[type] = windowListeners[type].filter((l) => l.handler !== handler);
  }
};

const testDispatcher = {
  useState: (initialValue: any) => {
    const idx = hookIdx++;
    if (hookStates[idx] === undefined) {
      hookStates[idx] =
        typeof initialValue === 'function' ? initialValue() : initialValue;
    }
    const setter = (next: any) => {
      hookStates[idx] =
        typeof next === 'function' ? next(hookStates[idx]) : next;
      triggerRender();
    };
    return [hookStates[idx], setter];
  },
  useRef: (initialValue: any) => {
    const idx = hookIdx++;
    if (hookStates[idx] === undefined) {
      hookStates[idx] = { current: initialValue };
    }
    return hookStates[idx];
  },
  useEffect: (effect: () => void | (() => void), deps?: any[]) => {
    const idx = hookIdx++;
    const prevDeps = hookStates[idx];
    const hasChanged = !prevDeps || !deps || deps.some((d, i) => d !== prevDeps[i]);
    if (hasChanged) {
      hookStates[idx] = deps;
      activeEffects.push(effect);
    }
  },
  useCallback: (fn: any) => fn,
  useMemo: (fn: any) => fn(),
};

function evaluateTree(node: any, path: string = '0'): any {
  if (!node || typeof node !== 'object') return node;
  if (typeof node.type === 'function') {
    return evaluateTree(node.type(node.props), path);
  }
  const cloned = { ...node, _path: path };
  if (node.props?.children) {
    if (Array.isArray(node.props.children)) {
      cloned.props = {
        ...node.props,
        children: node.props.children.map((c: any, i: number) =>
          evaluateTree(c, `${path}.${i}`)
        ),
      };
    } else {
      cloned.props = {
        ...node.props,
        children: evaluateTree(node.props.children, `${path}.0`),
      };
    }
  }
  return cloned;
}

function findNode(element: any, predicate: (el: any) => boolean): any {
  if (!element || typeof element !== 'object') return null;
  if (predicate(element)) return element;
  if (element.props?.children) {
    const children = Array.isArray(element.props.children)
      ? element.props.children
      : [element.props.children];
    for (const child of children) {
      const found = findNode(child, predicate);
      if (found) return found;
    }
  }
  return null;
}

function getNodeText(node: any): string {
  if (!node) return '';
  if (typeof node === 'string' || typeof node === 'number') return String(node);
  if (node.props?.children) {
    const children = Array.isArray(node.props.children)
      ? node.props.children
      : [node.props.children];
    return children.map(getNodeText).join(' ');
  }
  return '';
}

function triggerRender() {
  if (!rootElement) return;
  hookIdx = 0;
  activeEffects = [];
  if (internals) internals.H = testDispatcher;
  currentTree = evaluateTree(rootElement);
  if (internals) internals.H = null;
  currentHtml = renderToStaticMarkup(currentTree);
  for (const eff of activeEffects) {
    const cleanup = eff();
    if (typeof cleanup === 'function') {
      activeCleanups.push(cleanup);
    }
  }
}

function unmount() {
  for (const cleanup of activeCleanups) {
    cleanup();
  }
  activeCleanups = [];
  rootElement = null;
  currentTree = null;
  currentHtml = '';
}

function render(ui: React.ReactElement) {
  unmount();
  hookStates = [];
  rootElement = ui;
  triggerRender();
  return { html: currentHtml, unmount };
}

const decodeHtml = (str: string) =>
  str
    .replace(/&amp;/g, '&')
    .replace(/&lt;/g, '<')
    .replace(/&gt;/g, '>')
    .replace(/&quot;/g, '"')
    .replace(/&#39;/g, "'");

const screen = {
  getByText: (matcher: RegExp | string) => {
    const regex = typeof matcher === 'string' ? new RegExp(matcher, 'i') : matcher;
    const decoded = decodeHtml(currentHtml);
    const textOnly = decoded.replace(/<[^>]+>/g, ' ').replace(/\s+/g, ' ');
    const matches = regex.test(decoded) || regex.test(textOnly);
    if (!matches) {
      throw new Error(`Unable to find text matching ${matcher} in: ${textOnly}`);
    }
    return { matches, inDocument: true };
  },
  queryByRole: (role: string, options?: { name?: RegExp | string }) => {
    const targetName = options?.name;
    const nameRegex =
      typeof targetName === 'string'
        ? new RegExp(targetName, 'i')
        : targetName || /.*/;

    return findNode(currentTree, (el) => {
      if (el.type !== 'button') return false;
      const aria = el.props?.['aria-label'] || '';
      const title = el.props?.title || '';
      const text = getNodeText(el);
      return nameRegex.test(aria) || nameRegex.test(title) || nameRegex.test(text);
    });
  },
  getByRole: (role: string, options?: { name?: RegExp | string }) => {
    const node = screen.queryByRole(role, options);
    if (!node) {
      const targetName = options?.name;
      throw new Error(`Unable to find element with role ${role} and name ${targetName}`);
    }
    return node;
  },
};

const fireEvent = {
  click: (node: any) => {
    let target = node;
    if (target?._path && currentTree) {
      const fresh = findNode(currentTree, (el) => el._path === target._path);
      if (fresh) target = fresh;
    }
    if (target?.props?.disabled) {
      return;
    }
    if (target?.props?.onClick) {
      target.props.onClick({ preventDefault: () => {} });
    }
  },
  keyDown: (target: any, eventData: any) => {
    let stoppedImmediate = false;
    let stopped = false;
    let defaultPrevented = false;

    const event = {
      key: '',
      altKey: false,
      preventDefault: vi.fn(() => {
        defaultPrevented = true;
      }),
      stopPropagation: vi.fn(() => {
        stopped = true;
      }),
      stopImmediatePropagation: vi.fn(() => {
        stoppedImmediate = true;
      }),
      get defaultPrevented() {
        return defaultPrevented;
      },
      ...eventData,
    };

    if (target === (globalThis as any).window || target === globalThis) {
      const list = [...(windowListeners['keydown'] || [])];
      for (const entry of list) {
        if (stoppedImmediate) break;
        entry.handler(event);
      }
    }
    return event;
  },
};

describe('InlineStockMarket', () => {
  beforeEach(() => {
    vi.restoreAllMocks();
    for (const key of Object.keys(windowListeners)) {
      delete windowListeners[key];
    }
    unmount();
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  type MockCompany = CompanyState & { stockPrice?: number };

  const activeCompanies: Record<string, MockCompany> = {
    american: {
      id: 'american',
      name: 'American Natural Gas',
      tier: 'low',
      color: '#e63946',
      isActive: true,
      isSafe: false,
      size: 4,
      stockPrice: 400,
      availableShares: 15,
    },
  };

  it('renders active company and allows incrementing shares up to 3', () => {
    const onConfirm = vi.fn();
    render(
      <InlineStockMarket
        activePlayerCash={2000}
        companies={activeCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={onConfirm}
        onPass={vi.fn()}
      />
    );

    expect(screen.getByText(/American/i)).toBeInTheDocument();
    expect(screen.getByText(/\$400/i)).toBeInTheDocument();

    const incBtn = screen.getByRole('button', { name: /increase american/i });
    fireEvent.click(incBtn);
    fireEvent.click(incBtn);

    expect(screen.getByText(/2 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$800/i)).toBeInTheDocument();

    const confirmBtn = screen.getByRole('button', { name: /finish turn/i });
    fireEvent.click(confirmBtn);

    expect(onConfirm).toHaveBeenCalledWith([{ companyId: 'american', count: 2 }]);
  });

  it('triggers onPass when 0 shares selected and Finish Turn is clicked', () => {
    const onPass = vi.fn();
    const onConfirm = vi.fn();
    render(
      <InlineStockMarket
        activePlayerCash={2000}
        companies={activeCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={onConfirm}
        onPass={onPass}
      />
    );

    expect(screen.getByText(/Pass & Finish Turn/i)).toBeInTheDocument();
    expect(screen.getByText(/0 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$0/i)).toBeInTheDocument();

    const confirmBtn = screen.getByRole('button', { name: /finish turn/i });
    fireEvent.click(confirmBtn);

    expect(onPass).toHaveBeenCalledTimes(1);
    expect(onConfirm).not.toHaveBeenCalled();
  });

  it('enforces 3 shares limit across multiple corporations', () => {
    const multiCompanies: Record<string, MockCompany> = {
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        isActive: true,
        isSafe: false,
        size: 4,
        stockPrice: 400,
        availableShares: 10,
      },
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        isActive: true,
        isSafe: false,
        size: 5,
        stockPrice: 500,
        availableShares: 10,
      },
    };

    const onConfirm = vi.fn();
    render(
      <InlineStockMarket
        activePlayerCash={5000}
        companies={multiCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={onConfirm}
        onPass={vi.fn()}
      />
    );

    const incAmerican = screen.getByRole('button', { name: /increase american/i });
    const incImperial = screen.getByRole('button', { name: /increase imperial/i });

    fireEvent.click(incAmerican); // 1
    fireEvent.click(incAmerican); // 2
    fireEvent.click(incImperial); // 3

    expect(screen.getByText(/3 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$1300/i)).toBeInTheDocument();

    // 4th increment attempt should be blocked
    fireEvent.click(incImperial);
    expect(screen.getByText(/3 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$1300/i)).toBeInTheDocument();

    const confirmBtn = screen.getByRole('button', { name: /finish turn/i });
    fireEvent.click(confirmBtn);

    expect(onConfirm).toHaveBeenCalledWith([
      { companyId: 'american', count: 2 },
      { companyId: 'imperial', count: 1 },
    ]);
  });

  it('enforces player cash budget constraint', () => {
    render(
      <InlineStockMarket
        activePlayerCash={600} // can only afford 1 share at $400
        companies={activeCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={vi.fn()}
        onPass={vi.fn()}
      />
    );

    const incBtn = screen.getByRole('button', { name: /increase american/i });
    fireEvent.click(incBtn); // 1 share = $400 <= $600

    expect(screen.getByText(/1 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$400/i)).toBeInTheDocument();

    // 2nd share would cost $800 > $600, should be blocked
    fireEvent.click(incBtn);
    expect(screen.getByText(/1 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$400/i)).toBeInTheDocument();
  });

  it('enforces company available shares limit', () => {
    const limitedCompany: Record<string, MockCompany> = {
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        isActive: true,
        isSafe: false,
        size: 4,
        stockPrice: 400,
        availableShares: 1, // Only 1 share left in bank
      },
    };

    render(
      <InlineStockMarket
        activePlayerCash={5000}
        companies={limitedCompany}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={vi.fn()}
        onPass={vi.fn()}
      />
    );

    const incBtn = screen.getByRole('button', { name: /increase american/i });
    fireEvent.click(incBtn); // 1 share purchased
    expect(screen.getByText(/1 \/ 3 shares/i)).toBeInTheDocument();

    // 2nd share should be blocked because availableShares is 1
    fireEvent.click(incBtn);
    expect(screen.getByText(/1 \/ 3 shares/i)).toBeInTheDocument();
  });

  it('allows decrementing shares and removes entries with 0 count', () => {
    const onConfirm = vi.fn();
    render(
      <InlineStockMarket
        activePlayerCash={3000}
        companies={activeCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={onConfirm}
        onPass={vi.fn()}
      />
    );

    const incBtn = screen.getByRole('button', { name: /increase american/i });
    const decBtn = screen.getByRole('button', { name: /decrease american/i });

    // Decrement when 0 does nothing
    fireEvent.click(decBtn);
    expect(screen.getByText(/0 \/ 3 shares/i)).toBeInTheDocument();

    fireEvent.click(incBtn); // 1
    fireEvent.click(incBtn); // 2
    expect(screen.getByText(/2 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$800/i)).toBeInTheDocument();

    fireEvent.click(decBtn); // down to 1
    expect(screen.getByText(/1 \/ 3 shares/i)).toBeInTheDocument();
    expect(screen.getByText(/Total: \$400/i)).toBeInTheDocument();

    const confirmBtn = screen.getByRole('button', { name: /finish turn/i });
    fireEvent.click(confirmBtn);

    expect(onConfirm).toHaveBeenCalledWith([{ companyId: 'american', count: 1 }]);
  });

  it('renders message when no active companies with available shares exist', () => {
    render(
      <InlineStockMarket
        activePlayerCash={3000}
        companies={{}}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={vi.fn()}
        onPass={vi.fn()}
      />
    );

    expect(
      screen.getByText(/No active companies with available shares/i)
    ).toBeInTheDocument();
    expect(screen.getByText(/Pass & Finish Turn/i)).toBeInTheDocument();
  });

  it('triggers audio sound effects on user interactions', () => {
    const playClickSpy = vi.spyOn(audioModule, 'playClick');
    const playCashChimeSpy = vi.spyOn(audioModule, 'playCashChime');
    const onConfirm = vi.fn();

    render(
      <InlineStockMarket
        activePlayerCash={3000}
        companies={activeCompanies}
        definitions={CLASSIC_COMPANIES}
        onConfirmPurchases={onConfirm}
        onPass={vi.fn()}
      />
    );

    const incBtn = screen.getByRole('button', { name: /increase american/i });
    fireEvent.click(incBtn);
    expect(playClickSpy).toHaveBeenCalledTimes(1);

    const decBtn = screen.getByRole('button', { name: /decrease american/i });
    fireEvent.click(decBtn);
    expect(playClickSpy).toHaveBeenCalledTimes(2);

    // Increment again and confirm
    fireEvent.click(incBtn);
    expect(playClickSpy).toHaveBeenCalledTimes(3);

    const confirmBtn = screen.getByRole('button', { name: /finish turn/i });
    fireEvent.click(confirmBtn);

    expect(playCashChimeSpy).toHaveBeenCalledTimes(1);
    expect(onConfirm).toHaveBeenCalledWith([{ companyId: 'american', count: 1 }]);
  });

  describe('Alt+E Keyboard Shortcut', () => {
    it('executes handleFinish with purchased shares on Alt+E, preventing default and stopping propagation', () => {
      const onConfirm = vi.fn();
      const onPass = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={activeCompanies}
          definitions={CLASSIC_COMPANIES}
          onConfirmPurchases={onConfirm}
          onPass={onPass}
        />
      );

      // Select 2 shares of American
      const incBtn = screen.getByRole('button', { name: /increase american/i });
      fireEvent.click(incBtn);
      fireEvent.click(incBtn);

      expect(screen.getByText(/2 \/ 3 shares/i)).toBeInTheDocument();

      // Trigger Alt+E on window
      const event = fireEvent.keyDown(window, {
        key: 'e',
        altKey: true,
      });

      expect(event.preventDefault).toHaveBeenCalledTimes(1);
      expect(event.stopPropagation).toHaveBeenCalledTimes(1);
      expect(event.stopImmediatePropagation).toHaveBeenCalledTimes(1);
      expect(onConfirm).toHaveBeenCalledTimes(1);
      expect(onConfirm).toHaveBeenCalledWith([{ companyId: 'american', count: 2 }]);
      expect(onPass).not.toHaveBeenCalled();
    });

    it('executes onPass on Alt+E when 0 shares are selected', () => {
      const onConfirm = vi.fn();
      const onPass = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={activeCompanies}
          definitions={CLASSIC_COMPANIES}
          onConfirmPurchases={onConfirm}
          onPass={onPass}
        />
      );

      // Trigger Alt+E with 0 shares selected
      const event = fireEvent.keyDown(window, {
        key: 'E',
        altKey: true,
      });

      expect(event.preventDefault).toHaveBeenCalledTimes(1);
      expect(event.stopPropagation).toHaveBeenCalledTimes(1);
      expect(event.stopImmediatePropagation).toHaveBeenCalledTimes(1);
      expect(onPass).toHaveBeenCalledTimes(1);
      expect(onConfirm).not.toHaveBeenCalled();
    });

    it('ignores Alt+E when event target is an input element', () => {
      const onConfirm = vi.fn();
      const onPass = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={activeCompanies}
          definitions={CLASSIC_COMPANIES}
          onConfirmPurchases={onConfirm}
          onPass={onPass}
        />
      );

      const fakeInput = { tagName: 'INPUT' };
      const event = fireEvent.keyDown(window, {
        key: 'e',
        altKey: true,
        target: fakeInput,
      });

      expect(event.preventDefault).not.toHaveBeenCalled();
      expect(onConfirm).not.toHaveBeenCalled();
      expect(onPass).not.toHaveBeenCalled();
    });

    it('removes window keydown listener on unmount', () => {
      const onConfirm = vi.fn();
      const onPass = vi.fn();

      const { unmount } = render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={activeCompanies}
          definitions={CLASSIC_COMPANIES}
          onConfirmPurchases={onConfirm}
          onPass={onPass}
        />
      );

      unmount();

      const event = fireEvent.keyDown(window, {
        key: 'e',
        altKey: true,
      });

      expect(event.preventDefault).not.toHaveBeenCalled();
      expect(onConfirm).not.toHaveBeenCalled();
      expect(onPass).not.toHaveBeenCalled();
    });
  });

  describe('Sell Mode & Share Liquidation', () => {
    const multiCompaniesWithPrices: Record<string, MockCompany> = {
      american: {
        id: 'american',
        name: 'American Natural Gas',
        tier: 'low',
        color: '#e63946',
        isActive: true,
        isSafe: false,
        size: 4,
        stockPrice: 400,
        availableShares: 10,
      },
      imperial: {
        id: 'imperial',
        name: 'Imperial Petroleum',
        tier: 'mid',
        color: '#2a9d8f',
        isActive: true,
        isSafe: false,
        size: 5,
        stockPrice: 500,
        availableShares: 10,
      },
    };

    it('toggles between Buy and Sell modes via header tabs', () => {
      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={multiCompaniesWithPrices}
          definitions={CLASSIC_COMPANIES}
          playerShares={{ american: 3, imperial: 2 }}
          onConfirmPurchases={vi.fn()}
          onConfirmSales={vi.fn()}
          onSellAll={vi.fn()}
          onPass={vi.fn()}
        />
      );

      // Defaults to Buy Mode
      expect(screen.getByRole('button', { name: /ACQUIRE SHARES/i })).toBeInTheDocument();
      expect(screen.getByRole('button', { name: /LIQUIDATE SHARES/i })).toBeInTheDocument();

      // Switch to Sell Mode
      const sellTab = screen.getByRole('button', { name: /LIQUIDATE SHARES/i });
      fireEvent.click(sellTab);

      // In Sell Mode, shows owned corporations
      expect(screen.getByText(/3 shares owned/i)).toBeInTheDocument();
      expect(screen.getByText(/2 shares owned/i)).toBeInTheDocument();

      // Switch back to Buy Mode
      const buyTab = screen.getByRole('button', { name: /ACQUIRE SHARES/i });
      fireEvent.click(buyTab);

      expect(screen.getByText(/0 \/ 3 shares/i)).toBeInTheDocument();
    });

    it('shows empty state message when player owns 0 shares across all companies in Sell Mode', () => {
      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={multiCompaniesWithPrices}
          definitions={CLASSIC_COMPANIES}
          playerShares={{}}
          onConfirmPurchases={vi.fn()}
          onConfirmSales={vi.fn()}
          onSellAll={vi.fn()}
          onPass={vi.fn()}
        />
      );

      const sellTab = screen.getByRole('button', { name: /LIQUIDATE SHARES/i });
      fireEvent.click(sellTab);

      expect(screen.getByText(/You currently hold no corporate equity to liquidate/i)).toBeInTheDocument();

      // Global Liquidate button should be disabled
      const liquidateAllBtn = screen.getByRole('button', { name: /LIQUIDATE ENTIRE PORTFOLIO/i });
      expect((liquidateAllBtn as any).props?.disabled).toBe(true);
    });

    it('executes onConfirmSales and plays cash chime when selling 1 share', () => {
      const playCashChimeSpy = vi.spyOn(audioModule, 'playCashChime');
      const onConfirmSales = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={multiCompaniesWithPrices}
          definitions={CLASSIC_COMPANIES}
          playerShares={{ american: 3 }}
          onConfirmPurchases={vi.fn()}
          onConfirmSales={onConfirmSales}
          onSellAll={vi.fn()}
          onPass={vi.fn()}
        />
      );

      const sellTab = screen.getByRole('button', { name: /LIQUIDATE SHARES/i });
      fireEvent.click(sellTab);

      const sell1Btn = screen.getByRole('button', { name: /sell 1 share of american/i });
      fireEvent.click(sell1Btn);

      expect(onConfirmSales).toHaveBeenCalledWith([{ companyId: 'american', count: 1 }]);
      expect(playCashChimeSpy).toHaveBeenCalledTimes(1);
    });

    it('executes onConfirmSales and plays cash chime when selling all shares of a company', () => {
      const playCashChimeSpy = vi.spyOn(audioModule, 'playCashChime');
      const onConfirmSales = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={multiCompaniesWithPrices}
          definitions={CLASSIC_COMPANIES}
          playerShares={{ american: 4, imperial: 2 }}
          onConfirmPurchases={vi.fn()}
          onConfirmSales={onConfirmSales}
          onSellAll={vi.fn()}
          onPass={vi.fn()}
        />
      );

      const sellTab = screen.getByRole('button', { name: /LIQUIDATE SHARES/i });
      fireEvent.click(sellTab);

      const sellAllAmericanBtn = screen.getByRole('button', { name: /sell all shares of american/i });
      fireEvent.click(sellAllAmericanBtn);

      expect(onConfirmSales).toHaveBeenCalledWith([{ companyId: 'american', count: 4 }]);
      expect(playCashChimeSpy).toHaveBeenCalledTimes(1);
    });

    it('executes onSellAll when clicking LIQUIDATE ENTIRE PORTFOLIO', () => {
      const onSellAll = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={multiCompaniesWithPrices}
          definitions={CLASSIC_COMPANIES}
          playerShares={{ american: 3, imperial: 2 }}
          onConfirmPurchases={vi.fn()}
          onConfirmSales={vi.fn()}
          onSellAll={onSellAll}
          onPass={vi.fn()}
        />
      );

      const sellTab = screen.getByRole('button', { name: /LIQUIDATE SHARES/i });
      fireEvent.click(sellTab);

      const liquidateAllBtn = screen.getByRole('button', { name: /LIQUIDATE ENTIRE PORTFOLIO/i });
      fireEvent.click(liquidateAllBtn);

      expect(onSellAll).toHaveBeenCalledTimes(1);
    });

    it('triggers onPass when FINISH TURN is clicked in Sell Mode', () => {
      const onPass = vi.fn();

      render(
        <InlineStockMarket
          activePlayerCash={3000}
          companies={multiCompaniesWithPrices}
          definitions={CLASSIC_COMPANIES}
          playerShares={{ american: 3 }}
          onConfirmPurchases={vi.fn()}
          onConfirmSales={vi.fn()}
          onSellAll={vi.fn()}
          onPass={onPass}
        />
      );

      const sellTab = screen.getByRole('button', { name: /LIQUIDATE SHARES/i });
      fireEvent.click(sellTab);

      const finishBtn = screen.getByRole('button', { name: /finish turn/i });
      fireEvent.click(finishBtn);

      expect(onPass).toHaveBeenCalledTimes(1);
    });
  });
});
