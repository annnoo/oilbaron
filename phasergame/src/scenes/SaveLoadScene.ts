import Phaser from 'phaser';
import { Button } from '../ui/Button';
import {
  listLocalStorageSaveSlots,
  saveToLocalStorage,
  loadFromLocalStorage,
  type SaveSlotMetadata,
} from '../engine/persistence';
import type { GameScene } from './GameScene';

export interface SaveLoadData {
  mode: 'save' | 'load';
  returnScene: string;
}

export class SaveLoadScene extends Phaser.Scene {
  private mode: 'save' | 'load' = 'save';
  private returnScene: string = 'PauseScene';

  constructor() {
    super('SaveLoadScene');
  }

  init(data: SaveLoadData) {
    this.mode = data.mode || 'save';
    this.returnScene = data.returnScene || 'PauseScene';
  }

  create() {
    const { width, height } = this.scale;

    // Dark backdrop overlay
    this.add.rectangle(width / 2, height / 2, width, height, 0x020617, 0.85);

    // Frame
    this.add.rectangle(width / 2, height / 2, 540, 500, 0x0a101d)
      .setStrokeStyle(2, 0x38bdf8);

    // Title
    const titleText = this.mode === 'save' ? 'SAVE GAME MISSION' : 'LOAD SAVED MISSION';
    this.add.text(width / 2, height / 2 - 200, titleText, {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '28px',
      fontStyle: 'bold',
      color: '#38bdf8',
    }).setOrigin(0.5);

    // Render Slots (1 to 3)
    const slots = listLocalStorageSaveSlots();
    const slotMap = new Map<string, SaveSlotMetadata>(slots.map((s) => [s.slot, s]));

    const slotNames = ['slot-1', 'slot-2', 'slot-3'];
    const startY = height / 2 - 120;
    const spacing = 90;

    slotNames.forEach((slotId, index) => {
      const y = startY + index * spacing;
      const meta = slotMap.get(slotId);

      // Slot container card
      this.add.rectangle(width / 2, y, 480, 75, 0x1e293b, 0.6)
        .setStrokeStyle(1, 0x475569);

      const title = meta
        ? `SLOT ${index + 1}: (${meta.players.length} Barons: ${meta.players.join(', ')})`
        : `SLOT ${index + 1}: [EMPTY ARCHIVE]`;

      const subtitle = meta
        ? `${new Date(meta.date).toLocaleDateString()} ${new Date(meta.date).toLocaleTimeString()}`
        : 'Available for deployment';

      this.add.text(width / 2 - 220, y - 18, title, {
        fontFamily: 'Chakra Petch, sans-serif',
        fontSize: '16px',
        fontStyle: 'bold',
        color: meta ? '#f8fafc' : '#64748b',
      });

      this.add.text(width / 2 - 220, y + 8, subtitle, {
        fontFamily: 'Inter, sans-serif',
        fontSize: '12px',
        color: '#94a3b8',
      });

      // Action button
      if (this.mode === 'save') {
        new Button(this, {
          x: width / 2 + 160,
          y,
          width: 100,
          height: 38,
          text: 'SAVE',
          backgroundColor: 0x0284c7,
          hoverColor: 0x0369a1,
          strokeColor: 0x38bdf8,
          onClick: () => this.handleSave(slotId),
        });
      } else {
        if (meta) {
          new Button(this, {
            x: width / 2 + 160,
            y,
            width: 100,
            height: 38,
            text: 'LOAD',
            backgroundColor: 0x166534,
            hoverColor: 0x15803d,
            strokeColor: 0x4ade80,
            onClick: () => this.handleLoad(slotId),
          });
        }
      }
    });

    // Back button
    new Button(this, {
      x: width / 2,
      y: height / 2 + 200,
      width: 160,
      height: 40,
      text: 'BACK',
      backgroundColor: 0x334155,
      hoverColor: 0x475569,
      strokeColor: 0x64748b,
      onClick: () => {
        this.scene.stop('SaveLoadScene');
      },
    });

    // Escape listener
    if (this.input.keyboard) {
      const esc = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.ESC);
      esc.on('down', () => this.scene.stop('SaveLoadScene'));
    }
  }

  private handleSave(slotId: string) {
    const gameScene = this.scene.get('GameScene') as GameScene;
    if (gameScene && gameScene.gameState) {
      saveToLocalStorage(gameScene.gameState, slotId);
      this.scene.restart({ mode: 'save', returnScene: this.returnScene });
    }
  }

  private handleLoad(slotId: string) {
    const loadedState = loadFromLocalStorage(slotId);
    if (loadedState) {
      this.scene.stop('SaveLoadScene');
      this.scene.stop('PauseScene');
      this.scene.stop('TitleScene');
      this.scene.start('GameScene', { loadedState });
    }
  }
}
