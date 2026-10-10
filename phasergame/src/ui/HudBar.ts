import Phaser from 'phaser';
import type { GameState } from '../engine/types';
import { calculateNetWorth } from '../engine/market';
import { Button } from './Button';

export interface HudBarProps {
  onOpenMenu: () => void;
  onOpenStockMarket?: () => void;
}

export class HudBar extends Phaser.GameObjects.Container {
  private bgRect: Phaser.GameObjects.Rectangle;
  private playerNameText: Phaser.GameObjects.Text;
  private fundsText: Phaser.GameObjects.Text;
  private netWorthText: Phaser.GameObjects.Text;
  private directivePill: Phaser.GameObjects.Rectangle;
  private directivePhaseText: Phaser.GameObjects.Text;
  private directiveDescText: Phaser.GameObjects.Text;
  private menuBtn: Button;
  private marketBtn?: Button;

  constructor(scene: Phaser.Scene, width: number, props: HudBarProps) {
    super(scene, 0, 0);

    const height = 64;

    // Dark sleek HUD background
    this.bgRect = scene.add.rectangle(width / 2, height / 2, width, height, 0x070d19, 0.95)
      .setStrokeStyle(1, 0x1e293b);

    // Concessionaire profile badge (left)
    const badgeX = 30;
    const badgeY = height / 2;
    const badgeBg = scene.add.rectangle(badgeX + 16, badgeY, 32, 32, 0x0284c7)
      .setStrokeStyle(1, 0x38bdf8);
    const badgeGlyph = scene.add.text(badgeX + 16, badgeY, '▲', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '18px',
      fontStyle: 'bold',
      color: '#ffffff',
    }).setOrigin(0.5);

    this.playerNameText = scene.add.text(badgeX + 44, badgeY - 10, 'BARON 1', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '16px',
      fontStyle: 'bold',
      color: '#f8fafc',
    });

    this.fundsText = scene.add.text(badgeX + 44, badgeY + 8, '$6,000 CASH', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '13px',
      color: '#4ade80',
    });

    this.netWorthText = scene.add.text(badgeX + 160, badgeY + 8, '· $6,000 NET WORTH', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '13px',
      color: '#94a3b8',
    });

    // Center turn directive banner
    const centerX = width / 2;
    this.directivePill = scene.add.rectangle(centerX, badgeY, 440, 36, 0x0f172a, 0.8)
      .setStrokeStyle(1, 0x22c55e);

    this.directivePhaseText = scene.add.text(centerX - 180, badgeY, 'DRILL PHASE', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '13px',
      fontStyle: 'bold',
      color: '#22c55e',
    }).setOrigin(0, 0.5);

    this.directiveDescText = scene.add.text(centerX - 60, badgeY, 'Select a lease & drill well parcel', {
      fontFamily: 'Inter, sans-serif',
      fontSize: '13px',
      color: '#cbd5e1',
    }).setOrigin(0, 0.5);

    // Right action buttons
    this.menuBtn = new Button(scene, {
      x: width - 80,
      y: badgeY,
      width: 110,
      height: 36,
      fontSize: '14px',
      text: '⚙ [ESC] MENU',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x475569,
      onClick: () => props.onOpenMenu(),
    });

    this.add([
      this.bgRect,
      badgeBg,
      badgeGlyph,
      this.playerNameText,
      this.fundsText,
      this.netWorthText,
      this.directivePill,
      this.directivePhaseText,
      this.directiveDescText,
      this.menuBtn,
    ]);

    if (props.onOpenStockMarket) {
      this.marketBtn = new Button(scene, {
        x: width - 210,
        y: badgeY,
        width: 120,
        height: 36,
        fontSize: '14px',
        text: '📈 EXCHANGE',
        backgroundColor: 0x0f2744,
        hoverColor: 0x0284c7,
        strokeColor: 0x38bdf8,
        onClick: () => props.onOpenStockMarket?.(),
      });
      this.add(this.marketBtn);
    }

    scene.add.existing(this);
  }

  public updateState(state: GameState) {
    const activePlayer = state.players[state.activePlayerIndex];
    if (!activePlayer) return;

    this.playerNameText.setText(activePlayer.name.toUpperCase());
    this.fundsText.setText(`$${activePlayer.cash.toLocaleString()} CASH`);

    const netWorth = calculateNetWorth(activePlayer, state.companies, state.config.companies);
    this.netWorthText.setText(`· $${netWorth.toLocaleString()} NET WORTH`);

    switch (state.phase) {
      case 'place_tile':
        this.directivePhaseText.setText('DRILL PHASE');
        this.directivePhaseText.setColor('#22c55e');
        this.directiveDescText.setText('Select parcel lease to drill well');
        break;
      case 'buy_shares':
        this.directivePhaseText.setText('EQUITY PHASE');
        this.directivePhaseText.setColor('#38bdf8');
        this.directiveDescText.setText('Acquire up to 3 shares or liquidate');
        break;
      case 'choose_founding_company':
        this.directivePhaseText.setText('FORMATION PHASE');
        this.directivePhaseText.setColor('#fbbf24');
        this.directiveDescText.setText('Select oil conglomerate to incorporate');
        break;
      case 'choose_merger_survivor':
      case 'dispose_merger_shares':
        this.directivePhaseText.setText('MERGER PHASE');
        this.directivePhaseText.setColor('#f87171');
        this.directiveDescText.setText('Resolving corporate takeover');
        break;
      case 'game_over':
        this.directivePhaseText.setText('MATCH CONCLUDED');
        this.directivePhaseText.setColor('#c084fc');
        this.directiveDescText.setText('Game completed - review empire scores');
        break;
    }
  }
}
