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
  CardList *aCards; // мои
  CardList *aLeft;  // Противника с лева (предполагаемый или открытые)
  CardList *aRight; // С права (предполагаемый или открытые)

  CardList *aOut;   // Снос (мой или предполагаемый)
  CardList *aCardsOut; // во взятках мои
  CardList *aLeftOut;  // во взятках Противника с лева (предполагаемый или открытые)
  CardList *aRightOut; // во взятках С права (предполагаемый или открытые)
  tSuitProbs MastTable[5];

  int nGetsCard;
  eSuit Mast;
  eGameBid GamesType;
  eHand Enemy;
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
  virtual eGameBid makemove (eGameBid lMove,eGameBid rMove); //ход при торговле
  virtual eGameBid makemove (eGameBid MaxGame,int HaveAVist,int nGamerVist); // после получения игроком прикупа - пасс или вист
  virtual eGameBid makeout4game (void);
  virtual eGameBid makeout4miser (void);

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
  eGameBid makemove4out(void); //для расчета сноса
  void makestatfill(void);
  void makestatfill(int nCards,int maxmin);
  tSuitProbs vzatok4game(eSuit ,int a23);
  tSuitProbs vzatok(eSuit,CardList *,int a23);
  tSuitProbs vzatok4pass(eSuit,CardList *);

public:
  // Два списка
  tSuitProbs Compare2List4Max(CardList *My,CardList *Enemy); // Для максимального результата на 1 руке
  tSuitProbs Compare2List4Max23(CardList *My,CardList *Enemy); // Для максимального результата на 2 и 3 руке
  tSuitProbs Compare2List4Min(CardList *My,CardList *Enemy); // Для мин результата

private:
  // Три списка
  //tSuitProbs Compare3List4Max(CardList *My,CardList *Left,CardList *Right); // Для максимального результата
  //tSuitProbs Compare3List4Min(CardList *My,CardList *Left,CardList *Right); // Для мин результата
  Card *GetMaxCardPere(void);
  Card *GetMaxCardWithOutPere(void);
  Card *GetMinCardWithOutVz(void);

  void RecountTables(CardList *aMaxCardList,int a23); // Пересчитывает таблицу         tSuitProbs MastTable[5];
  void RecountTables4RasPass(CardList *aMaxCardList,int a23); // Пересчитывает таблицу дли распасов или мизера

  void LoadLists(Player *aLeftGamer,Player *aRightGamer,CardList *aMaxCardList); // Набор списков
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
