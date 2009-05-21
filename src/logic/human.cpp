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


HumanPlayer::HumanPlayer (int _nGamer) : Player(_nGamer) {
}


HumanPlayer::HumanPlayer (int _nGamer,TDeskView *_DeskView) : Player(_nGamer, _DeskView) {
}


void HumanPlayer::clear () {
  Player::clear();
  nInvisibleHand = false;
}


// ход при торговле
eGameBid HumanPlayer::makemove (eGameBid lMove, eGameBid rMove) {
  eGameBid tmpGamesType;
  WaitForMouse = 1;

  //fprintf(stderr, "select bid\n");
  formBid->EnableAll();
  if (qMax(lMove, rMove) != gtPass) formBid->DisableLessThan(qMax(lMove, rMove));
  if (GamesType != undefined) formBid->disableItem(g86);
  formBid->enableItem(gtPass);
  formBid->disableItem(vist);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(FALSE);

  do {
    tmpGamesType = DeskView->makemove(lMove, rMove);
    if (tmpGamesType == 0) {
      // вернуть снос (если есть) и снести заново
      clearCardArea();
      GetBackSnos();
      makemove(0, 0, 0, 0);
      //DeskView->mySleep(1);
      makemove(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // показать пулю
      kpref->DeskTop->nflShowPaper = 1;
      kpref->slotShowScore();
    }
  } while (tmpGamesType <= 1);
  GamesType = tmpGamesType;
  WaitForMouse = 0;
  formBid->EnableAll();
  return GamesType;
}


//ход
Card *HumanPlayer::makemove (Card *lMove, Card *rMove, Player *aLeftGamer, Player *aRightGamer) {
  Q_UNUSED(aLeftGamer)
  Q_UNUSED(aRightGamer)

  Card *RetVal = 0;
  X = Y = 0;
  WaitForMouse = 1;
  Repaint();
  while (!RetVal) {
    if (DeskView) DeskView->mySleep(0); // just a little pause
    int cNo = cardAt(X, Y, !nInvisibleHand);
    if (cNo == -1) {
      X = Y = 0;
      //Repaint();
      continue;
    }
    //qDebug() << "selected:" << cNo << "X:" << X << "Y:" << Y;
    Card *Validator;
    int koz = trumpSuit();
    Validator = RetVal = (Card *)aCards->at(cNo);
    if (lMove || rMove) {
      Validator = lMove ? lMove : rMove;
    }
    if (!Validator || !RetVal) continue;
    // пропускаем ход через правила
    if (!((Validator->suit() == RetVal->suit()) ||
        (!aCards->minInSuit(Validator->suit()) && (RetVal->suit() == koz || ((!aCards->minInSuit(koz)))))
       )) RetVal = NULL;
  }
  clearCardArea();
  oldii = -1;
  aCards->remove(RetVal);
  aCardsOut->append(RetVal);
  X = Y = 0;
  WaitForMouse = 0;
  Repaint();
  return RetVal;
}


// после сноса чего играем
eGameBid HumanPlayer::makeout4miser () {
  WaitForMouse = 1;
  makemove(0, 0, 0, 0);
  //DeskView->mySleep(1);
  makemove(0, 0, 0, 0);
  WaitForMouse = 0;
  return g86;
}


// после сноса чего играем
eGameBid HumanPlayer::makeout4game () {
  //WaitForMouse = 1;
  //X = Y = 0;
  eGameBid tmpGamesType;

  makemove(0, 0, 0, 0);
  //!.!DeskView->mySleep(1);
  makemove(0, 0, 0, 0);

  formBid->EnableAll();
  formBid->disableItem(vist);
  formBid->disableItem(gtPass);
  if (GamesType != g86) formBid->disableItem(g86);
  formBid->DisableLessThan(GamesType);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(TRUE);

  do {
    tmpGamesType = DeskView->makemove(zerogame, zerogame);
    if (tmpGamesType == 0) {
      // вернуть снос
      clearCardArea();
      GetBackSnos();
      makemove(0, 0, 0, 0);
      //DeskView->mySleep(1);
      makemove(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // показать пулю
      kpref->slotShowScore();
    }
  } while (tmpGamesType <= 1);
  GamesType = tmpGamesType;

  formBid->EnableAll();
  WaitForMouse = 0;
  return GamesType;
}


eGameBid HumanPlayer::makemove (eGameBid MaxGame, int HaveAVist, int nGamerVist) {
  Q_UNUSED(HaveAVist)
  Q_UNUSED(nGamerVist)

  //fprintf(stderr, "whist/pass\n");
  if (MaxGame == g86) {
    GamesType = g86catch;
  } else {
    // сталинград?
    if (g61stalingrad && MaxGame == g61) formBid->disableItem(gtPass);
    else formBid->enableItem(gtPass);
    formBid->DisalbeAll();
    formBid->enableItem(gtPass);
    formBid->enableItem(vist);
    GamesType = DeskView->makemove(zerogame, zerogame);
    formBid->EnableAll();
  }
  return GamesType;
}


void HumanPlayer::HintCard (int lx, int ly) {
  //qDebug() << "lx:" << lx << "ly:" << ly;
  if (!WaitForMouse) {
    // not in "card selection" mode
    if (oldii == -1) return; // nothing selected --> nothing to redraw
    oldii = -1;
  } else {
    int cNo = cardAt(lx, ly, !nInvisibleHand);
    if (cNo == oldii) return; // same selected --> nothing to redraw
    //qDebug() << "oldii:" << oldii << "cNo:" << cNo;
    oldii = cNo;
  }
  Repaint();
}
