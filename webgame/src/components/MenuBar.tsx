import React, { useState, useEffect, useRef } from 'react';
import {
  RotateCcw,
  FolderPlus,
  Save,
  Upload,
  Volume2,
  VolumeX,
  Coins,
  Crosshair,
  Trash2,
  ChevronRight,
  BarChart3,
  PieChart,
  Building2,
  Shield,
  Archive,
  Sliders,
  Bot,
  User,
  Check,
  BookOpen,
  Info,
  Monitor,
} from 'lucide-react';
import { playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

export type MenuCategory = 'game' | 'actions' | 'views' | 'settings' | 'help';

export type MenuActionId =
  | 'new_game_same'
  | 'new_game_new'
  | 'save_game'
  | 'load_game'
  | 'toggle_sound'
  | 'buy_shares'
  | 'discard_chit'
  | 'place_chit'
  | 'end_turn'
  | 'view_wealth'
  | 'view_shares'
  | 'view_companies'
  | 'view_warroom'
  | 'view_discarded'
  | 'view_classic'
  | 'open_options'
  | 'toggle_player_ai'
  | 'toggle_slow_cpu'
  | 'toggle_hide_chits'
  | 'help_rules'
  | 'help_about';

export interface MenuBarProps {
  onAction?: (actionId: MenuActionId) => void;
  onNewGameSame?: () => void;
  onNewGameNew?: () => void;
  onSaveGame?: () => void;
  onLoadGame?: () => void;
  onToggleSound?: () => void;
  onBuyShares?: () => void;
  onDiscardChit?: () => void;
  onPlaceChit?: () => void;
  onEndTurn?: () => void;
  onViewWealth?: () => void;
  onViewShares?: () => void;
  onViewCompanies?: () => void;
  onViewWarRoom?: () => void;
  onViewDiscarded?: () => void;
  onViewClassic?: () => void;
  onOpenOptions?: () => void;
  onTogglePlayerAi?: () => void;
  onToggleSlowCpu?: () => void;
  onToggleHideChits?: () => void;
  onHelpRules?: () => void;
  onHelpAbout?: () => void;

  soundMuted?: boolean;
  slowCpuTurns?: boolean;
  hideChits?: boolean;
  activePlayerIsAi?: boolean;
  activePlayerName?: string;

  canBuyShares?: boolean;
  canDiscardChit?: boolean;
  canPlaceChit?: boolean;
  canEndTurn?: boolean;

  shortcutsDisabled?: boolean;
}

export interface KeyboardShortcutHandlers {
  canBuyShares?: boolean;
  canDiscardChit?: boolean;
  canPlaceChit?: boolean;
  canEndTurn?: boolean;
  shortcutsDisabled?: boolean;
  onBuyShares?: () => void;
  onDiscardChit?: () => void;
  onPlaceChit?: () => void;
  onEndTurn?: () => void;
  onViewWealth?: () => void;
  onViewShares?: () => void;
  onViewCompanies?: () => void;
  onViewWarRoom?: () => void;
  onAction?: (actionId: MenuActionId) => void;
}

/**
 * Handles global Alt+... keyboard shortcuts faithful to original 1993 Oil Baron.
 * Returns true if a shortcut was processed and prevented.
 */
export function handleKeyboardShortcut(
  event: {
    key: string;
    altKey?: boolean;
    ctrlKey?: boolean;
    metaKey?: boolean;
    defaultPrevented?: boolean;
    preventDefault?: () => void;
    target?: unknown;
  },
  handlers: KeyboardShortcutHandlers
): boolean {
  if (handlers.shortcutsDisabled) {
    return false;
  }

  // Ignore keystrokes when typing inside text inputs, textareas, or selects
  if (
    event.target &&
    typeof event.target === 'object' &&
    'tagName' in event.target
  ) {
    const tagName = (event.target as { tagName?: string }).tagName?.toLowerCase();
    if (tagName === 'input' || tagName === 'textarea' || tagName === 'select') {
      return false;
    }
  }

  if (!event.altKey || event.ctrlKey || event.metaKey) {
    return false;
  }

  const key = event.key.toLowerCase();

  switch (key) {
    case 'b': {
      event.preventDefault?.();
      if (handlers.canBuyShares !== false) {
        if (handlers.onBuyShares) {
          handlers.onBuyShares();
        } else {
          handlers.onAction?.('buy_shares');
        }
        playClick();
      }
      return true;
    }
    case 'p': {
      event.preventDefault?.();
      if (handlers.canPlaceChit !== false) {
        if (handlers.onPlaceChit) {
          handlers.onPlaceChit();
        } else {
          handlers.onAction?.('place_chit');
        }
        playClick();
      }
      return true;
    }
    case 'd': {
      event.preventDefault?.();
      if (handlers.canDiscardChit !== false) {
        if (handlers.onDiscardChit) {
          handlers.onDiscardChit();
        } else {
          handlers.onAction?.('discard_chit');
        }
        playClick();
      }
      return true;
    }
    case 'e': {
      event.preventDefault?.();
      if (handlers.canEndTurn !== false) {
        if (handlers.onEndTurn) {
          handlers.onEndTurn();
        } else {
          handlers.onAction?.('end_turn');
        }
        playClick();
      }
      return true;
    }
    case 'n': {
      event.preventDefault?.();
      if (handlers.onViewWealth) {
        handlers.onViewWealth();
      } else {
        handlers.onAction?.('view_wealth');
      }
      playClick();
      return true;
    }
    case 's': {
      event.preventDefault?.();
      if (handlers.onViewShares) {
        handlers.onViewShares();
      } else {
        handlers.onAction?.('view_shares');
      }
      playClick();
      return true;
    }
    case 'c': {
      event.preventDefault?.();
      if (handlers.onViewCompanies) {
        handlers.onViewCompanies();
      } else {
        handlers.onAction?.('view_companies');
      }
      playClick();
      return true;
    }
    case 'w': {
      event.preventDefault?.();
      if (handlers.onViewWarRoom) {
        handlers.onViewWarRoom();
      } else {
        handlers.onAction?.('view_warroom');
      }
      playClick();
      return true;
    }
    default:
      return false;
  }
}

/**
 * Executes a menu action ensuring exclusive single-dispatch:
 * If a specific callback is supplied, it is invoked; otherwise, falls back to onAction.
 */
export function dispatchMenuAction(
  actionId: MenuActionId,
  callback?: () => void,
  onAction?: (actionId: MenuActionId) => void
): void {
  playClick();
  if (callback) {
    callback();
  } else {
    onAction?.(actionId);
  }
}

export const MenuBar: React.FC<MenuBarProps> = (props) => {
  const {
    onAction,
    onNewGameSame,
    onNewGameNew,
    onSaveGame,
    onLoadGame,
    onToggleSound,
    onBuyShares,
    onDiscardChit,
    onPlaceChit,
    onEndTurn,
    onViewWealth,
    onViewShares,
    onViewCompanies,
    onViewWarRoom,
    onViewDiscarded,
    onViewClassic,
    onOpenOptions,
    onTogglePlayerAi,
    onToggleSlowCpu,
    onToggleHideChits,
    onHelpRules,
    onHelpAbout,
    soundMuted = false,
    slowCpuTurns = false,
    hideChits = false,
    activePlayerIsAi = false,
    activePlayerName,
    canBuyShares = true,
    canDiscardChit = true,
    canPlaceChit = true,
    canEndTurn = true,
    shortcutsDisabled = false,
  } = props;

  const [openMenu, setOpenMenu] = useState<MenuCategory | null>(null);
  const menuBarRef = useRef<HTMLElement>(null);
  const { t, language, setLanguage } = useTranslation();

  const executeAction = (actionId: MenuActionId, callback?: () => void) => {
    dispatchMenuAction(actionId, callback, onAction);
    setOpenMenu(null);
  };

  // Close dropdown on click outside or Escape key
  useEffect(() => {
    const handleDocumentClick = (event: MouseEvent) => {
      if (menuBarRef.current && !menuBarRef.current.contains(event.target as Node)) {
        setOpenMenu(null);
      }
    };

    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key === 'Escape') {
        setOpenMenu(null);
        return;
      }

      handleKeyboardShortcut(event, {
        canBuyShares,
        canDiscardChit,
        canPlaceChit,
        canEndTurn,
        shortcutsDisabled,
        onBuyShares,
        onDiscardChit,
        onPlaceChit,
        onEndTurn,
        onViewWealth,
        onViewShares,
        onViewCompanies,
        onViewWarRoom,
        onAction,
      });
    };

    document.addEventListener('mousedown', handleDocumentClick);
    window.addEventListener('keydown', handleKeyDown);

    return () => {
      document.removeEventListener('mousedown', handleDocumentClick);
      window.removeEventListener('keydown', handleKeyDown);
    };
  }, [
    canBuyShares,
    canDiscardChit,
    canPlaceChit,
    canEndTurn,
    shortcutsDisabled,
    onBuyShares,
    onDiscardChit,
    onPlaceChit,
    onEndTurn,
    onViewWealth,
    onViewShares,
    onViewCompanies,
    onViewWarRoom,
    onAction,
  ]);

  const handleHeaderClick = (category: MenuCategory) => {
    playClick();
    setOpenMenu((prev) => (prev === category ? null : category));
  };

  const handleHeaderMouseEnter = (category: MenuCategory) => {
    if (openMenu !== null && openMenu !== category) {
      setOpenMenu(category);
    }
  };

  return (
    <nav className="menu-bar" ref={menuBarRef} role="menubar" aria-label="Game Menu">
      <div className="menu-bar-nav">
        {/* Game Menu */}
        <div className="menu-bar-item">
          <button
            type="button"
            className={`menu-bar-btn ${openMenu === 'game' ? 'active' : ''}`}
            aria-haspopup="true"
            aria-expanded={openMenu === 'game'}
            onClick={() => handleHeaderClick('game')}
            onMouseEnter={() => handleHeaderMouseEnter('game')}
          >
            {language === 'de' ? (
              <><span className="mnemonic">S</span>piel</>
            ) : (
              <><span className="mnemonic">G</span>ame</>
            )}
          </button>

          {openMenu === 'game' && (
            <div className="menu-dropdown" role="menu">
              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('new_game_same', onNewGameSame)}
              >
                <span className="menu-item-left">
                  <RotateCcw size={14} className="menu-icon" />
                  <span>{t('menu.newGameSame')}</span>
                </span>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('new_game_new', onNewGameNew)}
              >
                <span className="menu-item-left">
                  <FolderPlus size={14} className="menu-icon" />
                  <span>{t('menu.newGameNew')}</span>
                </span>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('save_game', onSaveGame)}
              >
                <span className="menu-item-left">
                  <Save size={14} className="menu-icon" />
                  <span>{t('menu.saveGame')}</span>
                </span>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('load_game', onLoadGame)}
              >
                <span className="menu-item-left">
                  <Upload size={14} className="menu-icon" />
                  <span>{t('menu.loadGame')}</span>
                </span>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('toggle_sound', onToggleSound)}
              >
                <span className="menu-item-left">
                  {soundMuted ? (
                    <VolumeX size={14} className="menu-icon text-muted" />
                  ) : (
                    <Volume2 size={14} className="menu-icon text-gold" />
                  )}
                  <span>{soundMuted ? t('menu.soundMuted') : t('menu.soundOn')}</span>
                </span>
                <span className="menu-item-check">
                  {!soundMuted && <Check size={14} className="check-icon" />}
                </span>
              </button>
            </div>
          )}
        </div>

        {/* Actions Menu */}
        <div className="menu-bar-item">
          <button
            type="button"
            className={`menu-bar-btn ${openMenu === 'actions' ? 'active' : ''}`}
            aria-haspopup="true"
            aria-expanded={openMenu === 'actions'}
            onClick={() => handleHeaderClick('actions')}
            onMouseEnter={() => handleHeaderMouseEnter('actions')}
          >
            {language === 'de' ? (
              <><span className="mnemonic">A</span>ktionen</>
            ) : (
              <><span className="mnemonic">A</span>ctions</>
            )}
          </button>

          {openMenu === 'actions' && (
            <div className="menu-dropdown" role="menu">
              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                disabled={!canBuyShares}
                onClick={() => executeAction('buy_shares', onBuyShares)}
              >
                <span className="menu-item-left">
                  <Coins size={14} className="menu-icon" />
                  <span>{t('menu.buyShares')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+B</kbd>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                disabled={!canPlaceChit}
                onClick={() => executeAction('place_chit', onPlaceChit)}
              >
                <span className="menu-item-left">
                  <Crosshair size={14} className="menu-icon" />
                  <span>{t('menu.placeChit')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+P</kbd>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                disabled={!canDiscardChit}
                onClick={() => executeAction('discard_chit', onDiscardChit)}
              >
                <span className="menu-item-left">
                  <Trash2 size={14} className="menu-icon" />
                  <span>{t('menu.discardChit')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+D</kbd>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                disabled={!canEndTurn}
                onClick={() => executeAction('end_turn', onEndTurn)}
              >
                <span className="menu-item-left">
                  <ChevronRight size={14} className="menu-icon" />
                  <span>{t('menu.endTurn')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+E</kbd>
              </button>
            </div>
          )}
        </div>

        {/* Views Menu */}
        <div className="menu-bar-item">
          <button
            type="button"
            className={`menu-bar-btn ${openMenu === 'views' ? 'active' : ''}`}
            aria-haspopup="true"
            aria-expanded={openMenu === 'views'}
            onClick={() => handleHeaderClick('views')}
            onMouseEnter={() => handleHeaderMouseEnter('views')}
          >
            {language === 'de' ? (
              <><span className="mnemonic">A</span>nsichten</>
            ) : (
              <><span className="mnemonic">V</span>iews</>
            )}
          </button>

          {openMenu === 'views' && (
            <div className="menu-dropdown" role="menu">
              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('view_wealth', onViewWealth)}
              >
                <span className="menu-item-left">
                  <BarChart3 size={14} className="menu-icon" />
                  <span>{t('menu.viewWealth')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+N</kbd>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('view_shares', onViewShares)}
              >
                <span className="menu-item-left">
                  <PieChart size={14} className="menu-icon" />
                  <span>{t('menu.viewShares')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+S</kbd>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('view_companies', onViewCompanies)}
              >
                <span className="menu-item-left">
                  <Building2 size={14} className="menu-icon" />
                  <span>{t('menu.viewCompanies')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+C</kbd>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('view_warroom', onViewWarRoom)}
              >
                <span className="menu-item-left">
                  <Shield size={14} className="menu-icon" />
                  <span>{t('menu.viewWarRoom')}</span>
                </span>
                <kbd className="menu-item-shortcut">Alt+W</kbd>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('view_discarded', onViewDiscarded)}
              >
                <span className="menu-item-left">
                  <Archive size={14} className="menu-icon" />
                  <span>{t('menu.viewDiscarded')}</span>
                </span>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('view_classic', onViewClassic)}
              >
                <span className="menu-item-left">
                  <Monitor size={14} className="menu-icon" />
                  <span>1993 Classic Edition View</span>
                </span>
              </button>
            </div>
          )}
        </div>

        {/* Settings Menu */}
        <div className="menu-bar-item">
          <button
            type="button"
            className={`menu-bar-btn ${openMenu === 'settings' ? 'active' : ''}`}
            aria-haspopup="true"
            aria-expanded={openMenu === 'settings'}
            onClick={() => handleHeaderClick('settings')}
            onMouseEnter={() => handleHeaderMouseEnter('settings')}
          >
            {language === 'de' ? (
              <><span className="mnemonic">O</span>ptionen</>
            ) : (
              <><span className="mnemonic">S</span>ettings</>
            )}
          </button>

          {openMenu === 'settings' && (
            <div className="menu-dropdown" role="menu">
              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('open_options', onOpenOptions)}
              >
                <span className="menu-item-left">
                  <Sliders size={14} className="menu-icon" />
                  <span>{t('menu.options')}</span>
                </span>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('toggle_player_ai', onTogglePlayerAi)}
              >
                <span className="menu-item-left">
                  {activePlayerIsAi ? (
                    <User size={14} className="menu-icon text-gold" />
                  ) : (
                    <Bot size={14} className="menu-icon text-blue" />
                  )}
                  <span>
                    {activePlayerName ? `${activePlayerName}: ` : `${t('menu.activePlayer')}: `}
                    {activePlayerIsAi ? t('menu.changeToHuman') : t('menu.changeToCpu')}
                  </span>
                </span>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('toggle_slow_cpu', onToggleSlowCpu)}
              >
                <span className="menu-item-left">
                  <span className="menu-icon-spacer" />
                  <span>{t('menu.slowCpuTurns')}</span>
                </span>
                <span className="menu-item-check">
                  {slowCpuTurns && <Check size={14} className="check-icon" />}
                </span>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('toggle_hide_chits', onToggleHideChits)}
              >
                <span className="menu-item-left">
                  <span className="menu-icon-spacer" />
                  <span>{t('menu.hideChits')}</span>
                </span>
                <span className="menu-item-check">
                  {hideChits && <Check size={14} className="check-icon" />}
                </span>
              </button>

              <div className="menu-dropdown-divider" role="separator" />

              {/* Language Switcher */}
              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => {
                  playClick();
                  setLanguage('en');
                  setOpenMenu(null);
                }}
              >
                <span className="menu-item-left">
                  <span className="menu-icon-spacer" />
                  <span>English</span>
                </span>
                <span className="menu-item-check">
                  {language === 'en' && <Check size={14} className="check-icon" />}
                </span>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => {
                  playClick();
                  setLanguage('de');
                  setOpenMenu(null);
                }}
              >
                <span className="menu-item-left">
                  <span className="menu-icon-spacer" />
                  <span>Deutsch</span>
                </span>
                <span className="menu-item-check">
                  {language === 'de' && <Check size={14} className="check-icon" />}
                </span>
              </button>
            </div>
          )}
        </div>

        {/* Help Menu */}
        <div className="menu-bar-item">
          <button
            type="button"
            className={`menu-bar-btn ${openMenu === 'help' ? 'active' : ''}`}
            aria-haspopup="true"
            aria-expanded={openMenu === 'help'}
            onClick={() => handleHeaderClick('help')}
            onMouseEnter={() => handleHeaderMouseEnter('help')}
          >
            {language === 'de' ? (
              <><span className="mnemonic">H</span>ilfe</>
            ) : (
              <><span className="mnemonic">H</span>elp</>
            )}
          </button>

          {openMenu === 'help' && (
            <div className="menu-dropdown" role="menu">
              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('help_rules', onHelpRules)}
              >
                <span className="menu-item-left">
                  <BookOpen size={14} className="menu-icon" />
                  <span>{t('menu.helpRules')}</span>
                </span>
              </button>

              <button
                type="button"
                className="menu-dropdown-item"
                role="menuitem"
                onClick={() => executeAction('help_about', onHelpAbout)}
              >
                <span className="menu-item-left">
                  <Info size={14} className="menu-icon" />
                  <span>{t('menu.helpAbout')}</span>
                </span>
              </button>
            </div>
          )}
        </div>
      </div>

      {/* Status Bar Indicator & Language Quick Toggle */}
      <div className="menu-bar-status" style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
        <button
          type="button"
          className="btn btn-ghost btn-sm"
          style={{
            padding: '2px 8px',
            fontSize: '11px',
            fontWeight: 700,
            border: '1px solid var(--border-subtle)',
            borderRadius: '4px',
            cursor: 'pointer',
          }}
          onClick={() => {
            playClick();
            setLanguage(language === 'en' ? 'de' : 'en');
          }}
          title={language === 'en' ? 'Auf Deutsch wechseln' : 'Switch to English'}
          aria-label="Toggle language"
        >
          🌐 {language.toUpperCase()}
        </button>
        <span className="menu-bar-badge">
          {activePlayerIsAi ? t('menu.statusCpuTurn') : t('menu.statusHumanTurn')}
        </span>
      </div>
    </nav>
  );
};

export default MenuBar;
