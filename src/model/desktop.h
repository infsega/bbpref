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

class DeskView;
class Player;

class PrefModel : public QObject {
  Q_OBJECT

public:
  PrefModel (DeskView *aDeskView/*=0*/);
  virtual ~PrefModel ();

  void runGame ();

  void draw (bool emitSignal=true);

  bool saveGame (const QString name);
  bool loadGame (const QString name);

  void closePool ();

  void serialize (QByteArray &ba);
  bool unserialize (QByteArray &ba, int *pos);

  Player *player (int num);
  Player *currentPlayer () const { return mPlayers[nCurrentMove.nValue]; }
  int activePlayerNumber () const { return mPlayerActive; }

signals:
  void deskChanged ();
  void gameOver ();

public:
  WrapCounter nCurrentStart, nCurrentMove;
  bool mGameRunning;
  bool mBiddingDone;

private:
  Player *player (const WrapCounter &cnt);
  void internalInit ();
  Card *makeGameMove (Card *lMove, Card *rMove, bool isPassOut);
  int playerWithMaxPool (); // except the players who closed the pool
  void calculateScore (int nPassCounter);
  void playingRound();

private:
  DeskView *mDeskView;
  CardList mDeck;
  QList<Player *> mPlayers;
  Card *mFirstCard, *mSecondCard, *mThirdCard;
  Card *mCardsOnDesk[4];
  bool mPlayingRound;
  int mPlayerActive; // who plays (if not raspass and mPlayingRound=true)
  int mPlayerHi; // highlighted message number
  bool mOnDeskClosed;
  int m_trump;
};


#endif
