import React from 'react';
import {
  User,
  Bot,
  Trophy,
  TrendingUp,
  Sparkles,
} from 'lucide-react';
import type {
  CompanyDefinition,
  CompanyId,
  CompanyState,
  PlayerState,
} from '../engine/types';
import { CLASSIC_COMPANIES } from '../engine/config';
import {
  calculateNetWorth,
  calculateSharePrice,
  getCompanyDefinition,
} from '../engine/market';
import { getCompanyVisual } from './TileInspector';
import { playClick } from '../assets/audio';
import { useTranslation } from '../i18n';

export interface PlayerScoreboardProps {
  /**
   * Complete list of 2–6 players in the match.
   */
  players: readonly PlayerState[];

  /**
   * 0-based index of the player whose turn it currently is.
   */
  activePlayerIndex: number;

  /**
   * Current company states, used for market valuation of shares.
   */
  companies?: Record<CompanyId, CompanyState> | Partial<Record<CompanyId, CompanyState>>;

  /**
   * Company definitions list (defaults to CLASSIC_COMPANIES).
   */
  definitions?: readonly CompanyDefinition[];

  /**
   * Callback invoked when a player's AI status is toggled mid-game.
   */
  onTogglePlayerAi?: (playerIndex: number, nextIsAi: boolean) => void;

  /**
   * Optional callback when clicking a player card to focus/inspect.
   */
  onSelectPlayer?: (playerIndex: number) => void;

  /**
   * Optional custom CSS class.
   */
  className?: string;
}

/**
 * Formats a numeric dollar amount with standard commas (e.g. $6,000).
 */
export function formatCurrency(amount: number): string {
  return `$${amount.toLocaleString()}`;
}

