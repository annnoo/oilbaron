import Phaser from 'phaser';
import type { CompanyId, CompanyDefinition } from '../engine/types';
import { Button } from './Button';

export interface FoundingDialogOptions {
  availableCompanies: CompanyDefinition[];
  onSelectCompany: (companyId: CompanyId) => void;
  onCancel?: () => void;
}

export class FoundingDialog extends Phaser.GameObjects.Container {
  constructor(scene: Phaser.Scene, options: FoundingDialogOptions) {
    const { width, height } = scene.scale;
    super(scene, width / 2, height / 2);

    // Dimmed backdrop
    const overlay = scene.add.rectangle(0, 0, width, height, 0x000000, 0.7)
      .setInteractive();

    // Dialog frame
    const frame = scene.add.rectangle(0, 0, 560, 440, 0x09111e)
      .setStrokeStyle(2, 0xfbbf24);

    const title = scene.add.text(0, -180, 'FOUND NEW PETROLEUM CONGLOMERATE', {
      fontFamily: 'Chakra Petch, sans-serif',
      fontSize: '20px',
      fontStyle: 'bold',
      color: '#fbbf24',
    }).setOrigin(0.5);

    const subtitle = scene.add.text(0, -150, 'Select which corporation to incorporate for this new oil basin:', {
      fontFamily: 'Inter, sans-serif',
      fontSize: '13px',
      color: '#cbd5e1',
    }).setOrigin(0.5);

    this.add([overlay, frame, title, subtitle]);

    const startY = -90;
    const spacing = 52;

    options.availableCompanies.forEach((def, idx) => {
      const y = startY + idx * spacing;
      const btn = new Button(scene, {
        x: 0,
        y,
        width: 420,
        height: 42,
        text: `★ ${def.name} (${def.tier.toUpperCase()} TIER · $${def.basePrice})`,
        fontSize: '15px',
        backgroundColor: 0x1e293b,
        hoverColor: 0x0284c7,
        strokeColor: 0x38bdf8,
        onClick: () => {
          this.destroy();
          options.onSelectCompany(def.id);
        },
      });
      this.add(btn);
    });

    scene.add.existing(this);
  }
}
