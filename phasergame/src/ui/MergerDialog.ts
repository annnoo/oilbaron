import Phaser from 'phaser';
import type { CompanyId, CompanyState } from '../engine/types';
import { Button } from './Button';

export interface MergerDialogOptions {
  tiedCompanyIds: CompanyId[];
  companies: Record<CompanyId, CompanyState>;
  onSelectSurvivor: (companyId: CompanyId) => void;
}

export class MergerDialog extends Phaser.GameObjects.Container {
  constructor(scene: Phaser.Scene, options: MergerDialogOptions) {
    const { width, height } = scene.scale;
    super(scene, width / 2, height / 2);

    const overlay = scene.add.rectangle(0, 0, width, height, 0x000000, 0.7)
      .setInteractive();

    const frame = scene.add.rectangle(0, 0, 560, 380, 0x09111e)
      .setStrokeStyle(2, 0xf87171);

    const title = scene.add.text(0, -150, 'CORPORATE TAKEOVER CONFLICT', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '22px',
      fontStyle: 'bold',
      color: '#f87171',
    }).setOrigin(0.5);

    const subtitle = scene.add.text(0, -115, 'Multiple corporations have tied well counts. Select the surviving entity:', {
      fontFamily: 'Inter, sans-serif',
      fontSize: '13px',
      color: '#cbd5e1',
    }).setOrigin(0.5);

    this.add([overlay, frame, title, subtitle]);

    const startY = -40;
    const spacing = 58;

    options.tiedCompanyIds.forEach((compId, idx) => {
      const comp = options.companies[compId];
      const name = comp?.name ?? compId;
      const y = startY + idx * spacing;

      const btn = new Button(scene, {
        x: 0,
        y,
        width: 400,
        height: 46,
        text: `SURVIVOR: ${name.toUpperCase()} (${comp?.size ?? 0} wells)`,
        fontSize: '15px',
        backgroundColor: 0x1e293b,
        hoverColor: 0x991b1b,
        strokeColor: 0xf87171,
        onClick: () => {
          this.destroy();
          options.onSelectSurvivor(compId);
        },
      });
      this.add(btn);
    });

    scene.add.existing(this);
  }
}
