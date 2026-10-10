import Phaser from 'phaser';
import type { GameScene } from './GameScene';
import type { CompanyId, SharePurchase } from '../engine/types';
import { calculateSharePrice } from '../engine/market';
import { gameReducer } from '../engine/reducer';
import { Button } from '../ui/Button';

export class StockScene extends Phaser.Scene {
  private mode: 'buy' | 'sell' = 'buy';
  private selectedPurchases: Map<CompanyId, number> = new Map();
  private containerGroup!: Phaser.GameObjects.Container;

  constructor() {
    super('StockScene');
  }

  create() {
    this.selectedPurchases.clear();
    this.renderMarket();

    if (this.input.keyboard) {
      const esc = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.ESC);
      esc.on('down', () => this.closeMarket());
    }
  }

  private renderMarket() {
    if (this.containerGroup) {
      this.containerGroup.destroy();
    }

    const { width, height } = this.scale;
    this.containerGroup = this.add.container(0, 0);

    // Dimmed backdrop
    const overlay = this.add.rectangle(width / 2, height / 2, width, height, 0x030712, 0.85);
    this.containerGroup.add(overlay);

    // Frame
    const frame = this.add.rectangle(width / 2, height / 2, 820, 560, 0x09111e)
      .setStrokeStyle(2, 0x38bdf8);
    this.containerGroup.add(frame);

    const gameScene = this.scene.get('GameScene') as GameScene;
    const state = gameScene?.gameState;
    if (!state) return;

    const activePlayer = state.players[state.activePlayerIndex];
    if (!activePlayer) return;

    // Header Title
    const title = this.add.text(width / 2, height / 2 - 240, 'PETROLEUM EQUITY EXCHANGE', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '26px',
      fontStyle: 'bold',
      color: '#38bdf8',
    }).setOrigin(0.5);

    const fundsBadge = this.add.text(width / 2, height / 2 - 205, `AVAILABLE RESERVES: $${activePlayer.cash.toLocaleString()}`, {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '14px',
      color: '#4ade80',
    }).setOrigin(0.5);

    this.containerGroup.add([title, fundsBadge]);

    // Mode Switcher Tabs
    const buyTabBtn = new Button(this, {
      x: width / 2 - 130,
      y: height / 2 - 165,
      width: 220,
      height: 38,
      text: 'ACQUIRE SHARES (BUY)',
      backgroundColor: this.mode === 'buy' ? 0x0284c7 : 0x1e293b,
      hoverColor: 0x0369a1,
      strokeColor: this.mode === 'buy' ? 0x38bdf8 : 0x475569,
      onClick: () => {
        this.mode = 'buy';
        this.renderMarket();
      },
    });

    const sellTabBtn = new Button(this, {
      x: width / 2 + 130,
      y: height / 2 - 165,
      width: 220,
      height: 38,
      text: 'LIQUIDATE SHARES (SELL)',
      backgroundColor: this.mode === 'sell' ? 0x0284c7 : 0x1e293b,
      hoverColor: 0x0369a1,
      strokeColor: this.mode === 'sell' ? 0x38bdf8 : 0x475569,
      onClick: () => {
        this.mode = 'sell';
        this.renderMarket();
      },
    });

    this.containerGroup.add([buyTabBtn, sellTabBtn]);

    if (this.mode === 'buy') {
      this.renderBuyMode(width, height, state, activePlayer);
    } else {
      this.renderSellMode(width, height, state, activePlayer);
    }
  }

  private renderBuyMode(width: number, height: number, state: any, activePlayer: any) {
    // List active companies that have available shares
    const activeCompanies = state.config.companies.filter((def: any) => {
      const comp = state.companies[def.id];
      return comp && comp.isActive && comp.size >= 2;
    });

    const startY = height / 2 - 110;
    const rowHeight = 44;

    if (activeCompanies.length === 0) {
      const emptyText = this.add.text(width / 2, height / 2, 'No active oil conglomerates founded on the map yet.', {
        fontFamily: 'Inter, sans-serif',
        fontSize: '15px',
        color: '#94a3b8',
      }).setOrigin(0.5);
      this.containerGroup.add(emptyText);
    } else {
      activeCompanies.forEach((def: any, idx: number) => {
        const y = startY + idx * rowHeight;
        const comp = state.companies[def.id];
        const price = calculateSharePrice(def, comp.size);
        const count = this.selectedPurchases.get(def.id) ?? 0;

        // Row background
        const rowBg = this.add.rectangle(width / 2, y, 740, 38, 0x131f33)
          .setStrokeStyle(1, 0x1e293b);

        const compName = this.add.text(width / 2 - 340, y, `${def.name} (${comp.size} wells)`, {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '14px',
          fontStyle: 'bold',
          color: '#f8fafc',
        }).setOrigin(0, 0.5);

        const priceText = this.add.text(width / 2 - 40, y, `$${price}/sh · Avail: ${comp.availableShares}`, {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '13px',
          color: '#94a3b8',
        }).setOrigin(0, 0.5);

        const countText = this.add.text(width / 2 + 180, y, `Qty: ${count}`, {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '14px',
          fontStyle: 'bold',
          color: count > 0 ? '#38bdf8' : '#64748b',
        }).setOrigin(0, 0.5);

        // Plus / Minus buttons
        const plusBtn = new Button(this, {
          x: width / 2 + 270,
          y,
          width: 32,
          height: 28,
          text: '+',
          fontSize: '16px',
          backgroundColor: 0x166534,
          hoverColor: 0x15803d,
          strokeColor: 0x4ade80,
          onClick: () => {
            const totalSelected = Array.from(this.selectedPurchases.values()).reduce((a, b) => a + b, 0);
            if (totalSelected < 3 && count < comp.availableShares) {
              this.selectedPurchases.set(def.id, count + 1);
              this.renderMarket();
            }
          },
        });

        const minusBtn = new Button(this, {
          x: width / 2 + 310,
          y,
          width: 32,
          height: 28,
          text: '-',
          fontSize: '16px',
          backgroundColor: 0x334155,
          hoverColor: 0x475569,
          strokeColor: 0x64748b,
          onClick: () => {
            if (count > 0) {
              this.selectedPurchases.set(def.id, count - 1);
              this.renderMarket();
            }
          },
        });

        this.containerGroup.add([rowBg, compName, priceText, countText, plusBtn, minusBtn]);
      });
    }

    // Bottom action actuators
    const totalCount = Array.from(this.selectedPurchases.values()).reduce((a, b) => a + b, 0);
    const confirmBtn = new Button(this, {
      x: width / 2 - 120,
      y: height / 2 + 220,
      width: 250,
      height: 48,
      text: totalCount > 0 ? `CONFIRM BUY (${totalCount})` : 'PASS BUYING',
      backgroundColor: totalCount > 0 ? 0x166534 : 0x1e293b,
      hoverColor: totalCount > 0 ? 0x15803d : 0x334155,
      strokeColor: totalCount > 0 ? 0x4ade80 : 0x64748b,
      onClick: () => this.confirmPurchases(),
    });

    const closeBtn = new Button(this, {
      x: width / 2 + 150,
      y: height / 2 + 220,
      width: 180,
      height: 48,
      text: 'BACK TO MAP',
      backgroundColor: 0x334155,
      hoverColor: 0x475569,
      strokeColor: 0x64748b,
      onClick: () => this.closeMarket(),
    });

    this.containerGroup.add([confirmBtn, closeBtn]);
  }

  private renderSellMode(width: number, height: number, state: any, activePlayer: any) {
    const ownedCompanies = state.config.companies.filter((def: any) => (activePlayer.shares[def.id] ?? 0) > 0);

    const startY = height / 2 - 110;
    const rowHeight = 44;

    if (ownedCompanies.length === 0) {
      const emptyText = this.add.text(width / 2, height / 2, 'You currently hold no corporate equity to liquidate.', {
        fontFamily: 'Inter, sans-serif',
        fontSize: '15px',
        color: '#94a3b8',
      }).setOrigin(0.5);
      this.containerGroup.add(emptyText);
    } else {
      ownedCompanies.forEach((def: any, idx: number) => {
        const y = startY + idx * rowHeight;
        const comp = state.companies[def.id];
        const owned = activePlayer.shares[def.id] ?? 0;
        const price = comp.isActive && comp.size >= 2 ? calculateSharePrice(def, comp.size) : def.basePrice;

        const rowBg = this.add.rectangle(width / 2, y, 740, 38, 0x131f33)
          .setStrokeStyle(1, 0x1e293b);

        const compName = this.add.text(width / 2 - 340, y, `${def.name} · Owned: ${owned} shares`, {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '14px',
          fontStyle: 'bold',
          color: '#f8fafc',
        }).setOrigin(0, 0.5);

        const valText = this.add.text(width / 2 - 40, y, `$${price}/sh · Value: $${(owned * price).toLocaleString()}`, {
          fontFamily: 'Chakra Petch, sans-serif',
          fontSize: '13px',
          color: '#4ade80',
        }).setOrigin(0, 0.5);

        // Sell 1
        const sellOneBtn = new Button(this, {
          x: width / 2 + 230,
          y,
          width: 80,
          height: 28,
          text: 'SELL 1',
          fontSize: '13px',
          backgroundColor: 0x7f1d1d,
          hoverColor: 0x991b1b,
          strokeColor: 0xf87171,
          onClick: () => this.sellShares(def.id, 1),
        });

        // Sell All
        const sellAllBtn = new Button(this, {
          x: width / 2 + 320,
          y,
          width: 80,
          height: 28,
          text: 'SELL ALL',
          fontSize: '13px',
          backgroundColor: 0x991b1b,
          hoverColor: 0xb91c1c,
          strokeColor: 0xfca5a5,
          onClick: () => this.sellShares(def.id, owned),
        });

        this.containerGroup.add([rowBg, compName, valText, sellOneBtn, sellAllBtn]);
      });
    }

    // Bottom action actuators
    const closeBtn = new Button(this, {
      x: width / 2,
      y: height / 2 + 220,
      width: 220,
      height: 48,
      text: 'FINISH TURN',
      backgroundColor: 0x166534,
      hoverColor: 0x15803d,
      strokeColor: 0x4ade80,
      onClick: () => this.finishTurn(),
    });

    this.containerGroup.add(closeBtn);
  }

  private sellShares(companyId: CompanyId, count: number) {
    const gameScene = this.scene.get('GameScene') as GameScene;
    if (!gameScene) return;

    const action = {
      type: 'SELL_SHARES' as const,
      sales: [{ companyId, count }],
    };
    gameScene.gameState = gameReducer(gameScene.gameState, action);
    this.renderMarket();
  }

  private confirmPurchases() {
    const gameScene = this.scene.get('GameScene') as GameScene;
    if (!gameScene) return;

    const purchases: SharePurchase[] = [];
    this.selectedPurchases.forEach((count, companyId) => {
      if (count > 0) purchases.push({ companyId, count });
    });

    if (purchases.length > 0) {
      const action = {
        type: 'BUY_SHARES' as const,
        purchases,
      };
      gameScene.gameState = gameReducer(gameScene.gameState, action);
    } else {
      const action = { type: 'END_TURN' as const };
      gameScene.gameState = gameReducer(gameScene.gameState, action);
    }

    this.closeMarket();
  }

  private finishTurn() {
    const gameScene = this.scene.get('GameScene') as GameScene;
    if (gameScene) {
      const action = { type: 'END_TURN' as const };
      gameScene.gameState = gameReducer(gameScene.gameState, action);
    }
    this.closeMarket();
  }

  private closeMarket() {
    this.scene.stop('StockScene');
    this.scene.resume('GameScene');
  }
}
