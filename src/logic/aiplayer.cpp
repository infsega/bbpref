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

#include <stdlib.h>
#include <stdio.h>

#include <QDebug>
#include <QPixmap>
#include <QPainter>

#include "aiplayer.h"
#include "desktop.h"

#include "debug.h"


enum eHand {
  LeftHand=0,
  RightHand,
  NoHand
};


AiPlayer::AiPlayer (int aMyNumber, PrefModel *model) : Player(aMyNumber, model) {
  internalInit();
}

Player * AiPlayer::create(int aMyNumber, PrefModel *model) {
  Player * pl = new AiPlayer(aMyNumber, model);
  return pl;
}


void AiPlayer::clear () {
  Player::clear();
  for (int f = 0; f <= 4; f++)  {
    mSuitProb[f].tricks = 0;
    mSuitProb[f].perehvatov = 0;
  }
  mLeft.clear();
  mRight.clear();
  mOut.clear();
}


///////////////////////////////////////////////////////////////////////////////
// utils
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// масть с перехватами (max)
Card *AiPlayer::GetMaxCardWithOutPere (int s0, int s1, int s2) {
  int index = 0, vz = 0, pere = 0;
  for (int f = 1; f <= 4; f++) {
    if (f == s0 || f == s1 || f == s2) // not possible if suit is left to be default
      continue;
    if (mSuitProb[f].tricks > vz || (mSuitProb[f].tricks == vz && mSuitProb[f].perehvatov < pere)) {
      index = f;
      vz = mSuitProb[f].tricks;
      pere = mSuitProb[f].perehvatov;
    }
  }
  if (index) return mCards.maxInSuit(index);
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// ???
Card *AiPlayer::GetMinCardWithOutVz (int s0, int s1, int s2) {
  int index = 0;
  double koef = 0, koef1 = 0;
  for (int f = 1; f <= 4; f++) {
    if (f == s0 || f == s1 || f == s2) // not possible if suit is left to be default
      continue;
    koef1 = mSuitProb[f].len+8/(1+mSuitProb[f].tricks);
    if ((koef1 > koef && mSuitProb[f].len != 0) || (mSuitProb[f].tricks == 0 && mSuitProb[f].len > 0)) {
      index = f;
      koef =koef1;
    }
  }
  if (index && index <=4) return mCards.minInSuit(index);
  return mCards.minFace();
}


///////////////////////////////////////////////////////////////////////////////
// ???
Card *AiPlayer::GetMaxCardPere (int s0, int s1, int s2) {
  int index = 0, vz = 0, pere = 0;
  for (int f = 1; f <= 4; f++) {
    if (f == s0 || f == s1 || f == s2) // not possible if suit is left to be default
      continue;
    if (mSuitProb[f].tricks > vz || (mSuitProb[f].tricks == vz && mSuitProb[f].perehvatov > pere)) {
      index = f;
      vz = mSuitProb[f].tricks;
      pere = mSuitProb[f].perehvatov;
    }
  }
  if (index) return mCards.maxInSuit(index);
  return 0;
}


tSuitProbs AiPlayer::countTricks (eSuit Mast, CardList &aMaxCardList, int a23) {
  tSuitProbs suitProb;
  Card *MyCard, *tmpCard;
  CardList MyCardStack, EnemyCardStack;
  for (int c = 7; c <= FACE_ACE; c++) {
    MyCard = mCards.exists(c, Mast);
    if (MyCard) MyCardStack.insert(MyCard);
    MyCard = aMaxCardList.exists(c, Mast);
    if (MyCard) EnemyCardStack.insert(MyCard);
  }
  if (a23 != 23) suitProb = calcProbsForMax(MyCardStack, EnemyCardStack);
  else suitProb = calcProbsForMaxH23(MyCardStack, EnemyCardStack);
  suitProb.len = mCards.cardsInSuit(Mast);
  suitProb.sum = 0;
  for (int j = 7; j <= FACE_ACE; j++ ) {
    tmpCard = mCards.exists(j, Mast);
    if (tmpCard) suitProb.sum += tmpCard->face();
  }
  return suitProb;
}


tSuitProbs AiPlayer::countGameTricks (eSuit Mast, int a23) {
  tSuitProbs suitProb;
  Card *MyCard, *tmpCard;
  CardList MyCardStack, EnemyCardStack;

  for (int c = 7; c <= FACE_ACE; c++) {
    MyCard = mCards.exists(c, Mast);
    if (MyCard) MyCardStack.insert(MyCard);
    else EnemyCardStack.insert(getCard(c, Mast));
  }
  if (MyCardStack.count() >= 4 && MyCardStack.count() <= 5) EnemyCardStack.remove(EnemyCardStack.minFace());
  if (a23 == 23) suitProb = calcProbsForMaxH23(MyCardStack, EnemyCardStack);
  else suitProb = calcProbsForMax(MyCardStack, EnemyCardStack);
  suitProb.len = mCards.cardsInSuit(Mast);
  suitProb.sum = 0;
  for (int j = 7; j <= FACE_ACE; j++) {
    tmpCard = mCards.exists(j, Mast);
    if (tmpCard) suitProb.sum += tmpCard->face();
  }
  return suitProb;
}


//-----------------------------------------------------------------------
tSuitProbs AiPlayer::countPassTricks (eSuit Mast, CardList &aMaxCardList) {
  tSuitProbs suitProb;
  Card *MyCard, *tmpCard;
  CardList MyCardStack, EnemyCardStack;
  for (int c = 7; c <= FACE_ACE; c++) {
    MyCard = mCards.exists(c, Mast);
    if (MyCard) MyCardStack.insert(MyCard);
    MyCard = aMaxCardList.exists(c, Mast);
    if (MyCard) EnemyCardStack.insert(MyCard);
  }
  suitProb = calcProbsForMin(MyCardStack, EnemyCardStack);
  suitProb.len = mCards.cardsInSuit(Mast);
  suitProb.sum = 0;
  for (int j = 7; j <= FACE_ACE; j++) {
    tmpCard = mCards.exists(j, Mast);
    if (tmpCard) suitProb.sum += tmpCard->face();
  }
  return suitProb;
}


bool AiPlayer::checkForMisere () {
  int cur = 0;
  for (int Mast = SuitSpades; Mast <= SuitHearts; Mast++) {
    if (mCards.exists(7, Mast) && (mCards.exists(9, Mast) || mCards.exists(8, Mast)) &&
        (mCards.exists(FACE_JACK, Mast) || mCards.exists(10, Mast))) {
      cur++;
    }
  }
  if (cur == 4) return true;
  return false;
}


//-----------------------------------------------------------------------
// Для мин результата
tSuitProbs AiPlayer::calcProbsForMin (CardList &my, CardList &enemy) {
  tSuitProbs suitProb;
  suitProb.tricks = 0;
  suitProb.perehvatov = 0;
  int nMaxLen;
  Card *MyCardMin, *EnemyCardMax, *EnemyCardMin;
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //enemy->free(enemy->minFace());
  nMaxLen = (my.count() > enemy.count()) ? my.count() : enemy.count() ;
  //my.count()>enemy->count() ? nMaxLen = my->count() : nMaxLen = enemy->count();
  for (int f = 1; f <= nMaxLen; f++)  {
    MyCardMin = my.minFace();
    EnemyCardMax = enemy.maxFace();
    if (MyCardMin) {
      EnemyCardMin = enemy.lesserInSuit(MyCardMin);
      if (!EnemyCardMin) EnemyCardMin = enemy.minFace();
    } else {
      //EnemyCardMin = enemy->minFace();
      break;
    }
    if (!EnemyCardMax && !EnemyCardMin) break;
    if (*MyCardMin > *EnemyCardMin) {
      // викидываем мою  его младшую
      my.remove(MyCardMin);
      enemy.remove(EnemyCardMin);
      suitProb.tricks++;
    } else {
      my.remove(MyCardMin);
      enemy.remove(EnemyCardMax);
    }
  }
  return suitProb;
}


//-----------------------------------------------------------------------
// Для максимального результата на 1 руке
tSuitProbs AiPlayer::calcProbsForMax (CardList &my, CardList &enemy) {
  tSuitProbs suitProb;
  suitProb.tricks = 0;
  suitProb.perehvatov = 0;
  int nMaxLen, nIget = 1;
  Card *MyCardMax = 0, *EnemyCardMax = 0, *EnemyCardMin = 0;
  //my->count()>enemy->count() ? nMaxLen = my->count() : nMaxLen = enemy->count();
  nMaxLen = (my.count() > enemy.count()) ? my.count() : enemy.count();
  for (int f = 1; f <= nMaxLen; f++) {
    MyCardMax = my.maxFace();
    if (MyCardMax) {
      EnemyCardMax = enemy.greaterInSuit(MyCardMax);
      if (!EnemyCardMax) EnemyCardMax = enemy.minFace();
    } else EnemyCardMax = enemy.maxFace();
    EnemyCardMin = enemy.minFace();
    if (!MyCardMax) break;
    if (!EnemyCardMax && !EnemyCardMin)  {
      // остаток -- мои взятки
      for (int j = 1; j <= my.count(); j++) suitProb.tricks++;
      break;
    }
    if (*MyCardMax > *EnemyCardMax) {
      // викидываем мою старшую, его младшую
      my.remove(MyCardMax);
      enemy.remove(EnemyCardMin);
      suitProb.tricks++;
      if (!nIget) suitProb.perehvatov++;
      nIget = 1;
    } else {
      my.remove(MyCardMax);
      enemy.remove(EnemyCardMax);
      nIget = 0;
    }
  }
  return suitProb;
}


//-----------------------------------------------------------------------
// Для максимального результата на 2 и 3 руке
tSuitProbs AiPlayer::calcProbsForMaxH23 (CardList &my, CardList &enemy) {
  tSuitProbs suitProb;
  suitProb.tricks = 0;
  suitProb.perehvatov = 0;
  int nMaxLen,nIget = 1;
  Card *MyCardMax, *EnemyCardMax;
  //my->count()>enemy->count() ? nMaxLen = my->count() : nMaxLen = enemy->count();
  nMaxLen = (my.count() > enemy.count()) ? my.count() : enemy.count();
  for (int f = 1; f <= nMaxLen; f++) {
    EnemyCardMax = enemy.maxFace();
    if (EnemyCardMax) {
      // Пытаемся н-ти больше чеm у него
      MyCardMax = my.greaterInSuit(EnemyCardMax);
      if (!MyCardMax) {
        // Нет у меня больше чем у него
        MyCardMax = my.minFace();
        if (!MyCardMax) break; // и меньше тож нет
        // Меньше чем у него
        my.remove(MyCardMax);
        enemy.remove(EnemyCardMax);
        nIget = 0;
      } else {
        // Есть больше чем у него
        my.remove(MyCardMax);
        enemy.remove(EnemyCardMax);
        suitProb.tricks++;
        if (!nIget) suitProb.perehvatov++;
        nIget = 1;
      }
    } else {
      // У него нет карт в данной масти! остаток -- мои взятки
      if (!enemy.count()) for (int j = 1; j <= my.count(); j++) suitProb.tricks++;
      //25.07.2000 for ( int j =1; j<=my->count();j++ ) suitProb.tricks++;
      break;
    }
  }
  return suitProb;
}


///////////////////////////////////////////////////////////////////////////////
// Пересчитывает таблицу дли распасов или мизера
void AiPlayer::recalcPassOutTables (CardList &aMaxCardList, int a23) {
  Q_UNUSED(a23)
  mSuitProb[0].tricks = 0;
  mSuitProb[0].perehvatov = 0;
  mSuitProb[0].sum = 0;
  for (int f = 1; f <= 4; f++)  {
    mSuitProb[f] = countPassTricks((eSuit)f, aMaxCardList);
    mSuitProb[0].tricks += mSuitProb[f].tricks;
    mSuitProb[0].perehvatov += mSuitProb[f].perehvatov;
    mSuitProb[0].sum += mSuitProb[f].sum;
  }
}


///////////////////////////////////////////////////////////////////////////////
// Пересчитывает таблицу tSuitProbs mSuitProb[5];
void AiPlayer::recalcTables (CardList &aMaxCardList, int a23) {
  int f;
  mSuitProb[0].tricks = 0;
  mSuitProb[0].perehvatov = 0;
  mSuitProb[0].sum = 0;
  for (f = 1; f <= 4; f++) {
    mSuitProb[f] = countTricks((eSuit)f, aMaxCardList, a23);
    mSuitProb[0].tricks += mSuitProb[f].tricks;
    mSuitProb[0].perehvatov += mSuitProb[f].perehvatov;
    mSuitProb[0].sum += mSuitProb[f].sum;
  }
}


void AiPlayer::loadLists (Player *aLeftPlayer, Player *aRightPlayer, CardList &aMaxCardList) {
/*
  int nLeftVisible = aLeftPlayer->nCardsVisible,nRightVisible = aRightPlayer->nCardsVisible;
  int nCards = mCards.count();
*/
  mLeft.clear();
  mRight.clear();
/*
  // !!!!!!!!!!!!!!!!!!!!!!!!! необходимо учитывать, что во взятках
  if ( !nLeftVisible && !nRightVisible ) {
      makestatfill(nCards,1); // ,1- for max list 2- for minlist
  }
  if ( nLeftVisible && !nRightVisible ) {
      makestatfill(nCards,1); // ,1- for max list 2- for minlist
  }
  if ( !nLeftVisible && nRightVisible ) {
      makestatfill(nCards,1); // ,1- for max list 2- for minlist
  }
  if ( nLeftVisible && nRightVisible ) {
*/
    mLeft = aLeftPlayer->mCards;
    mRight = aRightPlayer->mCards;
/*
  }
*/
// Get Max List only len
/*
  for (int m = 1; m <= 4; m++) {
    for (int c = FACE_ACE; c >= 7; c--) {
      if (mLeft.exists(c, m) || mRight.exists(c, m)) {
        aMaxCardList.insert(newCard(c, m));
      }
    }
  }
*/
  for (int m = 1; m <= 4; m++) {
    Card *LeftMax,*RightMax,*Max=0;
    while (mLeft.cardsInSuit(m) || mRight.cardsInSuit(m)) {
      LeftMax = mLeft.maxInSuit(m);
      RightMax = mRight.maxInSuit(m);
      if (LeftMax == 0 && RightMax == 0) {
        Max = 0;
      } else if (LeftMax == 0 && RightMax != 0) {
        Max = RightMax;
        mRight.remove(Max);
      } else if (LeftMax != 0 && RightMax == 0) {
        Max = LeftMax;
        mLeft.remove(Max);
      } else if (*LeftMax > *RightMax) {
        Max = LeftMax;
        mLeft.remove(LeftMax);
        mRight.remove(RightMax);
      } else {
        Max = RightMax;
        mLeft.remove(LeftMax);
        mRight.remove(RightMax);
      }
    }
    if (Max != 0) aMaxCardList.insert(getCard(Max->face(), Max->suit()));
  }
  mLeft.clear();
  mRight.clear();
  mLeft = aLeftPlayer->mCards;
  mRight = aRightPlayer->mCards;
}


///////////////////////////////////////////////////////////////////////////////
// game mechanics
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// miseres, play
///////////////////////////////////////////////////////////////////////////////
// misere, my move is first
Card *AiPlayer::Miser1 (Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  if (mCards.count() == 10) {
    // первый выход в 8..туз если она одна
    for (int m = 1; m <= 4; m++) {
      if (mCards.cardsInSuit(m) == 1) {
        cur = mCards.minInSuit(m);
        break;
      }
    }
  }
  if (!cur) {
    for (int m = 4; m >= 1; m--) {
      if (cur) break;
      for (int c = FACE_ACE; c >= 7; c--) {
        if (cur) break;
        Card *my;
        //,*leftmax,*leftmin,*rightmax,*rightmin;
        my = mCards.exists(c, m);
        if (my) {
          int matrixindex = 0;
          matrixindex += aLeftPlayer->mCards.lesserInSuit(my) ? 1:0; matrixindex <<= 1;
          matrixindex += aLeftPlayer->mCards.greaterInSuit(my) ? 1:0; matrixindex <<= 1;
          matrixindex += aRightPlayer->mCards.lesserInSuit(my) ? 1:0; matrixindex <<= 1;
          matrixindex += aRightPlayer->mCards.greaterInSuit(my) ? 1:0;
          if (matrixindex == 1 || matrixindex == 9 || matrixindex == 13 || (matrixindex >= 4 && matrixindex <= 7)) {
            cur = my;
          }
        }
      }
    }
  }
  if (!cur) cur = mCards.minInSuit(1);
  if (!cur) cur = mCards.minInSuit(2);
  if (!cur) cur = mCards.minInSuit(3);
  if (!cur) cur = mCards.minInSuit(4);
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere, my move is second
Card *AiPlayer::Miser2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  if (mCards.cardsInSuit(aRightCard->suit())) {
    for (int c = FACE_ACE; c >= 7; c--) {
      if (cur) break;
      Card *my;
      //,*leftmax,*leftmin,*rightmax,*rightmin;
      my = mCards.exists(c, aRightCard->suit());
      if (my) {
        int matrixindex = 0;
        matrixindex += aLeftPlayer->mCards.lesserInSuit(my) ? 1:0; matrixindex <<= 1;
        matrixindex += aLeftPlayer->mCards.greaterInSuit(my) ? 1:0; matrixindex <<= 1;
        matrixindex += (*aRightCard < *my) ? 1:0; matrixindex <<= 1;
        matrixindex += (*aRightCard > *my) ? 1:0;
        if (matrixindex == 1 || matrixindex == 9 || matrixindex==13 || (matrixindex >= 4 && matrixindex <= 7)) {
          cur = my;
        }
      }
    }
    if (!cur) cur = mCards.maxInSuit(aRightCard->suit());
  } else {
    CardList aMaxCardList;
    loadLists(aRightPlayer, aLeftPlayer, aMaxCardList);
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); // масть с перехватами (max)
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = mCards.maxFace();
  }
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere, my move is third
Card *AiPlayer::Miser3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  // copy from MyPass3
  Card *cur = 0;
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  //int mast = mMyGame-(mMyGame/10)*10;
  // набираем списки
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcPassOutTables(aMaxCardList, 23);
  if (mCards.cardsInSuit(aLeftCard->suit()))  {
    // у меня есть масть в которую зашел левый; постараемся пропустить
    if (aLeftCard->suit() == aRightCard->suit()) {
      cur = (*aRightCard > *aLeftCard) ?
        mCards.lesserInSuit(aRightCard) :
        mCards.lesserInSuit(aLeftCard);
    } else cur = mCards.lesserInSuit(aLeftCard);
    if (!cur) cur = mCards.maxInSuit(aLeftCard->suit());
  } else {
    // у меня нет масти в которую зашел левый
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); // масть с перехватами (max)
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = mCards.minFace();
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// miseres, catch
///////////////////////////////////////////////////////////////////////////////
// misere catch, my move is first
Card *AiPlayer::MiserCatch1 (Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  CardList aMaxCardList;
  CardList *Naparnik;
  eHand Side;
  if (aRightPlayer->game() == g86) {
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    Naparnik = &(aLeftPlayer->mCards);
    Side = RightHand;
  } else {
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    Naparnik = &(aRightPlayer->mCards);
    Side = LeftHand;
  }
  recalcPassOutTables(aMaxCardList, 1);

  // Randomization
  int suit[4];
  int s;
  for (int i=0; i<4; i++) {
	s=0;
	while (s==0) {
		s = qrand()%4+1;
		for (int j=0; j<i; j++)
			if (s==suit[j]) s=0;
	}
	suit[i]=s;
  }  
  
  for (int i = 0; i < 4; i++) {
	int m = suit[i];	// Random order of suits
    Card *EnemyMin = aMaxCardList.minInSuit(m);
    Card *NaparnikMin = Naparnik->minInSuit(m);
    Card *MyMin = mCards.minInSuit(m);
    if (cur) goto badlabel;
    if (EnemyMin) {
      //есть такая
      if (MyMin) {
        // и у меня есть такая
        if (NaparnikMin) {
          // и у напарника есть такая
          if (*NaparnikMin < *EnemyMin && *MyMin < *EnemyMin) {
            cur = MyMin;// Всунули
            goto badlabel;
          }
          if (*NaparnikMin > *EnemyMin && *MyMin < *EnemyMin) {
            // надо н-ти такую масть по которой он может пронести а
            // но должен вернутся ко мне
            for (int k = 1; k <= 4; k++) {
              if (k == m || aMaxCardList.cardsInSuit(k) == 0) continue;
              Card *EnemyMax = aMaxCardList.maxInSuit(k);
              Card *NaparnikMax = Naparnik->maxInSuit(k);
              Card *MyMax = mCards.maxInSuit(k);
              if (MyMax && NaparnikMax && EnemyMax) {
                if (Naparnik->cardsInSuit(k) < mCards.cardsInSuit(k) &&
                    Naparnik->cardsInSuit(k) < aMaxCardList.cardsInSuit(k)) {
                  cur = mCards.lesserInSuit(EnemyMax);
                  //MyMax; // на этой он должен пронести NaparnikMax
                  if (aRightPlayer->game() == g86) aRightPlayer->mCardCarryThru = NaparnikMax;
                  else aLeftPlayer->mCardCarryThru = NaparnikMax;
                  goto badlabel;
                }
              }
            }
          }
        } else { //у напорника нет масти
          if (*MyMin < *EnemyMin) {
            cur = MyMin;// Всунули
            goto badlabel;
          }
        }
      }
    }
  }

  
  if ((Side == LeftHand) && (!cur)) 
	// if we didn't choose anything wise, move lesser card
	cur = mCards.minFace();
  else {
  	//2 отгребаем свое
  	if (!cur) cur = GetMaxCardWithOutPere();	  
  	// 3 пытаемс
  	if (!cur) cur = GetMaxCardWithOutPere();
  	if (!cur) cur = mCards.minFace();
  }
//  } else { //Side == LeftHand
//  }
badlabel:
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere catch, my move is second
Card *AiPlayer::MiserCatch2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  CardList aMaxCardList;
  CardList *Naparnik;
  eHand Side;
  if (aRightPlayer->game() == g86) {
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    Naparnik = &(aLeftPlayer->mCards);
    Side = RightHand;
  } else {
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    Naparnik = &(aRightPlayer->mCards);
    Side = LeftHand;
  }
  recalcPassOutTables(aMaxCardList, 23);
  if (Side == LeftHand) {
    // enemy is left
    if (mCards.cardsInSuit(aRightCard->suit())) {
      // у меня есть карты в данной масти
      Card *MyMax = mCards.maxInSuit(aRightCard->suit());
      //Card *MyMin = mCards.minInSuit(aRightCard->suit());
      //Card *EnMax = aMaxCardList->maxInSuit(aRightCard->suit());
      Card *EnMin = aMaxCardList.minInSuit(aRightCard->suit());
      if (EnMin) {
        // у противника есть карты в данной масти
        if (*aRightCard < *EnMin) {
          // можно попытаться всунуть
          cur = mCards.lesserInSuit(EnMin);
          if (!cur) cur = mCards.greaterInSuit(EnMin);
        } else cur= mCards.greaterInSuit(EnMin);
      } else cur = MyMax; // А вот тут надо смотреть кому нужен ход
    } else {
      // у меня нет карт  в данной масти
      if (mCardCarryThru) {
        //надо пронести карту
        if (mCards.exists(mCardCarryThru->face(),mCardCarryThru->suit())) {
          cur = mCardCarryThru;
        } else {
          cur = mCards.maxInSuit(mCardCarryThru->suit());
          if (!cur) cur = mCards.maxFace();
        }
      } else {
        // указания на пронос - нет
        cur = GetMaxCardPere();
        if (!cur) cur = mCards.maxFace();
      }
    }
  } else { // противник зашел под меня
    Card *MyMax = mCards.maxInSuit(aRightCard->suit());
    Card *MyMin = mCards.minInSuit(aRightCard->suit());
    if (MyMax) {
      if (*MyMin < *aRightCard) {
        // if we both have lesser card in suit, or friend has nothing in
		// this suit, enemy will take the trick
        Card *NapMin = Naparnik->lesserInSuit(aRightCard);
		Card *NapMax = Naparnik->greaterInSuit(aRightCard);
        if (NapMin || !NapMax)
			cur = mCards.lesserInSuit(aRightCard);
        else
			cur = MyMax; // кому передать код ?
      } else cur = MyMax; // грохать ее самой большой
    } else {
      // а у меня нет масти
      cur = GetMaxCardPere();
      if (!cur) cur = mCards.maxFace();
    }
  }
  mCardCarryThru = 0;
  if (!cur) cur = mCards.minFace();
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere catch, my move is third
Card *AiPlayer::MiserCatch3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  CardList aMaxCardList;
  CardList *Naparnik;
  eHand Side;
  if (aRightPlayer->game() == g86) {
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    Naparnik = &(aLeftPlayer->mCards);
    Side = RightHand;
	printf("right is enemy");
  } else {
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    Naparnik = &(aRightPlayer->mCards);
    Side = LeftHand;
	printf("left is enemy");
  }
  recalcPassOutTables(aMaxCardList, 23);
  if (Side == LeftHand) {	// Right player is my friend
    if (mCards.cardsInSuit(aLeftCard->suit())) {
      // у меня есть карты  в данной масти
      //Card *MyMax = mCards.maxInSuit(aLeftCard->suit());
      //Card *MyMin = mCards.minInSuit(aLeftCard->suit());
      if (*aLeftCard < *aRightCard) {
        // напарник уже придавил карту
        cur = mCards.maxInSuit(aLeftCard->suit());
      } else {
        //есть шанс всунуть
        cur = mCards.lesserInSuit(aLeftCard);
        if (!cur) cur = mCards.maxInSuit(aLeftCard->suit());
      }
    } else {
      // у меня нет карт  в данной масти
      if (mCardCarryThru) {
        //надо пронести карту
        if (mCards.exists(mCardCarryThru->face(),mCardCarryThru->suit())) {
          cur = mCardCarryThru;
        } else {
          cur = mCards.maxInSuit(mCardCarryThru->suit());
          if (!cur) cur = mCards.maxFace();
        }
      } else {
        // указания на пронос - нет
        cur = GetMaxCardPere();
        if (!cur) cur = mCards.maxFace();
      }
    }
  } else { // Left player is my friend
    Card *MyMax = mCards.maxInSuit(aLeftCard->suit());
    Card *MyMin = mCards.minInSuit(aLeftCard->suit());
    if (MyMax) {
	  if ((*MyMin < *aRightCard) && (aLeftCard->suit()==aRightCard->suit())) {
        // push lesser card than enemy
        cur = mCards.lesserInSuit(aRightCard);
		if (!cur) cur = MyMax;
      } else cur = MyMax; // грохать ее самой большой
    } else {
      // have no car of this suit
      cur = GetMaxCardPere();
      if (!cur) cur = mCards.maxFace();
    }
  }
  mCardCarryThru = 0;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game
///////////////////////////////////////////////////////////////////////////////
// game, my move is first
Card *AiPlayer::MyGame1 (Player *aLeftPlayer, Player *aRightPlayer) {
  // 1-выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
  Card *cur = 0;
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  int mast = m_game-(m_game/10)*10;
  // набираем списки
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcTables(aMaxCardList, 1);
  if (mLeft.cardsInSuit(mast) || mRight.cardsInSuit(mast)) {
    // У противников есть козыря
    cur = mCards.maxInSuit(mast);
    if (!cur) cur = GetMaxCardWithOutPere(); // а у меня их нет !!!
    if (!cur) cur = GetMaxCardPere(); //масть с перехватами (max)
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = mCards.maxFace();
  } else {
    // а козырьков у них и нету !!!
    if (!cur) cur = GetMaxCardPere(); //масть с перехватами (max)
    if (!cur) cur = GetMaxCardWithOutPere(); // и у меня их нет !!!
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = mCards.maxInSuit(mast);
    if (!cur) cur = mCards.maxFace();
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game, my move is second
Card *AiPlayer::MyGame2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  CardList aMaxCardList;
  eGameBid tmpGamesType = m_game;
  Card *cur = 0;
  Card *MaxLeftCard = 0;
  int mast = aRightCard->suit();
  int koz = m_game-(m_game/10)*10;
  // набираем списки
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcTables(aMaxCardList, 23);
  cur = mCards.maxInSuit(mast);
  if (!cur) {
    // Нет Масти
    if (!aLeftPlayer->mCards.cardsInSuit(mast)) {
      // У левого тож нету
      MaxLeftCard = aLeftPlayer->mCards.maxInSuit(koz); // k8: это заебись, а чо, если у нас нихуя козыря нет?
      if (MaxLeftCard) {
        cur = mCards.greaterInSuit(MaxLeftCard); // k8: тогда тут тоже нет нихуя
        if (!cur) cur = mCards.lesserInSuit(MaxLeftCard); // k8: и тут тоже нихуя
      } else {
        cur = mCards.minInSuit(koz); // k8: тогда тут тоже нет нихуя
      }
      if (!cur) cur = GetMinCardWithOutVz(); // лабуду
      if (!cur) cur = mCards.minFace();
    } else {
      // Есть масть у левого
      cur = mCards.minInSuit(koz); // k8: это заебись, а чо, если у нас нихуя козыря нет?
      // k8: копипасты?
      if (!cur) cur = GetMinCardWithOutVz(); // лабуду
      if (!cur) cur = mCards.minFace();
    }
  } else {
    // У меня есть масть
    if (aLeftPlayer->mCards.cardsInSuit(mast)) {
      //У левого есть
      MaxLeftCard = aLeftPlayer->mCards.maxInSuit(mast);
      cur = mCards.greaterInSuit(MaxLeftCard);
      if (!cur) cur = mCards.lesserInSuit(MaxLeftCard);
    } else {
      // У левого нет
      if (!aLeftPlayer->mCards.cardsInSuit(koz)) {
        // И козырей нет
        cur = mCards.greaterInSuit(aRightCard);
        if (!cur) cur = mCards.minInSuit(mast);
      } else {
        // есть у левого козыря
        cur = mCards.minInSuit(mast);
      }
    }
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game, my move is third
Card *AiPlayer::MyGame3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  Card *maxInSuit;
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  int mast = m_game-(m_game/10)*10;
  // набираем списки
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcTables(aMaxCardList, 23);
  if (mCards.cardsInSuit(aLeftCard->suit())) {
    // у меня есть масть, в которую зашел левый
    if (aLeftCard->suit() == aRightCard->suit()) {
      // постараемся ударить
      cur = mCards.greaterInSuit(aLeftCard);
      if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
    } else { //aLeftCard->suit() == aRightCard->suit()
      if (aRightCard->suit() != mast) {
        // не бита козырем
        cur = mCards.greaterInSuit(aLeftCard);
        if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
      } else {
        // бита козырем, мне не надо бить
        cur = mCards.minInSuit(aLeftCard->suit());
      }
    }
  } else {
    // у меня нет масти в которую зашел левый
    if (aLeftCard->suit() == aRightCard->suit()) {
      //k8:? bug?
      //if (*aLeftCard > *aRightCard) maxInSuit = aLeftCard; else maxInSuit = aLeftCard;
      if (*aLeftCard > *aRightCard) maxInSuit = aLeftCard; else maxInSuit = aRightCard;
    } else {
      if (aLeftCard->suit() != mast && aRightCard->suit() != mast) maxInSuit = aLeftCard;
      else maxInSuit = aRightCard;
    }
    cur = mCards.greaterInSuit(maxInSuit);
    if (!cur) cur = mCards.minInSuit(mast);
    if (!cur) cur = GetMinCardWithOutVz(); // нет коз.
    if (!cur) cur = mCards.minFace();
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// whist
///////////////////////////////////////////////////////////////////////////////
// мой вист или пас 1 заход - мой
Card *AiPlayer::MyWhist1 (Player *aLeftPlayer, Player *aRightPlayer) {
  // 1 - выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
  Card *cur = 0;
  Player *aEnemy, *aFriend;
  // Кто игрок а кто напарник
  if (aLeftPlayer->game() != gtPass && aLeftPlayer->game() != whist) {
    aEnemy = aLeftPlayer;
    aFriend = aRightPlayer;
  } else {
    aEnemy = aRightPlayer;
    aFriend = aLeftPlayer;
  }
  const int trump = aEnemy->game() % 10; //aEnemy->myGame()-(aEnemy->myGame()/10)*10;
  Q_ASSERT(trump != 0);
  Q_ASSERT(trump == m_model->trumpSuit());
//  int suit = trump;
  // build lists
  //loadLists(aLeftPlayer,aRightPlayer,aMaxCardList);
  //recalcTables(aMaxCardList,1);
/*  if (aEnemy == aLeftPlayer) {
    // Слева игрок
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 1);
    for (int i=1; i<=4; i++) {
      if ((i == trump) || mCards.cardsInSuit(suit) == 0)
        continue; // skip trumps and suits I don't own

      if ((aLeftPlayer->mCards.cardsInSuit(i) > 0)
        && (aRightPlayer->mCards.cardsInSuit(i) == 0)
        && (aRightPlayer->mCards.cardsInSuit(trump) > 0)) {
          qDebug() << "1";
          // Enemy has this suite, but friend doesn't, and friend has a trump
          cur = mCards.minInSuit(i);
      } else if (aLeftPlayer->mCards.cardsInSuit(i) == 0) {
        if (aLeftPlayer->mCards.cardsInSuit(trump) > 0)
          continue; // he will beat our suit
        else {
                    qDebug() << "2";
          cur = mCards.minInSuit(i);}
      }
      if (cur) return cur;
    }
/*    if (aLeftPlayer->mCards.cardsInSuit(suit) <= 2) {
      Card *m = mCards.maxInSuit(suit);
      if (!m) cur = mCards.minInSuit(aLeftPlayer->mCards.emptySuit (suit));
      else if (aLeftPlayer->mCards.maxInSuit(suit)->face() < m->face()) cur = mCards.maxInSuit(suit);
      else cur = mCards.minInSuit(aLeftPlayer->mCards.emptySuit(suit));
    } else {
      cur = mCards.minInSuit(aLeftPlayer->mCards.emptySuit(suit));
    }*
    int s1=0, s2=0;
    for (int i=1; i<=4; i++) {
      if (aLeftPlayer->mCards.cardsInSuit(i) == 0) {
        if (s1 == 0)
          s1 = i;
        else
          s2 = i;
      }
    }
    qDebug() << "bad suits: " << trump << s1 << s2;
    if(aLeftPlayer->mCards.cardsInSuit(trump) > 0) {
      //!
      if (!cur) {cur = GetMaxCardPere(trump, s1, s2);           qDebug() << "3";}
      //!
      if (!cur) {cur = GetMaxCardWithOutPere(trump, s1, s2);           qDebug() << "4";}
      if (!cur) {cur = GetMinCardWithOutVz(trump, s1, s2);           qDebug() << "5";}  // под игрока - с семака
    } else {
      if (!cur) {cur = GetMaxCardPere();           qDebug() << "6";}
      if (!cur) {cur = GetMaxCardWithOutPere();           qDebug() << "7";}
      if (!cur) {cur = GetMinCardWithOutVz();           qDebug() << "8";}
    }
/*
    if ( mCards.exists(FACE_ACE,cur->suit()) || mCards.exists(FACE_KING,cur->suit())) {
      cur = 0;
      cur = mCards.minInSuit(aMaxCardList->emptySuit(0));
    }
*

    //if (!cur) cur = mCards.minInSuit(mast);
  } else {
    // слева - напарник
    // повторить трюк с козырем
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 1);

    int s1=0, s2=0;
    for (int i=1; i<=4; i++) {
      if (aRightPlayer->mCards.cardsInSuit(i) == 0) {
        if (s1 == 0)
          s1 = i;
        else
          s2 = i;
      }
    }
    qDebug() << "bad suits: " << trump << s1 << s2;

    if(aRightPlayer->mCards.cardsInSuit(trump) > 0) {
      // !!!
      cur = GetMaxCardWithOutPere(trump, s1, s2);           qDebug() << "9";
      // !!!
      if (!cur) { cur = GetMaxCardPere(trump, s1, s2);           qDebug() << "10";}
      if (!cur) {
        for (int f=1; f<=4; f++) {
          if (f == trump || f == s1 || f == s2) // not possible if suit is left to be default
            continue;
          cur = mCards.maxInSuit(f);
        }
        if (!cur) cur = mCards.maxFace();
      }
    } else {
      cur = GetMaxCardWithOutPere();           qDebug() << "11";
      if (!cur) {cur = GetMaxCardPere();           qDebug() << "12";}
      if (!cur) {cur = mCards.maxFace();           qDebug() << "13";} // под вистующего - с тузующего
    }
  }*/
  cur = sureTrick(aEnemy->mCards, aFriend->mCards, aFriend->mCards.hasSuit(trump));
  if(cur)
    return cur;
  else
    qDebug() << "No sure trick";

  cur = GetMinCardWithOutVz(trump);
  if (!cur) cur = GetMinCardWithOutVz();
  if(!cur) cur = mCards.minFace();
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// мой вист или пас 2 заход - мой
Card *AiPlayer::MyWhist2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  Player *aEnemy, *aFriend;

  // Кто игрок а кто напарник
  if (aLeftPlayer->game() != gtPass && aLeftPlayer->game() != whist) {
    aEnemy = aLeftPlayer;
    aFriend = aRightPlayer;
  } else {
    aEnemy = aRightPlayer;
    aFriend = aLeftPlayer;
  }
  const int trump = aEnemy->game()-(aEnemy->game()/10)*10;
  // набираем списки
  if (aEnemy == aLeftPlayer) {
    // Слева игрок
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 23);
    // Ну если он заходил под вистуза с туза
    if (aLeftPlayer->mCards.hasSuit(aRightCard) && (*aRightCard > *aLeftPlayer->mCards.maxInSuit(aRightCard)) ) {
      cur = mCards.minInSuit(aRightCard);
      qDebug() << "1";
    } else {
      if ( mCards.greaterInSuit(aRightCard) && aLeftPlayer->mCards.hasSuit(aRightCard)
        && *mCards.maxInSuit(aRightCard) > *aLeftPlayer->mCards.maxInSuit(aRightCard) )
      //cur = mCards.maxInSuit(aRightCard->suit()); // может умный и больше заходов у него нет в данную масть а есть козырь
      cur = mCards.greaterInSuit(aLeftPlayer->mCards.maxInSuit(aRightCard));
      qDebug() << "2";
    }
    if(!cur) { cur = mCards.minInSuit(aRightCard);
      qDebug() << "3";}
    if (!cur) {
      // No cards in this suit - using trump
      if (aLeftPlayer->mCards.hasSuit(trump))
        cur = mCards.greaterInSuit(aLeftPlayer->mCards.maxInSuit(trump));
      // Enemy has no trumps or I cannot beat them
      if (!cur) {cur = mCards.minInSuit(trump); qDebug() << "4"; }
    }
    if (!cur) {cur = GetMinCardWithOutVz(); qDebug() << "5"; }
  } else {
    // слева - напарник
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 23);
    cur = mCards.greaterInSuit(aRightCard);
    if (!cur) cur = mCards.minInSuit(aRightCard->suit());
    if (!cur) cur = mCards.minInSuit(trump); // Нет масти пробуем козырь
    if (!cur) cur = GetMinCardWithOutVz(); // Нет масти и козыря
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// мой вист или пас 3 заход - мой
Card *AiPlayer::MyWhist3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  Card *maxInSuit;
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  Player *aEnemy, *aFriend;
  int mast;

  // Кто игрок а кто напарник
  if (aLeftPlayer->game() != gtPass && aLeftPlayer->game() != whist) {
    aEnemy = aLeftPlayer;
    aFriend = aRightPlayer;
  } else {
    aEnemy = aRightPlayer;
    aFriend = aLeftPlayer;
  }
  mast = aEnemy->game()-(aEnemy->game()/10)*10;
  // набираем списки
  //loadLists(aLeftPlayer,aRightPlayer,aMaxCardList);
  //recalcTables(aMaxCardList,23);
  if (mCards.cardsInSuit(aLeftCard->suit()))  {
    // у меня есть масть в которую зашел левый
    if (aEnemy == aLeftPlayer) {
      // левый - вражина
      loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (aLeftCard->suit() == aRightCard->suit()) {
        if (*aRightCard > *aLeftCard) {
          // бить не надо
          cur = mCards.minInSuit(aRightCard->suit());
        } else {
          // постараемсмя ударить
          cur = mCards.greaterInSuit(aLeftCard);
          if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
        }
      } else { //aLeftCard->suit() == aRightCard->suit()
        if (aRightCard->suit() != mast)  {
          // не бита козырем
          cur = mCards.greaterInSuit(aLeftCard);
          if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
        } else {
          // бита козырем, мне не надо бить
          cur = mCards.minInSuit(aLeftCard->suit());
        }
      }
    } else {
      //правый вражииина
      loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (aLeftCard->suit() == aRightCard->suit()) {
        if (*aRightCard > *aLeftCard) {
          // попытаться прибить правого
          cur = mCards.greaterInSuit(aRightCard);
          if (!cur) cur = mCards.minInSuit(aRightCard->suit());
        } else {
          // бить не надо
          cur = mCards.minInSuit(aRightCard->suit());
        }
      } else {
        if (aRightCard->suit() == mast) {
          // левый зашел, а правый пригрел козырем
          cur = mCards.minInSuit(aLeftCard->suit());
        } else {
          // Ух ты .... Левый друг зашел а правый пронес ерунду !!!
          cur = mCards.minInSuit(aLeftCard->suit());
        }
      }
    }
  } else {
    //  у меня нет масти в которую зашел левый
    if (aLeftCard->suit() == aRightCard->suit()) {
      //k8: wtf?
      //if (*aLeftCard > *aRightCard) maxInSuit = aLeftCard; else maxInSuit = aLeftCard;
      if (*aLeftCard > *aRightCard) maxInSuit = aLeftCard; else maxInSuit = aRightCard;
    } else {
      if (aLeftCard->suit() != mast && aRightCard->suit() != mast) maxInSuit = aLeftCard;
      else maxInSuit = aRightCard;
    }
    if (aEnemy == aLeftPlayer) {
      // Слева игрок
      loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (maxInSuit == aLeftCard) {
        // попытаемся хлопнуть его карту
        cur = mCards.greaterInSuit(aLeftCard);
      } else {
        // Уже наша взятка... скидываем минимальную
        cur = mCards.minInSuit(maxInSuit->suit());
      }
    } else {
      // слева - напарник
      loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (maxInSuit == aLeftCard) {
        // Уже наша взятка... скидываем минимальную
        cur = mCards.minInSuit(maxInSuit->suit());
      } else {
        //попытаемся хлопнуть его карту
        cur = mCards.greaterInSuit(aLeftCard);
      }
    }
    if (!cur) cur = mCards.minInSuit(mast);
    if (!cur) cur = GetMinCardWithOutVz(); // нет коз.
    if (!cur) cur = mCards.minFace();
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// pass-outs
///////////////////////////////////////////////////////////////////////////////
// my passout first
Card *AiPlayer::MyPass1 (Card *rMove, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  CardList aTmpList, aStackStore;
  //int doRest = 0;

  if (rMove != 0)  {
	cur = mCards.minInSuit(rMove->suit());
	return cur;
    /*aTmpList.copySuit(&mCards, (eSuit)rMove->suit());
    aStackStore = mCards;
    mCards = aTmpList;
    doRest = 1;*/
    //loadLists(aLeftPlayer, aRightPlayer, aMaxCardList); // !!!!!!!!!
    //recalcPassOutTables(aMaxCardList, 1);
  }
  if (tmpGamesType == raspass || tmpGamesType == g86)  { // распасы/мизер
    loadLists(aLeftPlayer, aRightPlayer, aMaxCardList); // !!!!!!!!!
    recalcPassOutTables(aMaxCardList,1);
    // тема такая : 100 % отбери своё, если такового нет - мах в минимально длинной масти
    /*cur = GetMaxCardWithOutPere(); // за искл тех случ коды масти уже нет
    if (cur) {
      if (aLeftPlayer->mCards.cardsInSuit(cur->suit()) == 0 &&
          aRightPlayer->mCards.cardsInSuit(cur->suit()) == 0) {
        cur = 0;
      }
    }*/
    //if (!cur) cur = GetMaxCardPere(); //масть с перехватами (max)
    //if (!cur) cur = GetMinCardWithOutVz(); // лабуду
	
	// Randomization
  	int suit[4];
  	int s;
  	for (int i=0; i<4; i++) {
		s=0;
		while (s==0) {
			s = qrand()%4+1;
			for (int j=0; j<i; j++)
				if (s==suit[j]) s=0;
		}
		suit[i]=s;
  	}
	
	for (int i = 0; i < 4; i++) {
		int m = suit[i];	// Random order of suits
		cur = mCards.minInSuit(m);
      	if (aLeftPlayer->mCards.greaterInSuit(cur) != 0 ||
          aRightPlayer->mCards.greaterInSuit(cur) != 0) 
		  	return cur;
		else
			cur = 0;
	}
	
    if (!cur) cur = mCards.minFace();
  }
  //if (doRest) mCards = aStackStore;
  //mMyGame = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}

///////////////////////////////////////////////////////////////////////////////
// my passout second
Card *AiPlayer::MyPass2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  eGameBid tmpGamesType = m_game;
  int mast = aRightCard->suit();
  CardList aMaxCardList;
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcPassOutTables(aMaxCardList, 1);
  /////////////////////////////////////////////////////////
  if (tmpGamesType == raspass || tmpGamesType == g86) {
    // распасы/мизер
    cur = mCards.maxInSuit(mast); 
    if (!cur) {
      // Нет Масти
      // во шара !!! Льем мах в масти где есть 100% взятки
      cur = GetMaxCardWithOutPere();
      if (!cur) cur = GetMaxCardPere(); //масть с перехватами (max)
      if (!cur) cur = GetMinCardWithOutVz(); // лабуду
      if (!cur) cur = mCards.minFace();
    } else {
      // У меня есть масть
      if (aLeftPlayer->mCards.cardsInSuit(mast)) {
        //У левого есть
        cur = mCards.lesserInSuit(aLeftPlayer->mCards.minInSuit(mast));
		if (!cur) cur = mCards.lesserInSuit(aRightCard);
        if (!cur) cur = mCards.maxInSuit(mast);
      } else {
        // У левого нет
        cur = mCards.lesserInSuit(aRightCard);
        if (!cur) cur = mCards.maxInSuit(mast);
      }
    }
  }
  /////////////////////////////////////////////////////////
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// my passout third
Card *AiPlayer::MyPass3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur=0;
  /*Card *maxInSuit;*/
  eGameBid tmpGamesType = m_game;
  CardList aMaxCardList;
  //int mast = mMyGame-(mMyGame/10)*10;
  // набираем списки
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcPassOutTables(aMaxCardList, 23);
  if (mCards.cardsInSuit(aLeftCard->suit()))  {
    // у меня есть масть  в которую зашел левый
    // постараемсмя пропустить
    if (aLeftCard->suit() == aRightCard->suit()) {
      if (*aRightCard > *aLeftCard) cur = mCards.lesserInSuit(aRightCard);
      else cur = mCards.lesserInSuit(aLeftCard);
    } else cur = mCards.lesserInSuit(aLeftCard);
    if (!cur) cur = mCards.maxInSuit(aLeftCard->suit());
  } else {
    // у меня нет масти в которую зашел левый
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); //масть с перехватами (max)
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = mCards.minFace();
  }
  m_game = tmpGamesType;
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game moves
///////////////////////////////////////////////////////////////////////////////
// make game move (dispatcher)
Card *AiPlayer::makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) {
  Q_UNUSED(isPassOut)
  qDebug() << type() << "("<< mPlayerNo << ") moves";
  Card *cur = 0;
  if (lMove == 0 && rMove == 0) {
    // мой заход - первый
    if (m_game == gtPass || m_game == whist) cur = MyWhist1(aLeftPlayer, aRightPlayer); // кто-то играет а я как бы вистую
    else if (m_game == g86catch) cur = MiserCatch1(aLeftPlayer, aRightPlayer);
    else if (m_game == g86) cur = Miser1(aLeftPlayer,aRightPlayer);
    else if (m_game == raspass) cur = MyPass1(rMove, aLeftPlayer, aRightPlayer); // ну типа распасы или мизер
    else cur = MyGame1(aLeftPlayer, aRightPlayer); // ну типа моя игра
  }
  if (lMove == 0 && rMove != 0) {
    // мой заход - второй
    if (m_game == gtPass || m_game == whist) cur = MyWhist2(rMove, aLeftPlayer, aRightPlayer); // кто-то играет а я как бы вистую
    else if (m_game == g86catch) cur = MiserCatch2(rMove, aLeftPlayer, aRightPlayer);
    else if (m_game == g86) cur = Miser2(rMove, aLeftPlayer, aRightPlayer);
    else if (m_game == raspass) cur = MyPass2(rMove, aLeftPlayer, aRightPlayer); // ну типа распасы или мизер
    else cur = MyGame2(rMove, aLeftPlayer, aRightPlayer); // ну типа моя игра
  }
  if (lMove != 0 && rMove != 0) {
    // мой заход - 3
    if (m_game == gtPass || m_game == whist ) cur = MyWhist3(lMove, rMove, aLeftPlayer, aRightPlayer); // кто-то играет а я как бы вистую
    else if (m_game == g86catch) cur = MiserCatch3(lMove, rMove, aLeftPlayer, aRightPlayer);
    else if (m_game == g86) cur = Miser3(lMove, rMove, aLeftPlayer, aRightPlayer);
    else if (m_game == raspass) cur = MyPass3(lMove, rMove, aLeftPlayer, aRightPlayer); // ну типа распасы или мизер
    else cur = MyGame3(lMove, rMove, aLeftPlayer, aRightPlayer); // ну типа моя игра
  }
  mCards.remove(cur);
  mCardsOut.insert(cur);
  Q_ASSERT(cur != 0);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// Pass or Whist or Halfwhist
eGameBid AiPlayer::makeFinalBid (eGameBid MaxGame, int HaveAWhist, int nPlayerPass)
{
  eGameBid Answer;
  eGameBid MyMaxGame = moveCalcDrop();
  int vz = MyMaxGame/10;

  if (MaxGame == g86) {
  	Answer = g86catch; // Misere
	goto myGame;
  }
  if (m_model->optStalingrad && MaxGame == g61) {
  	Answer = whist; // Stalingrad  
	goto myGame;
  }
  if (!m_model->opt10Whist && MaxGame>=101 && MaxGame<=105) {
	Answer = whist;
	goto myGame;
  }

  
  //if (HaveAWhist == gtPass) {
  if (nPlayerPass > 0) {
    if (MaxGame < g71)
      Answer = (vz >= m_model->gameWhists(MaxGame)/2) ? whist : gtPass;
    else
      Answer = (vz >= m_model->gameWhists(MaxGame)) ? whist : gtPass;
  } else {
    if (MaxGame < g81)
      Answer = (vz > m_model->gameWhists(MaxGame)/2) ? whist : gtPass;
    else if (MaxGame < g91)
      Answer = (vz > m_model->gameWhists(MaxGame) && qrand()%4 == 0) ? whist : gtPass;
    else
      Answer = (vz > m_model->gameWhists(MaxGame) && qrand()%8 == 0) ? whist : gtPass;
  }
      
  //Answer = (HaveAWhist != whist && vz >= gameWhistsMin(MaxGame)) ? whist : gtPass ;
  //if (HaveAWhist == gtPass && vz < gameWhistsMin(MaxGame)) Answer = gtPass;

myGame:  
  if ((Answer == gtPass) && (nPlayerPass == 1) && MaxGame <= 81)
  	Answer = halfwhist;
	
  m_game = Answer;
  return Answer;
}


///////////////////////////////////////////////////////////////////////////////
// calc drop
eGameBid AiPlayer::moveCalcDrop () {
  eGameBid GamesTypeRetVal;
  int f;
  int nMaxMastLen = 0;
  eSuit nMaxMast = SuitNone;
  tSuitProbs LocalMastTable[5];
  LocalMastTable[0].tricks = 0;
  LocalMastTable[0].perehvatov = 0;
  LocalMastTable[0].sum = 0;
  for (f = 1; f <= 4; f++) {
    LocalMastTable[f] = countGameTricks((eSuit)f, 1);
    LocalMastTable[0].tricks += LocalMastTable[f].tricks;
    LocalMastTable[0].perehvatov +=LocalMastTable[f].perehvatov;
    LocalMastTable[0].sum += LocalMastTable[f].sum;
  }
  for (f = 1; f <= 4; f++) {
    // предполагаемый козырь - самая длинная масть
    if (mCards.cardsInSuit(f) > nMaxMastLen) {
      nMaxMastLen = mCards.cardsInSuit(f);
      nMaxMast = (eSuit)f;
    }
  }
  for (f = 1; f <= 4; f++) {
    if (mCards.cardsInSuit(f) == nMaxMastLen && nMaxMast != f) {
      // есть максимально длинные масти
      if (LocalMastTable[f].sum > LocalMastTable[nMaxMast].sum) nMaxMast = (eSuit)f;
    }
  }
  //масть и звятки уже посчитали
  GamesTypeRetVal = (eGameBid)(LocalMastTable[0].tricks*10+nMaxMast);
  // это то, что мы можем играть максимально
  return GamesTypeRetVal;
}


///////////////////////////////////////////////////////////////////////////////
//
eGameBid AiPlayer::dropForMisere () {
  Card *FirstCardOut = 0, *SecondCardOut = 0;
  Card *tmpFirstCardOut, *tmpSecondCardOut;
  Card *RealFirstCardOut, *RealSecondCardOut;
  eGameBid Hight = g105, tmpHight = g105;
  for (int f = 0; f < 12; f++) {
    tmpFirstCardOut = mCards.at(f);
    mCards.removeAt(f);
    //for ( int j=0;j<12;j++ )  {
    for (int j = f+1; j < 12; j++) { // patch from Rasskazov K. (kostik450@mail.ru)
      if (j != f) {
        tmpSecondCardOut = mCards.at(j);
        mCards.removeAt(j);
        /// @todo bad practise
        AiPlayer *tmpGamer = new AiPlayer(99, m_model);
        tmpGamer->mCards = mCards;
        tmpGamer->mCards.mySort();
        tmpHight = tmpGamer->moveCalcDrop();
        if ((tmpHight < Hight) ||
            (tmpHight == Hight && tmpGamer->mSuitProb[0].perehvatov < mSuitProb[0].perehvatov)) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        delete tmpGamer;
        mCards.putAt(j, tmpSecondCardOut);
      }
    }
    mCards.putAt(f, tmpFirstCardOut);
  }
  RealFirstCardOut = mCards.maxInSuit(FirstCardOut->suit());
  mCards.remove(RealFirstCardOut);
  mOut.insert(RealFirstCardOut);
  if (SecondCardOut->suit() != FirstCardOut->suit()) {
    RealSecondCardOut = mCards.maxInSuit(SecondCardOut->suit());
  } else {
    RealSecondCardOut = mCards.greaterInSuit(FirstCardOut);
    if (!RealSecondCardOut) RealSecondCardOut = mCards.maxFace(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
  }
  mCards.remove(RealSecondCardOut);
  mOut.insert(RealSecondCardOut);
  mCards.mySort();
  return g86;
}


///////////////////////////////////////////////////////////////////////////////
//
eGameBid AiPlayer::dropForGame () {
  Card *FirstCardOut = 0, *SecondCardOut = 0;
  Card *tmpFirstCardOut, *tmpSecondCardOut;
  Card *RealFirstCardOut, *RealSecondCardOut;
  eGameBid Hight = zerogame, tmpHight = zerogame;

  for (int f = 0; f < 12; f++) {
    tmpFirstCardOut = mCards.at(f);
    mCards.removeAt(f);
    for (int j = 0; j < 12; j++)  {
      if (j != f) {
        tmpSecondCardOut = mCards.at(j);
        mCards.removeAt(j);
        /// @todo bad practise
        AiPlayer *tmpGamer = new AiPlayer(99, m_model);
        tmpGamer->mCards = mCards;
        tmpGamer->mCards.mySort();
        tmpHight = tmpGamer->moveCalcDrop();
        if ((tmpHight > Hight) ||
            (tmpHight == Hight && tmpGamer->mSuitProb[0].perehvatov < mSuitProb[0].perehvatov)) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer->mCards.clear();
        delete tmpGamer;
        mCards.putAt(j, tmpSecondCardOut);
      }
    }
    mCards.putAt(f, tmpFirstCardOut);
  }

  clearCardArea();

  RealFirstCardOut = mCards.minInSuit(FirstCardOut->suit());
  mCards.remove(RealFirstCardOut);
  mOut.insert(RealFirstCardOut);
  if (SecondCardOut->suit() != FirstCardOut->suit()) {
    RealSecondCardOut = mCards.minInSuit(SecondCardOut->suit());
  } else {
    RealSecondCardOut = mCards.greaterInSuit(FirstCardOut);
    if (!RealSecondCardOut) RealSecondCardOut = mCards.minFace(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
  }
  mCards.remove(RealSecondCardOut);
  mOut.insert(RealSecondCardOut);
  mCards.mySort();
  draw();
  //Hight = moveBidding(undefined,undefined);
  Hight = moveCalcDrop();
  if (Hight < m_game)  {
    // Вот такая ерунда
    int tmpvz = Hight/10, tmpmast = Hight-tmpvz*10;
    while (tmpHight < m_game)  {
      tmpvz++;
      tmpHight = (eGameBid)(tmpvz*10+tmpmast);
    }
    m_game = tmpHight;
  } else {
    m_game = Hight;
  }
  return m_game;
}


//ход при торговле
eGameBid AiPlayer::makeBid (eGameBid lMove, eGameBid rMove) {
//mIStart = (lMove == undefined && rMove == undefined);
	//qDebug() << mPlayerNo << mIStart;
/*
  if ( mMyGame == gtPass )  {
    mMyGame=gtPass;
  } else {
*/
  //mMyGame = gtPass;
  if (m_game != gtPass)  {
    int f;
    int nMaxMastLen = 0;
    eSuit nMaxMast = SuitNone;
    tSuitProbs LocalMastTable[5];
/*
    if (optAggPass && optPassCount > 0) {
      if (lMove == undefined) lMove = g71;
      if (rMove == undefined) rMove = g71;
    }
*/
    eGameBid curMaxGame = qMax(lMove, rMove);
    if (m_model->optAggPass && m_model->optPassCount > 0) {
      if (curMaxGame < g71) curMaxGame = g65;
      if (lMove != gtPass && lMove != undefined && lMove < g65) lMove = g65;
      if (rMove != gtPass && rMove != undefined && rMove < g65) rMove = g65;
    }
    LocalMastTable[0].tricks=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum = 0;
    for (f = 1; f <= 4; f++)  {
      LocalMastTable[f] = countGameTricks((eSuit)f, 1);
      LocalMastTable[0].tricks += LocalMastTable[f].tricks;
      LocalMastTable[0].perehvatov +=LocalMastTable[f].perehvatov;
      LocalMastTable[0].sum += LocalMastTable[f].sum;
    }
    // предполагаемый козырь -- самая длинная масть
    for (f = 1; f <= 4; f++) {
      if (mCards.cardsInSuit(f) > nMaxMastLen) {
        nMaxMastLen = mCards.cardsInSuit(f);
        nMaxMast = (eSuit)f;
      }
    }
    for (f = 1; f <= 4; f++) {
      if (mCards.cardsInSuit(f) == nMaxMastLen && nMaxMast != f) {
        // есть максимально длинные масти
        if (LocalMastTable[f].sum > LocalMastTable[nMaxMast].sum) nMaxMast = (eSuit)f;
      }
    }
    // масть и взятки уже посчитали
    if (curMaxGame <= g75) {
      m_game = (eGameBid)((LocalMastTable[0].tricks+1)*10+nMaxMast);
    } else {
      m_game = (eGameBid)((LocalMastTable[0].tricks)*10+nMaxMast);
    }
	// if first move or one player passed
    if ((rMove == gtPass || rMove == undefined) && (lMove == gtPass || lMove == undefined)) {
      if (LocalMastTable[0].tricks >= 4) {
        /*if (optAggPass && optPassCount > 0) mMyGame = g71;
        else*/ m_game = g61;
      } else {
        if (LocalMastTable[0].tricks == 0)  {
          // check for misere !!!
          m_game = checkForMisere() ? g86 : gtPass ;
        } else m_game = gtPass;
      }
    }
	else if (m_game >= curMaxGame){
		if ((mIStart) && (m_game < succBid(curMaxGame)))
			m_game = curMaxGame;
		else
      		m_game = (eGameBid)succBid(curMaxGame);
		if (m_model->optAggPass && (m_model->optPassCount > 0) && (m_game < g71))
			m_game = g71;
	}
	else
      m_game = gtPass;
    
/*
    if( mMyGame >= curMaxGame  ) {
        mMyGame = (eGameBid) succBid(curMaxGame);
    } else {
    }*/
    // это то, что мы можем играть максимально
    /*if ( mMyGame > gtPass  ) {
      if ( rMove < lMove )  {
        if ( lMove <= mMyGame ) {
          if ( lMove!=g61 )
              mMyGame = lMove; // say here
          else
              mMyGame = (eGameBid) succBid(lMove);
        } else {
          mMyGame = gtPass;
        }
      } else { // rMove > lMove
        if ( mMyGame > rMove ) {
//          asm int 3;
            if (rMove!=undefined) {
              mMyGame = (eGameBid) succBid(rMove);
             } else {
              mMyGame = g61;
             }
        } else {
          mMyGame = gtPass;
        }

      }
    } else {     //  mMyGame > gtPass
      if ( (rMove==gtPass || rMove==undefined) && ( lMove==gtPass || lMove==undefined ) ) {
        if ( LocalMastTable[0].tricks >= 4 ) {
          mMyGame = g61;
        } else {
          if ( LocalMastTable[0].tricks == 0 )  {
            // check for miser !!!
            if (checkForMisere()) {
              mMyGame = g86;
            } else {
              mMyGame = gtPass;
            }
          } else {
            mMyGame = gtPass;
          }
        }
      } else {
        mMyGame = gtPass;
      }
    }*/
  }
  //???
  if (m_model->optAggPass && m_model->optPassCount > 0 && m_game != gtPass && m_game < g71) {
    makeBid(g71, g71);
   // if (optAggPass && optPassCount > 0 && mMyGame != gtPass && mMyGame < g71) moveBidding(g72, g72);
  }
  return m_game;
}


//-----------------------------------------------------------------------
/*
void AiPlayer::makestatfill () {
  eHand lr = LeftHand;
  for (int m = 1; m <= 4; m++) {
    for (int c = FACE_ACE; c >= 7; c--) {
      if (!mCards.exists(c, m)) {
        if (lr == LeftHand) {
          mLeft.insert(newCard(c, m));
          lr = RightHand;
        } else {
          mRight.insert(newCard(c, m));
          lr = LeftHand;
        }
      }
    }
  }
}
*/


//-----------------------------------------------------------------------
/*
void AiPlayer::makestatfill (int nCards, int maxmin) {
  int nCounter = 0;
  eHand lr = LeftHand;
  if (maxmin == 1) {
    for (int m = 1; m <= 4; m++) {
      for (int c = FACE_ACE; c >= 7; c--) {
        if (!mCards.exists(c, m) && !mOut.exists(c, m)) {
          if (lr == LeftHand) {
            mLeft.insert(newCard(c, m));
            lr = RightHand;
          } else {
            mRight.insert(newCard(c, m));
            lr = LeftHand;
            nCounter++;
            if (nCounter >= nCards) return;
          }
        }
      }
    }
  } else {
    for (int m = 1; m <= 4; m++) {
      for (int c = 7; c <= FACE_ACE; c++) {
        if (!mCards.exists(c, m) && !mOut.exists(c, m)) {
          if (lr == LeftHand) {
            mLeft.insert(newCard(c, m));
            lr = RightHand;
          } else {
            mRight.insert(newCard(c, m));
            lr = LeftHand;
            nCounter++;
            if (nCounter >= nCards) return;
          }
        }
      }
    }
  }
}
*/

bool AiPlayer::chooseClosedWhist () {
  if(m_model->currentGame() < g81) {
    //tSuitProbs countGameTricks((eSuit)f, 1)
	if (qrand()%4 == 0)
		return true;
	else
		return false;
  } else if(m_model->currentGame() < g91) {
    if (qrand()%8 == 0)
        return true;
    else
        return false;
  } else {
    return false;
  }
}

Card * AiPlayer::sureTrick(const CardList & enemyCards, const CardList & friendCards, const bool keepTrumps)
{
  Card *cur = 0;
  const int trump = m_model->trumpSuit();
  for (int i=1; i<=4; i++) {
    if ((keepTrumps && i == trump) || !mCards.hasSuit(i))
      continue; // skip trumps and suits I don't own
    cur = sureTrick(i, enemyCards, friendCards);
    if(cur)
      return cur;
  }
  if(!cur && keepTrumps)
    cur = sureTrick(trump, enemyCards, friendCards);
  return cur;
}

Card * AiPlayer::sureTrick(const int suit, const CardList & enemyCards, const CardList & friendCards)
{
  qDebug() << "suit" << suit << "me:" << mCards.hasSuit(suit) << "enemy:" << enemyCards.hasSuit(suit)
      << "friend:" << friendCards.hasSuit(suit);
  const int trump = m_model->trumpSuit();
  // 1. Me and enemy have this suit
  if (mCards.hasSuit(suit) && enemyCards.hasSuit(suit)) {
    // 1a.
    if (*mCards.maxInSuit(suit) > *enemyCards.maxInSuit(suit)) {
      qDebug() << "1a" << suit << mCards.maxInSuit(suit)->toUniString() << enemyCards.maxInSuit(suit)->toUniString();
      qDebug() << mCards.greaterInSuit(enemyCards.maxInSuit(suit))->toUniString();
      return mCards.greaterInSuit(enemyCards.maxInSuit(suit));
    }
    // 1b.
    if (friendCards.hasSuit(suit) && *friendCards.maxInSuit(suit) > *enemyCards.maxInSuit(suit)) {
      qDebug() << "1b" << suit << friendCards.maxInSuit(suit)->toUniString() << enemyCards.maxInSuit(suit)->toUniString();
      qDebug() << mCards.minInSuit(suit)->toUniString();
      return mCards.minInSuit(suit);
    }
  }
  // 2. Сюркуп
  if (enemyCards.hasSuit(suit) && !friendCards.hasSuit(suit) && friendCards.hasSuit(trump)) {
    qDebug() << "2a" << suit << mCards.minInSuit(suit)->toUniString();
    // Enemy has this suite, but friend doesn't, and friend has a trump
    return mCards.minInSuit(suit);
  }
  // 3. Enemy does not have both this suit and trumps
  if (!enemyCards.hasSuit(suit) && !enemyCards.hasSuit(trump)) {
    qDebug() << "2b" << suit << mCards.minInSuit(suit)->toUniString();
    return mCards.minInSuit(suit);
  }
  return 0;
}
