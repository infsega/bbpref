#include <stdlib.h>
#include <stdio.h>

#include <QPixmap>
#include <QPainter>

#include "player.h"


Player::Player (int _nGamer) {
  mPlayerNo = _nGamer;
  DeskView = 0;
  internalInit();
}


Player::Player (int _nGamer, TDeskView *aDeskView) {
  mPlayerNo = _nGamer;
  DeskView = aDeskView;
  internalInit();
}


Player::~Player () {
  delete aCards;
  delete aLeft;
  delete aRight;
  delete aOut;
  delete aCardsOut;
  delete aLeftOut;
  delete aRightOut;
  delete aScore;
}


void Player::internalInit () {
  aScore = new ScoreBoard;
  aCards = new CardList; // мои
  aLeft = new CardList;  // противника слева (предполагаемый или открытые)
  aRight = new CardList; // справа (предполагаемый или открытые)
  aOut = new CardList;   // снос (мой или предполагаемый)
  aCardsOut = new CardList; // во взятках мои
  aLeftOut = new CardList;  // во взятках противника слева (предполагаемый или открытые)
  aRightOut = new CardList; // во взятках справа (предполагаемый или открытые)
  NikName = QString();
  clear();
}


void Player::clear () {
  flMiser = 0;
  aCards->clear();
  aLeft->clear();
  aRight->clear();
  aOut->clear();
  aCardsOut->clear();
  aLeftOut->clear();
  aRightOut->clear();
  Mast = SuitNone;
  GamesType = undefined;
  Enemy = NoHand;
  nGetsCard = 0;
  WaitForMouse = 0;
  oldii = -1;
  for (int f = 0; f <= 4; f++)  {
    suitProb[f].vzatok = 0;
    suitProb[f].perehvatov = 0;
  }
  nInvisibleHand = (mPlayerNo != 1);
  Pronesti = 0;
}


void Player::AddCard (Card *aCard) {
  aCards->insert(aCard);
}


void Player::AddCard (int _CName, int _CMast) {
  aCards->insert(newCard(_CName, _CMast));
}


void Player::sortCards () {
  aCards->mySort();
}


///////////////////////////////////////////////////////////////////////////////
// utils
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// масть с перехватами (max)
Card *Player::GetMaxCardWithOutPere () {
  int index = 0, vz = 0, pere = 0;
  for (int f = 1; f <= 4; f++) {
    if (suitProb[f].vzatok > vz || (suitProb[f].vzatok == vz && suitProb[f].perehvatov < pere)) {
      index = f;
      vz = suitProb[f].vzatok;
      pere = suitProb[f].perehvatov;
    }
  }
  if (index) return aCards->maxInSuit(index);
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// ???
Card *Player::GetMinCardWithOutVz () {
  int index = 0;
  double koef = 0, koef1 = 0;
  for (int f = 1; f <= 4; f++) {
    koef1 = suitProb[f].len+8/(1+suitProb[f].vzatok);
    if ((koef1 > koef && suitProb[f].len != 0) || (suitProb[f].vzatok == 0 && suitProb[f].len > 0)) {
      index = f;
      koef =koef1;
    }
  }
  if (index && index <=4) return aCards->minInSuit(index);
  return aCards->minFace();
}


void Player::LoadLists (Player *aLeftGamer, Player *aRightGamer, CardList *aMaxCardList) {
/*
  int nLeftVisible = aLeftGamer->nCardsVisible,nRightVisible = aRightGamer->nCardsVisible;
  int nCards = aCards->count();
*/
  aLeft->clear();
  aRight->clear();
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
    aLeft->shallowCopy(aLeftGamer->aCards);
    aRight->shallowCopy(aRightGamer->aCards);
/*
  }
*/
// Get Max List only len
/*
  for (int m = 1; m <= 4; m++) {
    for (int c = FACE_ACE; c >= 7; c--) {
      if (aLeft->exists(c, m) || aRight->exists(c, m)) {
        aMaxCardList->insert(newCard(c, m));
      }
    }
  }
*/
  for (int m = 1; m <= 4; m++) {
    Card *LeftMax,*RightMax,*Max=0;
    while (aLeft->cardsInSuit(m) || aRight->cardsInSuit(m)) {
      LeftMax = aLeft->maxInSuit(m);
      RightMax = aRight->maxInSuit(m);
      if (LeftMax == 0 && RightMax == 0) {
        Max = 0;
      } else if (LeftMax == 0 && RightMax != 0) {
        Max = RightMax;
        aRight->remove(Max);
      } else if (LeftMax != 0 && RightMax == 0) {
        Max = LeftMax;
        aLeft->remove(Max);
      } else if (*LeftMax > *RightMax) {
        Max = LeftMax;
        aLeft->remove(LeftMax);
        aRight->remove(RightMax);
      } else {
        Max = RightMax;
        aLeft->remove(LeftMax);
        aRight->remove(RightMax);
      }
    }
    if (Max != 0) aMaxCardList->insert(newCard(Max->face(), Max->suit()));
  }
  aLeft->clear();
  aRight->clear();
  aLeft->shallowCopy(aLeftGamer->aCards);
  aRight->shallowCopy(aRightGamer->aCards);
}


///////////////////////////////////////////////////////////////////////////////
// game mechanics
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// misere, my move is first
Card *Player::Miser1 (Player *aLeftGamer, Player *aRightGamer) {
  Card *cur = 0;
  if (aCards->count() == 10) {
    // первый выход в 8..туз если она одна
    for (int m = 1; m <= 4; m++) {
      if (aCards->cardsInSuit(m) == 1) {
        cur = aCards->minInSuit(m);
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
        my = aCards->exists(c, m);
        if (my) {
          int matrixindex = 0;
          matrixindex += aLeftGamer->aCards->lesserInSuit(my) ? 1:0; matrixindex <<= 1;
          matrixindex += aLeftGamer->aCards->greaterInSuit(my) ? 1:0; matrixindex <<= 1;
          matrixindex += aRightGamer->aCards->lesserInSuit(my) ? 1:0; matrixindex <<= 1;
          matrixindex += aRightGamer->aCards->greaterInSuit(my) ? 1:0;
          if (matrixindex == 1 || matrixindex == 9 || matrixindex == 13 || (matrixindex >= 4 && matrixindex <= 7)) {
            cur = my;
          }
        }
      }
    }
  }
  if (!cur) cur = aCards->minInSuit(1);
  if (!cur) cur = aCards->minInSuit(2);
  if (!cur) cur = aCards->minInSuit(3);
  if (!cur) cur = aCards->minInSuit(4);
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere, my move is second
Card *Player::Miser2 (Card *aRightCard, Player *aLeftGamer, Player *aRightGamer) {
  Card *cur = 0;
  if (aCards->cardsInSuit(aRightCard->suit())) {
    for (int c = FACE_ACE; c >= 7; c--) {
      if (cur) break;
      Card *my;
      //,*leftmax,*leftmin,*rightmax,*rightmin;
      my = aCards->exists(c, aRightCard->suit());
      if (my) {
        int matrixindex = 0;
        matrixindex += aLeftGamer->aCards->lesserInSuit(my) ? 1:0; matrixindex <<= 1;
        matrixindex += aLeftGamer->aCards->greaterInSuit(my) ? 1:0; matrixindex <<= 1;
        matrixindex += (*aRightCard < *my) ? 1:0; matrixindex <<= 1;
        matrixindex += (*aRightCard > *my) ? 1:0;
        if (matrixindex == 1 || matrixindex == 9 || matrixindex==13 || (matrixindex >= 4 && matrixindex <= 7)) {
          cur = my;
        }
      }
    }
    if (!cur) cur = aCards->maxInSuit(aRightCard->suit());
  } else {
    CardList *aMaxCardList = new CardList;
    LoadLists(aRightGamer, aLeftGamer, aMaxCardList);
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); // масть с перехватами (max)
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = aCards->maxFace();
    delete aMaxCardList;
  }
  return cur;
}


///////////////////////////////////////////////////////////////////////////////
// misere, my move is third
Card *Player::Miser3 (Card *aLeftCard, Card *aRightCard, Player *aLeftGamer, Player *aRightGamer) {
  // copy from MyPass3
  Card *cur = 0;
  eGameBid tmpGamesType = GamesType;
  CardList *aMaxCardList=new CardList;
  //int mast = GamesType-(GamesType/10)*10;
  // набираем списки
  LoadLists(aLeftGamer, aRightGamer, aMaxCardList);
  RecountTables4RasPass(aMaxCardList, 23);
  if (aCards->cardsInSuit(aLeftCard->suit()))  {
    // у меня есть масть в которую зашел левый; постараемся пропустить
    if (aLeftCard->suit() == aRightCard->suit()) {
      cur = (*aRightCard > *aLeftCard) ?
        aCards->lesserInSuit(aRightCard) :
        aCards->lesserInSuit(aLeftCard);
    } else cur = aCards->lesserInSuit(aLeftCard);
    if (!cur) cur = aCards->maxInSuit(aLeftCard->suit());
  } else {
    // у меня нет масти в которую зашел левый
    cur = GetMaxCardWithOutPere();
    if (!cur) cur = GetMaxCardPere(); // масть с перехватами (max)
    if (!cur) cur = GetMinCardWithOutVz(); // лабуду
    if (!cur) cur = aCards->minFace();
  }
  GamesType = tmpGamesType;
  delete aMaxCardList;
  return cur;
}


//-----------------------------------------------------------------------
tSuitProbs Player::vzatok(eSuit Mast,CardList *aMaxCardList,int a23) {
   tSuitProbs suitProb;
   Card *MyCard,*tmpCard;
   CardList *MyCardStack     = new CardList;
   CardList *EnemyCardStack  = new CardList;
   for (int c=7;c<=FACE_ACE;c++ ) {
        MyCard = aCards->exists(c, Mast);
        if ( MyCard ) {
          MyCardStack->insert(MyCard);
        }
        MyCard = aMaxCardList->exists(c, Mast);
        if ( MyCard ) {
          EnemyCardStack->insert(MyCard);
        }
   }
   if (a23 != 23) {
       suitProb = Compare2List4Max(MyCardStack,EnemyCardStack);
   } else {
       suitProb = Compare2List4Max23(MyCardStack,EnemyCardStack);
   }
   suitProb.len = aCards->cardsInSuit(Mast);
   suitProb.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->exists(j,Mast);
    if (  tmpCard ) {
      suitProb.sum +=  tmpCard->face();
    }
   }
   MyCardStack->clear();
   EnemyCardStack->clear();
   delete MyCardStack;
   delete EnemyCardStack;
   return suitProb;
}
//-----------------------------------------------------------------------
void Player::RecountTables4RasPass(CardList *aMaxCardList,int a23) { // Пересчитывает таблицу дли распасов или мизера
  Q_UNUSED(a23)
    int f;
    suitProb[0].vzatok=0;
    suitProb[0].perehvatov=0;
    suitProb[0].sum=0;
    for (f=1; f<=4; f++ )  {
      suitProb[f] = vzatok4pass( (eSuit) f,aMaxCardList );
      suitProb[0].vzatok += suitProb[f].vzatok ;
      suitProb[0].perehvatov += suitProb[f].perehvatov ;
      suitProb[0].sum += suitProb[f].sum;
    }

}
//-----------------------------------------------------------------------
void Player::RecountTables( CardList *aMaxCardList,int a23 ){ // Пересчитывает таблицу         tSuitProbs suitProb[5];
    int f;
    suitProb[0].vzatok=0;
    suitProb[0].perehvatov=0;
    suitProb[0].sum=0;
    for (f=1; f<=4; f++ )  {
      suitProb[f] = vzatok ( (eSuit) f,aMaxCardList, a23 );
      suitProb[0].vzatok += suitProb[f].vzatok ;
      suitProb[0].perehvatov += suitProb[f].perehvatov ;
      suitProb[0].sum += suitProb[f].sum;
    }
}
//-----------------------------------------------------------------------
Card *Player::MiserCatch1(Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=0;
  CardList *aMaxCardList=new CardList;
  CardList *Naparnik;
  eHand Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer->aCards;
    Side = RightHand;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer->aCards;
    Side = LeftHand;
  }
  RecountTables4RasPass(aMaxCardList,1);
