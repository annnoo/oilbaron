import { describe, it, expect, beforeEach } from 'vitest';
import {
  soundEffects,
  playClick,
  playFoundCompany,
  playMergerAlert,
  playCashChime,
  playVictoryFanfare,
} from '../audio';

describe('AudioSynthesizer', () => {
  beforeEach(() => {
    soundEffects.setMuted(false);
  });

  it('toggles and manages mute status correctly', () => {
    expect(soundEffects.isMuted()).toBe(false);

    soundEffects.setMuted(true);
    expect(soundEffects.isMuted()).toBe(true);

    const toggled = soundEffects.toggleMute();
    expect(toggled).toBe(false);
    expect(soundEffects.isMuted()).toBe(false);
  });

  it('safely handles playback calls in non-browser/node environment without throwing', () => {
    expect(() => playClick()).not.toThrow();
    expect(() => playFoundCompany()).not.toThrow();
    expect(() => playMergerAlert()).not.toThrow();
    expect(() => playCashChime()).not.toThrow();
    expect(() => playVictoryFanfare()).not.toThrow();
  });

  it('safely handles playback calls when muted', () => {
    soundEffects.setMuted(true);
    expect(() => soundEffects.click()).not.toThrow();
    expect(() => soundEffects.foundCompany()).not.toThrow();
    expect(() => soundEffects.mergerAlert()).not.toThrow();
    expect(() => soundEffects.cashChime()).not.toThrow();
    expect(() => soundEffects.victoryFanfare()).not.toThrow();
  });
});
