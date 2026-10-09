# Oil Baron: Authentic 1993 Windows 3.1 Classic Mode & Turn Guidance System Spec

## 1. Objectives & Problems Solved
1. **Visual Atmosphere & Nostalgia**: The current UI feels overly dark/grey and modern-industrial. We provide an authentic 1:1 Windows 3.1 retro presentation using original 1993 16-color pixel art, 3D beveled chrome, navy titlebars, and Borland dialog aesthetics.
2. **Step-by-Step Onboarding ("What do I do now?")**: Introduce a prominent, interactive, color-coded **Turn Guidance Strip** (Beginner Hints enabled by default) that clearly walks the player through each turn phase step-by-step:
   - Selecting a chit from hand
   - Clicking the highlighted cell on the 10×12 grid
   - Founding companies & merger resolution
   - Purchasing shares or passing to end turn
3. **Seamless Multi-View Experience (Option A)**:
   - Modern Edition (`/` or Modern view mode) with enhanced contrast, vibrant highlights, and turn coach.
   - Classic 1993 Edition (`/classic` or Classic view mode) with 100% authentic Windows 3.1 chrome, original 32×32 pixel bitmaps (`chit.png` oil derrick, company logos), and retro dialogs.
   - An instant 1-click switcher in the top bar / menu to switch between Modern and Classic editions at any time during an active game without resetting or losing match state.

---

## 2. Visual Design & Theme System

### 2.1 Authentic 1993 Windows 3.1 Styling Tokens (`classic.css`)
- **Desktop Background**: Authentic Windows 3.1 Teal (`#008080`)
- **Window Surfaces**: Windows 3.1 ButtonFace grey (`#c0c0c0`)
- **Active Titlebars**: Windows 3.1 Navy blue (`#000080`) with bold white text and classic minimize/maximize buttons
- **Window Borders**: 3D beveled borders:
  - Raised elements: `border-top: 2px solid #ffffff; border-left: 2px solid #ffffff; border-right: 2px solid #808080; border-bottom: 2px solid #808080;`
  - Sunken grid/wells: `border-top: 2px solid #808080; border-left: 2px solid #808080; border-right: 2px solid #ffffff; border-bottom: 2px solid #ffffff;`
- **Typography**: Authentic bitmap font emulation (`"MS Sans Serif", "Microsoft Sans Serif", Tahoma, Arial, sans-serif`)
- **Original Bitmaps**:
  - Empty cells: Grey beveled squares with small coordinate notation
  - Wildcat wells: Original `chit.png` oil derrick icon
  - Corporate territory: Original 32×32 bitmap company logos (`american.png`, `arabian.png`, `imperial.png`, `supra.png`, `tower.png`, `royal.png`, `weemoco.png`, `purple.png`)
  - Dead / unplayable indicator: Original `not_playable.png`

---

## 3. Interactive Turn Guidance Strip (`TurnGuidanceBar.tsx`)

### 3.1 Dynamic Phase Coaching
The Turn Guidance Strip sits directly above the game board and hand tray, styled with high contrast:
- **Phase: `place_tile` (No chit selected)**:
  - Badge: `STEP 1 OF 3`
  - Message: `👉 Select an oil lease chit from your rack below to prepare drilling.`
  - Visual: Hand chits have an inviting pulsing amber outline.
- **Phase: `place_tile` (Chit selected, e.g. `D-5`)**:
  - Badge: `STEP 2 OF 3`
  - Message: `🎯 Selected [D-5]! Click coordinate D-5 on the board grid to drill your well.`
  - Action button: Direct `Drill Well at D-5` button.
  - Visual: Cell `D-5` on the board grid pulses with a high-visibility gold beacon border.
- **Phase: `choose_founding_company`**:
  - Badge: `COMPANY FOUNDED!`
  - Message: `🎉 You discovered a new oil field! Choose which corporation to incorporate (+10 Free Shares).`
- **Phase: `choose_merger_survivor`**:
  - Badge: `MERGER TIE-BREAK`
  - Message: `⚖️ Companies tied for largest size! Select which corporation survives the takeover.`
- **Phase: `dispose_merger_shares`**:
  - Badge: `SHARE DISPOSITION`
  - Message: `💼 Dispose of defunct stock: Sell for cash, Trade 2-for-1, or Hold for reserve.`
- **Phase: `buy_shares`**:
  - Badge: `STEP 3 OF 3`
  - Message: `📈 Placement complete! Buy up to 3 corporate shares on the market, or click End Turn.`
  - Quick action buttons: `[Buy Shares (Alt+B)]` and `[End Turn / Pass (Alt+E)]`.
- **Phase: `game_over`**:
  - Badge: `GAME OVER`
  - Message: `🏆 Match concluded! Portfolios liquidated. Inspect final standings.`

### 3.2 Beginner Hints Toggle
- A simple `Hints: ON / OFF` toggle badge right inside the guidance bar and under **Settings -> Beginner Hints**.

---

## 4. Architecture & Component Structure

1. **State Persistence & Synchronization**:
   - Single source of truth: `gameState` remains shared across both views.
   - Switcher toggles `uiTheme: 'modern' | 'classic'`.
   - URL synchronization: Supports `/` (Modern) and `/classic` (Classic Windows 3.1) via hash routing (`window.location.hash` or path) without losing in-memory game state.
2. **Components**:
   - `webgame/src/components/TurnGuidanceBar.tsx`: High-visibility turn guidance and action prompt.
   - `webgame/src/components/classic/ClassicGameBoard.tsx`: Board rendering using authentic 32×32 pixel art bitmaps and beveled Windows 3.1 cells.
   - `webgame/src/components/classic/ClassicPlayerHand.tsx`: Retro chit rack with original 3D push buttons.
   - `webgame/src/components/classic/ClassicMenuBar.tsx`: Windows 3.1 menu styling with classic dropdown boxes.
   - `webgame/src/components/classic/ClassicWindow.tsx`: Reusable Windows 3.1 window frame with navy title bar, close/minimize boxes, and bevels.
   - `webgame/src/styles/classic.css`: Complete Windows 3.1 stylesheet.

---

## 5. Verification & Test Plan
1. Unit tests for `TurnGuidanceBar`:
   - Renders step 1 when no tile selected.
   - Renders step 2 with target coordinate when tile is selected.
   - Renders buy shares step with direct action buttons in `buy_shares` phase.
   - Toggles hints visibility.
2. Unit tests for Classic components:
   - Renders original image bitmaps for chits and company tiles.
   - Toggles between Modern and Classic views without mutating game state.
3. Full Vitest test suite (`npm test`) & Vite production build (`npm run build`).
