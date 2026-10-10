import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { describe, it, expect, vi, beforeEach } from 'vitest';
import { GameHeader } from '../GameHeader';
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

// Lightweight render and screen helpers providing standard testing-library compatible APIs
// for Node/SSR environment without requiring heavy DOM emulation or uncommitted dependencies.
let currentHtml = '';
let currentTree: any = null;

function evaluateTree(node: any): any {
  if (!node || typeof node !== 'object') return node;
  if (typeof node.type === 'function') {
    return evaluateTree(node.type(node.props));
  }
  return node;
}

function render(ui: React.ReactElement) {
  currentTree = evaluateTree(ui);
  currentHtml = renderToStaticMarkup(ui);
  return { html: currentHtml };
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

const screen = {
  getByText: (matcher: RegExp | string) => {
    const regex = typeof matcher === 'string' ? new RegExp(matcher, 'i') : matcher;
    const matches = regex.test(currentHtml);
    if (!matches) {
      throw new Error(`Unable to find text matching ${matcher} in: ${currentHtml}`);
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
    if (node?.props?.onClick) {
      node.props.onClick({ preventDefault: () => {} });
    }
  },
};

describe('GameHeader', () => {
  beforeEach(() => {
    vi.restoreAllMocks();
  });

  it('renders title, active player name, and turn step', () => {
    render(
      <GameHeader
        activePlayerName="Player 1"
        activePlayerColor="#f59e0b"
        isAi={false}
        turnPhase="place_tile"
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLanguage={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
      />
    );

    expect(screen.getByText(/OIL BARON/i)).toBeInTheDocument();
    expect(screen.getByText(/Player 1/i)).toBeInTheDocument();
    expect(screen.getByText(/Step 1: Drill Well/i)).toBeInTheDocument();
  });

  it('triggers onToggleLedger and onToggleSound callbacks', () => {
    const onToggleLedger = vi.fn();
    const onToggleSound = vi.fn();

    render(
      <GameHeader
        activePlayerName="Computer 1"
        isAi={true}
        turnPhase="buy_shares"
        soundMuted={false}
        language="en"
        onToggleSound={onToggleSound}
        onToggleLanguage={vi.fn()}
        onToggleLedger={onToggleLedger}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
      />
    );

    const ledgerBtn = screen.getByRole('button', { name: /ledger/i });
    fireEvent.click(ledgerBtn);
    expect(onToggleLedger).toHaveBeenCalledTimes(1);

    const soundBtn = screen.getByRole('button', { name: /sound/i });
    fireEvent.click(soundBtn);
    expect(onToggleSound).toHaveBeenCalledTimes(1);
  });

  it('renders AI badge and step label for CPU players', () => {
    render(
      <GameHeader
        activePlayerName="Deep Thought"
        activePlayerColor="#38bdf8"
        isAi={true}
        turnPhase="place_tile"
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLanguage={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
      />
    );

    expect(screen.getByText(/Deep Thought \(CPU\)/i)).toBeInTheDocument();
    expect(screen.getByText(/AI Turn in Progress/i)).toBeInTheDocument();
  });

  it('renders accurate step labels across all turn phases for human players', () => {
    const phases = [
      { phase: 'place_tile' as const, label: /Step 1: Drill Well/i },
      { phase: 'choose_founding_company' as const, label: /Incorporate Company/i },
      { phase: 'choose_merger_survivor' as const, label: /Merger Tiebreak/i },
      { phase: 'dispose_merger_shares' as const, label: /Merger Disposition/i },
      { phase: 'buy_shares' as const, label: /Step 2: Invest in Stocks/i },
      { phase: 'game_over' as const, label: /Game Over/i },
    ];

    for (const { phase, label } of phases) {
      render(
        <GameHeader
          activePlayerName="Baron Von Oil"
          isAi={false}
          turnPhase={phase}
          soundMuted={false}
          language="en"
          onToggleSound={vi.fn()}
          onToggleLanguage={vi.fn()}
          onToggleLedger={vi.fn()}
          onOpenRules={vi.fn()}
          onOpenNewGame={vi.fn()}
        />
      );

      expect(screen.getByText(label)).toBeInTheDocument();
    }
  });

  it('triggers language, rules, and new game callbacks with audio feedback', () => {
    const playClickSpy = vi.spyOn(audioModule, 'playClick');
    const onToggleLanguage = vi.fn();
    const onOpenRules = vi.fn();
    const onOpenNewGame = vi.fn();

    render(
      <GameHeader
        activePlayerName="Player 1"
        isAi={false}
        turnPhase="place_tile"
        soundMuted={false}
        language="de"
        onToggleSound={vi.fn()}
        onToggleLanguage={onToggleLanguage}
        onToggleLedger={vi.fn()}
        onOpenRules={onOpenRules}
        onOpenNewGame={onOpenNewGame}
      />
    );

    // Language button displays uppercase 'DE'
    expect(screen.getByText(/DE/i)).toBeInTheDocument();

    const langBtn = screen.getByRole('button', { name: /language/i });
    fireEvent.click(langBtn);
    expect(onToggleLanguage).toHaveBeenCalledTimes(1);

    const rulesBtn = screen.getByRole('button', { name: /rules/i });
    fireEvent.click(rulesBtn);
    expect(onOpenRules).toHaveBeenCalledTimes(1);

    const newGameBtn = screen.getByRole('button', { name: /new game/i });
    fireEvent.click(newGameBtn);
    expect(onOpenNewGame).toHaveBeenCalledTimes(1);

    expect(playClickSpy).toHaveBeenCalledTimes(3);
  });

  it('handles optional onSwitchClassic callback and omits button when not provided', () => {
    const onSwitchClassic = vi.fn();

    // With onSwitchClassic provided
    render(
      <GameHeader
        activePlayerName="Player 1"
        isAi={false}
        turnPhase="place_tile"
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLanguage={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
        onSwitchClassic={onSwitchClassic}
      />
    );

    const classicBtn = screen.getByRole('button', { name: /classic view/i });
    expect(classicBtn).toBeDefined();
    fireEvent.click(classicBtn);
    expect(onSwitchClassic).toHaveBeenCalledTimes(1);

    // Without onSwitchClassic
    render(
      <GameHeader
        activePlayerName="Player 1"
        isAi={false}
        turnPhase="place_tile"
        soundMuted={false}
        language="en"
        onToggleSound={vi.fn()}
        onToggleLanguage={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
      />
    );

    const omittedBtn = screen.queryByRole('button', { name: /classic view/i });
    expect(omittedBtn).toBeNull();
  });
});
