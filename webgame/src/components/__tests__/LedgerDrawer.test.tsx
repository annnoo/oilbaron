import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { describe, it, expect, vi, beforeEach } from 'vitest';
import { LedgerDrawer } from '../LedgerDrawer';
import { DEFAULT_CONFIG, createInitialGameState } from '../../engine/config';
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
};

function evaluateTree(node: any, path: string = '0'): any {
  if (!node || typeof node !== 'object') return node;
  if (Array.isArray(node)) {
    return node.map((c: any, i: number) => evaluateTree(c, `${path}.${i}`));
  }
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
  if (internals) internals.H = testDispatcher;
  currentTree = evaluateTree(rootElement);
  if (internals) internals.H = null;
  currentHtml = currentTree ? renderToStaticMarkup(currentTree) : '';
}

function render(ui: React.ReactElement) {
  hookStates = [];
  rootElement = ui;
  triggerRender();
  return { html: currentHtml };
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
  queryByText: (matcher: RegExp | string) => {
    const regex = typeof matcher === 'string' ? new RegExp(matcher, 'i') : matcher;
    const decoded = decodeHtml(currentHtml);
    const textOnly = decoded.replace(/<[^>]+>/g, ' ').replace(/\s+/g, ' ');
    return regex.test(decoded) || regex.test(textOnly);
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
  click: (node: any, eventData?: any) => {
    let target = node;
    if (target?._path && currentTree) {
      const fresh = findNode(currentTree, (el) => el._path === target._path);
      if (fresh) target = fresh;
    }
    if (target?.props?.disabled) {
      return;
    }
    if (target?.props?.onClick) {
      const syntheticEvent = {
        preventDefault: () => {},
        stopPropagation: () => {},
        ...eventData,
      };
      target.props.onClick(syntheticEvent);
    }
  },
};

describe('LedgerDrawer', () => {
  beforeEach(() => {
    vi.restoreAllMocks();
  });

  const gameState = createInitialGameState(DEFAULT_CONFIG, [
    { name: 'Player 1', isAi: false },
    { name: 'Player 2', isAi: true },
  ]);

  it('renders tabs and closes on close button click', () => {
    const playClickSpy = vi.spyOn(audioModule, 'playClick');
    const onClose = vi.fn();
    render(<LedgerDrawer isOpen={true} onClose={onClose} gameState={gameState} />);

    expect(screen.getByText(/Ledger & Strategic Analytics/i)).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /wealth/i })).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /shares/i })).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /companies/i })).toBeInTheDocument();

    const closeBtn = screen.getByRole('button', { name: /close ledger/i });
    fireEvent.click(closeBtn);
    expect(onClose).toHaveBeenCalledTimes(1);
    expect(playClickSpy).toHaveBeenCalledTimes(1);
  });

  it('returns null when isOpen is false', () => {
    const onClose = vi.fn();
    const { html } = render(<LedgerDrawer isOpen={false} onClose={onClose} gameState={gameState} />);
    expect(html).toBe('');
    expect(screen.queryByText(/Ledger & Strategic Analytics/i)).toBe(false);
  });

  it('renders Wealth tab by default with player net worth and liquidity breakdown', () => {
    render(<LedgerDrawer isOpen={true} onClose={vi.fn()} gameState={gameState} />);

    expect(screen.getByText(/Net Worth & Liquidity/i)).toBeInTheDocument();
    expect(screen.getByText(/Player 1/i)).toBeInTheDocument();
    expect(screen.getByText(/Player 2/i)).toBeInTheDocument();
    expect(screen.getByText(/Cash: \$6,000/i)).toBeInTheDocument();
    expect(screen.getByText(/Stocks: \$0/i)).toBeInTheDocument();
  });

  it('switches to Shareholdings tab when clicking shares tab button', () => {
    render(<LedgerDrawer isOpen={true} onClose={vi.fn()} gameState={gameState} />);

    const sharesTabBtn = screen.getByRole('button', { name: /shares/i });
    fireEvent.click(sharesTabBtn);

    expect(screen.getByText(/Stock Ownership Matrix/i)).toBeInTheDocument();
    expect(screen.getByText(/Player/i)).toBeInTheDocument();
    expect(screen.getByText(/Player 1/i)).toBeInTheDocument();
  });

  it('switches to Companies tab when clicking companies tab button', () => {
    render(<LedgerDrawer isOpen={true} onClose={vi.fn()} gameState={gameState} />);

    const companiesTabBtn = screen.getByRole('button', { name: /companies/i });
    fireEvent.click(companiesTabBtn);

    expect(screen.getByText(/Corporation Overview/i)).toBeInTheDocument();
    expect(screen.getByText(/Imperial/i)).toBeInTheDocument();
    expect(screen.getByText(/Arabian/i)).toBeInTheDocument();
    expect(screen.getByText(/Dormant/i)).toBeInTheDocument();
    expect(screen.getByText(/Bank Shares:/i)).toBeInTheDocument();
  });

  it('handles backdrop click and stops event propagation on drawer panel', () => {
    const onClose = vi.fn();
    render(<LedgerDrawer isOpen={true} onClose={onClose} gameState={gameState} />);

    const backdrop = findNode(currentTree, (el) =>
      typeof el.props?.className === 'string' && el.props.className.includes('ledger-drawer-backdrop')
    );
    expect(backdrop).toBeDefined();
    fireEvent.click(backdrop);
    expect(onClose).toHaveBeenCalledTimes(1);

    const panel = findNode(currentTree, (el) =>
      typeof el.props?.className === 'string' && el.props.className.includes('ledger-drawer-panel')
    );
    expect(panel).toBeDefined();
    const stopPropagation = vi.fn();
    fireEvent.click(panel, { stopPropagation });
    expect(stopPropagation).toHaveBeenCalledTimes(1);
    expect(onClose).toHaveBeenCalledTimes(1); // not incremented
  });

  it('renders active and safe company status and calculates stock value in wealth tab', () => {
    const customState = {
      ...gameState,
      companies: {
        ...gameState.companies,
        imperial: {
          ...gameState.companies['imperial']!,
          isActive: true,
          isSafe: true,
          size: 12,
          availableShares: 20,
        },
      },
      players: [
        {
          ...gameState.players[0]!,
          cash: 4000,
          shares: { imperial: 5 },
        },
        gameState.players[1]!,
      ],
    };

    render(<LedgerDrawer isOpen={true} onClose={vi.fn()} gameState={customState} />);

    // In wealth tab:
    expect(screen.getByText(/Cash: \$4,000/i)).toBeInTheDocument();
    // 5 shares at $1,300 (size 12 tier high) = $6,500 in stocks, total net worth = $10,500
    expect(screen.getByText(/\$10,500/i)).toBeInTheDocument();
    expect(screen.getByText(/Stocks: \$6,500/i)).toBeInTheDocument();

    // In companies tab:
    const companiesTabBtn = screen.getByRole('button', { name: /companies/i });
    fireEvent.click(companiesTabBtn);
    expect(screen.getByText(/🛡️ Safe/i)).toBeInTheDocument();
    expect(screen.getByText(/Wells: 12/i)).toBeInTheDocument();
    expect(screen.getByText(/Price: \$1,?300/i)).toBeInTheDocument();
    expect(screen.getByText(/Bank Shares: 20/i)).toBeInTheDocument();

    // In shares tab:
    const sharesTabBtn = screen.getByRole('button', { name: /shares/i });
    fireEvent.click(sharesTabBtn);
    expect(screen.getByText('5')).toBeInTheDocument();
  });
});
