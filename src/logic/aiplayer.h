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
  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut); //ход
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //ход при торговле
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // после получения игроком прикупа - пасс или вист
  virtual eGameBid dropForGame ();
  virtual eGameBid dropForMisere ();

  virtual void clear ();

protected:
  int checkForMisere ();
  eGameBid moveCalcDrop (); //для расчета сноса
  tSuitProbs countGameTricks (eSuit, int a23);
  tSuitProbs countTricks (eSuit, CardList &, int a23);
  tSuitProbs countPassTricks (eSuit, CardList &);

  // Два списка
  tSuitProbs calcProbsForMax (CardList &my, CardList &enemy); // Для максимального результата на 1 руке
  tSuitProbs calcProbsForMaxH23 (CardList &my, CardList &enemy); // Для максимального результата на 2 и 3 руке
  tSuitProbs calcProbsForMin (CardList &my, CardList &enemy); // Для мин результата

  void recalcTables (CardList &aMaxCardList, int a23); // Пересчитывает таблицу tSuitProbs mSuitProb[5];
  void recalcPassOutTables (CardList &aMaxCardList, int a23); // Пересчитывает таблицу дли распасов или мизера

  void loadLists (Player *aLeftPlayer, Player *aRightPlayer, CardList &aMaxCardList); // Набор списков

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

protected:
  tSuitProbs mSuitProb[5];
};


#endif
