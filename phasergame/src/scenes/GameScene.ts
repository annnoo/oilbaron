import Phaser from 'phaser';
import type { GameState, TileCoord, CompanyId } from '../engine/types';
import { createInitialGameState, DEFAULT_CONFIG } from '../engine/config';
import { gameReducer } from '../engine/reducer';
import { HudBar } from '../ui/HudBar';
import { ConcessionsTray } from '../ui/ConcessionsTray';
import { FoundingDialog } from '../ui/FoundingDialog';
import { MergerDialog } from '../ui/MergerDialog';
import { parseCoord, formatCoord } from '../engine/board';

export class GameScene extends Phaser.Scene {
  public gameState!: GameState;
  private selectedTile: TileCoord | null = null;
  private hudBar!: HudBar;
  private concessionsTray!: ConcessionsTray;
  private boardContainer!: Phaser.GameObjects.Container;
  private cellSprites: Map<string, Phaser.GameObjects.Container> = new Map();
  private escKey!: Phaser.Input.Keyboard.Key;
  private spaceKey!: Phaser.Input.Keyboard.Key;

  constructor() {
    super('GameScene');
  }

  init(data?: { loadedState?: GameState }) {
    if (data?.loadedState) {
      this.gameState = data.loadedState;
    } else {
      this.gameState = createInitialGameState(DEFAULT_CONFIG, [
        { name: 'Baron Alpha', isAi: false },
        { name: 'Rockefeller AI', isAi: true, aiDifficulty: 'smart' },
      ]);
    }
  }

