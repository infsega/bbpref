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

#include <QDebug>

#include "prfconst.h"
#include "formbid.h"
#include "human.h"


HumanPlayer::HumanPlayer (int aMyNumber, PrefModel *model) : Player(aMyNumber, model) {
  internalInit();
}

Player * HumanPlayer::create(int aMyNumber, PrefModel *model) {
  Player * pl = new HumanPlayer(aMyNumber, model);
  return pl;
}


void HumanPlayer::clear () {
  Player::clear();
  mInvisibleHand = false;
}


// после сноса чего играем
eGameBid HumanPlayer::dropForMisere () {
  mClickX = mClickY = 0; mWaitingForClick = true;
  makeMove(0, 0, 0, 0);
  makeMove(0, 0, 0, 0);
  mWaitingForClick = false;
  return g86;
}


// после сноса чего играем
eGameBid HumanPlayer::dropForGame ()
{
  bool confirm;

  makeMove(0, 0, 0, 0);
  makeMove(0, 0, 0, 0);

  confirm = mDeskView->askConfirmDrop();
  while(!confirm) {
    clearCardArea();
    returnDrop();
    makeMove(0, 0, 0, 0);
    makeMove(0, 0, 0, 0);
    confirm = mDeskView->askConfirmDrop();
  }
  // Confirmed - remove dropped cards
  mCards.clearNulls();
  mCardsOut.clear();

  BidDialog *bidDialog = BidDialog::instance();
  bidDialog->disableLessThan(m_game);
  BidDialog::ActiveButtons buttons = BidDialog::Score;
  if (m_model->optWithoutThree)
    buttons |= BidDialog::WithoutThree;

  eGameBid tmpGamesType = mDeskView->selectBid(buttons);
  if ( tmpGamesType != withoutThree)
    m_game = tmpGamesType;

  mWaitingForClick = false;
  return tmpGamesType;
}


// ход при торговле
eGameBid HumanPlayer::makeBid (eGameBid lMove, eGameBid rMove)
{
  mClickX = mClickY = 0;
  mWaitingForClick = true;
  BidDialog *bidDialog = BidDialog::instance();
  bidDialog->enableBids();
  
  if (m_model->optAggPass && m_model->optPassCount > 0)
    bidDialog->disableLessThan(g71);
  
  if (qMax(lMove, rMove) != gtPass)
  {
    if (mIStart)
      bidDialog->disableLessThan(qMax(lMove, rMove));
    // otherwise he must increase
    else
      bidDialog->disableLessThan(qMax((eGameBid)succBid(lMove), (eGameBid)succBid(rMove)));
  }
  BidDialog::ActiveButtons buttons = BidDialog::Pass | BidDialog::Score;
  if (m_game == undefined)
    buttons |= BidDialog::Misere;

  m_game = mDeskView->selectBid(buttons);
  mWaitingForClick = false;
  return m_game;
}


//move
Card *HumanPlayer::makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) {
  Q_UNUSED(aLeftPlayer)
  Q_UNUSED(aRightPlayer)
  Q_UNUSED(isPassOut)
  Q_ASSERT(mDeskView);
  qDebug() << type() << "("<< mPlayerNo << ") moves";
  Card *res = 0;
  mClickX = mClickY = 0; mWaitingForClick = true;
  m_model->showMoveHint();
  draw();
  int cNo = -1;
  while (!res) {
    mDeskView->mySleep(-2);
    cNo = cardAt(mClickX, mClickY, !invisibleHand());
    if (cNo == -1) {
      mClickX = mClickY = 0;
      continue;
    }
    const int koz = mDeskView->model()->trumpSuit();
    res = mCards.at(cNo);
    // check if move accords with rules
    if (!isValidMove(res, lMove, rMove, koz)) {
      res = 0;
    }
  }
  clearCardArea();
  mPrevHiCardIdx = -1;
  mCards.remove(res);
  mCardsOut.insert(res);
  mClickX = mClickY = 0; mWaitingForClick = false;
  m_model->emitClearHint();
  draw();
  return res;
}


eGameBid HumanPlayer::makeFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerPass)
{
  Q_UNUSED(HaveAVist)

  BidDialog *bidDialog = BidDialog::instance();

  if (MaxGame == g86) {
    m_game = g86catch;
  } else if ((!m_model->opt10Whist && MaxGame>=101 && MaxGame<=105)
      || (m_model->optStalingrad && MaxGame == g61)) {
	m_game = whist;
  } else {
    bidDialog->disableBids();
    BidDialog::ActiveButtons buttons = BidDialog::Pass | BidDialog::Whist | BidDialog::Score;
    if (nGamerPass == 1 && MaxGame < 81)
        buttons |= BidDialog::HalfWhist;
    m_game = mDeskView->selectBid(buttons);
    bidDialog->enableBids();
  }
  return m_game;
}


void HumanPlayer::highlightCard (int lx, int ly)
{
  if (!mWaitingForClick) {
    // not in "card selection" mode
    if (mPrevHiCardIdx == -1) return; // nothing selected --> nothing to redraw
    mPrevHiCardIdx = -1;
  } else {
    int cNo = cardAt(lx, ly, !invisibleHand());
    if (cNo == mPrevHiCardIdx) return; // same selected --> nothing to redraw
    mPrevHiCardIdx = cNo;
  }
  draw();
  m_model->emitClearHint();
}

bool HumanPlayer::chooseClosedWhist () {
	return mDeskView->askWhistType();
}
