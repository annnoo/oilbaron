/**
 * Procedural Retro Audio Synthesizer using Web Audio API
 * Generates chiptune sound effects without external audio files.
 */

export type SoundEffectType =
  | 'click'
  | 'foundCompany'
  | 'mergerAlert'
  | 'cashChime'
  | 'victoryFanfare';

class AudioSynthesizer {
  private ctx: AudioContext | null = null;
  private muted: boolean = false;

  constructor() {
    if (typeof window !== 'undefined' && typeof localStorage !== 'undefined') {
      const storedMute = localStorage.getItem('oilbaron_muted');
      if (storedMute !== null) {
        this.muted = storedMute === 'true';
      }
    }
  }

  /**
   * Lazily initializes or resumes the AudioContext upon user gesture.
   */
  private getContext(): AudioContext | null {
    if (typeof window === 'undefined') {
      return null;
    }

    if (!this.ctx) {
      const AudioCtxClass =
        window.AudioContext ||
        (window as unknown as { webkitAudioContext: typeof AudioContext }).webkitAudioContext;
      if (AudioCtxClass) {
        this.ctx = new AudioCtxClass();
      }
    }

    if (this.ctx && this.ctx.state === 'suspended') {
      this.ctx.resume().catch(() => {
        // Autoplay policy might reject until explicit user gesture
      });
    }

    return this.ctx;
  }

  public isMuted(): boolean {
    return this.muted;
  }

  public setMuted(muted: boolean): void {
    this.muted = muted;
    if (typeof localStorage !== 'undefined') {
      localStorage.setItem('oilbaron_muted', String(muted));
    }
  }

  public toggleMute(): boolean {
    this.setMuted(!this.muted);
    return this.muted;
  }

  /**
   * Short subtle click (800Hz -> 200Hz sine sweep).
   */
  public click(): void {
    if (this.muted) return;
    const ctx = this.getContext();
    if (!ctx) return;

    const now = ctx.currentTime;
    const osc = ctx.createOscillator();
    const gain = ctx.createGain();

    osc.type = 'sine';
    osc.frequency.setValueAtTime(800, now);
    osc.frequency.exponentialRampToValueAtTime(200, now + 0.05);

    gain.gain.setValueAtTime(0.15, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.05);

    osc.connect(gain);
    gain.connect(ctx.destination);

    osc.start(now);
    osc.stop(now + 0.05);
  }

  /**
   * Rising major arpeggio celebrating company founding (C5 -> E5 -> G5 -> C6).
   */
  public foundCompany(): void {
    if (this.muted) return;
    const ctx = this.getContext();
    if (!ctx) return;

    const notes = [523.25, 659.25, 783.99, 1046.5]; // C5, E5, G5, C6
    const startTime = ctx.currentTime;

    notes.forEach((freq, index) => {
      const noteTime = startTime + index * 0.09;
      const osc = ctx.createOscillator();
      const gain = ctx.createGain();

      osc.type = 'triangle';
      osc.frequency.setValueAtTime(freq, noteTime);

      gain.gain.setValueAtTime(0.2, noteTime);
      gain.gain.exponentialRampToValueAtTime(0.001, noteTime + 0.12);

      osc.connect(gain);
      gain.connect(ctx.destination);

      osc.start(noteTime);
      osc.stop(noteTime + 0.12);
    });
  }

  /**
   * Dramatic two-tone siren / alert for mergers and takeovers.
   */
  public mergerAlert(): void {
    if (this.muted) return;
    const ctx = this.getContext();
    if (!ctx) return;

    const startTime = ctx.currentTime;
    const tones = [240, 180, 240, 180];
    const duration = 0.12;

    tones.forEach((freq, index) => {
      const noteTime = startTime + index * duration;
      const osc = ctx.createOscillator();
      const gain = ctx.createGain();

      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(freq, noteTime);

      gain.gain.setValueAtTime(0.18, noteTime);
      gain.gain.exponentialRampToValueAtTime(0.01, noteTime + duration);

      osc.connect(gain);
      gain.connect(ctx.destination);

      osc.start(noteTime);
      osc.stop(noteTime + duration);
    });
  }

  /**
   * Crisp dual-frequency chime (1200Hz + 2400Hz) for cash transactions & bonuses.
   */
  public cashChime(): void {
    if (this.muted) return;
    const ctx = this.getContext();
    if (!ctx) return;

    const now = ctx.currentTime;
    const frequencies = [1200, 2400];

    frequencies.forEach((freq) => {
      const osc = ctx.createOscillator();
      const gain = ctx.createGain();

      osc.type = 'sine';
      osc.frequency.setValueAtTime(freq, now);

      gain.gain.setValueAtTime(0.12, now);
      gain.gain.exponentialRampToValueAtTime(0.0001, now + 0.35);

      osc.connect(gain);
      gain.connect(ctx.destination);

      osc.start(now);
      osc.stop(now + 0.35);
    });
  }

  /**
   * Glorious fanfare melody for victory celebration.
   */
  public victoryFanfare(): void {
    if (this.muted) return;
    const ctx = this.getContext();
    if (!ctx) return;

    // Melody: C5, E5, G5, C6 (sustained arpeggio with celebratory final chord)
    const melody = [
      { freq: 523.25, time: 0, dur: 0.14 },
      { freq: 659.25, time: 0.14, dur: 0.14 },
      { freq: 783.99, time: 0.28, dur: 0.14 },
      { freq: 1046.5, time: 0.42, dur: 0.5 },
      // Harmonics for final chord
      { freq: 523.25, time: 0.42, dur: 0.5 },
      { freq: 659.25, time: 0.42, dur: 0.5 },
      { freq: 783.99, time: 0.42, dur: 0.5 },
    ];

    const baseTime = ctx.currentTime;

    melody.forEach((note) => {
      const noteTime = baseTime + note.time;
      const osc = ctx.createOscillator();
      const gain = ctx.createGain();

      osc.type = 'triangle';
      osc.frequency.setValueAtTime(note.freq, noteTime);

      gain.gain.setValueAtTime(0.15, noteTime);
      gain.gain.exponentialRampToValueAtTime(0.001, noteTime + note.dur);

      osc.connect(gain);
      gain.connect(ctx.destination);

      osc.start(noteTime);
      osc.stop(noteTime + note.dur);
    });
  }
}

export const soundEffects = new AudioSynthesizer();

export const playClick = () => soundEffects.click();
export const playFoundCompany = () => soundEffects.foundCompany();
export const playMergerAlert = () => soundEffects.mergerAlert();
export const playCashChime = () => soundEffects.cashChime();
export const playVictoryFanfare = () => soundEffects.victoryFanfare();