  create() {
    const { width, height } = this.scale;

    // Dark seismic survey grid background
    this.add.rectangle(width / 2, height / 2, width, height, 0x050814);

    // Board container
    this.boardContainer = this.add.container(0, 0);

    // Setup Top HUD
    this.hudBar = new HudBar(this, width, {
      onOpenMenu: () => this.openPauseMenu(),
      onOpenStockMarket: () => this.openStockMarket(),
    });

    // Setup Bottom Concessions Rack
    this.concessionsTray = new ConcessionsTray(this, width, height - 90, {
      onSelectTile: (coord) => {
        this.selectedTile = coord;
        this.refreshUI();
      },
      onDrillWell: () => {
        this.executeDrillAction();
      },
      onDiscardDead: () => {
        this.executeDiscardAction();
      },
    });

    // Create 10x12 Geological Survey Board
    this.createBoardGrid();

    // Register Keyboard Shortcuts (ESC opens Pause Menu)
    if (this.input.keyboard) {
      this.escKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.ESC);
      this.escKey.on('down', () => this.openPauseMenu());

      this.spaceKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SPACE);
      this.spaceKey.on('down', () => {
        if (this.gameState.phase === 'place_tile' && this.selectedTile) {
          this.executeDrillAction();
        } else if (this.gameState.phase === 'buy_shares') {
          this.openStockMarket();
        }
      });
    }

    this.refreshUI();
  }

  public openPauseMenu() {
    this.scene.pause('GameScene');
    this.scene.launch('PauseScene');
  }

  public openStockMarket() {
    this.scene.pause('GameScene');
    this.scene.launch('StockScene');
  }

  private createBoardGrid() {
    const rows = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'];
    const cols = Array.from({ length: 12 }, (_, i) => i + 1);

    const cellW = 54;
    const cellH = 46;
    const startX = 310;
    const startY = 100;

    // Perimeter coordinate rulers
    // Column rulers (1-12)
    cols.forEach((col, idx) => {
      const cx = startX + idx * cellW + cellW / 2;
      this.boardContainer.add(
        this.add.text(cx, startY - 18, String(col), {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '13px',
          fontStyle: 'bold',
          color: '#64748b',
        }).setOrigin(0.5)
      );
    });

    // Row rulers (A-J)
    rows.forEach((row, rIdx) => {
      const cy = startY + rIdx * cellH + cellH / 2;
      this.boardContainer.add(
        this.add.text(startX - 22, cy, row, {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '13px',
          fontStyle: 'bold',
          color: '#64748b',
        }).setOrigin(0.5)
      );
    });

    // Grid cells
    rows.forEach((row, rIdx) => {
      cols.forEach((col, cIdx) => {
        const coordKey = `${row}-${col}`;
        const parsed = parseCoord(coordKey);
        if (!parsed) return;

        const x = startX + cIdx * cellW + cellW / 2;
        const y = startY + rIdx * cellH + cellH / 2;

        const cellContainer = this.add.container(x, y);

        const rect = this.add.rectangle(0, 0, cellW - 2, cellH - 2, 0x0c1527)
          .setStrokeStyle(1, 0x1e293b)
          .setInteractive({ useHandCursor: true });

        // Center guide dot for empty parcel
        const dot = this.add.circle(0, 0, 2, 0x334155);

        // Text display for solitary / conglomerate well
        const wellGlyph = this.add.text(0, -6, '', {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '15px',
          fontStyle: 'bold',
          color: '#ffffff',
        }).setOrigin(0.5);

        const wellSub = this.add.text(0, 9, '', {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '10px',
          fontStyle: 'bold',
          color: '#cbd5e1',
        }).setOrigin(0.5);

        rect.on('pointerdown', () => {
          if (this.gameState.phase === 'place_tile') {
            const activePlayer = this.gameState.players[this.gameState.activePlayerIndex];
            if (activePlayer) {
              const inHand = activePlayer.hand.some((h) => h.row === parsed.row && h.col === parsed.col);
              if (inHand) {
                this.selectedTile = parsed;
                this.refreshUI();
              }
            }
          }
        });

        cellContainer.add([rect, dot, wellGlyph, wellSub]);
        this.boardContainer.add(cellContainer);
        this.cellSprites.set(coordKey, cellContainer);
      });
    });
  }

  private refreshUI() {
    this.hudBar.updateState(this.gameState);
    this.concessionsTray.updateState(this.gameState, this.selectedTile);

    // Update board cell graphics
    this.gameState.board.forEach((row, rIdx) => {
      row.forEach((cell, cIdx) => {
        const coord: TileCoord = { row: rIdx, col: cIdx };
        const coordKey = formatCoord(coord);
        const sprite = this.cellSprites.get(coordKey);
        if (!sprite) return;

        const rect = sprite.getAt(0) as Phaser.GameObjects.Rectangle;
        const dot = sprite.getAt(1) as Phaser.GameObjects.Arc;
        const wellGlyph = sprite.getAt(2) as Phaser.GameObjects.Text;
        const wellSub = sprite.getAt(3) as Phaser.GameObjects.Text;

        const isSelected = this.selectedTile !== null && this.selectedTile.row === rIdx && this.selectedTile.col === cIdx;

        if (cell.type === 'empty') {
          dot.setVisible(!isSelected);
          wellGlyph.setText('');
          wellSub.setText('');
          if (isSelected) {
            rect.setFillStyle(0x0f2744);
            rect.setStrokeStyle(2, 0x38bdf8);
          } else {
            rect.setFillStyle(0x0c1527);
            rect.setStrokeStyle(1, 0x1e293b);
          }
        } else if (cell.type === 'unincorporated') {
          dot.setVisible(false);
          rect.setFillStyle(0x334155);
          rect.setStrokeStyle(1, 0x94a3b8);
          wellGlyph.setText('▲');
          wellSub.setText('');
        } else if (cell.type === 'company' && cell.companyId) {
          dot.setVisible(false);
          const comp = this.gameState.companies[cell.companyId];
          const hex = this.getCompanyColorHex(cell.companyId);
          rect.setFillStyle(hex);
          rect.setStrokeStyle(comp.isSafe ? 2 : 1, comp.isSafe ? 0xfacc15 : 0xffffff);

          wellGlyph.setText('◆');
          wellSub.setText(comp.name.substring(0, 3).toUpperCase());
        }
      });
    });

    // Check if phase is founding company
    if (this.gameState.phase === 'choose_founding_company' && this.gameState.pendingFounding) {
      const activePlayer = this.gameState.players[this.gameState.activePlayerIndex];
      if (activePlayer && !activePlayer.isAi) {
        const available = this.gameState.config.companies.filter((c) =>
          this.gameState.pendingFounding?.availableCompanyIds.includes(c.id)
        );
        new FoundingDialog(this, {
          availableCompanies: available,
          onSelectCompany: (companyId) => {
            const action = {
              type: 'CHOOSE_FOUNDED_COMPANY' as const,
              companyId,
            };
            this.gameState = gameReducer(this.gameState, action);
            this.refreshUI();
          },
        });
      }
    }

    // Check if phase is choose merger survivor
    if (this.gameState.phase === 'choose_merger_survivor' && this.gameState.pendingMerger) {
      const activePlayer = this.gameState.players[this.gameState.activePlayerIndex];
      if (activePlayer && !activePlayer.isAi) {
        new MergerDialog(this, {
          tiedCompanyIds: this.gameState.pendingMerger.participatingCompanyIds,
          companies: this.gameState.companies,
          onSelectSurvivor: (survivorCompanyId) => {
            const action = {
              type: 'CHOOSE_MERGER_SURVIVOR' as const,
              survivorCompanyId,
            };
            this.gameState = gameReducer(this.gameState, action);
            this.refreshUI();
          },
        });
      }
    }

    // Check if phase is buy_shares to trigger exchange prompt
    if (this.gameState.phase === 'buy_shares') {
      const activePlayer = this.gameState.players[this.gameState.activePlayerIndex];
      if (activePlayer && !activePlayer.isAi) {
        if (!this.scene.isActive('StockScene')) {
          this.openStockMarket();
        }
      }
    }
  }

  private getCompanyColorHex(companyId: CompanyId): number {
    const compDef = this.gameState.config.companies.find((c) => c.id === companyId);
    const hexStr = compDef?.color || '#38bdf8';
    return parseInt(hexStr.replace('#', '0x'), 16);
  }

  private executeDrillAction() {
    if (!this.selectedTile) return;

    const action = {
      type: 'PLACE_TILE' as const,
      coord: this.selectedTile,
    };

    const nextState = gameReducer(this.gameState, action);
    this.gameState = nextState;
    this.selectedTile = null;
    this.refreshUI();
  }

  private executeDiscardAction() {
    if (!this.selectedTile) return;

    const action = {
      type: 'DISCARD_DEAD_TILE' as const,
      coord: this.selectedTile,
    };

    const nextState = gameReducer(this.gameState, action);
    this.gameState = nextState;
    this.selectedTile = null;
    this.refreshUI();
  }
}