//  if (Side == LeftHand) {
// 1. Пытаемся всунуть
    for (int m=1;m<=4;m++) {
      Card *EnemyMin = aMaxCardList->minInSuit(m);
      Card *NaparnikMin = Naparnik->minInSuit(m);
      Card *MyMin = aCards->minInSuit(m);
      if (cur)
        goto badlabel;
      if (EnemyMin) { //есть такая
        if (MyMin) { // и у меня есть такая
          if (NaparnikMin) { // и у напарника есть такая
            if ( *NaparnikMin < *EnemyMin && *MyMin < *EnemyMin) {
              cur = MyMin;// Всунули
              goto badlabel;
            }
            if ( *NaparnikMin > *EnemyMin && *MyMin < *EnemyMin) {
              // надо н-ти такую масть по которой он может пронести а
              // но должен вернутся ко мне
              for (int k=1;k<=4;k++)  {
                if (k==m || aMaxCardList->cardsInSuit(k)==0)
                  continue;
                Card *EnemyMax = aMaxCardList->maxInSuit(k);
                Card *NaparnikMax = Naparnik->maxInSuit(k);
                Card *MyMax = aCards->maxInSuit(k);
                if ( MyMax && NaparnikMax && EnemyMax) {
                  if (Naparnik->cardsInSuit(k) < aCards->cardsInSuit(k) && Naparnik->cardsInSuit(k) < aMaxCardList->cardsInSuit(k) ) {
                    cur = aCards->lesserInSuit(EnemyMax);
//                    MyMax; // на этой он должен пронести NaparnikMax
                    if (aRightGamer->GamesType == g86) {
                        aRightGamer->Pronesti = NaparnikMax;
                      }   else {
                        aLeftGamer->Pronesti = NaparnikMax;
                      }
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


//    2 отгребаем свое
    if (!cur) cur = GetMaxCardWithOutPere();
    if (Side == LeftHand) {
      // под мизирящегося надо заходить по другому !!!
    }
// 3    пытаемс
    if (!cur) cur = GetMaxCardWithOutPere();
    if (!cur) cur = aCards->minFace();
//  } else { //Side == LeftHand
//  }
badlabel:

  aMaxCardList->clear();
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::MiserCatch2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=0;
  CardList *aMaxCardList=new CardList;
  CardList *Naparnik;
  eHand Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer->aCards;
    Side = RightHand;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer->aCards;
    Side = LeftHand;
  }
  RecountTables4RasPass(aMaxCardList,23);

  if (Side == LeftHand) { //ход под меня , противник с права
      if (aCards->cardsInSuit(aRightCard->suit())) {
        // у меня есть карты  в данной масти
        Card *MyMax = aCards->maxInSuit(aRightCard->suit());
        //Card *MyMin = aCards->minInSuit(aRightCard->suit());
        //Card *EnMax = aMaxCardList->maxInSuit(aRightCard->suit());
        Card *EnMin = aMaxCardList->minInSuit(aRightCard->suit());
        if (EnMin) {
          // у противника есть карты в данной масти
          if (*aRightCard < *EnMin) {
            // можно попытаться всунуть
            cur=aCards->lesserInSuit(EnMin);
            if(!cur) cur = aCards->greaterInSuit(EnMin);
          } else {
          cur= aCards->greaterInSuit(EnMin);
          }
        } else {
          cur = MyMax; // А вот тут надо смотреть кому нужен ход
        }
      } else {
        // у меня нет карт  в данной масти
        if (Pronesti) {
          //надо пронести карту
          if (aCards->exists(Pronesti->face(),Pronesti->suit())) {
            cur = Pronesti;
          } else {
            cur = aCards->maxInSuit(Pronesti->suit());
            if (!cur)   cur = aCards->maxFace();
          }
        } else {
          // указания на пронос - нет
          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxFace();
        }
      }
  } else { // противник зашел под меня
    Card *MyMax = aCards->maxInSuit(aRightCard->suit());
    Card *MyMin = aCards->minInSuit(aRightCard->suit());
    if (MyMax) {

      if ( *MyMin < *aRightCard) {
//        есть возможность всунуть
        Card *NapMin = Naparnik->lesserInSuit(aRightCard);
        if (NapMin) {
          cur = aCards->lesserInSuit(aRightCard);
        } else {
          cur = MyMax; // кому передать код ?
        }
      } else {
        cur = MyMax; // грохать ее самой большой
      }

    } else {
      // а у меня нет масти

          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxFace();
    }

  }
  Pronesti = 0;
  if (!cur)
    cur = aCards->minFace();

  aMaxCardList->clear();
  delete aMaxCardList;
  return cur;

}
//-----------------------------------------------------------------------
Card *Player::MiserCatch3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=0;
  CardList *aMaxCardList=new CardList;
  CardList *Naparnik;
  eHand Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer->aCards;
    Side = RightHand;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer->aCards;
    Side = LeftHand;
  }
  RecountTables4RasPass(aMaxCardList,23);

  if (Side == LeftHand) {
      if (aCards->cardsInSuit(aLeftCard->suit())) {
        // у меня есть карты  в данной масти
//        Card *MyMax = aCards->maxInSuit(aLeftCard->suit());
//        Card *MyMin = aCards->minInSuit(aLeftCard->suit());
        if (*aLeftCard < *aRightCard) {
          // напарник уже придавил карту
          cur = aCards->maxInSuit(aLeftCard->suit());

        } else {
          //есть шанс всунуть
          cur = aCards->lesserInSuit(aLeftCard);
          if (!cur ) cur = aCards->maxInSuit(aLeftCard->suit());
        }
      } else {
        // у меня нет карт  в данной масти
        if (Pronesti) {
          //надо пронести карту
          if (aCards->exists(Pronesti->face(),Pronesti->suit())) {
            cur = Pronesti;
          } else {
            cur = aCards->maxInSuit(Pronesti->suit());
            if (!cur) cur = aCards->maxFace();
          }
        } else {
          // указания на пронос - нет
          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxFace();
        }
      }
  } else { // RightHand - my friend
    Card *MyMax = aCards->maxInSuit(aLeftCard->suit());
    Card *MyMin = aCards->minInSuit(aLeftCard->suit());
    if (MyMax) {
      if ( *MyMin < *aLeftCard) {
//        есть возможность всунуть
        Card *NapMin = Naparnik->lesserInSuit(aLeftCard);
        if (NapMin) {
          cur = aCards->lesserInSuit(aLeftCard);
        } else {
          cur = MyMax; // кому передать код ?
        }
      } else {
        cur = MyMax; // грохать ее самой большой
      }

    } else {
      // а у меня нет масти
          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxFace();
    }

  }
  Pronesti = 0;

  aMaxCardList->clear();
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::MyGame3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // моя игра 3 заход - мой
    Card *cur=0;
    Card *maxInSuit;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // у меня есть масть  в которую зашел левый
        if ( aLeftCard->suit() == aRightCard->suit())  {
            // постараемсмя ударить
            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur  ) {
               cur = aCards->minInSuit(aLeftCard->suit());
            }
        } else { //aLeftCard->suit() == aRightCard->suit()
          if ( aRightCard->suit() !=mast)  {
            // не бита козырем

            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur ) {
              cur = aCards->minInSuit(aLeftCard->suit());
            }
          } else {
            // бита козырем, мне не надо бить
            cur = aCards->minInSuit(aLeftCard->suit());
          }
        }
    } else {
      //  у меня нет масти в которую зашел левый
       if (aLeftCard->suit()==aRightCard->suit()) {
           if ( *aLeftCard > *aRightCard ) {
               maxInSuit = aLeftCard;
           } else {
               maxInSuit = aLeftCard;
           }
       } else {
           if ( aLeftCard->suit() != mast && aRightCard->suit() != mast) {
               maxInSuit = aLeftCard;
           } else {
               maxInSuit = aRightCard;
           }
       }
       cur = aCards->greaterInSuit(maxInSuit);
       if (!cur) {
           cur = aCards->minInSuit(mast);
       }
       if ( ! cur ) {// нет коз.
           cur = GetMinCardWithOutVz();
       }
       if ( ! cur ) {
           cur = aCards->minFace();
       }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------
Card *Player::MyVist3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // мой вист или пас 3 заход - мой
    Card *cur=0;
    Card *maxInSuit;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    Player *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer->GamesType != gtPass && aLeftGamer->GamesType != vist ) {
        aEnemy = aLeftGamer;
        aFriend = aRightGamer;
    } else {
        aEnemy = aRightGamer;
        aFriend = aLeftGamer;
    }
    mast = aEnemy->GamesType - (aEnemy->GamesType/10) * 10;
    // набираем списки
//    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
//    RecountTables(aMaxCardList,23);
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // у меня есть масть  в которую зашел левый
      if ( aEnemy == aLeftGamer ) { // левый - врыжина
        LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);

        if ( aLeftCard->suit() == aRightCard->suit())  {
          if ( *aRightCard > *aLeftCard ) {
            // бить не надо
            cur = aCards->minInSuit(aRightCard->suit());
          } else {
            // постараемсмя ударить
            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur  ) {
               cur = aCards->minInSuit(aLeftCard->suit());
            }
          }
        } else { //aLeftCard->suit() == aRightCard->suit()
          if ( aRightCard->suit() !=mast)  {
            // не бита козырем
            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur ) {
              cur = aCards->minInSuit(aLeftCard->suit());
            }
          } else {
            // бита козырем, мне не надо бить
            cur = aCards->minInSuit(aLeftCard->suit());
          }
        }

      } else {
        //правый вражииина
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        if ( aLeftCard->suit() == aRightCard->suit()  )  {
          if ( *aRightCard > *aLeftCard ) {
            // попытаться прибить правого
            cur = aCards->greaterInSuit(aRightCard);
            if ( !cur ) {
              cur = aCards->minInSuit(aRightCard->suit());
            }
          } else {
            // бить не надо
            cur = aCards->minInSuit(aRightCard->suit());
          }
        } else {
          if ( aRightCard->suit()==mast  ) {
            // лвевый зашел, а правый пригрел козырем
            cur = aCards->minInSuit(aLeftCard->suit());
          } else { // Ух ты .... Левый друг зашел а правый пронес ерунду !!!
            cur = aCards->minInSuit(aLeftCard->suit());
          }
        }

      }
    } else {
      //  у меня нет масти в которую зашел левый
       if (aLeftCard->suit()==aRightCard->suit()) {
           if ( *aLeftCard > *aRightCard ) {   maxInSuit = aLeftCard;
           } else { maxInSuit = aLeftCard; }
       } else {
           if ( aLeftCard->suit() != mast && aRightCard->suit() != mast) {               maxInSuit = aLeftCard;
           } else { maxInSuit = aRightCard;
           }
       }
       if ( aEnemy == aLeftGamer ) { // С лева игрок
            LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);

          RecountTables(aMaxCardList,23);

           if (maxInSuit == aLeftCard ) { //попытаемся хлопнуть его карту
               cur = aCards->greaterInSuit(aLeftCard);
           } else { // Уже наша взятка... скидываем минимальную
               cur = aCards->minInSuit(maxInSuit->suit());
           }
       } else { // с лева - напарник
          LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,23);

           if (maxInSuit == aLeftCard ) { // Уже наша взятка... скидываем минимальную
               cur = aCards->minInSuit(maxInSuit->suit());
           } else {//попытаемся хлопнуть его карту
               cur = aCards->greaterInSuit(aLeftCard);
           }
       }
       if (!cur) {
           cur = aCards->minInSuit(mast);
       }
       if ( ! cur ) {// нет коз.
           cur = GetMinCardWithOutVz();
       }
       if ( ! cur ) {
           cur = aCards->minFace();
       }


    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------

