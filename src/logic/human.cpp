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
}


void HumanPlayer::clear () {
  Player::clear();
  mInvisibleHand = false;
}


// ход при торговле
eGameBid HumanPlayer::makemove (eGameBid lMove, eGameBid rMove) {
  eGameBid tmpGamesType;
  mClickX = mClickY = 0; mWaitingForClick = true;

  //fprintf(stderr, "select bid\n");
  formBid->EnableAll();
  if (qMax(lMove, rMove) != gtPass) formBid->DisableLessThan(qMax(lMove, rMove));
  if (mMyGame != undefined) formBid->disableItem(g86);
  formBid->enableItem(gtPass);
  formBid->disableItem(vist);
  formBid->showbullet->setEnabled(true);
  formBid->bgetback->setEnabled(false);
  do {
    tmpGamesType = mDeskView->makemove(lMove, rMove);
    if (tmpGamesType == 0) {
      // вернуть снос (если есть) и снести заново
      clearCardArea();
      returnDrop();
      makemove(0, 0, 0, 0);
      //mDeskView->mySleep(1);
      makemove(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // показать пулю
      kpref->DeskTop->nflShowPaper = 1;
      kpref->slotShowScore();
    }
  } while (tmpGamesType <= 1);
  mMyGame = tmpGamesType;
  mWaitingForClick = false;
  formBid->EnableAll();
  return mMyGame;
}


//ход
Card *HumanPlayer::makemove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer) {
  Q_UNUSED(aLeftPlayer)
  Q_UNUSED(aRightPlayer)
  Card *RetVal = 0;
  mClickX = mClickY = 0; mWaitingForClick = true;
  draw();
  while (!RetVal) {
    if (mDeskView) mDeskView->mySleep(0); // just a little pause
    int cNo = cardAt(mClickX, mClickY, !mInvisibleHand);
    if (cNo == -1) {
      mClickX = mClickY = 0;
      //draw();
      continue;
    }
    //qDebug() << "selected:" << cNo << "mClickX:" << mClickX << "mClickY:" << mClickY;
    Card *Validator;
    int koz = trumpSuit();
    Validator = RetVal = mCards.at(cNo);
    if (lMove || rMove) {
      Validator = lMove ? lMove : rMove;
    }
    if (!Validator || !RetVal) continue;
    // пропускаем ход через правила
    if (!((Validator->suit() == RetVal->suit()) ||
        (!mCards.minInSuit(Validator->suit()) && (RetVal->suit() == koz || ((!mCards.minInSuit(koz)))))
       )) RetVal = 0;
  }
  clearCardArea();
  mPrevHiCardIdx = -1;
  mCards.remove(RetVal);
  mCardsOut.insert(RetVal);
  mClickX = mClickY = 0; mWaitingForClick = false;
  draw();
  return RetVal;
}


// после сноса чего играем
eGameBid HumanPlayer::makeout4miser () {
  mClickX = mClickY = 0; mWaitingForClick = true;
  makemove(0, 0, 0, 0);
  //mDeskView->mySleep(1);
  makemove(0, 0, 0, 0);
  mWaitingForClick = false;
  return g86;
}


// после сноса чего играем
eGameBid HumanPlayer::makeout4game () {
  //mWaitingForClick = 1;
  //mClickX = mClickY = 0;
  eGameBid tmpGamesType;

  makemove(0, 0, 0, 0);
  //!.!mDeskView->mySleep(1);
  makemove(0, 0, 0, 0);

  formBid->EnableAll();
  formBid->disableItem(vist);
  formBid->disableItem(gtPass);
  if (mMyGame != g86) formBid->disableItem(g86);
  formBid->DisableLessThan(mMyGame);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(TRUE);

  do {
    tmpGamesType = mDeskView->makemove(zerogame, zerogame);
    if (tmpGamesType == 0) {
      // вернуть снос
      clearCardArea();
      returnDrop();
      makemove(0, 0, 0, 0);
      //mDeskView->mySleep(1);
      makemove(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // показать пулю
      kpref->slotShowScore();
    }
  } while (tmpGamesType <= 1);
  mMyGame = tmpGamesType;

  formBid->EnableAll();
  mWaitingForClick = false;
  return mMyGame;
}


eGameBid HumanPlayer::makemove (eGameBid MaxGame, int HaveAVist, int nGamerVist) {
  Q_UNUSED(HaveAVist)
  Q_UNUSED(nGamerVist)

  //fprintf(stderr, "whist/pass\n");
  if (MaxGame == g86) {
    mMyGame = g86catch;
  } else {
    // сталинград?
    if (g61stalingrad && MaxGame == g61) formBid->disableItem(gtPass);
    else formBid->enableItem(gtPass);
    formBid->DisalbeAll();
    formBid->enableItem(gtPass);
    formBid->enableItem(vist);
    mMyGame = mDeskView->makemove(zerogame, zerogame);
    formBid->EnableAll();
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
    int cNo = cardAt(lx, ly, !mInvisibleHand);
    if (cNo == mPrevHiCardIdx) return; // same selected --> nothing to redraw
    //qDebug() << "mPrevHiCardIdx:" << mPrevHiCardIdx << "cNo:" << cNo;
    mPrevHiCardIdx = cNo;
  }
  draw();
}
