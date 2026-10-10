import Phaser from 'phaser';
import { Button } from '../ui/Button';

export class PauseScene extends Phaser.Scene {
  private escKey!: Phaser.Input.Keyboard.Key;

  constructor() {
    super('PauseScene');
  }

  create() {
    const { width, height } = this.scale;

    // Dim the screen behind pause menu
    this.add.rectangle(width / 2, height / 2, width, height, 0x030712, 0.75);

    // Modal frame
    const frame = this.add.rectangle(width / 2, height / 2, 420, 480, 0x0a101d)
      .setStrokeStyle(2, 0x38bdf8);

    // Decorative corner brackets
    const g = this.add.graphics();
    g.lineStyle(2, 0x0ea5e9, 1);
    const left = width / 2 - 210;
    const right = width / 2 + 210;
    const top = height / 2 - 240;
    const bottom = height / 2 + 240;

    // Top-left
    g.moveTo(left, top + 15);
    g.lineTo(left, top);
    g.lineTo(left + 15, top);

    // Top-right
    g.moveTo(right - 15, top);
    g.lineTo(right, top);
    g.lineTo(right, top + 15);

    // Bottom-left
    g.moveTo(left, bottom - 15);
    g.lineTo(left, bottom);
    g.lineTo(left + 15, bottom);

    // Bottom-right
    g.moveTo(right - 15, bottom);
    g.lineTo(right, bottom);
    g.lineTo(right, bottom - 15);
    g.strokePath();

    // Pause Header
    this.add.text(width / 2, height / 2 - 180, 'GAME PAUSED', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '32px',
      fontStyle: 'bold',
      color: '#38bdf8',
      stroke: '#0369a1',
      strokeThickness: 2,
    }).setOrigin(0.5);

    this.add.text(width / 2, height / 2 - 145, 'SIMULATION SUSPENDED · ESC TO RESUME', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '12px',
      color: '#64748b',
    }).setOrigin(0.5);

    // Buttons
    const startY = height / 2 - 90;
    const spacing = 58;

    new Button(this, {
      x: width / 2,
      y: startY,
      width: 260,
      height: 46,
      text: '▶  RESUME GAME',
      backgroundColor: 0x166534,
      hoverColor: 0x15803d,
      strokeColor: 0x4ade80,
      onClick: () => this.resumeGame(),
    });

    new Button(this, {
      x: width / 2,
      y: startY + spacing,
      width: 260,
      height: 46,
      text: '💾  SAVE MISSION',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x64748b,
      onClick: () => {
        this.scene.launch('SaveLoadScene', { mode: 'save', returnScene: 'PauseScene' });
      },
    });

    new Button(this, {
      x: width / 2,
      y: startY + spacing * 2,
      width: 260,
      height: 46,
      text: '📂  LOAD MISSION',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x64748b,
      onClick: () => {
        this.scene.launch('SaveLoadScene', { mode: 'load', returnScene: 'PauseScene' });
      },
    });

    new Button(this, {
      x: width / 2,
      y: startY + spacing * 3,
      width: 260,
      height: 46,
      text: '🔄  RESTART MATCH',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x64748b,
      onClick: () => {
        this.scene.stop('PauseScene');
        this.scene.stop('GameScene');
        this.scene.start('GameScene');
      },
    });

    new Button(this, {
      x: width / 2,
      y: startY + spacing * 4,
      width: 260,
      height: 46,
      text: '🚪  QUIT TO TITLE',
      backgroundColor: 0x7f1d1d,
      hoverColor: 0x991b1b,
      strokeColor: 0xf87171,
      onClick: () => {
        this.scene.stop('PauseScene');
        this.scene.stop('GameScene');
        this.scene.start('TitleScene');
      },
    });

    // Keyboard ESC listener
    if (this.input.keyboard) {
      this.escKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.ESC);
      this.escKey.on('down', () => this.resumeGame());
    }
  }

  private resumeGame() {
    this.scene.stop('PauseScene');
    this.scene.resume('GameScene');
  }
}
