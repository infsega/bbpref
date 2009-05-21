#ifndef HUMAN_G_H
#define HUMAN_G_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "gamer.h"


class THuman :public TGamer {
public:
  THuman (int _nGamer);
  THuman (int _nGamer, TDeskView *_DeskView);
  tGameBid makeout4game (); // ����� ����� ���� ������
  tGameBid makeout4miser ();
  TCard *makemove (TCard *lMove, TCard *rMove, TGamer *aLeftGamer, TGamer *aRightGamer); //���
  tGameBid makemove (tGameBid lMove, tGameBid rMove); //��� ��� ��������
  tGameBid makemove (tGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  void HintCard (int lx, int ly);
};


#endif
