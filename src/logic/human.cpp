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


//ход при торговле
eGameBid HumanPlayer::makemove (eGameBid lMove, eGameBid rMove) {
  eGameBid tmpGamesType;
  WaitForMouse = 1;

  formBid->EnableAll();
  if (qMax(lMove, rMove) != gtPass) formBid->DisableLessThan(qMax(lMove, rMove));
  if (GamesType != undefined) formBid->disableGames(g86);
  formBid->disableGames(vist);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(FALSE);

  do {
    tmpGamesType = DeskView->makemove(lMove, rMove);
    if (tmpGamesType == 0) {
      // вернуть снос (если есть) и снести заново
      GetBackSnos();
      makemove(0, 0, 0, 0);
      DeskView->mySleep(1);
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
    Validator = RetVal = (Card *)aCards->At(cNo);
    if (lMove || rMove) {
      Validator = lMove ? lMove : rMove;
    }
    if (!Validator || !RetVal) continue;
    // пропускаем ход через правила
    if (!((Validator->CMast == RetVal->CMast) ||
        (!aCards->MinCard(Validator->CMast) && (RetVal->CMast == koz || ((!aCards->MinCard(koz)))))
       )) RetVal = NULL;
  }
  clearCardArea();
  oldii = -1;
  aCards->Remove(RetVal);
  aCardsOut->Insert(RetVal);
  X = Y = 0;
  WaitForMouse = 0;
  Repaint();
  return RetVal;
}


// после сноса чего играем
eGameBid HumanPlayer::makeout4miser () {
  WaitForMouse = 1;
  makemove(0, 0, 0, 0);
  DeskView->mySleep(1);
  makemove(0, 0, 0, 0);
  WaitForMouse = 0;
  return g86;
}


// после сноса чего играем
eGameBid HumanPlayer::makeout4game () {
  WaitForMouse = 1;
  eGameBid tmpGamesType;
  X = Y = 0;
  makemove(0, 0, 0, 0);
  DeskView->mySleep(1);
  makemove(0, 0, 0, 0);
  formBid->EnableAll();
  formBid->disableGames(vist);
  formBid->disableGames(gtPass);
  if (GamesType != g86) formBid->disableGames(g86);
  formBid->DisableLessThan(GamesType);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(TRUE);

  do {
    tmpGamesType = DeskView->makemove(zerogame, zerogame);
    if (tmpGamesType == 0) {
      // вернуть снос
      GetBackSnos();
      makemove(0, 0, 0, 0);
      DeskView->mySleep(1);
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
  if (MaxGame == g86) {
    GamesType = g86catch;
  } else {
    // сталинград?
    if (g61stalingrad && MaxGame == g61) formBid->disableGames(gtPass);
    else formBid->EnableGames(gtPass);
    formBid->DisalbeAll();
    formBid->EnableGames(gtPass);
    formBid->EnableGames(vist);
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
