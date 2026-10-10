import Phaser from 'phaser';
import type { GameState, TileCoord } from '../engine/types';
import { classifyMove, formatCoord } from '../engine/board';
import { Button } from './Button';

export interface ConcessionsTrayProps {
  onSelectTile: (coord: TileCoord) => void;
  onDrillWell: () => void;
  onDiscardDead: () => void;
}

export class ConcessionsTray extends Phaser.GameObjects.Container {
  private bgRect: Phaser.GameObjects.Rectangle;
  private leaseButtons: Phaser.GameObjects.Container[] = [];
  private drillActuatorBtn: Button;
  private selectedCoord: TileCoord | null = null;
  private props: ConcessionsTrayProps;

  constructor(scene: Phaser.Scene, width: number, y: number, props: ConcessionsTrayProps) {
    super(scene, 0, y);
    this.props = props;

    const height = 90;

    // Dark tactile rack background
    this.bgRect = scene.add.rectangle(width / 2, height / 2, width, height, 0x050814, 0.95)
      .setStrokeStyle(1, 0x1e293b);

    this.add(this.bgRect);

    // Prominent Emerald Drill Actuator Button
    this.drillActuatorBtn = new Button(scene, {
      x: width - 150,
      y: height / 2,
      width: 240,
      height: 60,
      text: 'SELECT PARCEL',
      fontSize: '16px',
      backgroundColor: 0x1e293b,
      hoverColor: 0x334155,
      strokeColor: 0x64748b,
      onClick: () => {
        if (this.selectedCoord) {
          this.props.onDrillWell();
        }
      },
    });

    this.add(this.drillActuatorBtn);
    scene.add.existing(this);
  }

  public updateState(state: GameState, selectedTile: TileCoord | null) {
    this.selectedCoord = selectedTile;
    const activePlayer = state.players[state.activePlayerIndex];
    if (!activePlayer) return;

    // Clear old lease slips
    this.leaseButtons.forEach((btn) => btn.destroy());
    this.leaseButtons = [];

    const isPlacePhase = state.phase === 'place_tile';
    const leases = activePlayer.hand;
    const startX = 80;
    const slotSpacing = 95;

    leases.forEach((coord: TileCoord, idx: number) => {
      const x = startX + idx * slotSpacing;
      const y = 45;
      const isSelected = selectedTile && selectedTile.row === coord.row && selectedTile.col === coord.col;

      const card = this.scene.add.container(x, y);
      const bgCol = isSelected ? 0x0f2744 : 0x1e293b;
      const strokeCol = isSelected ? 0x38bdf8 : 0x475569;
      const strokeW = isSelected ? 2 : 1;

      const cardBg = this.scene.add.rectangle(0, 0, 80, 68, bgCol)
        .setStrokeStyle(strokeW, strokeCol)
        .setInteractive({ useHandCursor: isPlacePhase });

      const coordLabel = formatCoord(coord);
      const coordText = this.scene.add.text(0, -12, coordLabel, {
        fontFamily: 'Chakra Petch, sans-serif',
        fontSize: '18px',
        fontStyle: 'bold',
        color: isSelected ? '#38bdf8' : '#f8fafc',
      }).setOrigin(0.5);

      // Classify tactical tag
      const classification = classifyMove(state.board, coord, state.companies, state.config.safeThreshold);
      let tagText = 'OPEN';
      let tagCol = '#94a3b8';

      if (classification.type === 'expansion') {
        tagText = 'EXPAND';
        tagCol = '#38bdf8';
      } else if (classification.type === 'founding') {
        tagText = 'FOUND';
        tagCol = '#fbbf24';
      } else if (classification.type === 'merger') {
        tagText = 'MERGER';
        tagCol = '#f87171';
      } else if (classification.type === 'illegal_safe_merger') {
        tagText = 'DEAD';
        tagCol = '#ef4444';
      }

      const tagLabel = this.scene.add.text(0, 15, tagText, {
        fontFamily: 'Chakra Petch, sans-serif',
        fontSize: '11px',
        fontStyle: 'bold',
        color: tagCol,
      }).setOrigin(0.5);

      card.add([cardBg, coordText, tagLabel]);

      if (isPlacePhase) {
        cardBg.on('pointerdown', () => {
          this.props.onSelectTile(coord);
        });
      }

      this.add(card);
      this.leaseButtons.push(card);
    });

    // Update actuator text & state
    if (isPlacePhase) {
      if (selectedTile) {
        const selLabel = formatCoord(selectedTile);
        this.drillActuatorBtn.setText(`DRILL WELL [${selLabel}]`);
        // If dead tile, allow discard
        const cl = classifyMove(state.board, selectedTile, state.companies, state.config.safeThreshold);
        if (cl.type === 'illegal_safe_merger') {
          this.drillActuatorBtn.setText(`DISCARD [${selLabel}]`);
        }
      } else {
        this.drillActuatorBtn.setText('SELECT A LEASE');
      }
    } else if (state.phase === 'buy_shares') {
      this.drillActuatorBtn.setText('OPEN EXCHANGE');
    } else {
      this.drillActuatorBtn.setText('STANDBY');
    }
  }
}
