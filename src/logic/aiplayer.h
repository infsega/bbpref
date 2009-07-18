#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "player.h"
#include "plscore.h"
#include "deskview.h"


class AiPlayer : public Player {
public:
  AiPlayer (int aMyNumber, bool iStart=false, DeskView *aDeskView=0);

public:
  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut); //���
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //��� ��� ��������
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  virtual eGameBid dropForGame ();
  virtual eGameBid dropForMisere ();

  virtual void clear ();

protected:
  int checkForMisere ();
  eGameBid moveCalcDrop (); //��� ������� �����
  tSuitProbs countGameTricks (eSuit, int a23);
  tSuitProbs countTricks (eSuit, CardList &, int a23);
  tSuitProbs countPassTricks (eSuit, CardList &);

  // ��� ������
  tSuitProbs calcProbsForMax (CardList &my, CardList &enemy); // ��� ������������� ���������� �� 1 ����
  tSuitProbs calcProbsForMaxH23 (CardList &my, CardList &enemy); // ��� ������������� ���������� �� 2 � 3 ����
  tSuitProbs calcProbsForMin (CardList &my, CardList &enemy); // ��� ��� ����������

  void recalcTables (CardList &aMaxCardList, int a23); // ������������� ������� tSuitProbs mSuitProb[5];
  void recalcPassOutTables (CardList &aMaxCardList, int a23); // ������������� ������� ��� �������� ��� ������

  void loadLists (Player *aLeftPlayer, Player *aRightPlayer, CardList &aMaxCardList); // ����� �������

  Card *GetMaxCardPere ();
  Card *GetMaxCardWithOutPere ();
  Card *GetMinCardWithOutVz ();

  Card *Miser1 (Player *aLeftPlayer, Player *aRightPlayer);
  Card *Miser2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
  Card *Miser3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);

  Card *MyGame1 (Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� 1 ����� - ���
  Card *MyGame2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� 2 ����� - ���
  Card *MyGame3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� 3 ����� - ���

  Card *MyVist1 (Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� ��� ��� 1 ����� - ���
  Card *MyVist2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� ��� ��� 2 ����� - ���
  Card *MyVist3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� ��� ��� 3 ����� - ���

  // � ��� � ����� ���������� �� ������� � ����� !!!
  Card *MyPass1 (Card *, Player *aLeftPlayer, Player *aRightPlayer);
  Card *MyPass2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� 2 ����� - ���
  Card *MyPass3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // ��� ���� 3 ����� - ���

  Card *MiserCatch1 (Player *aLeftPlayer, Player *aRightPlayer);
  Card *MiserCatch2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
  Card *MiserCatch3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);

protected:
  tSuitProbs mSuitProb[5];
};


#endif
