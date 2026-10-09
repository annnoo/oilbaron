import React, { useState } from 'react';
import { Volume2, VolumeX, Flame, Coins, AlertTriangle, Trophy, Sparkles } from 'lucide-react';
import {
  soundEffects,
  playClick,
  playFoundCompany,
  playMergerAlert,
  playCashChime,
  playVictoryFanfare,
} from './assets/audio';
import { CLASSIC_COMPANIES } from './engine/config';

export const App: React.FC = () => {
  const [muted, setMuted] = useState<boolean>(soundEffects.isMuted());

  const handleToggleMute = () => {
    const nextMuted = soundEffects.toggleMute();
    setMuted(nextMuted);
  };

  return (
    <div className="app-container">
      {/* Header bar */}
      <header
        style={{
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'space-between',
          padding: '12px 20px',
          backgroundColor: 'var(--bg-surface)',
          borderBottom: '1px solid var(--border-subtle)',
        }}
      >
        <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
          <Flame size={24} color="var(--accent-gold)" />
          <h1 style={{ fontSize: '18px', fontWeight: 700, letterSpacing: '0.02em' }}>
            OIL BARON <span style={{ fontSize: '12px', color: 'var(--text-muted)' }}>(1993)</span>
          </h1>
        </div>

        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
          <button
            type="button"
            className="btn btn-secondary btn-sm"
            onClick={handleToggleMute}
            title={muted ? 'Unmute Sound' : 'Mute Sound'}
          >
            {muted ? <VolumeX size={16} /> : <Volume2 size={16} />}
            <span>{muted ? 'Muted' : 'Sound On'}</span>
          </button>
        </div>
      </header>

      {/* Main Container */}
      <main className="game-main">
        {/* Scaffolding status card */}
        <section className="panel">
          <div className="panel-header">
            <span>Frontend Scaffolding & Audio Synthesizer</span>
            <span className="badge badge-safe">Ready</span>
          </div>
          <div className="panel-body" style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
            <p style={{ color: 'var(--text-secondary)' }}>
              React 19 + TypeScript + Vite UI shell initialized with Web Audio API chiptune synthesis and
              industrial theme tokens.
            </p>

            {/* Audio Testing Controls */}
            <div>
              <h2 style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-amber)', marginBottom: '8px' }}>
                Procedural Sound Synthesizer Test
              </h2>
              <div style={{ display: 'flex', flexWrap: 'wrap', gap: '8px' }}>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playClick()}
                >
                  <Sparkles size={14} /> Click
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playCashChime()}
                >
                  <Coins size={14} /> Cash Chime
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playFoundCompany()}
                >
                  <Flame size={14} /> Found Company
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playMergerAlert()}
                >
                  <AlertTriangle size={14} /> Merger Siren
                </button>
                <button
                  type="button"
                  className="btn btn-secondary btn-sm"
                  onClick={() => playVictoryFanfare()}
                >
                  <Trophy size={14} /> Victory Fanfare
                </button>
              </div>
            </div>

            {/* Company Tokens Preview */}
            <div>
              <h2 style={{ fontSize: '13px', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: '8px' }}>
                Company Palette Tokens
              </h2>
              <div style={{ display: 'flex', flexWrap: 'wrap', gap: '8px' }}>
                {CLASSIC_COMPANIES.map((company) => (
                  <div
                    key={company.id}
                    style={{
                      display: 'flex',
                      alignItems: 'center',
                      gap: '8px',
                      padding: '4px 10px',
                      backgroundColor: 'var(--bg-surface-elevated)',
                      border: '1px solid var(--border-subtle)',
                      borderRadius: 'var(--radius-sm)',
                    }}
                  >
                    <span
                      style={{
                        width: '12px',
                        height: '12px',
                        borderRadius: '50%',
                        backgroundColor: company.color,
                        display: 'inline-block',
                      }}
                    />
                    <span style={{ fontSize: '12px', fontWeight: 500 }}>{company.name}</span>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </section>
      </main>
    </div>
  );
};

export default App;
