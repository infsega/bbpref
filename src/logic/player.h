#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "coloda.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"


class Player {
public:
  TDeskView *DeskView;

  TPlScore  *aScore;
  TCardList *aCards; // мои
  TCardList *aLeft;  // Противника с лева (предполагаемый или открытые)
  TCardList *aRight; // С права (предполагаемый или открытые)

  TCardList *aOut;   // Снос (мой или предполагаемый)
  TCardList *aCardsOut; // во взятках мои
  TCardList *aLeftOut;  // во взятках Противника с лева (предполагаемый или открытые)
  TCardList *aRightOut; // во взятках С права (предполагаемый или открытые)
  TMastTable MastTable[5];

  int nGetsCard;
  TMast Mast;
  tGameBid GamesType;
  TSide Enemy;
  int mPlayerNo;         // мой номер

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

  virtual Card *makemove (Card *lMove,Card *rMove,Player *aLeftGamer,Player *aRightGamer); //ход
  virtual tGameBid makemove (tGameBid lMove,tGameBid rMove); //ход при торговле
  virtual tGameBid makemove (tGameBid MaxGame,int HaveAVist,int nGamerVist); // после получения игроком прикупа - пасс или вист
  virtual tGameBid makeout4game (void);
  virtual tGameBid makeout4miser (void);

  virtual void HintCard (int lx, int ly);
  virtual void GetBackSnos ();

  virtual void AddCard (Card *aCard); // получить сданную карту
  virtual void AddCard (int _CName, int _CMast); // получить сданную карту

  virtual void clear ();
  void RepaintAt (TDeskView *aDeskView, int Left, int Top, int selNo=-1);
  void Repaint ();
  void clearCardArea ();

private:
  int flMiser;
  int Check4Miser(void);
  tGameBid makemove4out(void); //для расчета сноса
  void makestatfill(void);
  void makestatfill(int nCards,int maxmin);
  TMastTable vzatok4game(TMast ,int a23);
  TMastTable vzatok(TMast,TCardList *,int a23);
  TMastTable vzatok4pass(TMast,TCardList *);

public:
  // Два списка
  TMastTable Compare2List4Max(TCardList *My,TCardList *Enemy); // Для максимального результата на 1 руке
  TMastTable Compare2List4Max23(TCardList *My,TCardList *Enemy); // Для максимального результата на 2 и 3 руке
  TMastTable Compare2List4Min(TCardList *My,TCardList *Enemy); // Для мин результата

private:
  // Три списка
  //TMastTable Compare3List4Max(TCardList *My,TCardList *Left,TCardList *Right); // Для максимального результата
  //TMastTable Compare3List4Min(TCardList *My,TCardList *Left,TCardList *Right); // Для мин результата
  Card *GetMaxCardPere(void);
  Card *GetMaxCardWithOutPere(void);
  Card *GetMinCardWithOutVz(void);

  void RecountTables(TCardList *aMaxCardList,int a23); // Пересчитывает таблицу         TMastTable MastTable[5];
  void RecountTables4RasPass(TCardList *aMaxCardList,int a23); // Пересчитывает таблицу дли распасов или мизера

  void LoadLists(Player *aLeftGamer,Player *aRightGamer,TCardList *aMaxCardList); // Набор списков
  Card *MiserCatch1(Player *aLeftGamer,Player *aRightGamer);
  Card *Miser1(Player *aLeftGamer,Player *aRightGamer);
  Card *MyGame1(Player *aLeftGamer,Player *aRightGamer); // моя игра 1 заход - мой

  Card *MyVist1(Player *aLeftGamer,Player *aRightGamer); // мой вист или пас 1 заход - мой

  Card *MiserCatch2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *Miser2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *MyGame2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // моя игра 2 заход - мой
  Card *MyVist2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // мой вист или пас 2 заход - мой

  Card *MiserCatch3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *Miser3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer);
  Card *MyGame3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // моя игра 3 заход - мой
  Card *MyVist3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // мой вист или пас 3 заход - мой
  // А вот и часть отвечающая за распасы и мизер !!!
  Card *MyPass1(Card *,Player *aLeftGamer,Player *aRightGamer);
  Card *MyPass2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // моя игра 2 заход - мой
  Card *MyPass3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer); // моя игра 3 заход - мой
};
#endif