Card *Player::MyVist2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // мой вист или пас 2 заход - мой
    Card *cur=0;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    Player *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer->GamesType != gtPass && aLeftGamer->GamesType != vist ) {
        aEnemy = aLeftGamer;
        aFriend = aRightGamer;
    } else {
        aEnemy = aRightGamer;
        aFriend = aLeftGamer;
    }
    mast = aEnemy->GamesType - (aEnemy->GamesType/10) * 10;
    // набираем списки
    if ( aEnemy == aLeftGamer ) { // С лева игрок
        LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        // Ну если он заходил под вистуза с туза
        if ( aRightCard->face() >= 10 ) {
            cur = aCards->minInSuit(aRightCard->suit());
        } else { // может умный и больше заходов у него нет в данную масть а есть козырь
            cur = aCards->maxInSuit(aRightCard->suit());
        }
        if ( !cur ) { // а у меня масти и нет !!!
            cur = aCards->maxInSuit(mast); // прибъем максимальной
        }
        if ( ! cur ) {
            cur = GetMinCardWithOutVz();
        }
    } else { // с лева - напарник
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        cur = aCards->greaterInSuit(aRightCard);
        if (!cur) {
            cur = aCards->minInSuit(aRightCard->suit());
        }
        if (!cur) { // Нет масти пробуем козырь
            cur = aCards->minInSuit(mast);
        }
        if (!cur) { // Нет масти и козыря
            cur = GetMinCardWithOutVz();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------
Card *Player::MyVist1(Player *aLeftGamer,Player *aRightGamer) {
// 1 - выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
    Card *cur=0;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    Player *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer->GamesType != gtPass && aLeftGamer->GamesType != vist ) {
        aEnemy = aLeftGamer;
        aFriend = aRightGamer;
    } else {
        aEnemy = aRightGamer;
        aFriend = aLeftGamer;

    }
    mast = aEnemy->GamesType - (aEnemy->GamesType/10) * 10;
    // набираем списки
//    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
//    RecountTables(aMaxCardList,1);
    if ( aEnemy == aLeftGamer ) { // С лева игрок
          LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
            RecountTables(aMaxCardList,1);
            if ( aLeftGamer->aCards->cardsInSuit(mast) <=2  ) {
          Card *m =aCards->maxInSuit(mast);
          if (!m) {
                cur = aCards->minInSuit(aLeftGamer->aCards->emptySuit (mast));
              } else {
                if (aLeftGamer->aCards->maxInSuit(mast)->face() < m->face()) {
                  cur = aCards->maxInSuit(mast);
                } else {
                  cur = aCards->minInSuit(aLeftGamer->aCards->emptySuit (mast));
                }
              }
            } else {
                  cur = aCards->minInSuit(aLeftGamer->aCards->emptySuit (mast));
            }
        if ( !cur) {
          cur = GetMinCardWithOutVz();
        }

/*        if ( aCards->exists(FACE_ACE,cur->suit()) || aCards->exists(FACE_KING,cur->suit()) ) {
          cur = 0;
            cur = aCards->minInSuit(aMaxCardList->emptySuit (0));
        }*/


        if ( !cur) {
            cur = GetMaxCardWithOutPere();
        }
        if ( !cur) {
            cur = aCards->minInSuit(mast);
        }
        if ( !cur) {
            cur = GetMaxCardPere();
        }
    } else { // с лева - напарник
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,1);

        cur = GetMaxCardWithOutPere();
        if ( !cur) {
            cur = GetMaxCardPere();
        }
        if ( !cur) {
            cur = aCards->maxFace();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}

//-----------------------------------------------------------------------
Card *Player::MyGame2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
    CardList *aMaxCardList=new CardList;
    eGameBid tmpGamesType=GamesType;
    Card *cur=0;
    Card *MaxLeftCard=0;
    int mast = aRightCard->suit();
    int koz  = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);

    cur = aCards->maxInSuit(mast);
    if ( !cur ) { // Нет Масти
        if ( !aLeftGamer->aCards->cardsInSuit(mast) ) { // У левого тож нету
            MaxLeftCard = aLeftGamer->aCards->maxInSuit(koz);
            cur = aCards->greaterInSuit(MaxLeftCard); //
            if (!cur) {
                cur = aCards->lesserInSuit(MaxLeftCard);
            }
            if ( !cur ) {
                cur = GetMinCardWithOutVz(); // лабуду
            }
            if ( !cur ) {
                 cur = aCards->minFace();
            }
        } else { // Есть масть у левого
            cur = aCards->minInSuit(koz);
        }
    } else { // У меня есть масть
        if ( aLeftGamer->aCards->cardsInSuit(mast) ) { //У левого есть
            MaxLeftCard = aLeftGamer->aCards->maxInSuit(mast);
            cur = aCards->greaterInSuit(MaxLeftCard);
            if ( !cur ) {
                cur = aCards->lesserInSuit(MaxLeftCard);
            }
        } else { // У левого нет
            if ( !aLeftGamer->aCards->cardsInSuit(koz) ) { // И козыре й нет
                cur = aCards->greaterInSuit(aRightCard);
                if ( !cur) {
                    cur = aCards->minInSuit(mast);
                }
            } else { // есть у левого козыря
                cur = aCards->minInSuit(mast);
            }
        }
    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}
//-----------------------------------------------------------------------
Card *Player::MyGame1(Player *aLeftGamer,Player *aRightGamer) {
// 1-выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
    Card *cur=0;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,1);
    if ( aLeft->cardsInSuit(mast) || aRight->cardsInSuit(mast) ) { // У противников есть козыря
        cur = aCards->maxInSuit(mast);
        if ( !cur ) { // а у меня их нет !!!
             cur = GetMaxCardWithOutPere(); // без пере
        }
        if ( !cur ) {
             cur = GetMaxCardPere();  //масть с перехватами (max)
        }
        if ( !cur ) {
             cur = GetMinCardWithOutVz(); // лабуду
        }
        if ( !cur ) {
             cur = aCards->maxFace();
        }
    } else { // а козырьков у них и нету !!!
        if ( !cur ) {
             cur = GetMaxCardPere();  //масть с перехватами (max)
        }
        if ( !cur ) { // а у меня их нет !!!
             cur = GetMaxCardWithOutPere(); // без пере
        }
        if ( !cur ) {
             cur = GetMinCardWithOutVz(); // лабуду
        }
        if ( !cur ) {
            cur = aCards->maxInSuit(mast);
        }
        if ( !cur ) {
             cur = aCards->maxFace();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}
//-----------------------------------------------------------------------
Card *Player::GetMaxCardPere(void) {
    int index=0,vz=0,pere=0;

    for (int f=1;f<=4;f++) {
        if ( suitProb[f].vzatok > vz || (suitProb[f].vzatok == vz && suitProb[f].perehvatov > pere) ) {
            index = f;
            vz = suitProb[f].vzatok;
            pere = suitProb[f].perehvatov;
        }
    }

    if (index) return aCards->maxInSuit(index);
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// make game move (dispatcher)
Card *Player::makemove (Card *lMove, Card *rMove, Player *aLeftGamer, Player *aRightGamer) {
    Card *cur=0;
    if (lMove == 0 && rMove == 0) { // мой заход - первый
        if ( GamesType == gtPass || GamesType == vist ) {
              // кто-то играет а я как бы вистую
              cur = MyVist1(aLeftGamer,aRightGamer);
        } else  if (GamesType == g86catch) {
            cur = MiserCatch1(aLeftGamer,aRightGamer);
          } else  if (GamesType == g86) {
          cur = Miser1(aLeftGamer,aRightGamer);
          } else   if (  GamesType == raspass  ) {
            cur = MyPass1(rMove,aLeftGamer,aRightGamer);; // ну типа распасы или мизер
        } else {
            // ну типа моя игра
            cur = MyGame1(aLeftGamer,aRightGamer);
        }

    }
    if (lMove == 0 && rMove != 0) { // мой заход - второй
        if ( GamesType == gtPass || GamesType == vist ) {
                // кто-то играет а я как бы вистую
                cur = MyVist2(rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86catch) {
                cur = MiserCatch2(rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86) {
              cur = Miser2(rMove,aLeftGamer,aRightGamer);
          } else  if ( GamesType == raspass  ) {
                cur = MyPass2(rMove,aLeftGamer,aRightGamer); // ну типа распасы или мизер
        } else {   // ну типа моя игра
            cur = MyGame2(rMove,aLeftGamer,aRightGamer);
        }

    }
    if (lMove != 0 && rMove != 0) { // мой заход - 3
        if ( GamesType == gtPass || GamesType == vist ) {
                // кто-то играет а я как бы вистую
           cur = MyVist3(lMove,rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86catch) {
            cur = MiserCatch3(lMove,rMove,aLeftGamer,aRightGamer);
          } else  if (GamesType == g86) {
            cur = Miser3(lMove,rMove,aLeftGamer,aRightGamer);
          } else if ( GamesType == raspass ) {
           cur = MyPass3(lMove,rMove,aLeftGamer,aRightGamer);; // ну типа распасы или мизер
        } else {
           // ну типа моя игра
           cur = MyGame3(lMove,rMove,aLeftGamer,aRightGamer);
        }
    }
    aCards->remove(cur);
    aCardsOut->insert(cur);
    return cur;
}


//-----------------------------------------------------------------------
eGameBid Player::makemove4out(void) {
    eGameBid GamesTypeRetVal;
    int f;
    int nMaxMastLen=0;
    eSuit nMaxMast=SuitNone;
    tSuitProbs LocalMastTable[5];
    LocalMastTable[0].vzatok=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum=0;


    for (f=1; f<=4; f++ )  {
      LocalMastTable[f] = vzatok4game( (eSuit) f,1  );
      LocalMastTable[0].vzatok += LocalMastTable[f].vzatok ;
      LocalMastTable[0].perehvatov +=LocalMastTable[f].perehvatov ;
      LocalMastTable[0].sum += LocalMastTable[f].sum;
    }

    for (f=1; f<=4; f++) { // предполагаемый козырь - самая длинная масть
      if( aCards->cardsInSuit(f) > nMaxMastLen ) {
        nMaxMastLen = aCards->cardsInSuit(f);
        nMaxMast = (eSuit) f;
      }
    }

    for (f=1; f<=4; f++) {
      if(aCards->cardsInSuit(f) == nMaxMastLen && nMaxMast != f ) { // есть максимально длинные масти
       if ( LocalMastTable[f].sum  > LocalMastTable[nMaxMast].sum  )  {
         nMaxMast = (eSuit) f;
        }
      }
    }

    //масть и звятки уже посчитали
    GamesTypeRetVal = ( eGameBid ) (LocalMastTable[0].vzatok*10 + nMaxMast );
    // это то, что мы можем играть максимально
    return GamesTypeRetVal;

}
//-----------------------------------------------------------------------
eGameBid Player::makemove(eGameBid MaxGame,int HaveAVist,int nGamerVist) { // после получения игроком прикупа - пасс или вист
  Q_UNUSED(nGamerVist)
  eGameBid Answer;
  eGameBid MyMaxGame = makemove4out();

  int vz = MyMaxGame / 10;
  if ( HaveAVist != vist && vz >= gameWhistsMin(MaxGame) ) {
     Answer = vist;
  } else {
     Answer = gtPass;
  }
  if ( HaveAVist==gtPass && vz < gameWhistsMin(MaxGame) ) {
     Answer = gtPass;
  }
  if (g61stalingrad && MaxGame == g61) {
     Answer = vist; //STALINGRAD !!!
  }
  if (MaxGame == g86) { // miser
     Answer = g86catch;
  }
  GamesType = Answer;
  return Answer;
}
//-----------------------------------------------------------------------
eGameBid Player::makeout4miser(void) {
  Card *FirstCardOut=0,*SecondCardOut=0;
  Card *tmpFirstCardOut,*tmpSecondCardOut;
  Card *RealFirstCardOut,*RealSecondCardOut;
  eGameBid  Hight = g105,  tmpHight = g105;
  for ( int f=0;f<12;f++ ) {
    tmpFirstCardOut = (Card*) aCards->at(f);
    aCards->removeAt(f);
    //for ( int j=0;j<12;j++ )  {
    for (int j=f+1; j<12; j++) { // patch from Rasskazov K. (kostik450@mail.ru)
      if ( j!=f ) {
        tmpSecondCardOut = (Card*) aCards->at(j);
        aCards->removeAt(j);
        Player *tmpGamer = new Player(99);
        tmpGamer->aCards->shallowCopy(aCards);
        tmpGamer->aCards->mySort();
        tmpHight = tmpGamer->makemove4out();
        if ( (tmpHight < Hight )
         || (tmpHight == Hight  && tmpGamer->suitProb[0].perehvatov < suitProb[0].perehvatov) ) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer->aCards->clear();
        delete tmpGamer;
        aCards->putAt(j,tmpSecondCardOut);
      }
    }
    aCards->putAt(f,tmpFirstCardOut);
  }

  RealFirstCardOut     = aCards->maxInSuit(FirstCardOut->suit());
  aCards->remove(RealFirstCardOut);
  aOut->insert(RealFirstCardOut);
  if (SecondCardOut->suit() != FirstCardOut->suit()) {
    RealSecondCardOut = aCards->maxInSuit(SecondCardOut->suit());
   } else {
    RealSecondCardOut = aCards->greaterInSuit(FirstCardOut);
    if ( !RealSecondCardOut )
      RealSecondCardOut = aCards->maxFace(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
   }
  aCards->remove(RealSecondCardOut);
  aOut->insert(RealSecondCardOut);
  aCards->mySort();
  return g86;
}


eGameBid Player::makeout4game () {
  Card *FirstCardOut = 0, *SecondCardOut = 0;
  Card *tmpFirstCardOut, *tmpSecondCardOut;
  Card *RealFirstCardOut, *RealSecondCardOut;
  eGameBid Hight = zerogame, tmpHight = zerogame;

  for (int f = 0; f < 12; f++) {
    tmpFirstCardOut = aCards->at(f);
    aCards->removeAt(f);
    for (int j = 0; j < 12; j++)  {
      if (j != f) {
        tmpSecondCardOut = aCards->at(j);
        aCards->removeAt(j);
        Player *tmpGamer = new Player(99);
        tmpGamer->aCards->shallowCopy(aCards);
        tmpGamer->aCards->mySort();
        tmpHight = tmpGamer->makemove4out();
        if ((tmpHight > Hight) ||
            (tmpHight == Hight && tmpGamer->suitProb[0].perehvatov < suitProb[0].perehvatov)) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer->aCards->clear();
        delete tmpGamer;
        aCards->putAt(j, tmpSecondCardOut);
      }
    }
    aCards->putAt(f, tmpFirstCardOut);
  }

  RealFirstCardOut = aCards->minInSuit(FirstCardOut->suit());
  aCards->remove(RealFirstCardOut);
  aOut->insert(RealFirstCardOut);
  if (SecondCardOut->suit() != FirstCardOut->suit()) {
    RealSecondCardOut = aCards->minInSuit(SecondCardOut->suit());
  } else {
    RealSecondCardOut = aCards->greaterInSuit(FirstCardOut);
    if (!RealSecondCardOut)
      RealSecondCardOut = aCards->minFace(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
  }
  aCards->remove(RealSecondCardOut);
  aOut->insert(RealSecondCardOut);
  aCards->mySort();
  //Hight = makemove(undefined,undefined);
  Hight = makemove4out();
  if (Hight < GamesType )  {
    /* Вот такая ерунда */
    int tmpvz = Hight/10, tmpmast = Hight-tmpvz*10;
    while (tmpHight < GamesType)  {
      tmpvz++;
      tmpHight = (eGameBid)(tmpvz*10+tmpmast);
    }
    GamesType = tmpHight;
  } else {
    GamesType = Hight;
  }
  return GamesType;
}


//ход при торговле
eGameBid Player::makemove (eGameBid lMove, eGameBid rMove) {
/*
  if ( GamesType == gtPass )  {
    GamesType=gtPass;
  } else {
*/
  if (GamesType != gtPass)  {
    int f;
    int nMaxMastLen = 0;
    eSuit nMaxMast = SuitNone;
    tSuitProbs LocalMastTable[5];
    eGameBid MGT = qMax(lMove,rMove);
    LocalMastTable[0].vzatok=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum = 0;
    for (f = 1; f <= 4; f++)  {
      LocalMastTable[f] = vzatok4game((eSuit)f, 1);
      LocalMastTable[0].vzatok += LocalMastTable[f].vzatok;
      LocalMastTable[0].perehvatov +=LocalMastTable[f].perehvatov;
      LocalMastTable[0].sum += LocalMastTable[f].sum;
    }
    // предполагаемый козырь - самая длинная масть
    for (f = 1; f <= 4; f++) {
      if (aCards->cardsInSuit(f) > nMaxMastLen) {
        nMaxMastLen = aCards->cardsInSuit(f);
        nMaxMast = (eSuit)f;
      }
    }
    for (f = 1; f <= 4; f++) {
      if (aCards->cardsInSuit(f) == nMaxMastLen && nMaxMast != f) {
        // есть максимально длинные масти
        if (LocalMastTable[f].sum > LocalMastTable[nMaxMast].sum) nMaxMast = (eSuit)f;
      }
    }
    // масть и взятки уже посчитали
    if (MGT <= g75) {
      GamesType = (eGameBid)((LocalMastTable[0].vzatok+1)*10+nMaxMast);
    } else {
      GamesType = (eGameBid)((LocalMastTable[0].vzatok)*10+nMaxMast);
    }
    if ((rMove == gtPass || rMove == undefined) && (lMove == gtPass || lMove == undefined)) {
      if (LocalMastTable[0].vzatok >= 4) {
        GamesType = g61;
      } else {
        if (LocalMastTable[0].vzatok == 0)  {
          // check for misere !!!
          GamesType = Check4Miser() ? g86 : gtPass ;
        } else GamesType = gtPass;
      }
    } else if (GamesType >= MGT) {
      GamesType = (eGameBid)succBid(MGT);
    } else {
      GamesType = gtPass;
    }
/*
    if( GamesType >= MGT  ) {
        GamesType = (eGameBid) succBid(MGT);
    } else {
    }*/
    // это то, что мы можем играть максимально
    /*if ( GamesType > gtPass  ) {
      if ( rMove < lMove )  {
        if ( lMove <= GamesType ) {
          if ( lMove!=g61 )
              GamesType = lMove; // say here
          else
              GamesType = (eGameBid) succBid(lMove);
        } else {
          GamesType = gtPass;
        }
      } else { // rMove > lMove
        if ( GamesType > rMove ) {
//          asm int 3;
            if (rMove!=undefined) {
              GamesType = (eGameBid) succBid(rMove);
             } else {
              GamesType = g61;
             }
        } else {
          GamesType = gtPass;
        }

      }
    } else {     //  GamesType > gtPass
      if ( (rMove==gtPass || rMove==undefined) && ( lMove==gtPass || lMove==undefined ) ) {
        if ( LocalMastTable[0].vzatok >= 4 ) {
          GamesType = g61;
        } else {
          if ( LocalMastTable[0].vzatok == 0 )  {
            // check for miser !!!
            if (Check4Miser()) {
              GamesType = g86;
            } else {
              GamesType = gtPass;
            }
          } else {
            GamesType = gtPass;
          }
        }
      } else {
        GamesType = gtPass;
      }
    }*/
  }
  return GamesType;
}


int Player::Check4Miser ( ) {
  int cur = 0;
  for (int Mast = SuitSpades; Mast <= SuitHearts; Mast++) {
    if (aCards->exists(7, Mast) && (aCards->exists(9, Mast) || aCards->exists(8, Mast)) &&
        (aCards->exists(FACE_JACK, Mast) || aCards->exists(10, Mast))) {
      cur++;
    }
  }
  if (cur == 4) return 1;
  return 0;
}


tSuitProbs Player::vzatok4game (eSuit Mast, int a23) {
   tSuitProbs suitProb;
   Card *MyCard,*tmpCard;
   CardList *MyCardStack     = new CardList;
   CardList *EnemyCardStack  = new CardList;
   for (int c=7;c<=FACE_ACE;c++ ) {
    MyCard = aCards->exists(c, Mast);
    if ( MyCard )
      MyCardStack->insert(MyCard);
    else
      EnemyCardStack->insert(newCard(c, Mast));
   }
   if ( MyCardStack->count() >=4 && MyCardStack->count() <=5 )    {
      EnemyCardStack->remove(EnemyCardStack->minFace());
   }
   if ( a23 == 23 ) {
       suitProb = Compare2List4Max23(MyCardStack,EnemyCardStack);
   } else {
       suitProb = Compare2List4Max(MyCardStack,EnemyCardStack);
   }
   suitProb.len = aCards->cardsInSuit(Mast);
   suitProb.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->exists(j,Mast);
    if (  tmpCard ) {
      suitProb.sum +=  tmpCard->face();
    }
   }
   MyCardStack->clear();
   delete MyCardStack;
   delete EnemyCardStack;
   return suitProb;
}
//-----------------------------------------------------------------------
tSuitProbs Player::vzatok4pass(eSuit Mast,CardList *aMaxCardList) {
   tSuitProbs suitProb;
   Card *MyCard,*tmpCard;
   CardList *MyCardStack     = new CardList;
   CardList *EnemyCardStack  = new CardList;
   for (int c=7;c<=FACE_ACE;c++ ) {
        MyCard = aCards->exists(c, Mast);
        if ( MyCard ) {
          MyCardStack->insert(MyCard);
        }
        MyCard = aMaxCardList->exists(c, Mast);
        if ( MyCard ) {
          EnemyCardStack->insert(MyCard);
        }
   }
   suitProb = Compare2List4Min(MyCardStack,EnemyCardStack);
   suitProb.len = aCards->cardsInSuit(Mast);
   suitProb.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->exists(j,Mast);
    if (  tmpCard ) {
      suitProb.sum +=  tmpCard->face();
    }
   }
   MyCardStack->clear();
   EnemyCardStack->clear();
   delete MyCardStack;
   delete EnemyCardStack;
   return suitProb;

}

//-----------------------------------------------------------------------
tSuitProbs Player::Compare2List4Min(CardList *My,CardList *Enemy) { // Для мин результата
  tSuitProbs suitProb;
  suitProb.vzatok=0;
  suitProb.perehvatov=0;
  int nMaxLen;
  Card *MyCardMin,*EnemyCardMax,*EnemyCardMin;
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  Enemy->free(Enemy->minFace());
  My->count()>Enemy->count() ? nMaxLen = My->count() : nMaxLen = Enemy->count();
  for ( int f=1; f<=nMaxLen; f++ )  {
    MyCardMin = My->minFace();
    EnemyCardMax = Enemy->maxFace();
    if ( MyCardMin ) {
      EnemyCardMin = Enemy->lesserInSuit(MyCardMin);
      if (!EnemyCardMin) {
          EnemyCardMin = Enemy->minFace();
      }

    } else {
//      EnemyCardMin = Enemy->minFace();
      break;
    }
    if ( !EnemyCardMax && !EnemyCardMin)  {
      break;
    }
    if ( *MyCardMin > *EnemyCardMin ) {
      // викидываем мою  его младшую
      My->remove(MyCardMin);
//      Enemy->free(EnemyCardMin);
      Enemy->remove(EnemyCardMin);

      suitProb.vzatok++;
    } else {
      My->remove(MyCardMin);
//      Enemy->free(EnemyCardMax);
      Enemy->remove(EnemyCardMax);
    }
  }
  return suitProb;
}
//-----------------------------------------------------------------------
tSuitProbs Player::Compare2List4Max(CardList *My,CardList *Enemy) {
  tSuitProbs suitProb;
  suitProb.vzatok=0;

  suitProb.perehvatov=0;
  int nMaxLen,nIget=1;
  Card *MyCardMax=0,*EnemyCardMax=0,*EnemyCardMin=0;
  My->count()>Enemy->count() ? nMaxLen = My->count() : nMaxLen = Enemy->count();
  for ( int f=1; f<=nMaxLen; f++ )  {
    MyCardMax = My->maxFace();
    if ( MyCardMax ) {
      EnemyCardMax = Enemy->greaterInSuit(MyCardMax);
      if (!EnemyCardMax) {
        EnemyCardMax = Enemy->minFace();
      }
    } else {
      EnemyCardMax = Enemy->maxFace();
    }
    EnemyCardMin = Enemy->minFace();
    if ( !MyCardMax )  {
      break;
    }
    if ( !EnemyCardMax && !EnemyCardMin)  {
      // остаток - мои взятки
      for ( int j =1; j<=My->count();j++ ) {
        suitProb.vzatok++;
      }
      break;
    }
    if ( *MyCardMax > *EnemyCardMax ) {
      // викидываем мою старшую, его младшую
      My->remove(MyCardMax);
//      Enemy->free(EnemyCardMin);
  Enemy->remove(EnemyCardMin);
      suitProb.vzatok++;
      if ( !nIget )  {
        suitProb.perehvatov++;
      }

      nIget = 1;
    } else {
      My->remove(MyCardMax);
//      Enemy->free(EnemyCardMax);
      Enemy->remove(EnemyCardMax);
      nIget = 0;
    }
  }
  return suitProb;
}
//-----------------------------------------------------------------------
tSuitProbs Player::Compare2List4Max23(CardList *My,CardList *Enemy) {
  tSuitProbs suitProb;
  suitProb.vzatok=0;
  suitProb.perehvatov=0;
  int nMaxLen,nIget=1;
  Card *MyCardMax,*EnemyCardMax;
  My->count()>Enemy->count() ? nMaxLen = My->count() : nMaxLen = Enemy->count();
  for ( int f=1; f<=nMaxLen; f++ )  {
    EnemyCardMax = Enemy->maxFace();
    if (EnemyCardMax) {// Пытаемся н-ти больше чеm у него
      MyCardMax = My->greaterInSuit(EnemyCardMax);
      if ( ! MyCardMax ) {  // Нет у меня больше чем у него
        MyCardMax = My->minFace();
        if (!MyCardMax ) {  // и меньше тож нет
          break ;
        } else {  // Меньше чем у него
          My->remove(MyCardMax);
          Enemy->remove(EnemyCardMax);
          nIget = 0;
        }
      } else {  // Есть больше чем у него
        My->remove(MyCardMax);
        Enemy->remove(EnemyCardMax);
        suitProb.vzatok++;
        if (! nIget) suitProb.perehvatov++;
        nIget = 1;
      }
    } else {  // У него нет карт в данной масти ! остаток - мои взятки
      if ( ! Enemy->count() ) {
        for ( int j =1; j<=My->count();j++ ) suitProb.vzatok++;
      }
//25.07.2000 for ( int j =1; j<=My->count();j++ ) suitProb.vzatok++;
      break;
    }
  }
  return suitProb;
}
//-----------------------------------------------------------------------
Card *Player::MyPass1(Card *rMove,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=0;
  eGameBid tmpGamesType=GamesType;
  CardList *aMaxCardList=new CardList;
  CardList *aTmpList=new CardList;
  CardList *aStackStore=0;

  if ( rMove != 0 )  {
    aTmpList = new CardList;
    aTmpList->copySuit(aCards, (eSuit)rMove->suit());
    aStackStore = aCards;
    aCards = aTmpList;
//    LoadLists(aLeftGamer,aRightGamer,aMaxCardList); // !!!!!!!!!
//    RecountTables4RasPass(aMaxCardList,1);
  }
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // распасы // мизер
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList); // !!!!!!!!!
    RecountTables4RasPass(aMaxCardList,1);
     // тема такая : 100 % отбери своё, если такового нет - мах в минимально длинной масти
    cur = GetMaxCardWithOutPere(); // за искл тех случ коды масти уже нет
    if ( cur ) {
        if ( aLeftGamer->aCards->cardsInSuit(cur->suit())==0
             && aRightGamer->aCards->cardsInSuit(cur->suit())==0 ) {

          cur = 0;
        }
    }
    if ( !cur ) {
         cur = GetMaxCardPere();  //масть с перехватами (max)
    }
    if ( !cur ) {
       cur = GetMinCardWithOutVz(); // лабуду
    }
    if ( !cur ) {
        cur = aCards->minFace();
    }
  }
  if ( rMove != 0 )  {
    aCards=aStackStore;
    aTmpList->clear();
    delete aTmpList;

  }
  /*

  */
  GamesType=tmpGamesType;
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::MyPass2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=0;
  eGameBid tmpGamesType=GamesType;
  int mast = aRightCard->suit();
  CardList *aMaxCardList=new CardList;
  LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
  RecountTables4RasPass(aMaxCardList,1);
/////////////////////////////////////////////////////////
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // распасы // мизер
    cur = aCards->maxInSuit(mast); // !!!!!!!!!!!! здеся обышка
    if ( !cur ) { // Нет Масти
      // во шара !!! Льем мах в масти где  есть 100 % взятки
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //масть с перехватами (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // лабуду
       }
       if ( !cur ) {
              cur = aCards->minFace();
       }

    } else { // У меня есть масть
        if ( aLeftGamer->aCards->cardsInSuit(mast) ) { //У левого есть
            cur = aCards->lesserInSuit(aLeftGamer->aCards->maxInSuit(mast));
            if ( !cur ) {
                cur = aCards->maxInSuit(mast);
            }
        } else { // У левого нет
           cur = aCards->lesserInSuit(aRightCard);
           if ( !cur) {
               cur = aCards->maxInSuit(mast);
           }
        }
    }
  }
/////////////////////////////////////////////////////////
  delete aMaxCardList;
  GamesType=tmpGamesType;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::MyPass3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=0;
    /*Card *maxInSuit;*/
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
//    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // у меня есть масть  в которую зашел левый
       // постараемсмя пропустить
       if (aLeftCard->suit() == aRightCard->suit()) {
        if (*aRightCard > *aLeftCard) {
          cur = aCards->lesserInSuit(aRightCard);
        } else {
          cur = aCards->lesserInSuit(aLeftCard);
        }
       } else {
         cur = aCards->lesserInSuit(aLeftCard);
       }
       if ( !cur  ) {
           cur = aCards->maxInSuit(aLeftCard->suit());
       }
    } else {
      //  у меня нет масти в которую зашел левый
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //масть с перехватами (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // лабуду
       }
       if ( !cur ) {
              cur = aCards->minFace();
       }

    }
    GamesType=tmpGamesType;
    delete aMaxCardList;

  return cur;
}
//-----------------------------------------------------------------------
void Player::makestatfill(void) {
   eHand lr=LeftHand;
   for (int m=1;m<=4;m++) {
     for (int c=FACE_ACE;c>=7;c-- ) {
      if ( !aCards->exists(c,m) ) {
         if (lr==LeftHand) {
           aLeft->insert(newCard(c, m));
           lr = RightHand;
         } else {
           aRight->insert(newCard(c, m));
           lr = LeftHand;
         }
       }
     }
   }
}
//-----------------------------------------------------------------------
void Player::makestatfill(int nCards,int maxmin) {
    int nCounter=0;
    eHand lr=LeftHand;
    if ( maxmin ==1 ) {
       for (int m=1;m<=4;m++) {
         for (int c=FACE_ACE;c>=7;c-- ) {
          if ( !aCards->exists(c,m) && !aOut->exists(c,m) ) {
             if (lr==LeftHand) {
               aLeft->insert(newCard(c, m));
               lr = RightHand;
             } else {
               aRight->insert(newCard(c, m));
               lr = LeftHand;
               nCounter++;
               if ( nCounter >= nCards ) return;
             }
           }
         }
       }
    } else {
       for (int m=1;m<=4;m++) {
         for (int c=7;c<=FACE_ACE;c++ ) {
          if ( !aCards->exists(c,m) && !aOut->exists(c,m) ) {
             if (lr==LeftHand) {
               aLeft->insert(newCard(c, m));
               lr = RightHand;
             } else {
               aRight->insert(newCard(c, m));
               lr = LeftHand;
               nCounter++;
               if ( nCounter >= nCards ) return;
             }
           }
         }
       }
    }
}


