# Classic Mode (1993 Windows 3.1) & Turn Guidance System Implementation Plan

**Goal:** Implement Option A: Add a prominent, high-contrast, interactive **Turn Guidance Bar** (Beginner Hints enabled by default) explaining what to do at every step, and build an authentic 1993 Windows 3.1 **Classic Mode** with original 16-color pixel bitmaps, `#c0c0c0` beveled chrome, and an instant 1-click switcher between Modern and Classic editions (`/` vs `/classic`).

**Tech Stack:** React 18, TypeScript, CSS Variables, Original 1993 32×32 PNG Bitmaps, Vitest.

---

### Task 1: Turn Guidance Bar & Beginner Hints (`TurnGuidanceBar.tsx`)
**Files:**
- Create: `webgame/src/components/TurnGuidanceBar.tsx`
- Create: `webgame/src/components/__tests__/TurnGuidanceBar.test.tsx`

1. Implement `TurnGuidanceBar.tsx`:
   - Step 1: Tell user to select a chit in hand when none selected.
   - Step 2: Show selected coordinate (e.g. `[D-5]`), tell user to click `D-5` on board to drill, with quick "Drill Well at D-5" action button.
   - Step 3 (Buy Shares): Show purchase status, budget remaining, and direct `[Buy Shares (Alt+B)]` and `[End Turn (Alt+E)]` buttons.
   - Company Founding & Merger phases: Display prominent instructions on current event.
   - Beginner Hints toggle: Allows toggling between detailed coach mode and compact status mode.
2. Unit tests in `TurnGuidanceBar.test.tsx`.
3. Verify test pass.

---

### Task 2: Classic 1993 Windows 3.1 Assets & Styling Tokens (`classic.css`)
**Files:**
- Create: `webgame/src/assets/classicAssets.ts`
- Create: `webgame/src/styles/classic.css`
- Modify: `webgame/src/index.css` (import `classic.css`)

1. Register bitmap mappings in `classicAssets.ts`:
   - Map company IDs (`american`, `arabian`, `imperial`, `supra`, `tower`, `royal`, `weemoco`, `purple`) to `/classic_assets/{name}.png`.
   - Map wildcat well chit to `/classic_assets/chit.png`.
   - Map dead tile indicator to `/classic_assets/not_playable.png`.
   - Map discard and next turn icons.
2. Implement `classic.css`:
   - Authentic Windows 3.1 teal `#008080` desktop.
   - Beveled 3D windows (`#c0c0c0`, light top/left, dark bottom/right).
   - Windows 3.1 titlebar (`#000080` navy blue, white bold text, system box, minimize/maximize boxes).
   - Classic button styles (`border: 2px outset`, active `inset`).
   - High contrast retro grid cells.

---

### Task 3: Classic Windows 3.1 Game View (`ClassicGameView.tsx`)
**Files:**
- Create: `webgame/src/components/classic/ClassicGameView.tsx`
- Create: `webgame/src/components/classic/ClassicWindow.tsx`
- Create: `webgame/src/components/classic/__tests__/ClassicGameView.test.tsx`

1. Implement `ClassicWindow.tsx`:
   - Window frame with authentic Windows 3.1 title bar, system icon, and beveled container.
2. Implement `ClassicGameView.tsx`:
   - Renders 10×12 grid using original 32×32 bitmaps directly inside the cells (oil derrick for chits, company logo for corporation tiles).
   - Renders classic hand rack with original retro buttons.
   - Embeds the Turn Guidance Bar with retro styling.
   - Includes 1-click toggle button to switch back to Modern Edition at any time.

---

### Task 4: View Routing, Modern Polish & Full Integration in `App.tsx`
**Files:**
- Modify: `webgame/src/App.tsx`
- Modify: `webgame/src/__tests__/App.test.tsx`

1. Integrate view toggle (`viewMode: 'modern' | 'classic'`) in `App.tsx`:
   - Check `window.location.hash` or path for `#/classic` or `/classic`.
   - Add "Switch to 1993 Windows 3.1 Classic" button in top navigation bar and header.
   - Embed `TurnGuidanceBar` in both Modern and Classic views.
   - Enhance Modern view contrast so board and controls pop with vibrant highlights.
2. Run full test suite (`npm test`) and production build (`npm run build`).
3. Commit and verify.
