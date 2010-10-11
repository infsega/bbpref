/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "player.h"

typedef struct {
  int tricks;
  int perehvatov;
  int len;
  int sum;
} tSuitProbs;

/**
 * @class AiPlayer aiplayer.h
 */
class AiPlayer : public Player {
public:
  AiPlayer(int aMyNumber, PrefModel *model);

  virtual const QString type() const { return QLatin1String("Original"); }

  virtual Player * create(int aMyNumber, PrefModel *model);

public:
  virtual Card *makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut); //ход
  virtual eGameBid makeBid (eGameBid lMove, eGameBid rMove); //ход при торговле
  virtual eGameBid makeFinalBid (eGameBid MaxGame, int nPlayerPass); // после получения игроком прикупа - пасс или вист
  virtual eGameBid makeDrop();

  virtual bool chooseClosedWhist ();

  virtual void clear ();

protected:
  bool checkForMisere ();
  virtual eGameBid dropForGame ();
  virtual eGameBid dropForMisere ();
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

  Card *GetMaxCardPere (int s0=0, int s1=0, int s2=0);
  Card *GetMaxCardWithOutPere (int s0=0, int s1=0, int s2=0);
  Card *GetMinCardWithOutVz (int s0=0, int s1=0, int s2=0);

  Card *Miser1 (Player *aLeftPlayer, Player *aRightPlayer);
  Card *Miser2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
  Card *Miser3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);

  Card *MyGame1 (Player *aLeftPlayer, Player *aRightPlayer); // моя игра 1 заход - мой
  Card *MyGame2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 2 заход - мой
  Card *MyGame3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 3 заход - мой

  Card *MyWhist1 (Player *aLeftPlayer, Player *aRightPlayer); // мой вист или пас 1 заход - мой
  Card *MyWhist2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // мой вист или пас 2 заход - мой
  Card *MyWhist3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // мой вист или пас 3 заход - мой

  // А вот и часть отвечающая за распасы и мизер !!!
  Card *MyPass1 (Card *, Player *aLeftPlayer, Player *aRightPlayer);
  Card *MyPass2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 2 заход - мой
  Card *MyPass3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer); // моя игра 3 заход - мой

  Card *MiserCatch1 (Player *aLeftPlayer, Player *aRightPlayer);
  Card *MiserCatch2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);
  Card *MiserCatch3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer);

  Card * sureTrick(const CardList & enemyCards, const CardList & friendCards, const bool keepTrumps);
  Card * sureTrick(const int suit, const CardList & enemyCards, const CardList & friendCards);

protected:
  tSuitProbs mSuitProb[5];
  CardList mLeft;  // cards of left player (supposed or open)
  CardList mRight; // cards of right player (supposed or open)
  CardList mOut;   // Сdropped cards (my or supposed)
};


#endif
