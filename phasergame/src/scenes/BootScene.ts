import Phaser from 'phaser';

export class BootScene extends Phaser.Scene {
  constructor() {
    super('BootScene');
  }

  preload() {
    // Background and retro graphic assets
    this.load.image('classic_title', '/classic_assets/title.png');
    this.load.image('classic_player_info', '/classic_assets/player_info.png');
    this.load.image('chit', '/classic_assets/chit.png');
    this.load.image('american', '/classic_assets/american.png');
    this.load.image('imperial', '/classic_assets/imperial.png');
    this.load.image('royal', '/classic_assets/royal.png');
    this.load.image('supra', '/classic_assets/supra.png');
    this.load.image('weemoco', '/classic_assets/weemoco.png');
  }

  create() {
    this.scene.start('TitleScene');
  }
}
