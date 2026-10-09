# Oil Baron Modern React Web Game Specification (Phase 3)

## 1. Overview & Vision

Phase 3 builds the complete browser-based playable game in `webgame/` using **React, Vite, and Modern CSS**. It faithfully represents **every feature of the original 1993 Oil Baron for Windows**, but enhanced with modern responsive layout, smooth animations, accessible dialogs, interactive data visualizations, and retro-modern sound effects.

The application runs 100% client-side with zero backend requirement, connects directly to our headless engine in `webgame/src/engine/`, and is prepared for instant local play.

---

## 2. Comprehensive Feature Matrix (Original vs Modern)

| Original Feature | Windows 3.1 Dialog / Menu | Phase 3 React Component & Implementation |
|---|---|---|
| 2–6 Player Setup | `NEW GAME` dialog | `NewGameModal.tsx`: Select 2–6 players, custom names, Human vs CPU toggles. |
| Ruleset & Options | `OPTIONS` dialog | `OptionsModal.tsx`: Ruleset (Classic 10-founder vs Modern 1-founder), Hand size (4–10), Safe threshold (11), starting cash. |
| Speed & CPU Takeover | `COMPUTER` menu | Mid-game "Change to Human / Change to CPU" toggle, and "Slow Computer Turns" simulation speed toggle. |
| 10×12 Board Grid | Main window | `GameBoard.tsx`: $10 \times 12$ grid with row (A–J) and col (1–12) coordinates, company logos, hover move preview. |
| Quick Peek / Tile Info | `QUICK_PEEK` dialog | `TileInspector.tsx`: Right-click or hover tooltip showing tile coordinates, owning company, and placement preview. |
| Hand Management | `MAIN_MENU` chits | `PlayerHand.tsx`: Interactive chits tray with coordinate chips, legal placement highlights, and "Hide Chits" hotseat mode. |
| Discarding Dead Tiles | `DISCARDPIECE` dialog | `DiscardModal.tsx`: Discard dead/unplayable tiles to draw replacements, with "Show Discarded Pieces" history. |
| Company Statistics | `COMPANY_STATS` dialog | `CompanyStatsModal.tsx`: Grid of all 7 companies (+ Purple) with sizes, stock prices, bank inventory, and safe status. |
| Stock Purchases | `SHARES DIALOG` / `Alt+B` | `StockMarketModal.tsx`: Up to 3 shares per turn, dynamic price recalculation, cash budget enforcement. |
| Merger Detection & Survivor | `CHOOSE COMPANY` dialog | `MergerAnnouncement.tsx` & `MergerSurvivorModal.tsx`: Visual takeover fanfare and tied survivor picker. |
| Merger Payouts | `MERGER STATISTICS` dialog | `MergerStatsModal.tsx`: Breakdown of 1st place ($10 \times P$) and 2nd place ($5 \times P$) bonuses, including tie splits. |
| Share Disposition | `KEEP_SHARES` dialog | `ShareDispositionModal.tsx`: Interactive counters for Sell, Trade 2-for-1, and Hold with live payout preview. |
| Wealth Bar Chart | `Alt+N` / Wealth Bar Chart | `WealthChartModal.tsx`: Dynamic SVG/CSS bar chart comparing Cash vs Stock Value across all players. |
| Share Graph & Holdings | `SHARE_STATS` / `Alt+S` | `ShareGraphModal.tsx`: Matrix chart of stock holdings per company across players with **Percentages** vs **Totals** toggle. |
| War Room Dashboard | `War Room` menu | `WarRoomModal.tsx`: High-level strategic overview of board dominance, safe chains, and leaderboards. |
| Save / Load System | `.oil` file save/load | `SaveLoadModal.tsx`: Save/Load to `localStorage` and JSON file export/import. |
| Retro Sound Effects | PC Speaker / SoundBlaster | `audio.ts`: Web Audio API synthesizer for clicks, founding fanfare, takeover sirens, and cash payouts. |
| Game Over Screen | `GAME_OVER` dialog | `GameOverModal.tsx`: Final liquidation breakdown, net worth trophies, and victory celebration. |

