import { describe, it, expect, beforeEach, afterEach, vi } from 'vitest';
import React from 'react';
import { renderToStaticMarkup } from 'react-dom/server';
import { setLanguage, getLanguage } from '../index';
import { MenuBar } from '../../components/MenuBar';
import { CompanyTicker } from '../../components/CompanyTicker';
import { PlayerHand } from '../../components/PlayerHand';
import { PlayerScoreboard } from '../../components/PlayerScoreboard';
import { TileInspector } from '../../components/TileInspector';
import { OptionsModal } from '../../components/modals/OptionsModal';
import { NewGameModal } from '../../components/modals/NewGameModal';
import { FoundingCompanyModal } from '../../components/modals/FoundingCompanyModal';
import { DiscardedTilesModal } from '../../components/modals/DiscardedTilesModal';
import { StockMarketModal } from '../../components/modals/StockMarketModal';
import { CompanyStatsModal } from '../../components/modals/CompanyStatsModal';
import { MergerSurvivorModal } from '../../components/modals/MergerSurvivorModal';
import { MergerStatsModal } from '../../components/modals/MergerStatsModal';
import { ShareDispositionModal } from '../../components/modals/ShareDispositionModal';
import { WealthChartModal } from '../../components/modals/WealthChartModal';
import { ShareGraphModal } from '../../components/modals/ShareGraphModal';
import { WarRoomModal } from '../../components/modals/WarRoomModal';
import { SaveLoadModal } from '../../components/modals/SaveLoadModal';
import { GameOverModal } from '../../components/modals/GameOverModal';
import { HelpModal } from '../../components/modals/HelpModal';
import { GameHeader } from '../../components/GameHeader';
import { TurnAdvisor } from '../../components/TurnAdvisor';
import { LedgerDrawer } from '../../components/LedgerDrawer';
import { InlineStockMarket } from '../../components/InlineStockMarket';
import { ActionDock } from '../../components/ActionDock';
import { CLASSIC_CONFIG, createInitialGameState } from '../../engine/config';
import { GameState } from '../../engine/types';

