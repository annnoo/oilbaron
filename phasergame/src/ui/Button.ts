import Phaser from 'phaser';

export interface ButtonOptions {
  x: number;
  y: number;
  width?: number;
  height?: number;
  text: string;
  fontSize?: string;
  textColor?: string;
  backgroundColor?: number;
  hoverColor?: number;
  strokeColor?: number;
  strokeWidth?: number;
  onClick: () => void;
}

export class Button extends Phaser.GameObjects.Container {
  private background: Phaser.GameObjects.Rectangle;
  private label: Phaser.GameObjects.Text;
  private isHovered = false;

  constructor(scene: Phaser.Scene, options: ButtonOptions) {
    super(scene, options.x, options.y);

    const width = options.width ?? 240;
    const height = options.height ?? 50;
    const bgCol = options.backgroundColor ?? 0x1e293b;
    const hovCol = options.hoverColor ?? 0x0284c7;
    const strokeCol = options.strokeColor ?? 0x38bdf8;
    const strokeW = options.strokeWidth ?? 2;

    this.background = scene.add.rectangle(0, 0, width, height, bgCol)
      .setStrokeStyle(strokeW, strokeCol)
      .setOrigin(0.5);

    this.label = scene.add.text(0, 0, options.text, {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: options.fontSize ?? '18px',
      fontStyle: 'bold',
      color: options.textColor ?? '#f8fafc',
    }).setOrigin(0.5);

    this.add([this.background, this.label]);
    this.setSize(width, height);
    this.setInteractive({ useHandCursor: true });

    this.on('pointerover', () => {
      this.isHovered = true;
      this.background.setFillStyle(hovCol);
      this.setScale(1.03);
    });

    this.on('pointerout', () => {
      this.isHovered = false;
      this.background.setFillStyle(bgCol);
      this.setScale(1.0);
    });

    this.on('pointerdown', () => {
      this.setScale(0.97);
    });

    this.on('pointerup', () => {
      this.setScale(this.isHovered ? 1.03 : 1.0);
      options.onClick();
    });

    scene.add.existing(this);
  }

  public setText(text: string) {
    this.label.setText(text);
  }
}
