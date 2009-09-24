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

#ifndef PLAYER_H
#define PLAYER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"


class Player {
public:
  Player (int aMyNumber, DeskView *aDeskView=0);
  Player (const Player &pl);
  virtual ~Player ();

  Player &operator = (const Player &pl);

  virtual void clear ();

  virtual bool invisibleHand () const;
  virtual void setInvisibleHand (bool invis);

  virtual void setCurrentStart (bool start);

  virtual void sortCards ();

  virtual void dealCard (Card *aCard); // get dealed card

  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) = 0; //move
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove) = 0; //
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAWhist, int nGamerWhist) = 0; // после получения игроком прикупа -- пасс или вист
  virtual eGameBid dropForGame () = 0; // сброс для игры
  virtual eGameBid dropForMisere () = 0; // сброс для мизера
  virtual void returnDrop (); // вернуть сброс

  virtual bool chooseClosedWhist () = 0;

  virtual void getLeftTop (int *left, int *top);
  virtual void draw ();
  virtual void hilightCard (int lx, int ly); // подсветить карту по данным координатам (и перерисовать руку, если надо)

  inline void setMessage (const QString &msg) { mMessage = msg; }
  inline const QString &message () const { return mMessage; }

  inline int number () const { return mPlayerNo; }

  inline eGameBid myGame () const { return mMyGame; }
  inline void setMyGame (eGameBid game) { mMyGame = game; }

  inline int tricksTaken () const { return mTricksTaken; }
  virtual void gotTrick ();
  virtual void gotPassPassTricks (int cnt);

protected:
  virtual void internalInit ();
  virtual void clone (const Player *pl);

  virtual void drawAt (DeskView *aDeskView, int left, int top, int selNo=-1);
  virtual void clearCardArea ();

  // at least 28 ints (14 int pairs); return # of used ints; the last int is -1
  // result: ofs, cardNo, ..., -1
  virtual int buildHandXOfs (int *dest, int startX, bool opened);
  virtual int cardAt (int lx, int ly, bool opened=true);

public:
  DeskView *mDeskView;

  ScoreBoard mScore;
  CardList mCards; // my cards
  CardList mLeft;  // cards of left player (supposed or open)
  CardList mRight; // cards of right player (supposed or open)
  CardList mOut;   // Сdropped cards (my or supposed)
  CardList mCardsOut; // во взятках мои
  // this part for miser catches
  Card *mCardCarryThru;

  int mClickX, mClickY;
  bool mWaitingForClick;

protected:
  QString mNick;
  QString mMessage;
  int mPlayerNo; // my number
  bool mInvisibleHand;
  eGameBid mMyGame;
  int mTricksTaken;
  int mPrevHiCardIdx;
  bool mIStart;
};


#endif