describe('German UI Localization Integration', () => {
  const dummyState = createInitialGameState(CLASSIC_CONFIG, ['Hans', 'Fritz']);

  beforeEach(() => {
    setLanguage('de');
  });

  afterEach(() => {
    setLanguage('en');
  });

  it('verifies language state toggles to German', () => {
    expect(getLanguage()).toBe('de');
  });

  it('renders MenuBar in German', () => {
    const html = renderToStaticMarkup(
      <MenuBar
        onNewGameNew={vi.fn()}
        onOpenOptions={vi.fn()}
        onLoadGame={vi.fn()}
        onHelpRules={vi.fn()}
        onToggleSound={vi.fn()}
        onViewWarRoom={vi.fn()}
        onViewWealth={vi.fn()}
        onViewShares={vi.fn()}
        onViewDiscarded={vi.fn()}
        soundMuted={false}
      />
    );
    expect(html).toContain('piel'); // <span class="mnemonic">S</span>piel
    expect(html).toContain('ktionen'); // <span class="mnemonic">A</span>ktionen
    expect(html).toContain('nsichten'); // <span class="mnemonic">A</span>nsichten
    expect(html).toContain('ptionen'); // <span class="mnemonic">O</span>ptionen
    expect(html).toContain('ilfe'); // <span class="mnemonic">H</span>ilfe
    expect(html).toContain('Mensch-Zug');
    expect(html).toContain('DE');
  });

  it('renders CompanyTicker in German', () => {
    const html = renderToStaticMarkup(
      <CompanyTicker
        companies={dummyState.companies}
        onSelectCompany={vi.fn()}
      />
    );
    expect(html).toContain('BÖRSE');
    expect(html).toContain('Noch keine aktiven Konzerne auf dem Spielfeld');
    expect(html).toContain('Verfügbar:');
  });

  it('renders PlayerScoreboard in German', () => {
    const html = renderToStaticMarkup(
      <PlayerScoreboard
        players={dummyState.players}
        activePlayerIndex={0}
        companies={dummyState.companies}
      />
    );
    expect(html).toContain('PUNKTESTAND');
    expect(html).toContain('Bargeld');
    expect(html).toContain('Gesamtvermögen');
    expect(html).toContain('AM ZUG');
  });

  it('renders PlayerHand in German', () => {
    const html = renderToStaticMarkup(
      <PlayerHand
        hand={[{ row: 0, col: 0 }, { row: 1, col: 1 }]}
        activePlayerName="Hans"
        board={dummyState.board}
      />
    );
    expect(html).toContain('Hand von Hans:');
    expect(html).toContain('2 / 6 Felder');
    expect(html).toContain('Feld platzieren');
    expect(html).toContain('Totes Feld ablegen');
  });

  it('renders TileInspector in German', () => {
    const html = renderToStaticMarkup(
      <TileInspector
        coord={{ row: 0, col: 0 }}
        board={dummyState.board}
        companies={dummyState.companies}
      />
    );
    expect(html).toContain('Schnellansicht — Feld A-1');
    expect(html).toContain('Reihe A • Spalte 1');
  });

  it('renders OptionsModal in German', () => {
    const html = renderToStaticMarkup(
      <OptionsModal
        isOpen={true}
        onClose={vi.fn()}
        config={CLASSIC_CONFIG}
        onSaveConfig={vi.fn()}
        slowCpu={false}
      />
    );
    expect(html).toContain('Spieloptionen &amp; Regelwerk');
    expect(html).toContain('Regelwerk-Paradigma');
    expect(html).toContain('Einstellungen übernehmen');
    expect(html).toContain('Sprache');
  });

  it('renders NewGameModal in German', () => {
    const html = renderToStaticMarkup(
      <NewGameModal
        isOpen={true}
        onClose={vi.fn()}
        onStartGame={vi.fn()}
      />
    );
    expect(html).toContain('Neue Partie — Spielereinstellungen');
    expect(html).toContain('Schnellauswahl');
    expect(html).toContain('Spielerliste (2–6 Spieler)');
    expect(html).toContain('Spiel starten');
  });

  it('renders FoundingCompanyModal in German', () => {
    const html = renderToStaticMarkup(
      <FoundingCompanyModal
        isOpen={true}
        gameState={dummyState}
        availableCompanyIds={['american', 'imperial']}
        onSelectCompany={vi.fn()}
      />
    );
    expect(html).toContain('Neuen Ölkonzern gründen');
    expect(html).toContain('+10 Gratis-Aktien');
    expect(html).toContain('Gründen &amp; Bonus beanspruchen');
  });

  it('renders DiscardedTilesModal in German', () => {
    const html = renderToStaticMarkup(
      <DiscardedTilesModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
      />
    );
    expect(html).toContain('Abgelegte &amp; unspielbare Felder');
    expect(html).toContain('Dauerhaft abgelegt');
    expect(html).toContain('Schließen');
  });

  it('renders StockMarketModal in German', () => {
    const html = renderToStaticMarkup(
      <StockMarketModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
        onConfirmPurchase={vi.fn()}
      />
    );
    expect(html).toContain('Börse — Aktien kaufen');
    expect(html).toContain('Käufer-Portfolio');
    expect(html).toContain('Verfügbares Bargeld');
    expect(html).toContain('Passen / Zug beenden');
  });

  it('renders CompanyStatsModal in German', () => {
    const html = renderToStaticMarkup(
      <CompanyStatsModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
      />
    );
    expect(html).toContain('Unternehmensstatistik &amp; Aktionärsregister');
    expect(html).toContain('Feldgröße (Felder)');
    expect(html).toContain('Aktienkurs');
    expect(html).toContain('Aktionärsverteilung');
  });

  it('renders Merger modals in German', () => {
    const survivorHtml = renderToStaticMarkup(
      <MergerSurvivorModal
        isOpen={true}
        gameState={dummyState}
        tiedCompanyIds={['american', 'arabian']}
        onChooseSurvivor={vi.fn()}
      />
    );
    expect(survivorHtml).toContain('Fusionskonflikt — Entscheidung bei Gleichstand');
    expect(survivorHtml).toContain('Überlebenden bestätigen');

    const stateWithHoldings: GameState = {
      ...dummyState,
      players: [
        {
          ...dummyState.players[0]!,
          shares: { ...dummyState.players[0]!.shares, american: 10 },
        },
        {
          ...dummyState.players[1]!,
          shares: { ...dummyState.players[1]!.shares, american: 4 },
        },
      ],
    };

    const mergerStatsHtml = renderToStaticMarkup(
      <MergerStatsModal
        isOpen={true}
        onClose={vi.fn()}
        gameState={stateWithHoldings}
        defunctCompanyId="american"
        survivorCompanyId="imperial"
        preMergerPrice={500}
      />
    );
    expect(mergerStatsHtml).toContain('Fusionsauszahlungen &amp; Aktionärsboni');
    expect(mergerStatsHtml).toContain('1. Mehrheitsbonus-Pool (10×)');
    expect(mergerStatsHtml).toContain('2. Minderheitsbonus-Pool (5×)');
    expect(mergerStatsHtml).toContain('Weiter zur Aktienabwicklung');

    const mergerState: GameState = {
      ...dummyState,
      pendingMerger: {
        coord: { row: 0, col: 0 },
        participatingCompanyIds: ['american', 'imperial'],
        survivingCompanyId: 'imperial',
        defunctCompanyQueue: ['american'],
        currentDefunctCompanyId: 'american',
        shareholderQueue: [dummyState.players[0]!.id],
        currentShareholderIndex: 0,
        preMergerPrices: { american: 300, imperial: 500 },
      },
      players: [
        {
          ...dummyState.players[0]!,
          shares: { ...dummyState.players[0]!.shares, american: 6 },
        },
        dummyState.players[1]!,
      ],
    };

    const dispositionHtml = renderToStaticMarkup(
      <ShareDispositionModal
        isOpen={true}
        gameState={mergerState}
        defunctCompanyId="american"
        survivorCompanyId="imperial"
        preMergerPrice={300}
        onConfirmDisposition={vi.fn()}
      />
    );
    expect(dispositionHtml).toContain('Fusions-Aktienabwicklung');
    expect(dispositionHtml).toContain('Alle verkaufen');
    expect(dispositionHtml).toContain('Aktienabwicklung bestätigen');
  });

  it('renders Task 8 visualization and utility modals in German', () => {
    const wealthHtml = renderToStaticMarkup(
      <WealthChartModal isOpen={true} onClose={vi.fn()} gameState={dummyState} />
    );
    expect(wealthHtml).toContain('Vermögensbalkendiagramm — Gesamtvermögen');
    expect(wealthHtml).toContain('Flüssiges Bargeld');
    expect(wealthHtml).toContain('Aktienportfolio');

    const shareGraphHtml = renderToStaticMarkup(
      <ShareGraphModal isOpen={true} onClose={vi.fn()} gameState={dummyState} />
    );
    expect(shareGraphHtml).toContain('Aktiengrafik — Aktienbesitz-Matrix');
    expect(shareGraphHtml).toContain('Gesamt');
    expect(shareGraphHtml).toContain('Prozentual (%)');
    expect(shareGraphHtml).toContain('Bank-Reserve');

    const warRoomHtml = renderToStaticMarkup(
      <WarRoomModal isOpen={true} onClose={vi.fn()} gameState={dummyState} />
    );
    expect(warRoomHtml).toContain('Lagezentrum — Strategische Übersicht');
    expect(warRoomHtml).toContain('Spielfeldbelegung');
    expect(warRoomHtml).toContain('Sichere Konzerne');
    expect(warRoomHtml).toContain('Felder im Ziehbeutel');

    const saveLoadHtml = renderToStaticMarkup(
      <SaveLoadModal isOpen={true} onClose={vi.fn()} gameState={dummyState} onLoadGame={vi.fn()} />
    );
    expect(saveLoadHtml).toContain('Spielstand speichern &amp; laden');
    expect(saveLoadHtml).toContain('Spiel speichern');
    expect(saveLoadHtml).toContain('Als JSON exportieren');
    expect(saveLoadHtml).toContain('JSON-Spielstand importieren');

    const helpHtml = renderToStaticMarkup(
      <HelpModal isOpen={true} onClose={vi.fn()} />
    );
    expect(helpHtml).toContain('Oil Baron — Spielregeln &amp; Strategiehandbuch');
    expect(helpHtml).toContain('Übersicht &amp; Spielfeld');
    expect(helpHtml).toContain('Konzerne gründen');
    expect(helpHtml).toContain('Aktienmarkt');
    expect(helpHtml).toContain('Fusionen &amp; Boni');
    expect(helpHtml).toContain('Spielende &amp; Wertung');

    const gameoverHtml = renderToStaticMarkup(
      <GameOverModal isOpen={true} onClose={vi.fn()} gameState={dummyState} onNewGame={vi.fn()} />
    );
    expect(gameoverHtml).toContain('Spiel beendet — Der Sieger von Oil Baron!');
    expect(gameoverHtml).toContain('Großer Petroleum-Baron');
    expect(gameoverHtml).toContain('Endstand &amp; Liquidiertes Vermögen');
    expect(gameoverHtml).toContain('Spielfeld inspizieren');
    expect(gameoverHtml).toContain('Neues Spiel starten');
  });

  it('renders modern strategy game components in German', () => {
    const headerHtml = renderToStaticMarkup(
      <GameHeader
        activePlayerName="Hans"
        isAi={false}
        turnPhase="place_tile"
        soundMuted={false}
        language="de"
        onToggleSound={vi.fn()}
        onToggleLanguage={vi.fn()}
        onToggleLedger={vi.fn()}
        onOpenRules={vi.fn()}
        onOpenNewGame={vi.fn()}
        onOpenOptions={vi.fn()}
        onOpenSaveLoad={vi.fn()}
        onSwitchClassic={vi.fn()}
      />
    );
    expect(headerHtml).toContain('STRATEGIE');
    expect(headerHtml).toContain('Schritt 1: Ölbohrung setzen');
    expect(headerHtml).toContain('Hauptbuch');
    expect(headerHtml).toContain('1993-Ansicht');

    const advisorHtml = renderToStaticMarkup(
      <TurnAdvisor
        phase="place_tile"
        isAi={false}
        activePlayerName="Hans"
        selectedCoord={null}
        hoveredCoord={null}
        board={dummyState.board}
      />
    );
    expect(advisorHtml).toContain('Wähle ein Feld aus deiner Hand unten für eine Bohrung');

    const ledgerHtml = renderToStaticMarkup(
      <LedgerDrawer
        isOpen={true}
        onClose={vi.fn()}
        gameState={dummyState}
      />
    );
    expect(ledgerHtml).toContain('Hauptbuch &amp; Strategische Analysen');
    expect(ledgerHtml).toContain('Nettovermögen &amp; Liquidität');

    const shelfHtml = renderToStaticMarkup(
      <InlineStockMarket
        activePlayerCash={2000}
        companies={{}}
        definitions={dummyState.config.companies}
        onConfirmPurchases={vi.fn()}
        onPass={vi.fn()}
      />
    );
    expect(shelfHtml).toContain('Börsenhandel: Kaufe bis zu 3 Aktien');
    expect(shelfHtml).toContain('Passen &amp; Zug beenden (Alt+E)');
  });
});
