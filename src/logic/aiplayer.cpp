#include <stdlib.h>
#include <stdio.h>

#include <QPixmap>
#include <QPainter>

#include "aiplayer.h"


AiPlayer::AiPlayer (int aMyNumber, DeskView *aDeskView) : Player(aMyNumber, aDeskView) {
  internalInit();
}


void AiPlayer::clear () {
  Player::clear();
  for (int f = 0; f <= 4; f++)  {
    mSuitProb[f].tricks = 0;
    mSuitProb[f].perehvatov = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
// utils
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ����� � ����������� (max)
Card *AiPlayer::GetMaxCardWithOutPere () {
  int index = 0, vz = 0, pere = 0;
  for (int f = 1; f <= 4; f++) {
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
Card *AiPlayer::GetMinCardWithOutVz () {
  int index = 0;
  double koef = 0, koef1 = 0;
  for (int f = 1; f <= 4; f++) {
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
Card *AiPlayer::GetMaxCardPere () {
  int index = 0, vz = 0, pere = 0;
  for (int f = 1; f <= 4; f++) {
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
    else EnemyCardStack.insert(newCard(c, Mast));
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


int AiPlayer::checkForMisere () {
  int cur = 0;
  for (int Mast = SuitSpades; Mast <= SuitHearts; Mast++) {
    if (mCards.exists(7, Mast) && (mCards.exists(9, Mast) || mCards.exists(8, Mast)) &&
        (mCards.exists(FACE_JACK, Mast) || mCards.exists(10, Mast))) {
      cur++;
    }
  }
  if (cur == 4) return 1;
  return 0;
}


//-----------------------------------------------------------------------
// ��� ��� ����������
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
      // ���������� ���  ��� �������
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
// ��� ������������� ���������� �� 1 ����
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
      // ������� -- ��� ������
      for (int j = 1; j <= my.count(); j++) suitProb.tricks++;
      break;
    }
    if (*MyCardMax > *EnemyCardMax) {
      // ���������� ��� �������, ��� �������
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
// ��� ������������� ���������� �� 2 � 3 ����
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
      // �������� �-�� ������ ��m � ����
      MyCardMax = my.greaterInSuit(EnemyCardMax);
      if (!MyCardMax) {
        // ��� � ���� ������ ��� � ����
        MyCardMax = my.minFace();
        if (!MyCardMax) break; // � ������ ��� ���
        // ������ ��� � ����
        my.remove(MyCardMax);
        enemy.remove(EnemyCardMax);
        nIget = 0;
      } else {
        // ���� ������ ��� � ����
        my.remove(MyCardMax);
        enemy.remove(EnemyCardMax);
        suitProb.tricks++;
        if (!nIget) suitProb.perehvatov++;
        nIget = 1;
      }
    } else {
      // � ���� ��� ���� � ������ �����! ������� -- ��� ������
      if (!enemy.count()) for (int j = 1; j <= my.count(); j++) suitProb.tricks++;
      //25.07.2000 for ( int j =1; j<=my->count();j++ ) suitProb.tricks++;
      break;
    }
  }
  return suitProb;
}


///////////////////////////////////////////////////////////////////////////////
// ������������� ������� ��� �������� ��� ������
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
// ������������� ������� tSuitProbs mSuitProb[5];
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
  // !!!!!!!!!!!!!!!!!!!!!!!!! ���������� ���������, ��� �� �������
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
    if (Max != 0) aMaxCardList.insert(newCard(Max->face(), Max->suit()));
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
    // ������ ����� � 8..��� ���� ��� ����
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
    if (!cur) cur = GetMaxCardPere(); // ����� � ����������� (max)
    if (!cur) cur = GetMinCardWithOutVz(); // ������
    if (!cur) cur = mCards.maxFace();
  }
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere, my move is third
Card *AiPlayer::Miser3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  // copy from MyPass3
  Card *cur = 0;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  //int mast = mMyGame-(mMyGame/10)*10;
  // �������� ������
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcPassOutTables(aMaxCardList, 23);
  if (mCards.cardsInSuit(aLeftCard->suit()))  {
    // � ���� ���� ����� � ������� ����� �����; ����������� ����������
    if (aLeftCard->suit() == aRightCard->suit()) {
      cur = (*aRightCard > *aLeftCard) ?
        mCards.lesserInSuit(aRightCard) :
        mCards.lesserInSuit(aLeftCard);
    } else cur = mCards.lesserInSuit(aLeftCard);
    if (!cur) cur = mCards.maxInSuit(aLeftCard->suit());
  } else {
    // � ���� ��� ����� � ������� ����� �����
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); // ����� � ����������� (max)
    if (!cur) cur = GetMinCardWithOutVz(); // ������
    if (!cur) cur = mCards.minFace();
  }
  mMyGame = tmpGamesType;
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
  if (aRightPlayer->myGame() == g86) {
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    Naparnik = &(aLeftPlayer->mCards);
    Side = RightHand;
  } else {
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    Naparnik = &(aRightPlayer->mCards);
    Side = LeftHand;
  }
  recalcPassOutTables(aMaxCardList, 1);
  //if (Side == LeftHand) {
  // 1. �������� �������
  for (int m = 1; m <= 4; m++) {
    Card *EnemyMin = aMaxCardList.minInSuit(m);
    Card *NaparnikMin = Naparnik->minInSuit(m);
    Card *MyMin = mCards.minInSuit(m);
    if (cur) goto badlabel;
    if (EnemyMin) {
      //���� �����
      if (MyMin) {
        // � � ���� ���� �����
        if (NaparnikMin) {
          // � � ��������� ���� �����
          if (*NaparnikMin < *EnemyMin && *MyMin < *EnemyMin) {
            cur = MyMin;// �������
            goto badlabel;
          }
          if (*NaparnikMin > *EnemyMin && *MyMin < *EnemyMin) {
            // ���� �-�� ����� ����� �� ������� �� ����� �������� �
            // �� ������ �������� �� ���
            for (int k = 1; k <= 4; k++) {
              if (k == m || aMaxCardList.cardsInSuit(k) == 0) continue;
              Card *EnemyMax = aMaxCardList.maxInSuit(k);
              Card *NaparnikMax = Naparnik->maxInSuit(k);
              Card *MyMax = mCards.maxInSuit(k);
              if (MyMax && NaparnikMax && EnemyMax) {
                if (Naparnik->cardsInSuit(k) < mCards.cardsInSuit(k) &&
                    Naparnik->cardsInSuit(k) < aMaxCardList.cardsInSuit(k)) {
                  cur = mCards.lesserInSuit(EnemyMax);
                  //MyMax; // �� ���� �� ������ �������� NaparnikMax
                  if (aRightPlayer->myGame() == g86) aRightPlayer->mCardCarryThru = NaparnikMax;
                  else aLeftPlayer->mCardCarryThru = NaparnikMax;
                  goto badlabel;
                }
              }
            }
          }
        } else { //� ��������� ��� �����
          if (*MyMin < *EnemyMin) {
            cur = MyMin;// �������
            goto badlabel;
          }
        }
      }
    }
  }

  //2 ��������� ����
  if (!cur) cur = GetMaxCardWithOutPere();
  if (Side == LeftHand) {
    // ��� ������������ ���� �������� �� ������� !!!
  }
  // 3 �������
  if (!cur) cur = GetMaxCardWithOutPere();
  if (!cur) cur = mCards.minFace();
//  } else { //Side == LeftHand
//  }
badlabel:
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere catch, my move is second
Card *AiPlayer::MiserCatch2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  CardList aMaxCardList;
  CardList *Naparnik;
  eHand Side;
  if (aRightPlayer->myGame() == g86) {
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
    // ��� ��� ����, ��������� ������
    if (mCards.cardsInSuit(aRightCard->suit())) {
      // � ���� ���� ����� � ������ �����
      Card *MyMax = mCards.maxInSuit(aRightCard->suit());
      //Card *MyMin = mCards.minInSuit(aRightCard->suit());
      //Card *EnMax = aMaxCardList->maxInSuit(aRightCard->suit());
      Card *EnMin = aMaxCardList.minInSuit(aRightCard->suit());
      if (EnMin) {
        // � ���������� ���� ����� � ������ �����
        if (*aRightCard < *EnMin) {
          // ����� ���������� �������
          cur = mCards.lesserInSuit(EnMin);
          if (!cur) cur = mCards.greaterInSuit(EnMin);
        } else cur= mCards.greaterInSuit(EnMin);
      } else cur = MyMax; // � ��� ��� ���� �������� ���� ����� ���
    } else {
      // � ���� ��� ����  � ������ �����
      if (mCardCarryThru) {
        //���� �������� �����
        if (mCards.exists(mCardCarryThru->face(),mCardCarryThru->suit())) {
          cur = mCardCarryThru;
        } else {
          cur = mCards.maxInSuit(mCardCarryThru->suit());
          if (!cur) cur = mCards.maxFace();
        }
      } else {
        // �������� �� ������ - ���
        cur = GetMaxCardPere();
        if (!cur) cur = mCards.maxFace();
      }
    }
  } else { // ��������� ����� ��� ����
    Card *MyMax = mCards.maxInSuit(aRightCard->suit());
    Card *MyMin = mCards.minInSuit(aRightCard->suit());
    if (MyMax) {
      if (*MyMin < *aRightCard) {
        //���� ����������� �������
        Card *NapMin = Naparnik->lesserInSuit(aRightCard);
        if (NapMin) cur = mCards.lesserInSuit(aRightCard);
        else cur = MyMax; // ���� �������� ��� ?
      } else cur = MyMax; // ������� �� ����� �������
    } else {
      // � � ���� ��� �����
      cur = GetMaxCardPere();
      if (!cur) cur = mCards.maxFace();
    }
  }
  mCardCarryThru = 0;
  if (!cur) cur = mCards.minFace();
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere catch, my move is third
Card *AiPlayer::MiserCatch3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  CardList aMaxCardList;
  CardList *Naparnik;
  eHand Side;
  if (aRightPlayer->myGame() == g86) {
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
    if (mCards.cardsInSuit(aLeftCard->suit())) {
      // � ���� ���� �����  � ������ �����
      //Card *MyMax = mCards.maxInSuit(aLeftCard->suit());
      //Card *MyMin = mCards.minInSuit(aLeftCard->suit());
      if (*aLeftCard < *aRightCard) {
        // �������� ��� �������� �����
        cur = mCards.maxInSuit(aLeftCard->suit());
      } else {
        //���� ���� �������
        cur = mCards.lesserInSuit(aLeftCard);
        if (!cur) cur = mCards.maxInSuit(aLeftCard->suit());
      }
    } else {
      // � ���� ��� ����  � ������ �����
      if (mCardCarryThru) {
        //���� �������� �����
        if (mCards.exists(mCardCarryThru->face(),mCardCarryThru->suit())) {
          cur = mCardCarryThru;
        } else {
          cur = mCards.maxInSuit(mCardCarryThru->suit());
          if (!cur) cur = mCards.maxFace();
        }
      } else {
        // �������� �� ������ - ���
        cur = GetMaxCardPere();
        if (!cur) cur = mCards.maxFace();
      }
    }
  } else { // RightHand - my friend
    Card *MyMax = mCards.maxInSuit(aLeftCard->suit());
    Card *MyMin = mCards.minInSuit(aLeftCard->suit());
    if (MyMax) {
      if (*MyMin < *aLeftCard) {
        // ���� ����������� �������
        Card *NapMin = Naparnik->lesserInSuit(aLeftCard);
        if (NapMin) cur = mCards.lesserInSuit(aLeftCard);
        else cur = MyMax; // ���� �������� ��� ?
      } else cur = MyMax; // ������� �� ����� �������
    } else {
      // � � ���� ��� �����
      cur = GetMaxCardPere();
      if (!cur) cur = mCards.maxFace();
    }
  }
  mCardCarryThru = 0;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game
///////////////////////////////////////////////////////////////////////////////
// game, my move is first
Card *AiPlayer::MyGame1 (Player *aLeftPlayer, Player *aRightPlayer) {
  // 1-������ ������ 2-��������� ����� � ����������� 3-��� ���������� 4-???
  Card *cur = 0;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  int mast = mMyGame-(mMyGame/10)*10;
  // �������� ������
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcTables(aMaxCardList, 1);
  if (mLeft.cardsInSuit(mast) || mRight.cardsInSuit(mast)) {
    // � ����������� ���� ������
    cur = mCards.maxInSuit(mast);
    if (!cur) cur = GetMaxCardWithOutPere(); // � � ���� �� ��� !!!
    if (!cur) cur = GetMaxCardPere(); //����� � ����������� (max)
    if (!cur) cur = GetMinCardWithOutVz(); // ������
    if (!cur) cur = mCards.maxFace();
  } else {
    // � ��������� � ��� � ���� !!!
    if (!cur) cur = GetMaxCardPere(); //����� � ����������� (max)
    if (!cur) cur = GetMaxCardWithOutPere(); // � � ���� �� ��� !!!
    if (!cur) cur = GetMinCardWithOutVz(); // ������
    if (!cur) cur = mCards.maxInSuit(mast);
    if (!cur) cur = mCards.maxFace();
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game, my move is second
Card *AiPlayer::MyGame2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  CardList aMaxCardList;
  eGameBid tmpGamesType = mMyGame;
  Card *cur = 0;
  Card *MaxLeftCard = 0;
  int mast = aRightCard->suit();
  int koz = mMyGame-(mMyGame/10)*10;
  // �������� ������
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcTables(aMaxCardList, 23);
  cur = mCards.maxInSuit(mast);
  if (!cur) {
    // ��� �����
    if (!aLeftPlayer->mCards.cardsInSuit(mast)) {
      // � ������ ��� ����
      MaxLeftCard = aLeftPlayer->mCards.maxInSuit(koz); // k8: ��� �������, � ��, ���� � ��� ����� ������ ���?
      cur = mCards.greaterInSuit(MaxLeftCard); // k8: ����� ��� ���� ��� �����
      if (!cur) cur = mCards.lesserInSuit(MaxLeftCard); // k8: � ��� ���� �����
      if (!cur) cur = GetMinCardWithOutVz(); // ������
      if (!cur) cur = mCards.minFace();
    } else {
      // ���� ����� � ������
      cur = mCards.minInSuit(koz); // k8: ��� �������, � ��, ���� � ��� ����� ������ ���?
      // k8: ���������?
      if (!cur) cur = GetMinCardWithOutVz(); // ������
      if (!cur) cur = mCards.minFace();
    }
  } else {
    // � ���� ���� �����
    if (aLeftPlayer->mCards.cardsInSuit(mast)) {
      //� ������ ����
      MaxLeftCard = aLeftPlayer->mCards.maxInSuit(mast);
      cur = mCards.greaterInSuit(MaxLeftCard);
      if (!cur) cur = mCards.lesserInSuit(MaxLeftCard);
    } else {
      // � ������ ���
      if (!aLeftPlayer->mCards.cardsInSuit(koz)) {
        // � ������� ���
        cur = mCards.greaterInSuit(aRightCard);
        if (!cur) cur = mCards.minInSuit(mast);
      } else {
        // ���� � ������ ������
        cur = mCards.minInSuit(mast);
      }
    }
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game, my move is third
Card *AiPlayer::MyGame3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  Card *maxInSuit;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  int mast = mMyGame-(mMyGame/10)*10;
  // �������� ������
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcTables(aMaxCardList, 23);
  if (mCards.cardsInSuit(aLeftCard->suit())) {
    // � ���� ���� �����, � ������� ����� �����
    if (aLeftCard->suit() == aRightCard->suit()) {
      // ����������� �������
      cur = mCards.greaterInSuit(aLeftCard);
      if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
    } else { //aLeftCard->suit() == aRightCard->suit()
      if (aRightCard->suit() != mast) {
        // �� ���� �������
        cur = mCards.greaterInSuit(aLeftCard);
        if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
      } else {
        // ���� �������, ��� �� ���� ����
        cur = mCards.minInSuit(aLeftCard->suit());
      }
    }
  } else {
    // � ���� ��� ����� � ������� ����� �����
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
    if (!cur) cur = GetMinCardWithOutVz(); // ��� ���.
    if (!cur) cur = mCards.minFace();
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// whist
///////////////////////////////////////////////////////////////////////////////
// ��� ���� ��� ��� 1 ����� - ���
Card *AiPlayer::MyVist1 (Player *aLeftPlayer, Player *aRightPlayer) {
  // 1 - ������ ������ 2-��������� ����� � ����������� 3-��� ���������� 4-???
  Card *cur = 0;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  Player *aEnemy, *aFriend;
  int mast;
  // ��� ����� � ��� ��������
  if (aLeftPlayer->myGame() != gtPass && aLeftPlayer->myGame() != vist) {
    aEnemy = aLeftPlayer;
    aFriend = aRightPlayer;
  } else {
    aEnemy = aRightPlayer;
    aFriend = aLeftPlayer;
  }
  mast = aEnemy->myGame()-(aEnemy->myGame()/10)*10;
  // �������� ������
  //loadLists(aLeftPlayer,aRightPlayer,aMaxCardList);
  //recalcTables(aMaxCardList,1);
  if (aEnemy == aLeftPlayer) {
    // ����� �����
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 1);
    if (aLeftPlayer->mCards.cardsInSuit(mast) <= 2) {
      Card *m = mCards.maxInSuit(mast);
      if (!m) cur = mCards.minInSuit(aLeftPlayer->mCards.emptySuit (mast));
      else if (aLeftPlayer->mCards.maxInSuit(mast)->face() < m->face()) cur = mCards.maxInSuit(mast);
      else cur = mCards.minInSuit(aLeftPlayer->mCards.emptySuit(mast));
    } else {
      cur = mCards.minInSuit(aLeftPlayer->mCards.emptySuit (mast));
    }
    if (!cur) cur = GetMinCardWithOutVz();
/*
    if ( mCards.exists(FACE_ACE,cur->suit()) || mCards.exists(FACE_KING,cur->suit())) {
      cur = 0;
      cur = mCards.minInSuit(aMaxCardList->emptySuit(0));
    }
*/
    if (!cur) cur = GetMaxCardWithOutPere();
    if (!cur) cur = mCards.minInSuit(mast);
    if (!cur) cur = GetMaxCardPere();
  } else {
    // ����� - ��������
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 1);
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere();
    if (!cur) cur = mCards.maxFace();
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// ��� ���� ��� ��� 2 ����� - ���
Card *AiPlayer::MyVist2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  Player *aEnemy, *aFriend;
  int mast;

  // ��� ����� � ��� ��������
  if (aLeftPlayer->myGame() != gtPass && aLeftPlayer->myGame() != vist) {
    aEnemy = aLeftPlayer;
    aFriend = aRightPlayer;
  } else {
    aEnemy = aRightPlayer;
    aFriend = aLeftPlayer;
  }
  mast = aEnemy->myGame()-(aEnemy->myGame()/10)*10;
  // �������� ������
  if (aEnemy == aLeftPlayer) {
    // ����� �����
    loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 23);
    // �� ���� �� ������� ��� ������� � ����
    if ( aRightCard->face() >= 10 ) cur = mCards.minInSuit(aRightCard->suit());
    else cur = mCards.maxInSuit(aRightCard->suit()); // ����� ����� � ������ ������� � ���� ��� � ������ ����� � ���� ������
    if (!cur) cur = mCards.maxInSuit(mast); // � � ���� ����� � ��� !!! ������� ������������
    if (!cur) cur = GetMinCardWithOutVz();
  } else {
    // ����� - ��������
    loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
    recalcTables(aMaxCardList, 23);
    cur = mCards.greaterInSuit(aRightCard);
    if (!cur) cur = mCards.minInSuit(aRightCard->suit());
    if (!cur) cur = mCards.minInSuit(mast); // ��� ����� ������� ������
    if (!cur) cur = GetMinCardWithOutVz(); // ��� ����� � ������
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// ��� ���� ��� ��� 3 ����� - ���
Card *AiPlayer::MyVist3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  Card *maxInSuit;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  Player *aEnemy, *aFriend;
  int mast;

  // ��� ����� � ��� ��������
  if (aLeftPlayer->myGame() != gtPass && aLeftPlayer->myGame() != vist) {
    aEnemy = aLeftPlayer;
    aFriend = aRightPlayer;
  } else {
    aEnemy = aRightPlayer;
    aFriend = aLeftPlayer;
  }
  mast = aEnemy->myGame()-(aEnemy->myGame()/10)*10;
  // �������� ������
  //loadLists(aLeftPlayer,aRightPlayer,aMaxCardList);
  //recalcTables(aMaxCardList,23);
  if (mCards.cardsInSuit(aLeftCard->suit()))  {
    // � ���� ���� ����� � ������� ����� �����
    if (aEnemy == aLeftPlayer) {
      // ����� - �������
      loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (aLeftCard->suit() == aRightCard->suit()) {
        if (*aRightCard > *aLeftCard) {
          // ���� �� ����
          cur = mCards.minInSuit(aRightCard->suit());
        } else {
          // ������������ �������
          cur = mCards.greaterInSuit(aLeftCard);
          if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
        }
      } else { //aLeftCard->suit() == aRightCard->suit()
        if (aRightCard->suit() != mast)  {
          // �� ���� �������
          cur = mCards.greaterInSuit(aLeftCard);
          if (!cur) cur = mCards.minInSuit(aLeftCard->suit());
        } else {
          // ���� �������, ��� �� ���� ����
          cur = mCards.minInSuit(aLeftCard->suit());
        }
      }
    } else {
      //������ ���������
      loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (aLeftCard->suit() == aRightCard->suit()) {
        if (*aRightCard > *aLeftCard) {
          // ���������� ������� �������
          cur = mCards.greaterInSuit(aRightCard);
          if (!cur) cur = mCards.minInSuit(aRightCard->suit());
        } else {
          // ���� �� ����
          cur = mCards.minInSuit(aRightCard->suit());
        }
      } else {
        if (aRightCard->suit() == mast) {
          // ����� �����, � ������ ������� �������
          cur = mCards.minInSuit(aLeftCard->suit());
        } else {
          // �� �� .... ����� ���� ����� � ������ ������ ������ !!!
          cur = mCards.minInSuit(aLeftCard->suit());
        }
      }
    }
  } else {
    //  � ���� ��� ����� � ������� ����� �����
    if (aLeftCard->suit() == aRightCard->suit()) {
      //k8: wtf?
      //if (*aLeftCard > *aRightCard) maxInSuit = aLeftCard; else maxInSuit = aLeftCard;
      if (*aLeftCard > *aRightCard) maxInSuit = aLeftCard; else maxInSuit = aRightCard;
    } else {
      if (aLeftCard->suit() != mast && aRightCard->suit() != mast) maxInSuit = aLeftCard;
      else maxInSuit = aRightCard;
    }
    if (aEnemy == aLeftPlayer) {
      // ����� �����
      loadLists(aLeftPlayer, aLeftPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (maxInSuit == aLeftCard) {
        // ���������� �������� ��� �����
        cur = mCards.greaterInSuit(aLeftCard);
      } else {
        // ��� ���� ������... ��������� �����������
        cur = mCards.minInSuit(maxInSuit->suit());
      }
    } else {
      // ����� - ��������
      loadLists(aRightPlayer, aRightPlayer, aMaxCardList);
      recalcTables(aMaxCardList, 23);
      if (maxInSuit == aLeftCard) {
        // ��� ���� ������... ��������� �����������
        cur = mCards.minInSuit(maxInSuit->suit());
      } else {
        //���������� �������� ��� �����
        cur = mCards.greaterInSuit(aLeftCard);
      }
    }
    if (!cur) cur = mCards.minInSuit(mast);
    if (!cur) cur = GetMinCardWithOutVz(); // ��� ���.
    if (!cur) cur = mCards.minFace();
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// pass-outs
///////////////////////////////////////////////////////////////////////////////
// my passout first
Card *AiPlayer::MyPass1 (Card *rMove, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  CardList aTmpList, aStackStore;
  int doRest = 0;

  if (rMove != 0)  {
    aTmpList.copySuit(&mCards, (eSuit)rMove->suit());
    aStackStore = mCards;
    mCards = aTmpList;
    doRest = 1;
    //loadLists(aLeftPlayer, aRightPlayer, aMaxCardList); // !!!!!!!!!
    //recalcPassOutTables(aMaxCardList, 1);
  }
  if (tmpGamesType == raspass || tmpGamesType == g86)  { // �������/�����
    loadLists(aLeftPlayer, aRightPlayer, aMaxCardList); // !!!!!!!!!
    recalcPassOutTables(aMaxCardList,1);
    // ���� ����� : 100 % ������ ��ϣ, ���� �������� ��� - ��� � ���������� ������� �����
    cur = GetMaxCardWithOutPere(); // �� ���� ��� ���� ���� ����� ��� ���
    if (cur) {
      if (aLeftPlayer->mCards.cardsInSuit(cur->suit()) == 0 &&
          aRightPlayer->mCards.cardsInSuit(cur->suit()) == 0) {
        cur = 0;
      }
    }
    if (!cur) cur = GetMaxCardPere(); //����� � ����������� (max)
    if (!cur) cur = GetMinCardWithOutVz(); // ������
    if (!cur) cur = mCards.minFace();
  }
  if (doRest) mCards = aStackStore;
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// my passout second
Card *AiPlayer::MyPass2 (Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur = 0;
  eGameBid tmpGamesType = mMyGame;
  int mast = aRightCard->suit();
  CardList aMaxCardList;
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcPassOutTables(aMaxCardList, 1);
  /////////////////////////////////////////////////////////
  if (tmpGamesType == raspass || tmpGamesType == g86) {
    // �������/�����
    cur = mCards.maxInSuit(mast); // !!!!!!!!!!!! ����� ������
    if (!cur) {
      // ��� �����
      // �� ���� !!! ���� ��� � ����� ��� ���� 100% ������
      cur = GetMaxCardWithOutPere();
      if (!cur) cur = GetMaxCardPere(); //����� � ����������� (max)
      if (!cur) cur = GetMinCardWithOutVz(); // ������
      if (!cur) cur = mCards.minFace();
    } else {
      // � ���� ���� �����
      if (aLeftPlayer->mCards.cardsInSuit(mast)) {
        //� ������ ����
        cur = mCards.lesserInSuit(aLeftPlayer->mCards.maxInSuit(mast));
        if (!cur) cur = mCards.maxInSuit(mast);
      } else {
        // � ������ ���
        cur = mCards.lesserInSuit(aRightCard);
        if (!cur) cur = mCards.maxInSuit(mast);
      }
    }
  }
  /////////////////////////////////////////////////////////
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// my passout third
Card *AiPlayer::MyPass3 (Card *aLeftCard, Card *aRightCard, Player *aLeftPlayer, Player *aRightPlayer) {
  Card *cur=0;
  /*Card *maxInSuit;*/
  eGameBid tmpGamesType = mMyGame;
  CardList aMaxCardList;
  //int mast = mMyGame-(mMyGame/10)*10;
  // �������� ������
  loadLists(aLeftPlayer, aRightPlayer, aMaxCardList);
  recalcPassOutTables(aMaxCardList, 23);
  if (mCards.cardsInSuit(aLeftCard->suit()))  {
    // � ���� ���� �����  � ������� ����� �����
    // ������������ ����������
    if (aLeftCard->suit() == aRightCard->suit()) {
      if (*aRightCard > *aLeftCard) cur = mCards.lesserInSuit(aRightCard);
      else cur = mCards.lesserInSuit(aLeftCard);
    } else cur = mCards.lesserInSuit(aLeftCard);
    if (!cur) cur = mCards.maxInSuit(aLeftCard->suit());
  } else {
    // � ���� ��� ����� � ������� ����� �����
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); //����� � ����������� (max)
    if (!cur) cur = GetMinCardWithOutVz(); // ������
    if (!cur) cur = mCards.minFace();
  }
  mMyGame = tmpGamesType;
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// game moves
///////////////////////////////////////////////////////////////////////////////
// make game move (dispatcher)
Card *AiPlayer::moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) {
  Q_UNUSED(isPassOut)
  Card *cur = 0;
  if (lMove == 0 && rMove == 0) {
    // ��� ����� - ������
    if (mMyGame == gtPass || mMyGame == vist) cur = MyVist1(aLeftPlayer, aRightPlayer); // ���-�� ������ � � ��� �� ������
    else if (mMyGame == g86catch) cur = MiserCatch1(aLeftPlayer, aRightPlayer);
    else if (mMyGame == g86) cur = Miser1(aLeftPlayer,aRightPlayer);
    else if (mMyGame == raspass) cur = MyPass1(rMove, aLeftPlayer, aRightPlayer); // �� ���� ������� ��� �����
    else cur = MyGame1(aLeftPlayer, aRightPlayer); // �� ���� ��� ����
  }
  if (lMove == 0 && rMove != 0) {
    // ��� ����� - ������
    if (mMyGame == gtPass || mMyGame == vist) cur = MyVist2(rMove, aLeftPlayer, aRightPlayer); // ���-�� ������ � � ��� �� ������
    else if (mMyGame == g86catch) cur = MiserCatch2(rMove, aLeftPlayer, aRightPlayer);
    else if (mMyGame == g86) cur = Miser2(rMove, aLeftPlayer, aRightPlayer);
    else if (mMyGame == raspass) cur = MyPass2(rMove, aLeftPlayer, aRightPlayer); // �� ���� ������� ��� �����
    else cur = MyGame2(rMove, aLeftPlayer, aRightPlayer); // �� ���� ��� ����
  }
  if (lMove != 0 && rMove != 0) {
    // ��� ����� - 3
    if (mMyGame == gtPass || mMyGame == vist ) cur = MyVist3(lMove, rMove, aLeftPlayer, aRightPlayer); // ���-�� ������ � � ��� �� ������
    else if (mMyGame == g86catch) cur = MiserCatch3(lMove, rMove, aLeftPlayer, aRightPlayer);
    else if (mMyGame == g86) cur = Miser3(lMove, rMove, aLeftPlayer, aRightPlayer);
    else if (mMyGame == raspass) cur = MyPass3(lMove, rMove, aLeftPlayer, aRightPlayer); // �� ���� ������� ��� �����
    else cur = MyGame3(lMove, rMove, aLeftPlayer, aRightPlayer); // �� ���� ��� ����
  }
  mCards.remove(cur);
  mCardsOut.insert(cur);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// ����� ��������� ������� ������� - ���� ��� ����
eGameBid AiPlayer::moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist) {
  Q_UNUSED(nGamerVist)
  eGameBid Answer;
  eGameBid MyMaxGame = moveCalcDrop();

  int vz = MyMaxGame/10;
  Answer = (HaveAVist != vist && vz >= gameWhistsMin(MaxGame)) ? vist : gtPass ;
  if (HaveAVist == gtPass && vz < gameWhistsMin(MaxGame)) Answer = gtPass;
  if (optStalingrad && MaxGame == g61) Answer = vist; //STALINGRAD !!!
  if (MaxGame == g86) Answer = g86catch; // miser
  mMyGame = Answer;
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
    // �������������� ������ - ����� ������� �����
    if (mCards.cardsInSuit(f) > nMaxMastLen) {
      nMaxMastLen = mCards.cardsInSuit(f);
      nMaxMast = (eSuit)f;
    }
  }
  for (f = 1; f <= 4; f++) {
    if (mCards.cardsInSuit(f) == nMaxMastLen && nMaxMast != f) {
      // ���� ����������� ������� �����
      if (LocalMastTable[f].sum > LocalMastTable[nMaxMast].sum) nMaxMast = (eSuit)f;
    }
  }
  //����� � ������ ��� ���������
  GamesTypeRetVal = (eGameBid)(LocalMastTable[0].tricks*10+nMaxMast);
  // ��� ��, ��� �� ����� ������ �����������
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
        AiPlayer *tmpGamer = new AiPlayer(99);
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
        AiPlayer *tmpGamer = new AiPlayer(99);
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
  //Hight = moveBidding(undefined,undefined);
  Hight = moveCalcDrop();
  if (Hight < mMyGame)  {
    // ��� ����� ������
    int tmpvz = Hight/10, tmpmast = Hight-tmpvz*10;
    while (tmpHight < mMyGame)  {
      tmpvz++;
      tmpHight = (eGameBid)(tmpvz*10+tmpmast);
    }
    mMyGame = tmpHight;
  } else {
    mMyGame = Hight;
  }
  return mMyGame;
}


//��� ��� ��������
eGameBid AiPlayer::moveBidding (eGameBid lMove, eGameBid rMove) {
/*
  if ( mMyGame == gtPass )  {
    mMyGame=gtPass;
  } else {
*/
  //mMyGame = gtPass;
  if (mMyGame != gtPass)  {
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
    if (optAggPass && optPassCount > 0) {
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
    // �������������� ������ -- ����� ������� �����
    for (f = 1; f <= 4; f++) {
      if (mCards.cardsInSuit(f) > nMaxMastLen) {
        nMaxMastLen = mCards.cardsInSuit(f);
        nMaxMast = (eSuit)f;
      }
    }
    for (f = 1; f <= 4; f++) {
      if (mCards.cardsInSuit(f) == nMaxMastLen && nMaxMast != f) {
        // ���� ����������� ������� �����
        if (LocalMastTable[f].sum > LocalMastTable[nMaxMast].sum) nMaxMast = (eSuit)f;
      }
    }
    // ����� � ������ ��� ���������
    if (curMaxGame <= g75) {
      mMyGame = (eGameBid)((LocalMastTable[0].tricks+1)*10+nMaxMast);
    } else {
      mMyGame = (eGameBid)((LocalMastTable[0].tricks)*10+nMaxMast);
    }
    if ((rMove == gtPass || rMove == undefined) && (lMove == gtPass || lMove == undefined)) {
      if (LocalMastTable[0].tricks >= 4) {
        /*if (optAggPass && optPassCount > 0) mMyGame = g71;
        else*/ mMyGame = g61;
      } else {
        if (LocalMastTable[0].tricks == 0)  {
          // check for misere !!!
          mMyGame = checkForMisere() ? g86 : gtPass ;
        } else mMyGame = gtPass;
      }
    } else if (mMyGame >= curMaxGame) {
      mMyGame = (eGameBid)succBid(curMaxGame);
    } else {
      mMyGame = gtPass;
    }
/*
    if( mMyGame >= curMaxGame  ) {
        mMyGame = (eGameBid) succBid(curMaxGame);
    } else {
    }*/
    // ��� ��, ��� �� ����� ������ �����������
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
  if (optAggPass && optPassCount > 0 && mMyGame != gtPass && mMyGame < g71) {
    moveBidding(g71, g71);
    if (optAggPass && optPassCount > 0 && mMyGame != gtPass && mMyGame < g71) moveBidding(g72, g72);
  }
  return mMyGame;
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
