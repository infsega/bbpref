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
  HumanPlayer (int aMyNumber, DeskView *aDeskView=0);
  eGameBid makeout4game (); // ����� ����� ���� ������
  eGameBid makeout4miser ();
  Card *makemove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer); //���
  eGameBid makemove (eGameBid lMove, eGameBid rMove); //��� ��� ��������
  eGameBid makemove (eGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����

  virtual void hilightCard (int lx, int ly);
  virtual void clear ();
};


#endif
