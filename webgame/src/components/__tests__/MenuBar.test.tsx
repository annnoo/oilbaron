import { describe, it, expect, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { MenuBar, handleKeyboardShortcut, dispatchMenuAction } from '../MenuBar';
import { setLanguage } from '../../i18n';

describe('MenuBar Component', () => {
  it('renders top navigation menu bar with all categories', () => {
    const html = renderToStaticMarkup(
      <MenuBar
        activePlayerIsAi={false}
        activePlayerName="Player 1"
        soundMuted={false}
        slowCpuTurns={false}
        hideChits={false}
      />
    );

    expect(html).toContain('role="menubar"');
    expect(html).toContain('<span class="mnemonic">G</span>ame');
    expect(html).toContain('<span class="mnemonic">A</span>ctions');
    expect(html).toContain('<span class="mnemonic">V</span>iews');
    expect(html).toContain('<span class="mnemonic">S</span>ettings');
    expect(html).toContain('<span class="mnemonic">H</span>elp');
    expect(html).toContain('Human Turn');
  });

  it('renders CPU Turn status badge when active player is AI', () => {
    const html = renderToStaticMarkup(
      <MenuBar
        activePlayerIsAi={true}
        activePlayerName="Computer 1"
      />
    );

    expect(html).toContain('CPU Turn');
  });

  it('renders German navigation headers and status badge when language is de', () => {
    setLanguage('de');
    const html = renderToStaticMarkup(
      <MenuBar
        activePlayerIsAi={false}
        activePlayerName="Spieler 1"
        soundMuted={false}
        slowCpuTurns={false}
        hideChits={false}
      />
    );

    expect(html).toContain('<span class="mnemonic">S</span>piel');
    expect(html).toContain('<span class="mnemonic">A</span>ktionen');
    expect(html).toContain('<span class="mnemonic">A</span>nsichten');
    expect(html).toContain('<span class="mnemonic">O</span>ptionen');
    expect(html).toContain('<span class="mnemonic">H</span>ilfe');
    expect(html).toContain('Mensch-Zug');
    setLanguage('en');
  });

  describe('dispatchMenuAction & Single Callback Execution', () => {
    it('invokes callback exactly once and does not double-invoke onAction when both are provided', () => {
      const callback = vi.fn();
      const onAction = vi.fn();

      dispatchMenuAction('toggle_sound', callback, onAction);

      expect(callback).toHaveBeenCalledTimes(1);
      expect(onAction).not.toHaveBeenCalled();
    });

    it('falls back to onAction exactly once when no specific callback is provided', () => {
      const onAction = vi.fn();

      dispatchMenuAction('toggle_slow_cpu', undefined, onAction);

      expect(onAction).toHaveBeenCalledTimes(1);
      expect(onAction).toHaveBeenCalledWith('toggle_slow_cpu');
    });

    it('toggles boolean state exactly once without cancel-out', () => {
      let soundMuted = false;
      const toggleSound = vi.fn(() => {
        soundMuted = !soundMuted;
      });
      const onAction = vi.fn((actionId) => {
        if (actionId === 'toggle_sound') soundMuted = !soundMuted;
      });

      // Exclusive dispatch ensures sound is muted once (not muted and then unmuted)
      dispatchMenuAction('toggle_sound', toggleSound, onAction);

      expect(toggleSound).toHaveBeenCalledTimes(1);
      expect(onAction).not.toHaveBeenCalled();
      expect(soundMuted).toBe(true);
    });

    it('dispatches save_game and load_game to callbacks or onAction', () => {
      const onSave = vi.fn();
      const onLoad = vi.fn();
      const onAction = vi.fn();

      dispatchMenuAction('save_game', onSave, onAction);
      expect(onSave).toHaveBeenCalledTimes(1);
      expect(onAction).not.toHaveBeenCalled();

      dispatchMenuAction('load_game', onLoad, onAction);
      expect(onLoad).toHaveBeenCalledTimes(1);
      expect(onAction).not.toHaveBeenCalled();

      dispatchMenuAction('save_game', undefined, onAction);
      expect(onAction).toHaveBeenCalledWith('save_game');

      dispatchMenuAction('load_game', undefined, onAction);
      expect(onAction).toHaveBeenCalledWith('load_game');
    });
  });

  describe('handleKeyboardShortcut', () => {
    it('triggers onBuyShares exclusively on Alt+B without double-calling onAction', () => {
      const onBuyShares = vi.fn();
      const onAction = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'b', altKey: true, preventDefault },
        { onBuyShares, onAction, canBuyShares: true }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(onBuyShares).toHaveBeenCalledTimes(1);
      expect(onAction).not.toHaveBeenCalled();
    });

    it('falls back to onAction on Alt+B when onBuyShares is not provided', () => {
      const onAction = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'b', altKey: true, preventDefault },
        { onAction, canBuyShares: true }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(onAction).toHaveBeenCalledTimes(1);
      expect(onAction).toHaveBeenCalledWith('buy_shares');
    });

    it('handles uppercase Alt+B', () => {
      const onBuyShares = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'B', altKey: true, preventDefault },
        { onBuyShares, canBuyShares: true }
      );

      expect(handled).toBe(true);
      expect(onBuyShares).toHaveBeenCalledTimes(1);
    });

    it('does not trigger onBuyShares when canBuyShares is false', () => {
      const onBuyShares = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'b', altKey: true, preventDefault },
        { onBuyShares, canBuyShares: false }
      );

      expect(handled).toBe(true);
      expect(preventDefault).toHaveBeenCalled();
      expect(onBuyShares).not.toHaveBeenCalled();
    });

    it('triggers onPlaceChit on Alt+P', () => {
      const onPlaceChit = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'p', altKey: true, preventDefault },
        { onPlaceChit, canPlaceChit: true }
      );

      expect(handled).toBe(true);
      expect(onPlaceChit).toHaveBeenCalledTimes(1);
    });

    it('does not trigger onPlaceChit when canPlaceChit is false', () => {
      const onPlaceChit = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'p', altKey: true, preventDefault },
        { onPlaceChit, canPlaceChit: false }
      );

      expect(handled).toBe(true);
      expect(onPlaceChit).not.toHaveBeenCalled();
    });

    it('triggers onDiscardChit on Alt+D', () => {
      const onDiscardChit = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'd', altKey: true, preventDefault },
        { onDiscardChit, canDiscardChit: true }
      );

      expect(handled).toBe(true);
      expect(onDiscardChit).toHaveBeenCalledTimes(1);
    });

    it('triggers onEndTurn on Alt+E', () => {
      const onEndTurn = vi.fn();
      const preventDefault = vi.fn();

      const handled = handleKeyboardShortcut(
        { key: 'e', altKey: true, preventDefault },
        { onEndTurn, canEndTurn: true }
      );

      expect(handled).toBe(true);
      expect(onEndTurn).toHaveBeenCalledTimes(1);
    });

    it('triggers view shortcuts (Alt+N, Alt+S, Alt+C, Alt+W)', () => {
      const onViewWealth = vi.fn();
      const onViewShares = vi.fn();
      const onViewCompanies = vi.fn();
      const onViewWarRoom = vi.fn();

      handleKeyboardShortcut({ key: 'n', altKey: true }, { onViewWealth });
      handleKeyboardShortcut({ key: 's', altKey: true }, { onViewShares });
      handleKeyboardShortcut({ key: 'c', altKey: true }, { onViewCompanies });
      handleKeyboardShortcut({ key: 'w', altKey: true }, { onViewWarRoom });

      expect(onViewWealth).toHaveBeenCalledTimes(1);
      expect(onViewShares).toHaveBeenCalledTimes(1);
      expect(onViewCompanies).toHaveBeenCalledTimes(1);
      expect(onViewWarRoom).toHaveBeenCalledTimes(1);
    });

    it('ignores shortcut when altKey is false', () => {
      const onBuyShares = vi.fn();
      const handled = handleKeyboardShortcut(
        { key: 'b', altKey: false },
        { onBuyShares }
      );
      expect(handled).toBe(false);
      expect(onBuyShares).not.toHaveBeenCalled();
    });

    it('ignores shortcut when ctrlKey or metaKey is active', () => {
      const onBuyShares = vi.fn();
      const handled1 = handleKeyboardShortcut(
        { key: 'b', altKey: true, ctrlKey: true },
        { onBuyShares }
      );
      const handled2 = handleKeyboardShortcut(
        { key: 'b', altKey: true, metaKey: true },
        { onBuyShares }
      );
      expect(handled1).toBe(false);
      expect(handled2).toBe(false);
      expect(onBuyShares).not.toHaveBeenCalled();
    });

    it('ignores shortcut when shortcutsDisabled is true', () => {
      const onBuyShares = vi.fn();
      const handled = handleKeyboardShortcut(
        { key: 'b', altKey: true },
        { onBuyShares, shortcutsDisabled: true }
      );
      expect(handled).toBe(false);
      expect(onBuyShares).not.toHaveBeenCalled();
    });

    it('ignores shortcut when target is an input or textarea', () => {
      const onBuyShares = vi.fn();
      const handledInput = handleKeyboardShortcut(
        { key: 'b', altKey: true, target: { tagName: 'INPUT' } },
        { onBuyShares }
      );
      const handledTextarea = handleKeyboardShortcut(
        { key: 'b', altKey: true, target: { tagName: 'TEXTAREA' } },
        { onBuyShares }
      );
      expect(handledInput).toBe(false);
      expect(handledTextarea).toBe(false);
      expect(onBuyShares).not.toHaveBeenCalled();
    });
  });
});
