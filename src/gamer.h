#ifndef GAMER_H
#define GAMER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "coloda.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#define MAXMESSAGELEN 256
#define MAXNIKNAMELEN 16
//-------------------------------------------------------------------------
class TGamer {
public:
  TDeskView *DeskView;

  TPlScore  *aScore;
  TCardList *aCards; // ���
  TCardList *aLeft;  // ���������� � ���� (�������������� ��� ��������)
  TCardList *aRight; // � ����� (�������������� ��� ��������)

  TCardList *aOut;   // ���� (��� ��� ��������������)
  TCardList *aCardsOut; // �� ������� ���
  TCardList *aLeftOut;  // �� ������� ���������� � ���� (�������������� ��� ��������)
  TCardList *aRightOut; // �� ������� � ����� (�������������� ��� ��������)
  TMastTable MastTable[5];

  int nGetsCard;
  TMast Mast;
  TGamesType GamesType;
  TSide Enemy;
  int nGamer;         // ��� �����

  TGamer (int _nGamer);
  TGamer (int _nGamer, TDeskView *aDeskView);
  virtual ~TGamer ();

  // at least 28 ints (14 int pairs); return # of used ints; the last int is -1
  // result: ofs, cardNo, ..., -1
  int buildHandXOfs (int *dest, int startX, bool opened);
  int cardAt (int lx, int ly, bool opened=true);

  QString NikName;
  // networking part
  virtual int connecttoserver (const QString &host, unsigned short port);
  virtual int connecttodesk (int);
  virtual int join (int);
  void NetGetCards (TColoda *Coloda);
  virtual int SendnPlayerTakeCards (int);
  int GetNikName (void);

  int X, Y;
  int WaitForMouse;
  int oldii;
  bool nInvisibleHand;
  // this part for miser catchs
  TCard *Pronesti;

  virtual TCard *makemove (TCard *lMove,TCard *rMove,TGamer *aLeftGamer,TGamer *aRightGamer); //���
  virtual TGamesType makemove (TGamesType lMove,TGamesType rMove); //��� ��� ��������
  virtual TGamesType makemove (TGamesType MaxGame,int HaveAVist,int nGamerVist); // ����� ��������� ������� ������� - ���� ��� ����
  virtual TGamesType makeout4game (void);
  virtual TGamesType makeout4miser (void);
  virtual void HintCard (int, int);
  virtual void GetBackSnos ();

  virtual void AddCard (TCard *aCard); // �������� ������� �����
  virtual void AddCard (int _CName, int _CMast); // �������� ������� �����

  void clear ();
  void Repaint (TDeskView *aDeskView, int Left, int Top, int Width, int Height, bool opened=false, int selNo=-1);
  //void OnlyMessage (TDeskView *,int,int,int,int);

private:
  int flMiser;
  int Check4Miser(void);
  TGamesType makemove4out(void); //��� ������� �����
  void makestatfill(void);
  void makestatfill(int nCards,int maxmin);
  TMastTable vzatok4game(TMast ,int a23);
  TMastTable vzatok(TMast,TCardList *,int a23);
  TMastTable vzatok4pass(TMast,TCardList *);
public:
  // ��� ������
  TMastTable Compare2List4Max(TCardList *My,TCardList *Enemy); // ��� ������������� ���������� �� 1 ����
  TMastTable Compare2List4Max23(TCardList *My,TCardList *Enemy); // ��� ������������� ���������� �� 2 � 3 ����
  TMastTable Compare2List4Min(TCardList *My,TCardList *Enemy); // ��� ��� ����������
private:
  // ��� ������
  //TMastTable Compare3List4Max(TCardList *My,TCardList *Left,TCardList *Right); // ��� ������������� ����������
  //TMastTable Compare3List4Min(TCardList *My,TCardList *Left,TCardList *Right); // ��� ��� ����������
  TCard *GetMaxCardPere(void);
  TCard *GetMaxCardWithOutPere(void);
  TCard *GetMinCardWithOutVz(void);

  void RecountTables(TCardList *aMaxCardList,int a23); // ������������� �������         TMastTable MastTable[5];
  void RecountTables4RasPass(TCardList *aMaxCardList,int a23); // ������������� ������� ��� �������� ��� ������

  void LoadLists(TGamer *aLeftGamer,TGamer *aRightGamer,TCardList *aMaxCardList); // ����� �������
  TCard *MiserCatch1(TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *Miser1(TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *MyGame1(TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� 1 ����� - ���

  TCard *MyVist1(TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� ��� ��� 1 ����� - ���

  TCard *MiserCatch2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *Miser2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *MyGame2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� 2 ����� - ���
  TCard *MyVist2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� ��� ��� 2 ����� - ���

  TCard *MiserCatch3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *Miser3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *MyGame3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� 3 ����� - ���
  TCard *MyVist3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� ��� ��� 3 ����� - ���
  // � ��� � ����� ���������� �� ������� � ����� !!!
  TCard *MyPass1(TCard *,TGamer *aLeftGamer,TGamer *aRightGamer);
  TCard *MyPass2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� 2 ����� - ���
  TCard *MyPass3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer); // ��� ���� 3 ����� - ���
};
#endif
