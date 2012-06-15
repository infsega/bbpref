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

#pragma once

#include "prfconst.h"
#include "cardlist.h"
#include "scoreboard.h"

#include <QList>

class DeskView;

/**
 * @class Player player.h
 * @brief Base class for all players
 *
 */
class Player
{
public:
  Player (int number, PrefModel *model);
  virtual ~Player ();

  /// String identifier of concrete player
  virtual const QString type() const = 0;
  /// @return true if this player object interacts with GUI user
  virtual bool isHuman() const { return false; }

  /// Factory method, creates player of the same subclass as current
  virtual Player * create(int number, PrefModel *model) = 0;

  virtual void clear ();

  virtual bool invisibleHand () const;
  virtual void setInvisibleHand (bool invis);

  virtual void setCurrentStart (bool start);

  void sortCards() { mCards.mySort(); }

  void dealCard(Card *aCard) { mCards.insert(aCard); }
  void takeDeskCard(Card *aCard)
  {
    mCards.insert(aCard);
    mDealedCards.append(aCard);
  }

  /// Player's decision on move in the game
  virtual Card *makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) = 0;
  /// Player's decision on move in the bidding
  virtual eGameBid makeBid (eGameBid lMove, eGameBid rMove) = 0;

  virtual eGameBid makeFinalBid (eGameBid MaxGame, int nPlayerPass) = 0;
  virtual eGameBid makeDrop() = 0;
  virtual void returnDrop();

  /// Player's decision on open or closed whist
  virtual bool chooseClosedWhist () = 0;

  /// @todo Move to view delegate
  virtual void draw();
  virtual void highlightCard (int lx, int ly);
  void getCardPos(const Card *card, int& x, int& y);

  void setNick (const QString nick) { m_nick = nick; }
  QString nick() const { return m_nick; }

  void setMessage (const QString msg) { mMessage = msg; }
  void setThinking();
  void clearMessage();
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

  virtual void drawAt (int left, int right, int top, int selNo=-1);
  virtual void clearCardArea ();

  struct CardPosInfo
  {
    CardPosInfo() {}
    CardPosInfo(int i_idx, int i_offset): idx(i_idx), offset(i_offset) {}
    int idx;
    int offset;
  };

  int buildHandXOfs (QList<CardPosInfo>& offsets, int startX, int endX, bool opened);
  virtual int cardAt(int lx, int ly, bool opened = true);
  bool isValidMove(const Card *move, const Card *lMove, const Card *rMove, const int trump);

public:
  ScoreBoard mScore;
  CardList mCards; // my cards
  CardList mCardsOut;
  QCardList mDealedCards;
  // this part for miser catches
  Card *mCardCarryThru;

  int mClickX, mClickY;
  bool mWaitingForClick;

protected:
  DeskView *mDeskView;
  PrefModel *m_model;
  QString m_nick;
  QString mMessage;
  bool mIStart;
  int mPlayerNo; // my number
  bool mInvisibleHand;
  eGameBid m_game;
  int mTricksTaken;
  int mPrevHiCardIdx;
};
