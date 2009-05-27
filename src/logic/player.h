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
  CardList mCards; // мои
  CardList mLeft;  // Противника с лева (предполагаемый или открытые)
  CardList mRight; // С права (предполагаемый или открытые)

  CardList mOut;   // Снос (мой или предполагаемый)
  CardList mCardsOut; // во взятках мои
  //CardList mLeftOut;  // во взятках Противника слева (предполагаемый или открытые): unused
  //CardList mRightOut; // во взятках Справа (предполагаемый или открытые)
  tSuitProbs mSuitProb[5];

  int mTricksTaken;
  //eSuit Mast;
  eGameBid mMyGame;
  //eHand mEnemy;
  int mPlayerNo; // мой номер

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
  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer); //ход
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //ход при торговле
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // после получения игроком прикупа - пасс или вист
  virtual eGameBid dropForGame ();
  virtual eGameBid dropForMisere ();

  virtual void hilightCard (int lx, int ly);
  virtual void returnDrop ();

  virtual void dealCard (Card *aCard); // получить сданную карту

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
  eGameBid moveCalcDrop (); //для расчета сноса
  //void makestatfill ();
  //void makestatfill (int nCards,int maxmin);
  tSuitProbs vzatok4game (eSuit, int a23);
  tSuitProbs vzatok (eSuit, CardList &, int a23);
  tSuitProbs vzatok4pass (eSuit, CardList &);

public:
  // Два списка
  tSuitProbs compare2List4Max (CardList &my, CardList &enemy); // Для максимального результата на 1 руке
  tSuitProbs compare2List4Max23 (CardList &my, CardList &enemy); // Для максимального результата на 2 и 3 руке
  tSuitProbs compare2List4Min (CardList &my, CardList &enemy); // Для мин результата

protected:
  void internalInit ();
  void clone (const Player *pl);

  void recalcTables (CardList &aMaxCardList, int a23); // Пересчитывает таблицу tSuitProbs mSuitProb[5];
  void recalcPassOutTables (CardList &aMaxCardList, int a23); // Пересчитывает таблицу дли распасов или мизера

  void loadLists (Player *aLeftPlayer, Player *aRightPlayer, CardList &aMaxCardList); // Набор списков

  // Три списка
  //tSuitProbs Compare3List4Max(CardList *my,CardList *Left,CardList *Right); // Для максимального результата
  //tSuitProbs Compare3List4Min(CardList *my,CardList *Left,CardList *Right); // Для мин результата
  Card *GetMaxCardPere ();
  Card *GetMaxCardWithOutPere ();
  Card *GetMinCardWithOutVz ();

  Card *Miser1 (Player *aLeftPlayer, Player *aRightPlayer);
  Card *Miser2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
  Card *Miser3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);

  Card *MyGame1 (Player *aLeftPlayer, Player *aRightPlayer); // моя игра 1 заход - мой
  Card *MyGame2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 2 заход - мой
  Card *MyGame3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 3 заход - мой

  Card *MyVist1 (Player *aLeftPlayer, Player *aRightPlayer); // мой вист или пас 1 заход - мой
  Card *MyVist2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // мой вист или пас 2 заход - мой
  Card *MyVist3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // мой вист или пас 3 заход - мой

  // А вот и часть отвечающая за распасы и мизер !!!
  Card *MyPass1 (Card *, Player *aLeftPlayer, Player *aRightPlayer);
  Card *MyPass2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 2 заход - мой
  Card *MyPass3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 3 заход - мой

  Card *MiserCatch1 (Player *aLeftPlayer, Player *aRightPlayer);
  Card *MiserCatch2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
  Card *MiserCatch3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
};


#endif
