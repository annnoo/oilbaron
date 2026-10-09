export type Language = 'en' | 'de';

export interface LanguageOption {
  code: Language;
  name: string;
  nativeName: string;
}

export type TranslationParams = Record<string, string | number>;

export type TranslationDictionary = Record<string, string>;
