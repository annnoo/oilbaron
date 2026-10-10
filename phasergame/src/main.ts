import Phaser from 'phaser';

export class BootScene extends Phaser.Scene {
  constructor() {
    super('BootScene');
  }

  preload() {
    // Load classic assets if available
    this.load.image('classic_title', '/classic_assets/title.png');
  }

  create() {
    this.scene.start('TitleScene');
  }
}

export class TitleScene extends Phaser.Scene {
  constructor() {
    super('TitleScene');
  }

  create() {
    const { width, height } = this.scale;

    // Background
    this.add.rectangle(width / 2, height / 2, width, height, 0x070b14);

    // Title text
    this.add.text(width / 2, height / 3, 'OIL BARON', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '64px',
      fontStyle: 'bold',
      color: '#38bdf8',
    }).setOrigin(0.5);

    // Subtitle
    this.add.text(width / 2, height / 3 + 55, 'PETROLEUM EMPIRE SIMULATION', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '20px',
      color: '#94a3b8',
      letterSpacing: 4,
    }).setOrigin(0.5);
  }
}

const config: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  parent: 'game-container',
  width: 1280,
  height: 720,
  backgroundColor: '#030712',
  scale: {
    mode: Phaser.Scale.FIT,
    autoCenter: Phaser.Scale.CENTER_BOTH,
  },
  scene: [BootScene, TitleScene],
};

export const game = new Phaser.Game(config);
