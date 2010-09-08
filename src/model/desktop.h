/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

#ifndef DESKTOP_H
#define DESKTOP_H

#include <QObject>

#include "cardlist.h"
#include "ncounter.h"

const char * sGameName (eGameBid game);

/**
 * @struct GameLogEntry
 *
 * Keeps information about all played rounds
 */
struct GameLogEntry {
    eGameBid game;
    qint8 player;
    bool whist[3];
    qint16 takes[3];
    qint16 score[3];
    qint16 pool[3];
    qint16 mountain[3];
    qint16 leftWhists[3];
    qint16 rightWhists[3];
    float time;
    QList<Card> cardList[3];
};

class DeskView;
class Player;

/**
 * @class PrefModel desktop.h
 * @brief Model of preferans game
 *
 * Manages all game process but doesn't respond for any visual representation of
 * game events
 */
class PrefModel : public QObject {
  Q_OBJECT

public:
  explicit PrefModel (DeskView *aDeskView/*=0*/);
  virtual ~PrefModel ();

  void runGame ();
  Card *cardOnDesk(int index) const;

  bool saveGame (const QString & name);
  bool loadGame (const QString & name);

  void closePool ();

  void serialize (QByteArray &ba);
  bool unserialize (QByteArray &ba, int *pos);

  Player *player (int num);
  Player *currentPlayer () const { return mPlayers[nCurrentMove.nValue]; }
  int activePlayerNumber () const { return mPlayerActive; }
  int playerWithMaxPool (); // except the players who closed the pool
  int trumpSuit () const;
  eGameBid currentGame() const { return m_currentGame; }
  const QList<GameLogEntry> & gameLog() const { return m_gameLog; }

  DeskView *view() const { Q_ASSERT(mDeskView); return mDeskView; }
  int gameWhists (eGameBid gType) const;

  void emitShowHint(const QString text) { emit showHint(text); }
  void emitClearHint() { emit clearHint(); }
  void showMoveImpossible(const bool canRetry);
  void showMoveHint();

signals:
  void deskChanged ();
  void gameOver ();
  void showHint (const QString & text);
  void clearHint();

public:
  WrapCounter nCurrentStart, nCurrentMove;
  bool mGameRunning;
  bool mBiddingDone;
  bool mOnDeskClosed;
  bool mPlayingRound;
  int mPlayerHi; // highlighted message number

  // Conventions
  bool optStalingrad;
  bool opt10Whist;
  bool optWhistGreedy;
  int optMaxPool;
  bool optQuitAfterMaxRounds;
  int optMaxRounds;
  bool optAggPass;
  int optPassCount;
  bool optWithoutThree;

  // Names
  QString optHumanName;
  QString optPlayerName1;
  QString optPlayerName2;

  // AIs
  bool optAlphaBeta1;
  bool optAlphaBeta2;

private:
  static const QString bidMessage(const eGameBid game);
  void initPlayers ();
  Player *player (const WrapCounter &cnt);
  Card *makeGameMove (Card *lMove, Card *rMove, bool isPassOut);
  void playingRound();
  bool checkMoves();

private:
  DeskView *mDeskView;
  CardList mDeck;
  QList<Player *> mPlayers;
  Card *mFirstCard, *mSecondCard, *mThirdCard;
  Card *mCardsOnDesk[4];
  QCardList m_outCards;
  int mPlayerActive; // who plays (if not raspass and mPlayingRound=true)
  int m_trump;  
  bool m_closedWhist;
  bool m_keepLog;
  QList<GameLogEntry> m_gameLog;
  eGameBid m_currentGame;
};


#endif
