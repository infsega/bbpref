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

#include <QDebug>

#include <stdlib.h>
#include <stdio.h>

#include "prfconst.h"

#include "formbid.h"
#include "kpref.h"


#include "deskview.h"
#include "player.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "human.h"


HumanPlayer::HumanPlayer (int aMyNumber, DeskView *aDeskView) : Player(aMyNumber, aDeskView) {
  internalInit();
}


HumanPlayer &HumanPlayer::operator = (const Player &pl) {
  Player::clone(&pl);
  return *this;
}


HumanPlayer &HumanPlayer::operator = (const HumanPlayer &pl) {
  Player::clone(&pl);
  return *this;
}


void HumanPlayer::clear () {
  Player::clear();
  mInvisibleHand = false;
}


// после сноса чего играем
eGameBid HumanPlayer::dropForMisere () {
  mClickX = mClickY = 0; mWaitingForClick = true;
  moveSelectCard(0, 0, 0, 0);
  //mDeskView->mySleep(1);
  moveSelectCard(0, 0, 0, 0);
  mWaitingForClick = false;
  return g86;
}


// после сноса чего играем
eGameBid HumanPlayer::dropForGame () {
  //mWaitingForClick = 1;
  //mClickX = mClickY = 0;
  eGameBid tmpGamesType;

  moveSelectCard(0, 0, 0, 0);
  //!.!mDeskView->mySleep(1);
  moveSelectCard(0, 0, 0, 0);

  formBid->enableAll();
  formBid->disableItem(whist);
  //formBid->disableItem(halfwhist);
  formBid->bhalfvist->setEnabled(false);
  formBid->disableItem(gtPass);
  if (mMyGame != g86) formBid->disableItem(g86);
  formBid->disableLessThan(mMyGame);
  formBid->showbullet->setEnabled(true);
  if (optWithoutThree)
  	formBid->bwithoutthree->setEnabled(true);

  do {
    tmpGamesType = mDeskView->selectBid(zerogame, zerogame);
    if (tmpGamesType == 0) {
      // вернуть снос
      clearCardArea();
      returnDrop();
      moveSelectCard(0, 0, 0, 0);
      //mDeskView->mySleep(1);
      moveSelectCard(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // показать пулю
      kpref->slotShowScore();
    }
  } while (tmpGamesType <= 1);
  if ( tmpGamesType != withoutThree)
  	mMyGame = tmpGamesType;

  formBid->enableAll();
  mWaitingForClick = false;
  return tmpGamesType;
}


// ход при торговле
eGameBid HumanPlayer::moveBidding (eGameBid lMove, eGameBid rMove) {
  eGameBid tmpGamesType;
  mClickX = mClickY = 0; mWaitingForClick = true;

  //fprintf(stderr, "select bid\n");
  formBid->enableAll();
  /*if (optAggPass && optPassCount > 0) {
    if (lMove == undefined) lMove = g71;
    if (rMove == undefined) rMove = g71;
  }*/
  if (optAggPass && optPassCount > 0)
	  formBid->disableLessThan(g71);
  
  if (qMax(lMove, rMove) != gtPass)
  {
  	// If HumanPlayer started bid, he can stay
	//mIStart = (lMove == undefined && rMove == undefined);
	
	if (mIStart)
		formBid->disableLessThan(qMax(lMove, rMove));
  	// otherwise he must increase
	else
   		formBid->disableLessThan(qMax((eGameBid)succBid(lMove), (eGameBid)succBid(rMove)));
  }
  if (mMyGame != undefined)
    formBid->disableItem(g86);
  formBid->enableItem(gtPass);
  formBid->disableItem(whist);
  //formBid->disableItem(halfwhist);
  formBid->bhalfvist->setEnabled(false);
  formBid->showbullet->setEnabled(true);
  formBid->bwithoutthree->setEnabled(false);
  do {
    tmpGamesType = mDeskView->selectBid(lMove, rMove);
    if (tmpGamesType == 0) {
      // вернуть снос (если есть) и снести заново
      clearCardArea();
      returnDrop();
      moveSelectCard(0, 0, 0, 0);
      //mDeskView->mySleep(1);
      moveSelectCard(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // show pool
      kpref->mDesktop->mShowPool = true;
      kpref->slotShowScore();
    }
  } while (tmpGamesType <= 1);
  mMyGame = tmpGamesType;
  mWaitingForClick = false;
  formBid->enableAll();
  return mMyGame;
}


//move
Card *HumanPlayer::moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) {
  Q_UNUSED(aLeftPlayer)
  Q_UNUSED(aRightPlayer)
  Q_UNUSED(isPassOut)
  printf ("HumanPlayer (%d) moves\n", mPlayerNo);
  Card *res = 0;
  mClickX = mClickY = 0; mWaitingForClick = true;
  kpref->HintMove();
  draw();
  while (!res) {
    if (mDeskView) mDeskView->mySleep(-2);
    int cNo = cardAt(mClickX, mClickY, !invisibleHand());
    if (cNo == -1) {
      mClickX = mClickY = 0;
      //draw();
      continue;
    }
    //qDebug() << "selected:" << cNo << "mClickX:" << mClickX << "mClickY:" << mClickY;
    Card *Validator;
    int koz = trumpSuit();
    Validator = res = mCards.at(cNo);
    if (lMove || rMove) {
      Validator = lMove ? lMove : rMove;
    }
    if (!Validator || !res) continue;
    // check if move accords with rules
    if (!((Validator->suit() == res->suit()) ||
        (!mCards.minInSuit(Validator->suit()) && (res->suit() == koz || ((!mCards.minInSuit(koz)))))
       )) {
		kpref->MoveImpossible(); 
		res = 0;
	}
  }
  clearCardArea();
  mPrevHiCardIdx = -1;
  mCards.remove(res);
  mCardsOut.insert(res);
  mClickX = mClickY = 0; mWaitingForClick = false;
  kpref->HintBar->clearMessage();
  draw();
  return res;
}


eGameBid HumanPlayer::moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerPass) {
  Q_UNUSED(HaveAVist)
  //Q_UNUSED(nGamerVist)

  //fprintf(stderr, "whist/pass\n");
  if (MaxGame == g86) {
    mMyGame = g86catch;
  } else if (!opt10Whist && MaxGame>=101 && MaxGame<=105) {
	mMyGame = whist;
  } else {
    // Stalingrad?
    formBid->disableAll();
    if (optStalingrad && MaxGame == g61) formBid->disableItem(gtPass);
    else formBid->enableItem(gtPass);
	
    //formBid->enableItem(gtPass);
    formBid->enableItem(whist);
	if (nGamerPass == 1)
		formBid->bhalfvist->setEnabled(true);
    mMyGame = mDeskView->selectBid(zerogame, zerogame);
    formBid->enableAll();
  }
  return mMyGame;
}


void HumanPlayer::hilightCard (int lx, int ly) {
  //qDebug() << "lx:" << lx << "ly:" << ly;
  if (!mWaitingForClick) {
    // not in "card selection" mode
    if (mPrevHiCardIdx == -1) return; // nothing selected --> nothing to redraw
    mPrevHiCardIdx = -1;
  } else {
    int cNo = cardAt(lx, ly, !invisibleHand());
    if (cNo == mPrevHiCardIdx) return; // same selected --> nothing to redraw
    //qDebug() << "mPrevHiCardIdx:" << mPrevHiCardIdx << "cNo:" << cNo;
    mPrevHiCardIdx = cNo;
  }
  draw();
  kpref->HintBar->clearMessage();
}

bool HumanPlayer::chooseClosedWhist () {
	return kpref->WhistType();
}
