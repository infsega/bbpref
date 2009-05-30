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

  HumanPlayer &operator = (const Player &pl);
  HumanPlayer &operator = (const HumanPlayer &pl);

  virtual void clear ();

  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut=false); //���
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //��� ��� ��������
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  virtual eGameBid dropForGame (); // ����� ����� ���� ������
  virtual eGameBid dropForMisere ();

  virtual void hilightCard (int lx, int ly);
};


#endif
