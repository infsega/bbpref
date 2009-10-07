/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
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

#include <QByteArray>
#include <QObject>

#include "card.h"
#include "ncounter.h"
#include "player.h"


class PrefDesktop : public QObject {
  Q_OBJECT

public:
  PrefDesktop (DeskView *aDeskView=0);
  virtual ~PrefDesktop ();

  Player *addPlayer (Player *);

  void runGame ();

  void draw (bool emitSignal=true);
  void drawPool ();

  bool saveGame (const QString &name); // don't working!
  bool loadGame (const QString &name); // don't working!

  void closePool ();

  inline Player *currentPlayer () const { return mPlayers[nCurrentMove.nValue]; }

  void serialize (QByteArray &ba);
  bool unserialize (QByteArray &ba, int *pos);

  void emitRepaint ();

signals:
  void deskChanged ();
  void gameOver ();

public:
  DeskView *mDeskView;
  CardList mDeck;
  QList<Player *> mPlayers;
  WrapCounter nCurrentStart, nCurrentMove;
  Card *mFirstCard, *mSecondCard, *mThirdCard;
  Card *mCardsOnDesk[4];

  bool mShowPool;
  bool mOnDeskClosed;
  bool mGameRunning;
  bool mBiddingDone;

private:
  void internalInit ();

  Player *player (int num);
  Player *player (const WrapCounter &cnt);
  Card *makeGameMove (Card *lMove, Card *rMove, bool isPassOut);

  void drawInGameCard (int mCardNo, Card *card);
  void inGameCardLeftTop (int mCardNo, int *left, int *top);
  void animateDeskToPlayer (int plrNo, bool doAnim);

private:
  //void drawBidWindows (const eGameBid *bids, int curPlr);
  void getPMsgXY (int plr, int *x, int *y);

  int playerWithMaxPool (); // except the players who closed the pool

  int whoseTrick (Card *p1, Card *p2, Card *p3, int koz);

private:
  bool mPlayingRound;
  int mPlayerActive; // who plays (if not raspass and mPlayingRound=true)
  int iMoveX, iMoveY;
  int mPlayerHi; // подсвеченая мессага
};


#endif
