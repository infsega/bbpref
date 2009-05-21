#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "deck.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"


class Player {
public:
  TDeskView *DeskView;

  TPlScore  *aScore;
  CardList *aCards; // ���
  CardList *aLeft;  // ���������� � ���� (�������������� ��� ��������)
  CardList *aRight; // � ����� (�������������� ��� ��������)

  CardList *aOut;   // ���� (��� ��� ��������������)
  CardList *aCardsOut; // �� ������� ���
  CardList *aLeftOut;  // �� ������� ���������� � ���� (�������������� ��� ��������)
  CardList *aRightOut; // �� ������� � ����� (�������������� ��� ��������)
  tSuitProbs MastTable[5];

  int nGetsCard;
  eSuit Mast;
  eGameBid GamesType;
  eHand Enemy;
  int mPlayerNo;         // ��� �����

  Player (int _nGamer);
  Player (int _nGamer, TDeskView *aDeskView);
  virtual ~Player ();

  // at least 28 ints (14 int pairs); return # of used ints; the last int is -1
  // result: ofs, cardNo, ..., -1
  int buildHandXOfs (int *dest, int startX, bool opened);
  int cardAt (int lx, int ly, bool opened=true);
  void getLeftTop (int playerNo, int *left, int *top);

  QString NikName;

  int X, Y;
  int WaitForMouse;
  int oldii;
  bool nInvisibleHand;
  // this part for miser catchs
  Card *Pronesti;

  virtual Card *makemove (Card *lMove,Card *rMove,Player *aLeftGamer,Player *aRightGamer); //���
  virtual eGameBid makemove (eGameBid lMove,eGameBid rMove); //��� ��� ��������
  virtual eGameBid makemove (eGameBid MaxGame,int HaveAVist,int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  virtual eGameBid makeout4game (void);
  virtual eGameBid makeout4miser (void);

  virtual void HintCard (int lx, int ly);
  virtual void GetBackSnos ();

  virtual void AddCard (Card *aCard); // �������� ������� �����
  virtual void AddCard (int _CName, int _CMast); // �������� ������� �����

  virtual void clear ();
  void RepaintAt (TDeskView *aDeskView, int Left, int Top, int selNo=-1);
  void Repaint ();
  void clearCardArea ();

private:
  int flMiser;
  int Check4Miser(void);
  eGameBid makemove4out(void); //��� ������� �����
  void makestatfill(void);
  void makestatfill(int nCards,int maxmin);
  tSuitProbs vzatok4game(eSuit ,int a23);
  tSuitProbs vzatok(eSuit,CardList *,int a23);
  tSuitProbs vzatok4pass(eSuit,CardList *);

public:
  // ��� ������
  tSuitProbs Compare2List4Max(CardList *My,CardList *Enemy); // ��� ������������� ���������� �� 1 ����
  tSuitProbs Compare2List4Max23(CardList *My,CardList *Enemy); // ��� ������������� ���������� �� 2 � 3 ����
  tSuitProbs Compare2List4Min(CardList *My,CardList *Enemy); // ��� ��� ����������

private:
  // ��� ������
  //tSuitProbs Compare3List4Max(CardList *My,CardList *Left,CardList *Right); // ��� ������������� ����������
  //tSuitProbs Compare3List4Min(CardList *My,CardList *Left,CardList *Right); // ��� ��� ����������
  Card *GetMaxCardPere(void);
  Card *GetMaxCardWithOutPere(void);
  Card *GetMinCardWithOutVz(void);

  void RecountTables(CardList *aMaxCardList,int a23); // ������������� �������         tSuitProbs MastTable[5];
  void RecountTables4RasPass(CardList *aMaxCardList,int a23); // ������������� ������� ��� �������� ��� ������

  void LoadLists(Player *aLeftGamer,Player *aRightGamer,CardList *aMaxCardList); // ����� �������
  Card *MiserCatch1(Player *aLeftGamer,Player *aRightGamer);
  Card *Miser1(Player *aLeftGamer,Player *aRightGamer);
  Card *MyGame1(Player *aLeftGamer,Player *aRightGamer); // ��� ���� 1 ����� - ���

  Card *MyVist1(Player *aLeftGamer,Player *aRightGamer); // ��� ���� ��� ��� 1 ����� - ���

  Card *MiserCatch2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *Miser2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *MyGame2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // ��� ���� 2 ����� - ���
  Card *MyVist2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // ��� ���� ��� ��� 2 ����� - ���

  Card *MiserCatch3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *Miser3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *MyGame3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // ��� ���� 3 ����� - ���
  Card *MyVist3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // ��� ���� ��� ��� 3 ����� - ���
  // � ��� � ����� ���������� �� ������� � ����� !!!
  Card *MyPass1(Card *,Player *aLeftGamer,Player *aRightGamer);
  Card *MyPass2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // ��� ���� 2 ����� - ���
  Card *MyPass3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // ��� ���� 3 ����� - ���
};
#endif