export const PlayerScoreboard: React.FC<PlayerScoreboardProps> = ({
  players,
  activePlayerIndex,
  companies = {},
  definitions = CLASSIC_COMPANIES,
  onTogglePlayerAi,
  onSelectPlayer,
  className = '',
}) => {
  const { t, language } = useTranslation();

  // Precompute net worth for all players to determine current leader
  const playerValuations = React.useMemo(() => {
    return players.map((player) => ({
      player,
      netWorth: calculateNetWorth(player, companies, definitions),
    }));
  }, [players, companies, definitions]);

  const maxNetWorth = React.useMemo(() => {
    if (playerValuations.length === 0) return 0;
    return Math.max(...playerValuations.map((v) => v.netWorth));
  }, [playerValuations]);

  return (
    <aside
      className={`player-scoreboard-sidebar ${className}`}
      aria-label="Player Standings & Scoreboard"
      data-testid="player-scoreboard"
    >
      {/* Sidebar Header */}
      <div className="scoreboard-header">
        <div className="scoreboard-header-title">
          <TrendingUp size={15} className="text-gold" />
          <span>{t('scoreboard.title')}</span>
        </div>
        <span className="scoreboard-player-count">
          {players.length} {language === 'de' ? 'Spieler' : 'Players'}
        </span>
      </div>

      {/* Players List */}
      <div className="scoreboard-players-list" role="feed" aria-label="Player rankings and status">
        {players.map((player, index) => {
          const isActive = index === activePlayerIndex;
          const netWorth = playerValuations[index]?.netWorth ?? player.cash;
          const isLeader = maxNetWorth > 0 && netWorth === maxNetWorth;

          // Extract active shareholdings
          const shareholdings = Object.entries(player.shares)
            .filter(([_, count]) => count > 0)
            .map(([companyId, count]) => {
              const def = getCompanyDefinition(definitions, companyId);
              const compState = companies[companyId];
              const price = def && compState && compState.isActive && compState.size >= 2
                ? calculateSharePrice(def, compState.size)
                : 0;
              const totalVal = count * price;
              const visual = getCompanyVisual(companyId, definitions);
              return {
                companyId,
                count,
                price,
                totalVal,
                visual,
              };
            });

          const totalSharesCount = shareholdings.reduce((sum, s) => sum + s.count, 0);

          return (
            <article
              key={player.id || `player-${index}`}
              className={`scoreboard-player-card ${isActive ? 'is-active' : ''} ${
                isLeader ? 'is-leader' : ''
              }`}
              onClick={() => onSelectPlayer?.(index)}
              aria-current={isActive ? 'step' : undefined}
              data-testid={`player-card-${index}`}
            >
              {/* Card Top Row: Name, Status Badges, and Active Marker */}
              <div className="player-card-header">
                <div className="player-title-group">
                  <span className="player-index-tag">#{index + 1}</span>
                  <span className="player-name-text" title={player.name}>
                    {player.name}
                  </span>
                </div>

                <div className="player-badges-group">
                  {/* Active Turn Badge */}
                  {isActive && (
                    <span
                      className="badge badge-gold player-turn-badge"
                      title="Currently active player's turn"
                      data-testid={`badge-active-turn-${index}`}
                    >
                      <Sparkles size={10} /> {t('scoreboard.turn')}
                    </span>
                  )}

                  {/* Leader Badge */}
                  {isLeader && (
                    <span
                      className="badge player-leader-badge"
                      title="Current net worth leader"
                    >
                      <Trophy size={10} /> #1
                    </span>
                  )}

                  {/* Human vs CPU Badge */}
                  <span
                    className={`badge ${player.isAi ? 'badge-cpu' : 'badge-human'}`}
                    title={player.isAi ? 'Computer AI player' : 'Human player'}
                    data-testid={`badge-type-${index}`}
                  >
                    {player.isAi ? <Bot size={11} /> : <User size={11} />}
                    {player.isAi ? 'CPU' : 'HUMAN'}
                  </span>
                </div>
              </div>

              {/* Financial Metrics Row */}
              <div className="player-metrics-grid">
                <div className="player-metric-box">
                  <span className="metric-box-label">{t('scoreboard.cash')}</span>
                  <span className="metric-box-value cash-value">
                    {formatCurrency(player.cash)}
                  </span>
                </div>

                <div className="player-metric-box">
                  <span className="metric-box-label">{t('scoreboard.netWorth')}</span>
                  <span className="metric-box-value networth-value">
                    {formatCurrency(netWorth)}
                  </span>
                </div>

                <div className="player-metric-box">
                  <span className="metric-box-label">{language === 'de' ? 'Hand' : 'Hand'}</span>
                  <span className="metric-box-value hand-value">
                    {player.hand.length} {language === 'de' ? 'Felder' : 'chits'}
                  </span>
                </div>
              </div>

              {/* Shareholdings Portfolio Summary */}
              <div className="player-shares-section">
                <div className="shares-header-row">
                  <span className="shares-section-label">
                    {language === 'de' ? 'Aktien' : 'Shares'} ({totalSharesCount})
                  </span>
                  {totalSharesCount > 0 && (
                    <span className="shares-count-total">
                      {shareholdings.length} corp{shareholdings.length > 1 ? 's' : ''}
                    </span>
                  )}
                </div>

                {shareholdings.length === 0 ? (
                  <span className="shares-empty-text">No shares owned</span>
                ) : (
                  <div className="shares-pills-wrap">
                    {shareholdings.map((holding) => (
                      <span
                        key={holding.companyId}
                        className="shareholder-pill"
                        style={{
                          borderColor: `${holding.visual.color}55`,
                          backgroundColor: `${holding.visual.color}18`,
                        }}
                        title={`${holding.visual.name}: ${holding.count} shares (Worth ${formatCurrency(
                          holding.totalVal
                        )})`}
                        data-testid={`share-${index}-${holding.companyId}`}
                      >
                        <span
                          className="shareholder-dot"
                          style={{ backgroundColor: holding.visual.color }}
                        />
                        <span className="shareholder-code">
                          {holding.visual.shortCode}
                        </span>
                        <span className="shareholder-count">
                          {holding.count}
                        </span>
                      </span>
                    ))}
                  </div>
                )}
              </div>

              {/* Card Footer: Inline CPU/Human Toggle Button */}
              <div className="player-card-footer">
                <button
                  type="button"
                  className={`btn btn-sm ${
                    player.isAi ? 'btn-secondary' : 'btn-ghost'
                  } cpu-toggle-btn`}
                  onClick={(e) => {
                    e.stopPropagation();
                    playClick();
                    onTogglePlayerAi?.(index, !player.isAi);
                  }}
                  aria-label={
                    player.isAi
                      ? `Change ${player.name} to Human`
                      : `Change ${player.name} to CPU`
                  }
                  title={
                    player.isAi
                      ? `Change ${player.name} from Computer to Human player`
                      : `Change ${player.name} from Human to Computer CPU player`
                  }
                  data-testid={`btn-toggle-ai-${index}`}
                >
                  {player.isAi ? (
                    <>
                      <User size={12} />
                      <span>Change to Human</span>
                    </>
                  ) : (
                    <>
                      <Bot size={12} />
                      <span>Change to CPU</span>
                    </>
                  )}
                </button>
              </div>
            </article>
          );
        })}
      </div>
    </aside>
  );
};

export default PlayerScoreboard;
