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


HumanPlayer::HumanPlayer(int _nGamer) : Player(_nGamer) {
}


HumanPlayer::HumanPlayer(int _nGamer,TDeskView *_DeskView) : Player(_nGamer, _DeskView) {
}


//��� ��� ��������
tGameBid HumanPlayer::makemove (tGameBid lMove, tGameBid rMove) {
  tGameBid tmpGamesType;
  WaitForMouse = 1;

  formBid->EnableAll();
  if (qMax(lMove, rMove) != gtPass) formBid->DisableLessThan(qMax(lMove, rMove));
  if (GamesType != undefined) formBid->DisalbeGames(g86);
  formBid->DisalbeGames(vist);
  formBid->EnableGames(gtPass);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(FALSE);

  do {
    tmpGamesType = DeskView->makemove(lMove, rMove);
    if (tmpGamesType == 0) {
      // ������� ���� (���� ����) � ������ ������
      GetBackSnos();
      makemove(0, 0, 0, 0);
      DeskView->mySleep(1);
      makemove(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // �������� ����
      kpref->DeskTop->nflShowPaper = 1;
      kpref->slotShowBollet();
    }
  } while (tmpGamesType <= 1);
  GamesType = tmpGamesType;
  WaitForMouse = 0;
  formBid->EnableAll();
  return GamesType;
}


//���
TCard *HumanPlayer::makemove (TCard *lMove, TCard *rMove, Player *aLeftGamer, Player *aRightGamer) {
  Q_UNUSED(aLeftGamer)
  Q_UNUSED(aRightGamer)

  TCard *RetVal = 0;
  WaitForMouse = 1;
  RepaintSimple(true);
  while (!RetVal) {
    int cNo = cardAt(X, Y);
    if (cNo == -1) {
      RepaintSimple(true);
      if (DeskView) DeskView->mySleep(0); // just a little pause
    }
    TCard *Validator;
    int koz = nGetKoz();
    Validator = RetVal = (TCard *)aCards->At(cNo);
    if (lMove || rMove) {
      Validator = lMove ? lMove : rMove;
    }
    // ���������� ��� ����� �������
    if (!((Validator->CMast == RetVal->CMast) ||
        (!aCards->MinCard(Validator->CMast) && (RetVal->CMast == koz || ((!aCards->MinCard(koz)))))
       )) RetVal = NULL;
  }
  aCards->Remove(RetVal);
  aCardsOut->Insert(RetVal);
  X = Y = 0;
  WaitForMouse = 0;
  RepaintSimple(true);
  return RetVal;
}


// ����� ����� ���� ������
tGameBid HumanPlayer::makeout4miser () {
  WaitForMouse = 1;
  makemove(0, 0, 0, 0);
  DeskView->mySleep(1);
  makemove(0, 0, 0, 0);
  WaitForMouse = 0;
  return g86;
}


// ����� ����� ���� ������
tGameBid HumanPlayer::makeout4game () {
  WaitForMouse = 1;
  tGameBid tmpGamesType;
  X = Y = 0;
  makemove(0, 0, 0, 0);
  DeskView->mySleep(1);
  makemove(0, 0, 0, 0);
  formBid->EnableAll();
  formBid->DisalbeGames(vist);
  formBid->DisalbeGames(gtPass);
  if (GamesType != g86) formBid->DisalbeGames(g86);
  formBid->DisableLessThan( GamesType);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(TRUE);

  do {
    tmpGamesType = DeskView->makemove(zerogame, zerogame);
    if (tmpGamesType == 0) {
      // ������� ����
      GetBackSnos();
      makemove(0, 0, 0, 0);
      DeskView->mySleep(1);
      makemove(0, 0, 0, 0);
    } else if (tmpGamesType == 1) {
      // �������� ����
      kpref->slotShowBollet();
    }
  } while (tmpGamesType <= 1);
  GamesType = tmpGamesType;
  formBid->EnableAll();
  WaitForMouse = 0;
  return GamesType;
}


tGameBid HumanPlayer::makemove (tGameBid MaxGame, int HaveAVist, int nGamerVist) {
  Q_UNUSED(HaveAVist)
  Q_UNUSED(nGamerVist)
  if (MaxGame == g86) {
    GamesType = g86catch;
  } else {
    formBid->DisalbeAll();
    formBid->EnableGames(gtPass);
    formBid->EnableGames(vist);
    GamesType = DeskView->makemove(zerogame, zerogame);
    formBid->EnableAll();
  }
  return GamesType;
}


void HumanPlayer::HintCard (int lx, int ly) {
  if (!WaitForMouse) {
    // not in "card selection" mode
    if (oldii == -1) return; // nothing selected --> nothing to redraw
    oldii = -1;
  } else {
    int cNo = cardAt(lx, ly);
    if (cNo == oldii) return; // same selected --> nothing to redraw
    oldii = cNo;
  }
  RepaintSimple(true);
}