---

## 3. Architecture & Component Hierarchy

```
webgame/src/
├── main.tsx                  # React DOM root
├── App.tsx                   # Main game orchestrator & top navigation menu
├── index.css                 # Modern CSS design tokens, typography, grid styling
├── assets/
│   ├── logos/                # High-res SVG and extracted 16-bit company logos
│   └── audio.ts              # Web Audio API procedural retro sound synthesizer
├── components/
│   ├── MenuBar.tsx           # Authentic Windows/Modern hybrid top bar with keyboard shortcuts
│   ├── GameBoard.tsx         # Responsive 10x12 board with hover preview and animations
│   ├── PlayerHand.tsx        # Active player's chit tray with placement actions
│   ├── PlayerScoreboard.tsx  # Sidebar listing all players, net worth, cash, and turn indicators
│   ├── CompanyTicker.tsx     # Stock market summary bar showing all active companies
│   ├── ActionHistory.tsx     # Game log recording all actions, purchases, and mergers
│   └── modals/
│       ├── NewGameModal.tsx
│       ├── OptionsModal.tsx
│       ├── CompanyStatsModal.tsx
│       ├── StockMarketModal.tsx
│       ├── MergerSurvivorModal.tsx
│       ├── MergerStatsModal.tsx
│       ├── ShareDispositionModal.tsx
│       ├── WealthChartModal.tsx
│       ├── ShareGraphModal.tsx
│       ├── WarRoomModal.tsx
│       ├── DiscardedTilesModal.tsx
│       ├── SaveLoadModal.tsx
│       ├── HelpModal.tsx
│       └── GameOverModal.tsx
```

---

## 4. Key UX & Interaction Details

### 4.1 Turn Progression Flow
1. **Human Turn**:
   - Player hovers over chits in hand: the board highlights target cells with color-coded previews (Green = Legal/Founding, Yellow = Expansion, Red/Orange = Merger, Dim = Dead).
   - Player clicks board cell or chit: engine applies `PLACE_TILE`.
   - If Founding: `NewCompanyModal` opens; player picks available company.
   - If Merger: Takeover splash triggers $\rightarrow$ `MergerStatsModal` shows payouts $\rightarrow$ `ShareDispositionModal` resolves shareholder stocks.
   - Phase advances to `buy_shares`: stock drawer opens; player can buy up to 3 shares or click "End Turn".
2. **CPU Turn**:
   - If "Slow Computer Turns" is enabled, displays a brief thinking indicator (~400ms), highlights the CPU's chosen tile, animates placement, handles dialogs automatically, and passes turn cleanly.

### 4.2 Keyboard Shortcuts (Faithful to 1993 Original)
- `Alt+P` or `Space`: Place selected chit
- `Alt+B`: Open Stock Market / Buy Shares
- `Alt+D`: Discard dead chit
- `Alt+E`: End Turn / Pass
- `Alt+N`: Wealth Bar Chart
- `Alt+S`: Share Ownership Graph
- `Alt+C`: Company Statistics
- `Alt+W`: War Room

### 4.3 Procedural Retro Web Audio (`audio.ts`)
Zero sound file loading required: utilizes browser `AudioContext` to synthesize nostalgic chiptune tones:
- *Tile Click*: Short subtle click (800Hz $\rightarrow$ 200Hz sine).
- *Found Company*: Rising major arpeggio.
- *Merger Siren*: Low dramatic two-tone alarm.
- *Cash Register*: Crisp dual-frequency chime (1200Hz + 2400Hz).
- *Victory*: Glorious fanfare melody.

---

## 5. Verification & Acceptance Criteria
1. Game runs in modern browser via `npm run dev` with clean console (0 errors).
2. Production build succeeds with `npm run build` with 0 TypeScript errors.
3. All original 1993 features (New Game, Options, Stock Market, Company Stats, Merger Payouts & Dispositions, Wealth Bar Chart, Share Matrix Graph, War Room, Save/Load, Human/CPU toggling) are fully functional.
4. Engine tests continue to pass 100% on `npm test`.
