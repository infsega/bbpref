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

#ifndef PLAYER_H
#define PLAYER_H

#include "prfconst.h"
#include "cardlist.h"
#include "plscore.h"

class DeskView;

class Player {
public:
  Player (int aMyNumber, DeskView *aDeskView=0);
  //Player (const Player &pl);
  virtual ~Player ();

  virtual QString type() const = 0;
  virtual bool isHuman() const { return false; }

  virtual Player * create(int aMyNumber, DeskView *aDeskView = 0) = 0;

  //Player &operator = (const Player &pl);

  virtual void clear ();

  virtual bool invisibleHand () const;
  virtual void setInvisibleHand (bool invis);

  virtual void setCurrentStart (bool start);

  void sortCards () { mCards.mySort(); }

  void dealCard (Card *aCard) { mCards.insert(aCard); } // get dealed card

  virtual Card *makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) = 0; //move
  virtual eGameBid makeBid (eGameBid lMove, eGameBid rMove) = 0; //
  virtual eGameBid makeFinalBid (eGameBid MaxGame, int HaveAWhist, int nGamerWhist) = 0; // после получения игроком прикупа -- пасс или вист
  virtual eGameBid dropForGame () = 0; // сброс для игры
  virtual eGameBid dropForMisere () = 0; // сброс для мизера
  virtual void returnDrop (); // вернуть сброс

  virtual bool chooseClosedWhist () = 0;

  /// @todo Move to view delegate
  virtual void draw ();
  virtual void highlightCard (int lx, int ly); // подсветить карту по данным координатам (и перерисовать руку, если надо)

  void setMessage (const QString msg) { mMessage = msg; }
  const QString message () const { return mMessage; }

  int number () const { return mPlayerNo; }

  eGameBid myGame () const { return mMyGame; }
  void setMyGame (eGameBid game) { mMyGame = game; }

  /// @todo Move to shared score board
  int tricksTaken () const { return mTricksTaken; }
  void gotTrick () { mTricksTaken++; }
  void gotPassPassTricks (int cnt) { mTricksTaken = cnt; }

protected:
  virtual void internalInit ();
  //virtual void clone (const Player *pl);

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
