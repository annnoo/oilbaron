import React from 'react';

export interface ClassicWindowProps {
  title: string;
  children: React.ReactNode;
  onClose?: () => void;
  className?: string;
  style?: React.CSSProperties;
}

export const ClassicWindow: React.FC<ClassicWindowProps> = ({
  title,
  children,
  onClose,
  className = '',
  style,
}) => {
  return (
    <div className={`win31-window ${className}`} style={style}>
      <div className="win31-titlebar">
        <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
          <div className="win31-titlebar-btn" title="System Menu" aria-label="System Menu">
            —
          </div>
          <span>{title}</span>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '2px' }}>
          <div className="win31-titlebar-btn" title="Minimize" aria-label="Minimize">
            ▼
          </div>
          <div className="win31-titlebar-btn" title="Maximize" aria-label="Maximize">
            ▲
          </div>
        </div>
      </div>
      <div>{children}</div>
    </div>
  );
};
