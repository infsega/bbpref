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
  tGameBid makeout4game (); // ����� ����� ���� ������
  tGameBid makeout4miser ();
  TCard *makemove (TCard *lMove, TCard *rMove, Player *aLeftGamer, Player *aRightGamer); //���
  tGameBid makemove (tGameBid lMove, tGameBid rMove); //��� ��� ��������
  tGameBid makemove (tGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  void HintCard (int lx, int ly);
};


#endif
