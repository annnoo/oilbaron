import { useSyncExternalStore } from 'react';
import type { Language, LanguageOption, TranslationParams } from './types';
import { en, type TranslationKey } from './locales/en';
import { de } from './locales/de';

export type { Language, LanguageOption, TranslationParams, TranslationKey };

export const AVAILABLE_LANGUAGES: readonly LanguageOption[] = [
  { code: 'en', name: 'English', nativeName: 'English' },
  { code: 'de', name: 'German', nativeName: 'Deutsch' },
] as const;

const dictionaries: Record<Language, Record<string, string>> = {
  en,
  de,
};

const STORAGE_KEY = 'oilbaron_language';

function getInitialLanguage(): Language {
  if (typeof window !== 'undefined' && typeof window.localStorage !== 'undefined') {
    try {
      const stored = window.localStorage.getItem(STORAGE_KEY);
      if (stored === 'en' || stored === 'de') {
        return stored;
      }
    } catch {
      // Ignore localStorage access errors
    }
  }
  return 'en';
}

let activeLanguage: Language = getInitialLanguage();
const listeners = new Set<() => void>();

export function getLanguage(): Language {
  return activeLanguage;
}

export function setLanguage(lang: Language): void {
  if (activeLanguage === lang) return;
  activeLanguage = lang;

  if (typeof window !== 'undefined' && typeof window.localStorage !== 'undefined') {
    try {
      window.localStorage.setItem(STORAGE_KEY, lang);
    } catch {
      // Ignore localStorage write errors
    }
  }

  listeners.forEach((listener) => {
    try {
      listener();
    } catch (err) {
      console.error('Error notifying i18n subscriber:', err);
    }
  });
}

export function subscribe(listener: () => void): () => void {
  listeners.add(listener);
  return () => {
    listeners.delete(listener);
  };
}

/**
 * Translates a key into current language with optional string interpolation parameters.
 * Falls back to English if key is missing in active language.
 */
export function t(key: TranslationKey, params?: TranslationParams): string {
  const dict = dictionaries[activeLanguage] ?? dictionaries.en;
  let text = dict[key] ?? dictionaries.en[key] ?? (key as string);

  if (params) {
    for (const [k, v] of Object.entries(params)) {
      text = text.replaceAll(`{${k}}`, String(v));
    }
  }

  return text;
}

/**
 * React hook to access translations and change active language.
 * Subscribed components re-render immediately when language is changed.
 */
export function useTranslation() {
  let language: Language = getLanguage();
  try {
    language = useSyncExternalStore(subscribe, getLanguage, getLanguage);
  } catch {
    language = getLanguage();
  }

  return {
    t,
    language,
    setLanguage,
    languages: AVAILABLE_LANGUAGES,
  };
}
