#ifndef HUMAN_G_H
#define HUMAN_G_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "gamer.h"
//------------------------------------------------------------
class THuman :public TGamer {
  public:
    THuman(int );
    THuman(int ,TDeskView *);
    TGamesType makeout4game(void); // ����� ����� ���� ������
    TGamesType makeout4miser(void);
    TCard *makemove(TCard *lMove,TCard *rMove,TGamer *aLeftGamer,TGamer *aRightGamer); //���
    TGamesType makemove(TGamesType lMove,TGamesType rMove); //��� ��� ��������
    TGamesType makemove(TGamesType MaxGame,int HaveAVist,int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
    void HintCard(int,int);
};
//------------------------------------------------------------
#endif
