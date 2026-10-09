import React, { useState } from 'react';
import { HelpCircle, X, BookOpen, Award, GitMerge, TrendingUp, ShieldCheck, Flag } from 'lucide-react';
import { playClick } from '../../assets/audio';

export interface HelpModalProps {
  isOpen: boolean;
  onClose: () => void;
}

export const HelpModal: React.FC<HelpModalProps> = ({ isOpen, onClose }) => {
  const [activeSection, setActiveSection] = useState<'overview' | 'founding' | 'mergers' | 'market' | 'gameover'>('overview');

  if (!isOpen) return null;

  return (
    <div
      className="modal-backdrop"
      role="presentation"
      onClick={(e) => {
        if (e.target === e.currentTarget) onClose();
      }}
    >
      <div
        className="modal-dialog modal-lg"
        role="dialog"
        aria-modal="true"
        aria-labelledby="help-title"
      >
        <div className="modal-header">
          <div className="modal-title" id="help-title">
            <HelpCircle size={18} style={{ color: 'var(--accent-gold)' }} />
            <span>Oil Baron — Rules of Play & Strategy Guide</span>
          </div>
          <button
            type="button"
            className="modal-close-btn"
            onClick={onClose}
            aria-label="Close help manual"
          >
            <X size={18} />
          </button>
        </div>

        <div className="modal-body">
          {/* Section Navigation Tabs */}
          <div className="discarded-tabs-bar" style={{ marginBottom: '14px' }}>
            <button
              type="button"
              className={`discarded-tab-btn ${activeSection === 'overview' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveSection('overview');
              }}
            >
              Overview & Board
            </button>
            <button
              type="button"
              className={`discarded-tab-btn ${activeSection === 'founding' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveSection('founding');
              }}
            >
              Founding Companies
            </button>
            <button
              type="button"
              className={`discarded-tab-btn ${activeSection === 'market' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveSection('market');
              }}
            >
              Stock Market
            </button>
            <button
              type="button"
              className={`discarded-tab-btn ${activeSection === 'mergers' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveSection('mergers');
              }}
            >
              Mergers & Payouts
            </button>
            <button
              type="button"
              className={`discarded-tab-btn ${activeSection === 'gameover' ? 'is-active' : ''}`}
              onClick={() => {
                playClick();
                setActiveSection('gameover');
              }}
            >
              End Game & Scoring
            </button>
          </div>

          <div style={{ fontSize: '13px', lineHeight: 1.6, color: 'var(--text-secondary)' }}>
            {activeSection === 'overview' && (
              <div>
                <h4 style={{ color: 'var(--text-primary)', marginBottom: '8px' }}>Object of the Game</h4>
                <p>
                  Oil Baron is a corporate acquisition game of speculative investment and board control set on a 10×12 grid (coordinates A1 through J12, 120 oil lease sites).
                  Players take turns placing oil drilling chits, incorporating corporations, purchasing stock, orchestrating hostile corporate takeovers, and liquidating assets to accumulate the greatest net worth.
                </p>
                <h4 style={{ color: 'var(--text-primary)', margin: '12px 0 6px 0' }}>Turn Sequence</h4>
                <ol style={{ paddingLeft: '20px', display: 'flex', flexDirection: 'column', gap: '4px' }}>
                  <li><strong>Place a Chit:</strong> Select one of the tiles from your hand and place it on the board.</li>
                  <li><strong>Resolve Board Action:</strong> If the placement forms a company or triggers a merger, that event resolves immediately.</li>
                  <li><strong>Buy Shares:</strong> Purchase up to 3 shares of stock in any active corporations (if capital permits).</li>
                  <li><strong>Replenish Hand:</strong> Draw a new chit from the reserve bag to maintain your hand of 6 tiles.</li>
                </ol>
              </div>
            )}

            {activeSection === 'founding' && (
              <div>
                <h4 style={{ color: 'var(--text-primary)', marginBottom: '8px' }}>Founding a Corporation</h4>
                <p>
                  When you place a chit adjacent (horizontally or vertically) to an existing unincorporated chit, an oil corporation is founded!
                  You choose which of the available corporations will be established on the newly discovered field.
                </p>
                <div style={{ padding: '10px 14px', background: 'rgba(245, 158, 11, 0.1)', border: '1px solid var(--border-accent)', borderRadius: 'var(--radius-sm)', margin: '12px 0' }}>
                  <strong style={{ color: 'var(--accent-gold)' }}>Founder's Reward:</strong>
                  {' '}Under authentic 1993 Oil Baron rules, the founder immediately receives <strong>10 FREE SHARES</strong> from the corporate treasury! (Modern balanced rules offer 1 free share).
                </div>
                <h4 style={{ color: 'var(--text-primary)', margin: '12px 0 6px 0' }}>The 7 Corporations & Tiers</h4>
                <ul style={{ paddingLeft: '20px' }}>
                  <li><strong>Low Tier ($200 Base):</strong> American Natural Gas, Arabian Oil</li>
                  <li><strong>Mid Tier ($300 Base):</strong> Imperial Petroleum, Supra Oil, Tower Drilling</li>
                  <li><strong>High Tier ($400 Base):</strong> Royal Dutch, Weemoco Oil</li>
                </ul>
              </div>
            )}

            {activeSection === 'market' && (
              <div>
                <h4 style={{ color: 'var(--text-primary)', marginBottom: '8px' }}>Stock Valuation & Trading</h4>
                <p>
                  Each corporation has a fixed treasury of 25 shares. Share prices increase as companies expand their territory on the board:
                </p>
                <div style={{ padding: '10px 14px', background: 'var(--bg-surface-elevated)', border: '1px solid var(--border-default)', borderRadius: 'var(--radius-sm)', margin: '10px 0', fontFamily: 'var(--font-mono)' }}>
                  Price = BasePrice + (Size - 2) × $100 (capped at size 41)
                </div>
                <p>
                  At the end of your turn, after placing a tile, you may purchase up to <strong>3 shares</strong> across any active corporations with available stock in the bank.
                </p>
              </div>
            )}

            {activeSection === 'mergers' && (
              <div>
                <h4 style={{ color: 'var(--text-primary)', marginBottom: '8px' }}>Corporate Mergers</h4>
                <p>
                  When a placed tile connects two or more established corporations, a merger occurs!
                </p>
                <ul style={{ paddingLeft: '20px', display: 'flex', flexDirection: 'column', gap: '6px' }}>
                  <li><strong>Survivor:</strong> The larger corporation survives and absorbs the smaller (defunct) corporation's territory. If tied, the player who placed the tile chooses the survivor.</li>
                  <li><strong>Safe Corporations:</strong> Any corporation with <strong>11 or more tiles</strong> is permanently SAFE and can never be absorbed in a merger!</li>
                  <li><strong>Dead Tiles:</strong> A tile that would merge two or more safe corporations is illegal/dead and cannot be played.</li>
                </ul>
                <h4 style={{ color: 'var(--text-primary)', margin: '12px 0 6px 0' }}>Shareholder Bonuses</h4>
                <p>
                  Before defunct shares are resolved, cash bonuses are paid:
                  <br />• <strong>1st Majority Bonus:</strong> 10× pre-merger share price.
                  <br />• <strong>2nd Minority Bonus:</strong> 5× pre-merger share price.
                  <br />• A solo shareholder holding all issued stock receives <strong>both</strong> bonuses (15× share price)!
                </p>
                <h4 style={{ color: 'var(--text-primary)', margin: '12px 0 6px 0' }}>Share Disposition</h4>
                <p>
                  Each shareholder of the defunct corporation may then:
                  <br />1. <strong>Sell</strong> shares for cash at the pre-merger price.
                  <br />2. <strong>Trade</strong> 2 defunct shares for 1 surviving corporation share (if bank shares exist).
                  <br />3. <strong>Hold</strong> shares in dormant reserve for potential refounding later.
                </p>
              </div>
            )}

            {activeSection === 'gameover' && (
              <div>
                <h4 style={{ color: 'var(--text-primary)', marginBottom: '8px' }}>Game Over Conditions</h4>
                <p>
                  A match concludes when:
                  <br />• All active corporations on the board are SAFE (≥11 tiles), OR
                  <br />• Any single corporation reaches 41 or more tiles, OR
                  <br />• The tile draw bag is exhausted and no player can make a legal placement.
                </p>
                <h4 style={{ color: 'var(--text-primary)', margin: '12px 0 6px 0' }}>Final Liquidation</h4>
                <p>
                  When the game ends, all surviving active corporations distribute final 1st and 2nd place shareholder bonuses, and all stock is liquidated at full market value for cash! The player with the highest total net worth is crowned the Oil Baron.
                </p>
              </div>
            )}
          </div>
        </div>

        <div className="modal-footer" style={{ display: 'flex', justifyContent: 'flex-end' }}>
          <button type="button" className="btn btn-secondary" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
};
