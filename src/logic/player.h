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
#include "scoreboard.h"

class DeskView;

/**
 * @class Player player.h
 * @brief Base class for all players
 *
 */
class Player {
public:
  Player (int number, PrefModel *model);
  //Player (const Player &pl);
  virtual ~Player ();

  /// String identifier of concrete player
  virtual const QString type() const = 0;
  /// @return true if this player object interacts with GUI user
  virtual bool isHuman() const { return false; }

  /// Factory method, creates player of the same subclass as current
  virtual Player * create(int number, PrefModel *model) = 0;

  //Player &operator = (const Player &pl);

  virtual void clear ();

  virtual bool invisibleHand () const;
  virtual void setInvisibleHand (bool invis);

  virtual void setCurrentStart (bool start);

  void sortCards () { mCards.mySort(); }

  void dealCard (Card *aCard) { mCards.insert(aCard); } // get dealed card

  /// Player's decision on move in the game
  virtual Card *makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) = 0;
  /// Player's decision on move in the bidding
  virtual eGameBid makeBid (eGameBid lMove, eGameBid rMove) = 0;

  virtual eGameBid makeFinalBid (eGameBid MaxGame, int nPlayerPass) = 0; // после получения игроком прикупа -- пасс или вист
  virtual eGameBid makeDrop() = 0;
  virtual void returnDrop (); // вернуть сброс

  /// Player's decision on open or closed whist
  virtual bool chooseClosedWhist () = 0;

  /// @todo Move to view delegate
  virtual void draw ();
  virtual void highlightCard (int lx, int ly); // подсветить карту по данным координатам (и перерисовать руку, если надо)

  void setNick (const QString nick) { m_nick = nick; }
  QString nick() const { return m_nick; }

  void setMessage (const QString msg) { mMessage = msg; }
  const QString message () const { return mMessage; }

  int number () const { return mPlayerNo; }

  eGameBid game () const { return m_game; }
  void setGame (eGameBid game) { m_game = game; }

  /// @todo Move to shared score board
  int tricksTaken () const { Q_ASSERT(mTricksTaken >= 0); return mTricksTaken; }
  void gotTrick () { mTricksTaken++; }
  void gotPassPassTricks (int cnt) { mTricksTaken = cnt; }

protected:
  virtual void internalInit ();

  virtual void drawAt (int left, int top, int selNo=-1);
  virtual void clearCardArea ();

  // at least 28 ints (14 int pairs); return # of used ints; the last int is -1
  // result: ofs, cardNo, ..., -1
  virtual int buildHandXOfs (int *dest, int startX, bool opened);
  virtual int cardAt (int lx, int ly, bool opened=true);

  bool isValidMove(const Card *move, const Card *lMove, const Card *rMove, const int trump);

public:
  ScoreBoard mScore;
  CardList mCards; // my cards
  CardList mCardsOut; // во взятках мои
  // this part for miser catches
  Card *mCardCarryThru;

  int mClickX, mClickY;
  bool mWaitingForClick;

protected:
  DeskView *mDeskView;
  PrefModel *m_model;
  QString m_nick;
  QString mMessage;
  int mPlayerNo; // my number
  bool mInvisibleHand;
  eGameBid m_game;
  int mTricksTaken;
  int mPrevHiCardIdx;
  bool mIStart;
};


#endif
