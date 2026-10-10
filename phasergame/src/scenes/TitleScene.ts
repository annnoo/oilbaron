import Phaser from 'phaser';
import { Button } from '../ui/Button';

export class TitleScene extends Phaser.Scene {
  constructor() {
    super('TitleScene');
  }

  create() {
    const { width, height } = this.scale;

    // Dark seismic grid backdrop
    this.add.rectangle(width / 2, height / 2, width, height, 0x050814);

    // Decorative grid lines
    const graphics = this.add.graphics();
    graphics.lineStyle(1, 0x0e243a, 0.4);
    for (let x = 0; x < width; x += 40) {
      graphics.moveTo(x, 0);
      graphics.lineTo(x, height);
    }
    for (let y = 0; y < height; y += 40) {
      graphics.moveTo(0, y);
      graphics.lineTo(width, y);
    }
    graphics.strokePath();

    // Classic title logo or banner
    if (this.textures.exists('classic_title')) {
      const banner = this.add.image(width / 2, 140, 'classic_title');
      banner.setScale(Math.min(1.2, (width * 0.7) / banner.width));
    }

    // Title Title Heading
    this.add.text(width / 2, 240, 'OIL BARON', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '56px',
      fontStyle: 'bold',
      color: '#38bdf8',
      stroke: '#0369a1',
      strokeThickness: 3,
    }).setOrigin(0.5);

    // Subtitle
    this.add.text(width / 2, 290, 'TACTICAL PETROLEUM EXPLORATION SIMULATION', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '15px',
      color: '#94a3b8',
      letterSpacing: 4,
    }).setOrigin(0.5);

    // Buttons
    const startY = 370;
    const spacing = 65;

    new Button(this, {
      x: width / 2,
      y: startY,
      width: 280,
      height: 52,
      text: '▶  NEW GAME',
      backgroundColor: 0x166534,
      hoverColor: 0x15803d,
      strokeColor: 0x4ade80,
      onClick: () => {
        this.scene.start('GameScene');
      },
    });

    new Button(this, {
      x: width / 2,
      y: startY + spacing,
      width: 280,
      height: 52,
      text: '💾  LOAD GAME',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x64748b,
      onClick: () => {
        this.scene.launch('SaveLoadScene', { mode: 'load', returnScene: 'TitleScene' });
      },
    });

    new Button(this, {
      x: width / 2,
      y: startY + spacing * 2,
      width: 280,
      height: 52,
      text: '📜  HOW TO PLAY',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x64748b,
      onClick: () => {
        this.showHowToPlayModal();
      },
    });

    // Version footer
    this.add.text(width / 2, height - 30, 'OIL BARON CLASSICAL RE-CREATION · PHASER 3 ENGINE · ESC FOR PAUSE MENU', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '12px',
      color: '#475569',
    }).setOrigin(0.5);
  }

  private showHowToPlayModal() {
    const { width, height } = this.scale;
    const modal = this.add.container(width / 2, height / 2);

    const overlay = this.add.rectangle(0, 0, width, height, 0x000000, 0.7)
      .setInteractive();

    const panel = this.add.rectangle(0, 0, 680, 420, 0x0a1220)
      .setStrokeStyle(2, 0x38bdf8);

    const title = this.add.text(0, -170, 'FIELD OPERATIONS MANUAL', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '22px',
      fontStyle: 'bold',
      color: '#38bdf8',
    }).setOrigin(0.5);

    const guideText = [
      '• SURVEY & DRILL: Select a coordinate lease from your concessions rack and drill a well.',
      '• FORM CONGLOMERATES: Placing adjacent solitary wells allows founding a new petroleum giant.',
      '• EXPAND & MERGE: Connecting company oil fields triggers corporate mergers.',
      '  The smaller company is acquired; shareholders receive generous cash bonuses.',
      '• EQUITY MARKETS: Acquire up to 3 shares per turn, or liquidate holdings for emergency cash.',
      '• VICTORY: Build maximum net worth through petroleum empire domination!',
      '',
      'KEYBOARD SHORTCUTS:',
      '• [ESC] opens the classical Pause Menu anytime during play.',
      '• [SPACE] drills selected well or passes turn.',
    ].join('\n');

    const body = this.add.text(-300, -120, guideText, {
      fontFamily: 'Inter, sans-serif',
      fontSize: '14px',
      color: '#cbd5e1',
      lineSpacing: 7,
      wordWrap: { width: 600 },
    });

    modal.add([overlay, panel, title, body]);

    const closeBtn = new Button(this, {
      x: 0,
      y: 160,
      width: 180,
      height: 40,
      text: 'CLOSE',
      backgroundColor: 0x0284c7,
      hoverColor: 0x0369a1,
      strokeColor: 0x38bdf8,
      onClick: () => {
        modal.destroy();
      },
    });
    modal.add(closeBtn);
  }
}