void Player::GetBackSnos () {
  // Vernut snos
  if (aCardsOut->at(0)) aCards->insert(aCardsOut->at(0));
  if (aCardsOut->at(1)) aCards->insert(aCardsOut->at(1));
  aCardsOut->clear();
}


///////////////////////////////////////////////////////////////////////////////
// game graphics
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// build array with cards offsets and indicies
// at least 28 ints (14 int pairs); return # of used ints; the last int is -1
// result: ofs, cardNo, ..., -1
int Player::buildHandXOfs (int *dest, int startX, bool opened) {
  int cnt = 0, oldXX = startX, *oDest = dest;
  Card *cur = 0, *prev = 0;

  CardList lst; lst.shallowCopy(aCards);
  lst.mySort();

  if (mPlayerNo == 3) startX = 0;
  // normal
  startX -= opened ? SUIT_OFFSET : CLOSED_CARD_OFFSET ;
  for (int f = 0; f < lst.size(); f++) {
    Card *pp = lst.at(f);
    if (!pp) continue;
    prev = cur;
    cur = pp;
    if (opened) {
      startX += (prev && prev->suit() != cur->suit()) ? NEW_SUIT_OFFSET : SUIT_OFFSET ;
    } else startX += CLOSED_CARD_OFFSET;
    *dest++ = startX;
    int idx = aCards->indexOf(cur);
    *dest++ = idx;
    if (idx < 0) abort();
    cnt++;
    if (cnt > 12) abort();
  }
  *dest++ = -1;
  *dest = -1;

  if (mPlayerNo == 3 && cnt) {
    // righttop
    startX = oldXX-(oDest[(cnt-1)*2]+CARDWIDTH+4);
    for (int f = 0; f < cnt; f++) oDest[f*2] += startX;
  }

  return cnt;
}


