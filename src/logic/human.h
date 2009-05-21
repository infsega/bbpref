#ifndef HUMAN_H
#define HUMAN_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "player.h"


class HumanPlayer : public Player {
public:
  HumanPlayer (int _nGamer);
  HumanPlayer (int _nGamer, TDeskView *_DeskView);
  eGameBid makeout4game (); // ����� ����� ���� ������
  eGameBid makeout4miser ();
  Card *makemove (Card *lMove, Card *rMove, Player *aLeftGamer, Player *aRightGamer); //���
  eGameBid makemove (eGameBid lMove, eGameBid rMove); //��� ��� ��������
  eGameBid makemove (eGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����

  virtual void HintCard (int lx, int ly);
  virtual void clear ();
};


#endif
