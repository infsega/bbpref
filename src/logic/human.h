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

  eGameBid dropForGame (); // ����� ����� ���� ������
  eGameBid dropForMisere ();

  Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer); //���
  eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //��� ��� ��������
  eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����

  virtual void hilightCard (int lx, int ly);
  virtual void clear ();

  HumanPlayer &operator = (const Player &pl);
  HumanPlayer &operator = (const HumanPlayer &pl);
};


#endif