int Player::cardAt (int lx, int ly, bool opened) {
  int res = -1, ofs[28];
  int left, top;
  getLeftTop(mPlayerNo, &left, &top);
  buildHandXOfs(ofs, left, opened);
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x1 = ofs[f], y1 = top;//+DeskView->yBorder;
    int x2 = x1+CARDWIDTH, y2 = y1+CARDHEIGHT;
    if (x1 < lx && lx < x2 && y1 < ly && ly < y2) res = ofs[f+1];
  }
  return res;
}


void Player::getLeftTop (int playerNo, int *left, int *top) {
  *left = 0; *top = 0;
  if (!DeskView) return;
  switch (playerNo) {
    case 1:
      *left = (DeskView->DesktopWidth-(DeskView->DesktopWidth/2-2*DeskView->xBorder))/2;
      *top = DeskView->DesktopHeight-(DeskView->yBorder)-DeskView->CardHeight-10;
      break;
    case 2:
      *left = DeskView->xBorder;
      *top = DeskView->yBorder+20;
      break;
    case 3:
      *left = DeskView->DesktopWidth-DeskView->xBorder;
      *top = DeskView->yBorder+20;
      break;
    default: ;
  }
}


void Player::RepaintAt (TDeskView *aDeskView, int Left, int Top, int selNo) {
  int ofs[28];
  TDeskView *oDesk = DeskView;

  if (!aDeskView) return;
  DeskView = aDeskView;

  //aCards->mySort();

  int cnt = buildHandXOfs(ofs, Left, !nInvisibleHand);
  if (cnt) Left = ofs[0];
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = Top;
    Card *card = aCards->at(ofs[f+1]);
    aDeskView->drawCard(card, x, y, !nInvisibleHand, ofs[f+1]==selNo);
  }
