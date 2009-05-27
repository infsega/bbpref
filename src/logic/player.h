#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"


class Player {
public:
  DeskView *mDeskView;

  ScoreBoard mScore;
  CardList mCards; // ���
  CardList mLeft;  // ���������� � ���� (�������������� ��� ��������)
  CardList mRight; // � ����� (�������������� ��� ��������)

  CardList mOut;   // ���� (��� ��� ��������������)
  CardList mCardsOut; // �� ������� ���
  //CardList mLeftOut;  // �� ������� ���������� ����� (�������������� ��� ��������): unused
  //CardList mRightOut; // �� ������� ������ (�������������� ��� ��������)
  tSuitProbs mSuitProb[5];

  int mTricksTaken;
  //eSuit Mast;
  eGameBid mMyGame;
  //eHand mEnemy;
  int mPlayerNo; // ��� �����

  Player (int aMyNumber, DeskView *aDeskView=0);
  Player (const Player &pl);
  virtual ~Player ();

  // at least 28 ints (14 int pairs); return # of used ints; the last int is -1
  // result: ofs, cardNo, ..., -1
  int buildHandXOfs (int *dest, int startX, bool opened);
  int cardAt (int lx, int ly, bool opened=true);
  void getLeftTop (int *left, int *top);

  void sortCards ();

  Player &operator = (const Player &pl);

  virtual bool isInvisibleHand ();

public:
  QString mNick;

  int mClickX, mClickY;
  bool mWaitingForClick;
  bool mInvisibleHand;
  // this part for miser catchs
  Card *mCardCarryThru;

public:
  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer); //���
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //��� ��� ��������
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  virtual eGameBid dropForGame ();
  virtual eGameBid dropForMisere ();

  virtual void hilightCard (int lx, int ly);
  virtual void returnDrop ();

  virtual void dealCard (Card *aCard); // �������� ������� �����

  virtual void clear ();

  void draw ();

  inline void setMessage (const QString &msg) { mMessage = msg; }
  inline const QString &message () const { return mMessage; }

  inline int number () const { return mPlayerNo; }

protected:
  int mPrevHiCardIdx;
  //int flMiser;
  QString mMessage;

protected:
  void drawAt (DeskView *aDeskView, int left, int top, int selNo=-1);
  void clearCardArea ();

  int checkForMisere ();
  eGameBid moveCalcDrop (); //��� ������� �����
  //void makestatfill ();
  //void makestatfill (int nCards,int maxmin);
  tSuitProbs vzatok4game (eSuit, int a23);
  tSuitProbs vzatok (eSuit, CardList &, int a23);
  tSuitProbs vzatok4pass (eSuit, CardList &);

public:
  // ��� ������
  tSuitProbs compare2List4Max (CardList &my, CardList &enemy); // ��� ������������� ���������� �� 1 ����
  tSuitProbs compare2List4Max23 (CardList &my, CardList &enemy); // ��� ������������� ���������� �� 2 � 3 ����
  tSuitProbs compare2List4Min (CardList &my, CardList &enemy); // ��� ��� ����������

protected:
  void internalInit ();
  void clone (const Player *pl);

  void recalcTables (CardList &aMaxCardList, int a23); // ������������� ������� tSuitProbs mSuitProb[5];
  void recalcPassOutTables (CardList &aMaxCardList, int a23); // ������������� ������� ��� �������� ��� ������

  void loadLists (Player *aLeftPlayer, Player *aRightPlayer, CardList &aMaxCardList); // ����� �������

  // ��� ������
  //tSuitProbs Compare3List4Max(CardList *my,CardList *Left,CardList *Right); // ��� ������������� ����������
  //tSuitProbs Compare3List4Min(CardList *my,CardList *Left,CardList *Right); // ��� ��� ����������
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
};


#endif
