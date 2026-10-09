import { describe, it, expect, beforeEach } from 'vitest';
import { t, getLanguage, setLanguage, useTranslation, AVAILABLE_LANGUAGES } from '../index';
import { en } from '../locales/en';
import { de } from '../locales/de';

describe('i18n translation system', () => {
  beforeEach(() => {
    setLanguage('en');
  });

  it('provides available languages English and German', () => {
    expect(AVAILABLE_LANGUAGES.map((l) => l.code)).toEqual(['en', 'de']);
  });

  it('returns English strings by default', () => {
    expect(getLanguage()).toBe('en');
    expect(t('common.ok')).toBe('OK');
    expect(t('menu.newGame')).toBe('New Game');
    expect(t('options.title')).toBe('Game Options & Ruleset');
  });

  it('interpolates parameters correctly in English', () => {
    expect(t('board.selectedPrompt', { coord: '4-B' })).toBe(
      'Selected Tile: 4-B — Click board to place'
    );
    expect(t('app.turn', { name: 'Player 1' })).toBe("Player 1's Turn");
  });

  it('switches language to German and returns accurate German translations', () => {
    setLanguage('de');
    expect(getLanguage()).toBe('de');
    expect(t('common.ok')).toBe('OK');
    expect(t('menu.newGame')).toBe('Neues Spiel');
    expect(t('options.title')).toBe('Spieloptionen & Regelwerk');
    expect(t('board.selectedPrompt', { coord: '4-B' })).toBe(
      'Ausgewähltes Feld: 4-B — Klicke aufs Spielfeld zum Platzieren'
    );
    expect(t('app.turn', { name: 'Spieler 1' })).toBe('Spieler 1 ist am Zug');
  });

  it('has identical keys in en and de translation dictionaries', () => {
    const enKeys = Object.keys(en).sort();
    const deKeys = Object.keys(de).sort();
    expect(deKeys).toEqual(enKeys);
  });

  it('contains non-empty translated strings in German for every English key', () => {
    for (const [key, value] of Object.entries(en)) {
      const deValue = (de as any)[key];
      expect(deValue, `Key ${key} should have a German translation`).toBeDefined();
      expect(typeof deValue).toBe('string');
      expect(deValue.trim().length).toBeGreaterThan(0);
    }
  });
});