/*
  QString msg;
  int textX = Left, textY = Top+CARDHEIGHT+4;
  if (GamesType != undefined) {
    if (nGetsCard) {
      msg += QString::number(nGetsCard);
      msg += " | ";
    }
    msg += sGameName(GamesType);
    if (!NikName.isEmpty()) {
      msg += " (";
      msg += NikName;
      msg += ")";
    }
  }
  if (msg.isEmpty()) msg = "...";
/ *
  msg += " ";
  if (!nInvisibleHand) msg += "OPEN "; else msg += "closed ";
  msg += QString::number(selNo);
* /
  if (!msg.isEmpty()) aDeskView->drawText(msg, textX, textY);
*/
  aDeskView->emitRepaint();
  DeskView = oDesk;
}


void Player::Repaint () {
  int left, top;
  if (!DeskView) return;
  getLeftTop(mPlayerNo, &left, &top);
  RepaintAt(DeskView, left, top, oldii);
}


void Player::HintCard (int lx, int ly) {
  Q_UNUSED(lx)
  Q_UNUSED(ly)
}


void Player::clearCardArea () {
  int left, top, ofs[28];
  if (!DeskView) return;
  getLeftTop(mPlayerNo, &left, &top);
  //aCards->mySort();
  int cnt = buildHandXOfs(ofs, left, !nInvisibleHand);
  if (!cnt) return;
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = top;
    DeskView->ClearBox(x, y, CARDWIDTH, CARDHEIGHT);
  }
}
