#include <stdlib.h>
#include <stdio.h>

#include <QPixmap>
#include <QPainter>

//#include "pcommon.h"

#include "prfconst.h"
#include "deskview.h"
#include "player.h"
#include "card.h"
#include "cardlist.h"
#include "coloda.h"
#include "ncounter.h"
#include "plscore.h"

//#include "netconst.h"


Player::Player(int _nGamer) {
  nGamer = _nGamer;
  aScore = new TPlScore();
  aCards = new TCardList(12); // мои
  aLeft  = new TCardList(10);  // Противника с лева (предполагаемый или открытые)
  aRight = new TCardList(10); // С права (предполагаемый или открытые)
  aOut   = new TCardList(2);   // Снос (мой или предполагаемый)
  aCardsOut = new TCardList(12); // во взятках мои
  aLeftOut  = new TCardList(12);  // во взятках Противника с лева (предполагаемый или открытые)
  aRightOut = new TCardList(12); // во взятках С права (предполагаемый или открытые)
  NikName = QString();
  clear();
}


Player::Player(int _nGamer,TDeskView *aDeskView) {
  nGamer = _nGamer;
  aScore = new TPlScore();
  aCards = new TCardList(12);
  aLeft  = new TCardList(10);
  aRight = new TCardList(10);
  aOut   = new TCardList(2);
  aCardsOut = new TCardList(12);
  aLeftOut  = new TCardList(12);
  aRightOut = new TCardList(12);
  DeskView = aDeskView;
  NikName = QString();
  clear();
}


Player::~Player() {
  delete aCards;
  delete aLeft;
  delete aRight;
  delete aOut;
  delete aCardsOut;
  delete aLeftOut;
  delete aRightOut;
  delete aScore;
}


void Player::clear () {
  flMiser = 0;
  aCards->RemoveAll();
  aLeft->RemoveAll();
  aRight->RemoveAll();
  aOut->RemoveAll();
  aCardsOut->RemoveAll();
  aLeftOut->RemoveAll();
  aRightOut->RemoveAll();
  Mast = SuitNone;
  GamesType = undefined;
  Enemy = NoHand;
  nGetsCard = 0;
  WaitForMouse = 0;
  oldii = -1;
  for (int i = 0; i <= 4; i++)  {
    MastTable[i].vzatok = 0;
    MastTable[i].perehvatov = 0;
  }
  nInvisibleHand = (nGamer != 1);
  Pronesti = NULL;
}


void Player::AddCard (TCard *aCard) {
  aCards->Insert(aCard);
}


void Player::AddCard (int _CName, int _CMast) {
  aCards->Insert(new TCard (_CName,_CMast));
}


///////////////////////////////////////////////////////////////////////////////
// game mechanics
///////////////////////////////////////////////////////////////////////////////
TCard *Player::Miser1(Player *aLeftGamer,Player *aRightGamer) {
    TCard *cur=NULL;
    if (aCards -> AllCard()==10) {  // first move первый выход в 8..туз если она одна
        for (int m=1;m<=4;m++) {
          if (aCards->AllCard(m)==1) {
            cur = aCards -> MinCard(m);
            break;
          }
        }
    }
    if (!cur) {
        for (int m=4;m>=1;m--) {
          if (cur) break;
          for (int c=FACE_ACE;c>=7;c--) {
            if (cur) break;
            TCard *my;
            //,*leftmax,*leftmin,*rightmax,*rightmin;
            my = aCards->Exist(c,m);
            if (my) {
              int matrixindex=0;
              matrixindex  += aLeftGamer -> aCards -> LessThan(my) ? 1:0;     matrixindex <<= 1;
              matrixindex  += aLeftGamer -> aCards -> MoreThan(my) ? 1:0;   matrixindex <<= 1;
              matrixindex  += aRightGamer -> aCards -> LessThan(my) ? 1:0;  matrixindex <<= 1;
              matrixindex  += aRightGamer -> aCards -> MoreThan(my) ? 1:0;
              if (matrixindex == 1 || matrixindex == 9 || matrixindex==13||(matrixindex >=4 && matrixindex <=7)) {
                cur = my;
              }
            }
          }
        }
    }
    if (!cur) cur = aCards->MinCard(1);
    if (!cur) cur = aCards->MinCard(2);
    if (!cur) cur = aCards->MinCard(3);
    if (!cur) cur = aCards->MinCard(4);
    return cur;
}
//-----------------------------------------------------------------------
TCard *Player::Miser2(TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
    TCard *cur=NULL;
      if ( aCards->AllCard( aRightCard->CMast) ) {
          for (int c=FACE_ACE;c>=7;c--) {
            if (cur) break;
            TCard *my;
//,*leftmax,*leftmin,*rightmax,*rightmin;
            my = aCards->Exist(c,aRightCard->CMast);
            if (my) {
              int matrixindex=0;
              matrixindex  += aLeftGamer -> aCards -> LessThan(my) ? 1:0;   matrixindex <<= 1;
              matrixindex  += aLeftGamer -> aCards -> MoreThan(my) ? 1:0;   matrixindex <<= 1;
              matrixindex  += (*aRightCard < *my) ? 1:0;  matrixindex <<= 1;
              matrixindex  += (*aRightCard > *my) ? 1:0;
              if (matrixindex == 1 || matrixindex == 9 || matrixindex==13||(matrixindex >=4 && matrixindex <=7)) {
                cur = my;
              }
            }
          }
          if (!cur) cur = aCards->MaxCard(aRightCard->CMast);
        } else {
          TCardList *aMaxCardList=new TCardList(20);
          LoadLists(aRightGamer,aLeftGamer,aMaxCardList);
           cur = GetMaxCardWithOutPere();
             if ( !cur ) {
               cur = GetMaxCardPere();  //масть с перехватами (max)
           }
             if ( !cur ) {
             cur = GetMinCardWithOutVz(); // лабуду
           }
             if ( !cur ) {
              cur = aCards->MaxCard();
          }
          delete aMaxCardList;
        }
    return cur;
}
//-----------------------------------------------------------------------
TCard *Player::Miser3(TCard *aLeftCard,TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
// copy from MyPass3
  TCard *cur=NULL;
    /*TCard *MaxCard;*/
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
//    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // у меня есть масть  в которую зашел левый
       // постараемсмя пропустить
       if (aLeftCard->CMast == aRightCard ->CMast) {
        if (*aRightCard > *aLeftCard) {
          cur = aCards -> LessThan(aRightCard);
        } else {
          cur = aCards -> LessThan(aLeftCard);
        }
       } else {
         cur = aCards -> LessThan(aLeftCard);
       }
       if ( !cur  ) {
           cur = aCards ->MaxCard(aLeftCard->CMast);
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
              cur = aCards->MinCard();
       }

    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
TCard *Player::makemove(TCard *lMove,TCard *rMove,Player *aLeftGamer,Player *aRightGamer) { //ход
    TCard *cur=NULL;
    if (lMove == NULL && rMove == NULL) { // мой заход - первый
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
    if (lMove == NULL && rMove != NULL) { // мой заход - второй
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
    if (lMove != NULL && rMove != NULL) { // мой заход - 3
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
    aCards    -> Remove(cur);
    aCardsOut -> Insert(cur);
    return cur;
}
//-----------------------------------------------------------------------
TMastTable Player::vzatok(TMast Mast,TCardList *aMaxCardList,int a23) {
   TMastTable MastTable;
   TCard *MyCard,*tmpCard;
   TCardList *MyCardStack     = new TCardList(MAXMASTLEN);
   TCardList *EnemyCardStack  = new TCardList(MAXMASTLEN);
   for (int c=7;c<=FACE_ACE;c++ ) {
        MyCard = aCards->Exist(c, Mast);
        if ( MyCard ) {
          MyCardStack->Insert(MyCard);
        }
        MyCard = aMaxCardList->Exist(c, Mast);
        if ( MyCard ) {
          EnemyCardStack->Insert(MyCard);
        }
   }
   if (a23 != 23) {
       MastTable = Compare2List4Max(MyCardStack,EnemyCardStack);
   } else {
       MastTable = Compare2List4Max23(MyCardStack,EnemyCardStack);
   }
   MastTable.len = aCards->AllCard(Mast);
   MastTable.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->Exist(j,Mast);
    if (  tmpCard ) {
      MastTable.sum +=  tmpCard -> CName;
    }
   }
   MyCardStack -> RemoveAll();
   EnemyCardStack-> RemoveAll();
   delete MyCardStack;
   delete EnemyCardStack;
   return MastTable;
}
//-----------------------------------------------------------------------
void Player::RecountTables4RasPass(TCardList *aMaxCardList,int a23) { // Пересчитывает таблицу дли распасов или мизера
  Q_UNUSED(a23)
    int i;
    MastTable[0].vzatok=0;
    MastTable[0].perehvatov=0;
    MastTable[0].sum=0;
    for (i=1; i<=4; i++ )  {
      MastTable[i] = vzatok4pass( (TMast) i,aMaxCardList );
      MastTable[0].vzatok += MastTable[i].vzatok ;
      MastTable[0].perehvatov += MastTable[i].perehvatov ;
      MastTable[0].sum += MastTable[i].sum;
    }

}
//-----------------------------------------------------------------------
void Player::RecountTables( TCardList *aMaxCardList,int a23 ){ // Пересчитывает таблицу         TMastTable MastTable[5];
    int i;
    MastTable[0].vzatok=0;
    MastTable[0].perehvatov=0;
    MastTable[0].sum=0;
    for (i=1; i<=4; i++ )  {
      MastTable[i] = vzatok ( (TMast) i,aMaxCardList, a23 );
      MastTable[0].vzatok += MastTable[i].vzatok ;
      MastTable[0].perehvatov += MastTable[i].perehvatov ;
      MastTable[0].sum += MastTable[i].sum;
    }
}
//-----------------------------------------------------------------------
TCard *Player::MiserCatch1(Player *aLeftGamer,Player *aRightGamer) {
  TCard *cur=NULL;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *Naparnik;
  TSide Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer -> aCards;
    Side = RightHand;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer -> aCards;
    Side = LeftHand;
  }
  RecountTables4RasPass(aMaxCardList,1);
//  if (Side == LeftHand) {
// 1. Пытаемся всунуть
    for (int m=1;m<=4;m++) {
      TCard *EnemyMin = aMaxCardList -> MinCard(m);
      TCard *NaparnikMin = Naparnik -> MinCard(m);
      TCard *MyMin = aCards -> MinCard(m);
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
                if (k==m || aMaxCardList->AllCard(k)==0)
                  continue;
                TCard *EnemyMax = aMaxCardList -> MaxCard(k);
                TCard *NaparnikMax = Naparnik -> MaxCard(k);
                TCard *MyMax = aCards -> MaxCard(k);
                if ( MyMax && NaparnikMax && EnemyMax) {
                  if (Naparnik->AllCard(k) < aCards-> AllCard(k) && Naparnik->AllCard(k) < aMaxCardList->AllCard(k) ) {
                    cur = aCards ->LessThan(EnemyMax);
//                    MyMax; // на этой он должен пронести NaparnikMax
                    if (aRightGamer->GamesType == g86) {
                        aRightGamer -> Pronesti = NaparnikMax;
                      }   else {
                        aLeftGamer -> Pronesti = NaparnikMax;
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
    if (!cur) cur = aCards->MinCard();
//  } else { //Side == LeftHand
//  }
badlabel:

  aMaxCardList->RemoveAll();
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
TCard *Player::MiserCatch2(TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  TCard *cur=NULL;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *Naparnik;
  TSide Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer -> aCards;
    Side = RightHand;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer -> aCards;
    Side = LeftHand;
  }
  RecountTables4RasPass(aMaxCardList,23);

  if (Side == LeftHand) { //ход под меня , противник с права
      if (aCards->AllCard(aRightCard->CMast)) {
        // у меня есть карты  в данной масти
        TCard *MyMax = aCards->MaxCard(aRightCard->CMast);
        //TCard *MyMin = aCards->MinCard(aRightCard->CMast);
        //TCard *EnMax = aMaxCardList->MaxCard(aRightCard->CMast);
        TCard *EnMin = aMaxCardList->MinCard(aRightCard->CMast);
        if (EnMin) {
          // у противника есть карты в данной масти
          if (*aRightCard < *EnMin) {
            // можно попытаться всунуть
            cur=aCards->LessThan(EnMin);
            if(!cur) cur = aCards -> MoreThan(EnMin);
          } else {
          cur= aCards -> MoreThan(EnMin);
          }
        } else {
          cur = MyMax; // А вот тут надо смотреть кому нужен ход
        }
      } else {
        // у меня нет карт  в данной масти
        if (Pronesti) {
          //надо пронести карту
          if (aCards->Exist(Pronesti->CName,Pronesti->CMast)) {
            cur = Pronesti;
          } else {
            cur = aCards -> MaxCard(Pronesti->CMast);
            if (!cur)   cur = aCards -> MaxCard();
          }
        } else {
          // указания на пронос - нет
          cur = GetMaxCardPere();
        if (!cur) cur = aCards -> MaxCard();
        }
      }
  } else { // противник зашел под меня
    TCard *MyMax = aCards->MaxCard(aRightCard->CMast);
    TCard *MyMin = aCards->MinCard(aRightCard->CMast);
    if (MyMax) {

      if ( *MyMin < *aRightCard) {
//        есть возможность всунуть
        TCard *NapMin = Naparnik->LessThan(aRightCard);
        if (NapMin) {
          cur = aCards->LessThan(aRightCard);
        } else {
          cur = MyMax; // кому передать код ?
        }
      } else {
        cur = MyMax; // грохать ее самой большой
      }

    } else {
      // а у меня нет масти

          cur = GetMaxCardPere();
        if (!cur) cur = aCards -> MaxCard();
    }

  }
  Pronesti = NULL;
  if (!cur)
    cur = aCards -> MinCard();

  aMaxCardList->RemoveAll();
  delete aMaxCardList;
  return cur;

}
//-----------------------------------------------------------------------
TCard *Player::MiserCatch3(TCard *aLeftCard,TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  TCard *cur=NULL;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *Naparnik;
  TSide Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer -> aCards;
    Side = RightHand;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer -> aCards;
    Side = LeftHand;
  }
  RecountTables4RasPass(aMaxCardList,23);

  if (Side == LeftHand) {
      if (aCards->AllCard(aLeftCard->CMast)) {
        // у меня есть карты  в данной масти
//        TCard *MyMax = aCards->MaxCard(aLeftCard->CMast);
//        TCard *MyMin = aCards->MinCard(aLeftCard->CMast);
        if (*aLeftCard < *aRightCard) {
          // напарник уже придавил карту
          cur = aCards -> MaxCard(aLeftCard->CMast);

        } else {
          //есть шанс всунуть
          cur = aCards -> LessThan(aLeftCard);
          if (!cur ) cur = aCards -> MaxCard(aLeftCard->CMast);
        }
      } else {
        // у меня нет карт  в данной масти
        if (Pronesti) {
          //надо пронести карту
          if (aCards->Exist(Pronesti->CName,Pronesti->CMast)) {
            cur = Pronesti;
          } else {
            cur = aCards -> MaxCard(Pronesti->CMast);
            if (!cur) cur = aCards -> MaxCard();
          }
        } else {
          // указания на пронос - нет
          cur = GetMaxCardPere();
        if (!cur) cur = aCards -> MaxCard();
        }
      }
  } else { // RightHand - my friend
    TCard *MyMax = aCards->MaxCard(aLeftCard->CMast);
    TCard *MyMin = aCards->MinCard(aLeftCard->CMast);
    if (MyMax) {
      if ( *MyMin < *aLeftCard) {
//        есть возможность всунуть
        TCard *NapMin = Naparnik->LessThan(aLeftCard);
        if (NapMin) {
          cur = aCards->LessThan(aLeftCard);
        } else {
          cur = MyMax; // кому передать код ?
        }
      } else {
        cur = MyMax; // грохать ее самой большой
      }

    } else {
      // а у меня нет масти
          cur = GetMaxCardPere();
        if (!cur) cur = aCards -> MaxCard();
    }

  }
  Pronesti = NULL;

  aMaxCardList->RemoveAll();
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
TCard *Player::MyGame3(TCard *aLeftCard,TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // моя игра 3 заход - мой
    TCard *cur=NULL;
    TCard *MaxCard;
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // у меня есть масть  в которую зашел левый
        if ( aLeftCard->CMast == aRightCard->CMast)  {
            // постараемсмя ударить
            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur  ) {
               cur = aCards ->MinCard(aLeftCard->CMast);
            }
        } else { //aLeftCard->CMast == aRightCard->CMast
          if ( aRightCard->CMast !=mast)  {
            // не бита козырем

            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur ) {
              cur = aCards -> MinCard(aLeftCard->CMast);
            }
          } else {
            // бита козырем, мне не надо бить
            cur = aCards -> MinCard(aLeftCard->CMast);
          }
        }
    } else {
      //  у меня нет масти в которую зашел левый
       if (aLeftCard->CMast==aRightCard->CMast) {
           if ( *aLeftCard > *aRightCard ) {
               MaxCard = aLeftCard;
           } else {
               MaxCard = aLeftCard;
           }
       } else {
           if ( aLeftCard->CMast != mast && aRightCard->CMast != mast) {
               MaxCard = aLeftCard;
           } else {
               MaxCard = aRightCard;
           }
       }
       cur = aCards -> MoreThan(MaxCard);
       if (!cur) {
           cur = aCards -> MinCard(mast);
       }
       if ( ! cur ) {// нет коз.
           cur = GetMinCardWithOutVz();
       }
       if ( ! cur ) {
           cur = aCards -> MinCard();
       }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------
TCard *Player::MyVist3(TCard *aLeftCard,TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // мой вист или пас 3 заход - мой
    TCard *cur=NULL;
    TCard *MaxCard;
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    Player *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer -> GamesType != gtPass && aLeftGamer -> GamesType != vist ) {
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
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // у меня есть масть  в которую зашел левый
      if ( aEnemy == aLeftGamer ) { // левый - врыжина
        LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);

        if ( aLeftCard->CMast == aRightCard->CMast)  {
          if ( *aRightCard > *aLeftCard ) {
            // бить не надо
            cur = aCards->MinCard(aRightCard->CMast);
          } else {
            // постараемсмя ударить
            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur  ) {
               cur = aCards ->MinCard(aLeftCard->CMast);
            }
          }
        } else { //aLeftCard->CMast == aRightCard->CMast
          if ( aRightCard->CMast !=mast)  {
            // не бита козырем
            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur ) {
              cur = aCards -> MinCard(aLeftCard->CMast);
            }
          } else {
            // бита козырем, мне не надо бить
            cur = aCards -> MinCard(aLeftCard->CMast);
          }
        }

      } else {
        //правый вражииина
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        if ( aLeftCard->CMast == aRightCard->CMast  )  {
          if ( *aRightCard > *aLeftCard ) {
            // попытаться прибить правого
            cur = aCards->MoreThan(aRightCard);
            if ( !cur ) {
              cur = aCards->MinCard(aRightCard->CMast);
            }
          } else {
            // бить не надо
            cur = aCards->MinCard(aRightCard->CMast);
          }
        } else {
          if ( aRightCard->CMast==mast  ) {
            // лвевый зашел, а правый пригрел козырем
            cur = aCards->MinCard(aLeftCard->CMast);
          } else { // Ух ты .... Левый друг зашел а правый пронес ерунду !!!
            cur = aCards->MinCard(aLeftCard->CMast);
          }
        }

      }
    } else {
      //  у меня нет масти в которую зашел левый
       if (aLeftCard->CMast==aRightCard->CMast) {
           if ( *aLeftCard > *aRightCard ) {   MaxCard = aLeftCard;
           } else { MaxCard = aLeftCard; }
       } else {
           if ( aLeftCard->CMast != mast && aRightCard->CMast != mast) {               MaxCard = aLeftCard;
           } else { MaxCard = aRightCard;
           }
       }
       if ( aEnemy == aLeftGamer ) { // С лева игрок
            LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);

          RecountTables(aMaxCardList,23);

           if (MaxCard == aLeftCard ) { //попытаемся хлопнуть его карту
               cur = aCards -> MoreThan(aLeftCard);
           } else { // Уже наша взятка... скидываем минимальную
               cur = aCards -> MinCard(MaxCard->CMast);
           }
       } else { // с лева - напарник
          LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,23);

           if (MaxCard == aLeftCard ) { // Уже наша взятка... скидываем минимальную
               cur = aCards -> MinCard(MaxCard->CMast);
           } else {//попытаемся хлопнуть его карту
               cur = aCards -> MoreThan(aLeftCard);
           }
       }
       if (!cur) {
           cur = aCards -> MinCard(mast);
       }
       if ( ! cur ) {// нет коз.
           cur = GetMinCardWithOutVz();
       }
       if ( ! cur ) {
           cur = aCards -> MinCard();
       }


    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------

TCard *Player::MyVist2(TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // мой вист или пас 2 заход - мой
    TCard *cur=NULL;
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    Player *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer -> GamesType != gtPass && aLeftGamer -> GamesType != vist ) {
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
        if ( aRightCard -> CName >= 10 ) {
            cur = aCards -> MinCard(aRightCard -> CMast);
        } else { // может умный и больше заходов у него нет в данную масть а есть козырь
            cur = aCards -> MaxCard(aRightCard -> CMast);
        }
        if ( !cur ) { // а у меня масти и нет !!!
            cur = aCards -> MaxCard(mast); // прибъем максимальной
        }
        if ( ! cur ) {
            cur = GetMinCardWithOutVz();
        }
    } else { // с лева - напарник
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        cur = aCards->MoreThan(aRightCard);
        if (!cur) {
            cur = aCards->MinCard(aRightCard->CMast);
        }
        if (!cur) { // Нет масти пробуем козырь
            cur = aCards->MinCard(mast);
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
TCard *Player::MyVist1(Player *aLeftGamer,Player *aRightGamer) {
// 1 - выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
    TCard *cur=NULL;
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    Player *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer -> GamesType != gtPass && aLeftGamer -> GamesType != vist ) {
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
            if ( aLeftGamer ->aCards ->AllCard(mast) <=2  ) {
          TCard *m =aCards ->MaxCard(mast);
          if (!m) {
                cur = aCards ->MinCard(aLeftGamer ->aCards ->EmptyMast (mast));
              } else {
                if (aLeftGamer ->aCards ->MaxCard(mast)->CName < m->CName) {
                  cur = aCards -> MaxCard(mast);
                } else {
                  cur = aCards ->MinCard(aLeftGamer ->aCards ->EmptyMast (mast));
                }
              }
            } else {
                  cur = aCards ->MinCard(aLeftGamer ->aCards ->EmptyMast (mast));
            }
        if ( !cur) {
          cur = GetMinCardWithOutVz();
        }

/*        if ( aCards -> Exist(FACE_ACE,cur->CMast) || aCards -> Exist(FACE_KING,cur->CMast) ) {
          cur = NULL;
            cur = aCards ->MinCard(aMaxCardList ->EmptyMast (0));
        }*/


        if ( !cur) {
            cur = GetMaxCardWithOutPere();
        }
        if ( !cur) {
            cur = aCards -> MinCard(mast);
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
            cur = aCards -> MaxCard();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}

//-----------------------------------------------------------------------
TCard *Player::MyGame2(TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
    TCardList *aMaxCardList=new TCardList(20);
    tGameBid tmpGamesType=GamesType;
    TCard *cur=NULL;
    TCard *MaxLeftCard=NULL;
    int mast = aRightCard -> CMast;
    int koz  = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);

    cur = aCards -> MaxCard(mast);
    if ( !cur ) { // Нет Масти
        if ( !aLeftGamer -> aCards -> AllCard(mast) ) { // У левого тож нету
            MaxLeftCard = aLeftGamer -> aCards ->MaxCard(koz);
            cur = aCards -> MoreThan(MaxLeftCard); //
            if (!cur) {
                cur = aCards -> LessThan(MaxLeftCard);
            }
            if ( !cur ) {
                cur = GetMinCardWithOutVz(); // лабуду
            }
            if ( !cur ) {
                 cur = aCards->MinCard();
            }
        } else { // Есть масть у левого
            cur = aCards -> MinCard(koz);
        }
    } else { // У меня есть масть
        if ( aLeftGamer -> aCards -> AllCard(mast) ) { //У левого есть
            MaxLeftCard = aLeftGamer -> aCards ->MaxCard(mast);
            cur = aCards -> MoreThan(MaxLeftCard);
            if ( !cur ) {
                cur = aCards -> LessThan(MaxLeftCard);
            }
        } else { // У левого нет
            if ( !aLeftGamer -> aCards -> AllCard(koz) ) { // И козыре й нет
                cur = aCards -> MoreThan(aRightCard);
                if ( !cur) {
                    cur = aCards -> MinCard(mast);
                }
            } else { // есть у левого козыря
                cur = aCards -> MinCard(mast);
            }
        }
    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}
//-----------------------------------------------------------------------
TCard *Player::MyGame1(Player *aLeftGamer,Player *aRightGamer) {
// 1-выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
    TCard *cur=NULL;
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,1);
    if ( aLeft->AllCard(mast) || aRight->AllCard(mast) ) { // У противников есть козыря
        cur = aCards->MaxCard(mast);
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
             cur = aCards->MaxCard();
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
            cur = aCards->MaxCard(mast);
        }
        if ( !cur ) {
             cur = aCards->MaxCard();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}
//-----------------------------------------------------------------------
TCard *Player::GetMaxCardPere(void) {
    int index=0,vz=0,pere=0;

    for (int i=1;i<=4;i++) {
        if ( MastTable[i].vzatok > vz || (MastTable[i].vzatok == vz && MastTable[i].perehvatov > pere) ) {
            index = i;
            vz = MastTable[i].vzatok;
            pere = MastTable[i].perehvatov;
        }
    }

    if (index) return aCards->MaxCard(index);
    return NULL;
}
//-----------------------------------------------------------------------
TCard *Player::GetMaxCardWithOutPere(void) {
    int index=0,vz=0,pere=0;
    for (int i=1;i<=4;i++) {
        if ( MastTable[i].vzatok > vz || (MastTable[i].vzatok == vz && MastTable[i].perehvatov < pere) ) {
            index = i;
            vz = MastTable[i].vzatok;
            pere = MastTable[i].perehvatov;
        }
    }
    if (index) return aCards->MaxCard(index);
    return NULL;
}
//-----------------------------------------------------------------------
TCard *Player::GetMinCardWithOutVz(void) {
    int index=0;
    //,vz=8;
    float koef = 0,koef1 = 0 ;
    for (int i=1;i<=4;i++) {
        koef1 =MastTable[i].len+ 8/(1+MastTable  [i].vzatok);
        if ( (koef1 > koef && MastTable[i].len!=0) || (MastTable  [i].vzatok == 0 && MastTable[i].len > 0)) {
          index = i;
          koef =koef1;
        }
    }
    if (index && index <=4)
        return aCards->MinCard(index);
      else
        return aCards->MinCard();
//    return NULL;
}
//-----------------------------------------------------------------------
void Player::LoadLists(Player *aLeftGamer,Player *aRightGamer,TCardList *aMaxCardList) {
/*    int nLeftVisible = aLeftGamer -> nCardsVisible,nRightVisible = aRightGamer -> nCardsVisible;
    int nCards = aCards -> AllCard();       */
    aLeft  -> RemoveAll();
    aRight -> RemoveAll();
/*    // !!!!!!!!!!!!!!!!!!!!!!!!! необходимо учитывать, что во взятках
    if ( !nLeftVisible && !nRightVisible ) {
        makestatfill(nCards,1); // ,1- for max list 2- for minlist
    }
    if ( nLeftVisible && !nRightVisible ) {
        makestatfill(nCards,1); // ,1- for max list 2- for minlist
    }
    if ( !nLeftVisible && nRightVisible ) {
        makestatfill(nCards,1); // ,1- for max list 2- for minlist
    }
    if ( nLeftVisible && nRightVisible ) {*/
        aLeft -> Assign(aLeftGamer -> aCards);
        aRight -> Assign(aRightGamer -> aCards);
/*    }*/
// Get Max List only len
/*    for (int m=1;m<=4;m++) {
      for (int c=FACE_ACE;c>=7;c-- ) {
       if ( aLeft -> Exist(c,m) || aRight -> Exist(c,m) ) {
           aMaxCardList->Insert(new TCard(c,m));
       }
      }
    }*/
    for (int m=1;m<=4;m++) {
          TCard *LeftMax,*RightMax,*Max=NULL;
          while ( aLeft ->AllCard(m) || aRight ->AllCard(m)) {
            LeftMax = aLeft -> MaxCard(m);
            RightMax=aRight-> MaxCard(m);
            if (LeftMax == NULL && RightMax== NULL) {
              Max=NULL;
            } else if (LeftMax == NULL && RightMax!= NULL) {
              Max = RightMax;
              aRight -> Remove(Max);
            } else if (LeftMax != NULL && RightMax== NULL) {
              Max = LeftMax ;
              aLeft ->Remove(Max);
            } else if  ( *LeftMax > *RightMax ) {
              Max = LeftMax;
              aLeft ->Remove(LeftMax);
              aRight -> Remove(RightMax);
            } else {
              Max = RightMax;
              aLeft ->Remove(LeftMax);
              aRight -> Remove(RightMax);
            }
          }
          if (Max !=NULL) {
            aMaxCardList->Insert(new TCard(Max->CName,Max->CMast));
          }
    }
    aLeft  -> RemoveAll();
    aRight -> RemoveAll();
    aLeft -> Assign(aLeftGamer -> aCards);
    aRight -> Assign(aRightGamer -> aCards);
}
//-----------------------------------------------------------------------
tGameBid Player::makemove4out(void) {
    tGameBid GamesTypeRetVal;
    int i;
    int nMaxMastLen=0;
    TMast nMaxMast=SuitNone;
    TMastTable LocalMastTable[5];
    LocalMastTable[0].vzatok=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum=0;


    for (i=1; i<=4; i++ )  {
      LocalMastTable[i] = vzatok4game( (TMast) i,1  );
      LocalMastTable[0].vzatok += LocalMastTable[i].vzatok ;
      LocalMastTable[0].perehvatov +=LocalMastTable[i].perehvatov ;
      LocalMastTable[0].sum += LocalMastTable[i].sum;
    }

    for (i=1; i<=4; i++) { // предполагаемый козырь - самая длинная масть
      if( aCards->AllCard(i) > nMaxMastLen ) {
        nMaxMastLen = aCards->AllCard(i);
        nMaxMast = (TMast) i;
      }
    }

    for (i=1; i<=4; i++) {
      if(aCards->AllCard(i) == nMaxMastLen && nMaxMast != i ) { // есть максимально длинные масти
       if ( LocalMastTable[i].sum  > LocalMastTable[nMaxMast].sum  )  {
         nMaxMast = (TMast) i;
        }
      }
    }

    //масть и звятки уже посчитали
    GamesTypeRetVal = ( tGameBid ) (LocalMastTable[0].vzatok*10 + nMaxMast );
    // это то, что мы можем играть максимально
    return GamesTypeRetVal;

}
//-----------------------------------------------------------------------
tGameBid Player::makemove(tGameBid MaxGame,int HaveAVist,int nGamerVist) { // после получения игроком прикупа - пасс или вист
  Q_UNUSED(nGamerVist)
  tGameBid Answer;
  tGameBid MyMaxGame = makemove4out();

  int vz = MyMaxGame / 10;
  if ( HaveAVist != vist && vz >= nGetMinCard4Vist(MaxGame) ) {
     Answer = vist;
  } else {
     Answer = gtPass;
  }
  if ( HaveAVist==gtPass && vz < nGetMinCard4Vist(MaxGame) ) {
     Answer = gtPass;
  }
  if (MaxGame == g61) {
     Answer = vist; //STALINGRAD !!!
  }
  if (MaxGame == g86) { // miser
     Answer = g86catch;
  }
  GamesType = Answer;
  return Answer;
}
//-----------------------------------------------------------------------
tGameBid Player::makeout4miser(void) {
  TCard *FirstCardOut=NULL,*SecondCardOut=NULL;
  TCard *tmpFirstCardOut,*tmpSecondCardOut;
  TCard *RealFirstCardOut,*RealSecondCardOut;
  tGameBid  Hight = g105,  tmpHight = g105;
  for ( int i=0;i<12;i++ ) {
    tmpFirstCardOut = (TCard*) aCards -> At(i);
    aCards -> AtRemove(i);
    //for ( int j=0;j<12;j++ )  {
    for (int j=i+1; j<12; j++) { // patch from Rasskazov K. (kostik450@mail.ru)
      if ( j!=i ) {
        tmpSecondCardOut = (TCard*) aCards -> At(j);
        aCards -> AtRemove(j);
        Player *tmpGamer = new Player(99);
        tmpGamer -> aCards -> Assign(aCards);
        tmpGamer -> aCards -> mySort();
        tmpHight = tmpGamer -> makemove4out();
        if ( (tmpHight < Hight )
         || (tmpHight == Hight  && tmpGamer->MastTable[0].perehvatov < MastTable[0].perehvatov) ) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer -> aCards -> RemoveAll();
        delete tmpGamer;
        aCards -> AtPut(j,tmpSecondCardOut);
      }
    }
    aCards -> AtPut(i,tmpFirstCardOut);
  }

  RealFirstCardOut     = aCards -> MaxCard(FirstCardOut->CMast);
  aCards -> Remove(RealFirstCardOut);
  aOut -> Insert(RealFirstCardOut);
  if (SecondCardOut->CMast != FirstCardOut->CMast) {
    RealSecondCardOut = aCards -> MaxCard(SecondCardOut->CMast);
   } else {
    RealSecondCardOut = aCards -> MoreThan(FirstCardOut);
    if ( !RealSecondCardOut )
      RealSecondCardOut = aCards -> MaxCard(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
   }
  aCards -> Remove(RealSecondCardOut);
  aOut -> Insert(RealSecondCardOut);
  aCards -> mySort();
  return g86;
}
//-----------------------------------------------------------------------
tGameBid Player::makeout4game(void) {
  TCard *FirstCardOut=NULL,*SecondCardOut=NULL;
  TCard *tmpFirstCardOut,*tmpSecondCardOut;
  TCard *RealFirstCardOut,*RealSecondCardOut;
  tGameBid  Hight = zerogame,  tmpHight = zerogame;
  for ( int i=0;i<12;i++ ) {
    tmpFirstCardOut = (TCard*) aCards -> At(i);
    aCards -> AtRemove(i);
    for ( int j=0;j<12;j++ )  {
      if ( j!=i ) {
        tmpSecondCardOut = (TCard*) aCards -> At(j);
        aCards -> AtRemove(j);
        Player *tmpGamer = new Player(99);
        tmpGamer -> aCards -> Assign(aCards);
        tmpGamer -> aCards -> mySort();
        tmpHight = tmpGamer -> makemove4out();
        if ( (tmpHight > Hight )
         || (tmpHight == Hight  && tmpGamer->MastTable[0].perehvatov < MastTable[0].perehvatov) ) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer -> aCards -> RemoveAll();
        delete tmpGamer;
        aCards -> AtPut(j,tmpSecondCardOut);
      }
    }
    aCards -> AtPut(i,tmpFirstCardOut);
  }

  RealFirstCardOut     = aCards -> MinCard(FirstCardOut->CMast);
  aCards -> Remove(RealFirstCardOut);
  aOut -> Insert(RealFirstCardOut);
  if (SecondCardOut->CMast != FirstCardOut->CMast) {
    RealSecondCardOut = aCards -> MinCard(SecondCardOut->CMast);
   } else {
    RealSecondCardOut = aCards -> MoreThan(FirstCardOut);
    if ( !RealSecondCardOut )
      RealSecondCardOut = aCards -> MinCard(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
   }
  aCards -> Remove(RealSecondCardOut);
  aOut -> Insert(RealSecondCardOut);
  aCards -> mySort();
//  Hight = makemove(undefined,undefined);
  Hight =  makemove4out();
  if ( Hight < GamesType )  {
    /* Вот такая ерунда */
    int tmpvz = Hight/10,tmpmast = Hight - tmpvz*10;
    while ( tmpHight < GamesType )  {
      tmpvz++;
      tmpHight = (tGameBid) (tmpvz*10+tmpmast);
    }
    GamesType = tmpHight;
  } else {
    GamesType = Hight;
  }
  return GamesType;
}
//-----------------------------------------------------------------------
tGameBid Player::makemove(tGameBid lMove,tGameBid rMove) { //ход при торговле
/*  if ( GamesType == gtPass )  {
    GamesType=gtPass;
  } else {*/
  if ( GamesType != gtPass )  {
    int i;
    int nMaxMastLen=0;
    TMast nMaxMast=SuitNone;
    TMastTable LocalMastTable[5];
    tGameBid MGT = qMax(lMove,rMove);
    LocalMastTable[0].vzatok=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum = 0;
    for (i=1; i<=4; i++ )  {
      LocalMastTable[i] = vzatok4game( (TMast) i,1  );
      LocalMastTable[0].vzatok += LocalMastTable[i].vzatok ;
      LocalMastTable[0].perehvatov +=LocalMastTable[i].perehvatov ;
      LocalMastTable[0].sum += LocalMastTable[i].sum;
    }
    for (i=1; i<=4; i++) { // предполагаемый козырь - самая длинная масть
      if( aCards->AllCard(i) > nMaxMastLen ) {
        nMaxMastLen = aCards->AllCard(i);
        nMaxMast = (TMast) i;
      }
    }
    for (i=1; i<=4; i++) {
      if(aCards->AllCard(i) == nMaxMastLen && nMaxMast != i ) { // есть максимально длинные масти
       if ( LocalMastTable[i].sum  > LocalMastTable[nMaxMast].sum  )  {
         nMaxMast = (TMast) i;
        }
      }
    }
    //масть и взятки уже посчитали
    if (MGT <= g75) {
      GamesType = ( tGameBid ) ((LocalMastTable[0].vzatok+1)*10 + nMaxMast );
    } else {
      GamesType = ( tGameBid ) ((LocalMastTable[0].vzatok)*10 + nMaxMast );
    }
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
    } else if(GamesType >= MGT) {
        GamesType = (tGameBid) NextGame(MGT);
    } else {
      GamesType = gtPass;
    }

/*    if( GamesType >= MGT  ) {
        GamesType = (tGameBid) NextGame(MGT);
    } else {
    }*/
    // это то, что мы можем играть максимально
    /*if ( GamesType > gtPass  ) {
      if ( rMove < lMove )  {
        if ( lMove <= GamesType ) {
          if ( lMove!=g61 )
              GamesType = lMove; // say here
          else
              GamesType = (tGameBid) NextGame(lMove);
        } else {
          GamesType = gtPass;
        }
      } else { // rMove > lMove
        if ( GamesType > rMove ) {
//          asm int 3;
            if (rMove!=undefined) {
              GamesType = (tGameBid) NextGame(rMove);
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
//-----------------------------------------------------------------------
int Player::Check4Miser( void) {
  int cur = 0;
  for (int Mast = SuitSpades;Mast<=SuitHearts;Mast++) {
    if ( aCards -> Exist(7,Mast) && ( aCards -> Exist(9,Mast) || aCards -> Exist(8,Mast) ) && ( aCards -> Exist(FACE_JACK,Mast) || aCards -> Exist(10,Mast) )    ){
        cur++;
    }
  }
  if (cur==4)  return 1;
  return 0;
}
//-----------------------------------------------------------------------
TMastTable Player::vzatok4game(TMast Mast, int a23) {
   TMastTable MastTable;
   TCard *MyCard,*tmpCard;
   TCardList *MyCardStack     = new TCardList(MAXMASTLEN);
   TCardList *EnemyCardStack  = new TCardList(MAXMASTLEN);
   for (int c=7;c<=FACE_ACE;c++ ) {
    MyCard = aCards->Exist(c, Mast);
    if ( MyCard )
      MyCardStack->Insert(MyCard);
    else
      EnemyCardStack->Insert(new TCard(c, Mast));
   }
   if ( MyCardStack->AllCard() >=4 && MyCardStack->AllCard() <=5 )    {
      EnemyCardStack -> Free(EnemyCardStack ->MinCard());
   }
   if ( a23 == 23 ) {
       MastTable = Compare2List4Max23(MyCardStack,EnemyCardStack);
   } else {
       MastTable = Compare2List4Max(MyCardStack,EnemyCardStack);
   }
   MastTable.len = aCards->AllCard(Mast);
   MastTable.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->Exist(j,Mast);
    if (  tmpCard ) {
      MastTable.sum +=  tmpCard -> CName;
    }
   }
   MyCardStack -> RemoveAll();
   delete MyCardStack;
   delete EnemyCardStack;
   return MastTable;
}
//-----------------------------------------------------------------------
TMastTable Player::vzatok4pass(TMast Mast,TCardList *aMaxCardList) {
   TMastTable MastTable;
   TCard *MyCard,*tmpCard;
   TCardList *MyCardStack     = new TCardList(MAXMASTLEN);
   TCardList *EnemyCardStack  = new TCardList(MAXMASTLEN);
   for (int c=7;c<=FACE_ACE;c++ ) {
        MyCard = aCards->Exist(c, Mast);
        if ( MyCard ) {
          MyCardStack->Insert(MyCard);
        }
        MyCard = aMaxCardList->Exist(c, Mast);
        if ( MyCard ) {
          EnemyCardStack->Insert(MyCard);
        }
   }
   MastTable = Compare2List4Min(MyCardStack,EnemyCardStack);
   MastTable.len = aCards->AllCard(Mast);
   MastTable.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->Exist(j,Mast);
    if (  tmpCard ) {
      MastTable.sum +=  tmpCard -> CName;
    }
   }
   MyCardStack -> RemoveAll();
   EnemyCardStack-> RemoveAll();
   delete MyCardStack;
   delete EnemyCardStack;
   return MastTable;

}

//-----------------------------------------------------------------------
TMastTable Player::Compare2List4Min(TCardList *My,TCardList *Enemy) { // Для мин результата
  TMastTable MastTable;
  MastTable.vzatok=0;
  MastTable.perehvatov=0;
  int nMaxLen;
  TCard *MyCardMin,*EnemyCardMax,*EnemyCardMin;
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  Enemy->Free(Enemy->MinCard());
  My->AllCard()>Enemy->AllCard() ? nMaxLen = My->AllCard() : nMaxLen = Enemy->AllCard();
  for ( int i=1; i<=nMaxLen; i++ )  {
    MyCardMin = My->MinCard();
    EnemyCardMax = Enemy->MaxCard();
    if ( MyCardMin ) {
      EnemyCardMin = Enemy->LessThan(MyCardMin);
      if (!EnemyCardMin) {
          EnemyCardMin = Enemy->MinCard();
      }

    } else {
//      EnemyCardMin = Enemy->MinCard();
      break;
    }
    if ( !EnemyCardMax && !EnemyCardMin)  {
      break;
    }
    if ( *MyCardMin > *EnemyCardMin ) {
      // викидываем мою  его младшую
      My -> Remove(MyCardMin);
//      Enemy -> Free(EnemyCardMin);
      Enemy -> Remove(EnemyCardMin);

      MastTable.vzatok++;
    } else {
      My -> Remove(MyCardMin);
//      Enemy -> Free(EnemyCardMax);
      Enemy -> Remove(EnemyCardMax);
    }
  }
  return MastTable;
}
//-----------------------------------------------------------------------
TMastTable Player::Compare2List4Max(TCardList *My,TCardList *Enemy) {
  TMastTable MastTable;
  MastTable.vzatok=0;

  MastTable.perehvatov=0;
  int nMaxLen,nIget=1;
  TCard *MyCardMax=0,*EnemyCardMax=0,*EnemyCardMin=0;
  My->AllCard()>Enemy->AllCard() ? nMaxLen = My->AllCard() : nMaxLen = Enemy->AllCard();
  for ( int i=1; i<=nMaxLen; i++ )  {
    MyCardMax = My->MaxCard();
    if ( MyCardMax ) {
      EnemyCardMax = Enemy->MoreThan(MyCardMax);
      if (!EnemyCardMax) {
        EnemyCardMax = Enemy->MinCard();
      }
    } else {
      EnemyCardMax = Enemy->MaxCard();
    }
    EnemyCardMin = Enemy->MinCard();
    if ( !MyCardMax )  {
      break;
    }
    if ( !EnemyCardMax && !EnemyCardMin)  {
      // остаток - мои взятки
      for ( int j =1; j<=My->AllCard();j++ ) {
        MastTable.vzatok++;
      }
      break;
    }
    if ( *MyCardMax > *EnemyCardMax ) {
      // викидываем мою старшую, его младшую
      My -> Remove(MyCardMax);
//      Enemy -> Free(EnemyCardMin);
  Enemy -> Remove(EnemyCardMin);
      MastTable.vzatok++;
      if ( !nIget )  {
        MastTable.perehvatov++;
      }

      nIget = 1;
    } else {
      My -> Remove(MyCardMax);
//      Enemy -> Free(EnemyCardMax);
      Enemy -> Remove(EnemyCardMax);
      nIget = 0;
    }
  }
  return MastTable;
}
//-----------------------------------------------------------------------
TMastTable Player::Compare2List4Max23(TCardList *My,TCardList *Enemy) {
  TMastTable MastTable;
  MastTable.vzatok=0;
  MastTable.perehvatov=0;
  int nMaxLen,nIget=1;
  TCard *MyCardMax,*EnemyCardMax;
  My->AllCard()>Enemy->AllCard() ? nMaxLen = My->AllCard() : nMaxLen = Enemy->AllCard();
  for ( int i=1; i<=nMaxLen; i++ )  {
    EnemyCardMax = Enemy->MaxCard();
    if (EnemyCardMax) {// Пытаемся н-ти больше чеm у него
      MyCardMax = My -> MoreThan(EnemyCardMax);
      if ( ! MyCardMax ) {  // Нет у меня больше чем у него
        MyCardMax = My -> MinCard();
        if (!MyCardMax ) {  // и меньше тож нет
          break ;
        } else {  // Меньше чем у него
          My -> Remove(MyCardMax);
          Enemy -> Remove(EnemyCardMax);
          nIget = 0;
        }
      } else {  // Есть больше чем у него
        My -> Remove(MyCardMax);
        Enemy -> Remove(EnemyCardMax);
        MastTable.vzatok++;
        if (! nIget) MastTable.perehvatov++;
        nIget = 1;
      }
    } else {  // У него нет карт в данной масти ! остаток - мои взятки
      if ( ! Enemy->AllCard() ) {
        for ( int j =1; j<=My->AllCard();j++ ) MastTable.vzatok++;
      }
//25.07.2000 for ( int j =1; j<=My->AllCard();j++ ) MastTable.vzatok++;
      break;
    }
  }
  return MastTable;
}
//-----------------------------------------------------------------------
TCard *Player::MyPass1(TCard *rMove,Player *aLeftGamer,Player *aRightGamer) {
  TCard *cur=NULL;
  tGameBid tmpGamesType=GamesType;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *aTmpList=new TCardList(MAXMASTLEN);
  TCardList *aStackStore=NULL;

  if ( rMove != NULL )  {
    aTmpList=new TCardList(MAXMASTLEN);
    aTmpList -> AssignMast(aCards,(TMast)rMove->CMast);
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
        if ( aLeftGamer->aCards->AllCard(cur->CMast)==0
             && aRightGamer->aCards->AllCard(cur->CMast)==0 ) {

          cur = NULL;
        }
    }
    if ( !cur ) {
         cur = GetMaxCardPere();  //масть с перехватами (max)
    }
    if ( !cur ) {
       cur = GetMinCardWithOutVz(); // лабуду
    }
    if ( !cur ) {
        cur = aCards->MinCard();
    }
  }
  if ( rMove != NULL )  {
    aCards=aStackStore;
    aTmpList -> RemoveAll();
    delete aTmpList;

  }
  /*

  */
  GamesType=tmpGamesType;
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
TCard *Player::MyPass2(TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  TCard *cur=NULL;
  tGameBid tmpGamesType=GamesType;
  int mast = aRightCard -> CMast;
  TCardList *aMaxCardList=new TCardList(20);
  LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
  RecountTables4RasPass(aMaxCardList,1);
/////////////////////////////////////////////////////////
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // распасы // мизер
    cur = aCards -> MaxCard(mast); // !!!!!!!!!!!! здеся обышка
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
              cur = aCards->MinCard();
       }

    } else { // У меня есть масть
        if ( aLeftGamer -> aCards -> AllCard(mast) ) { //У левого есть
            cur = aCards -> LessThan(aLeftGamer -> aCards ->MaxCard(mast));
            if ( !cur ) {
                cur = aCards -> MaxCard(mast);
            }
        } else { // У левого нет
           cur = aCards -> LessThan(aRightCard);
           if ( !cur) {
               cur = aCards -> MaxCard(mast);
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
TCard *Player::MyPass3(TCard *aLeftCard,TCard *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  TCard *cur=NULL;
    /*TCard *MaxCard;*/
    tGameBid tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
//    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // у меня есть масть  в которую зашел левый
       // постараемсмя пропустить
       if (aLeftCard->CMast == aRightCard ->CMast) {
        if (*aRightCard > *aLeftCard) {
          cur = aCards -> LessThan(aRightCard);
        } else {
          cur = aCards -> LessThan(aLeftCard);
        }
       } else {
         cur = aCards -> LessThan(aLeftCard);
       }
       if ( !cur  ) {
           cur = aCards ->MaxCard(aLeftCard->CMast);
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
              cur = aCards->MinCard();
       }

    }
    GamesType=tmpGamesType;
    delete aMaxCardList;

  return cur;
}
//-----------------------------------------------------------------------
void Player::makestatfill(void) {
   TSide lr=LeftHand;
   for (int m=1;m<=4;m++) {
     for (int c=FACE_ACE;c>=7;c-- ) {
      if ( !aCards->Exist(c,m) ) {
         if (lr==LeftHand) {
           aLeft->Insert(new TCard(c,m));
           lr = RightHand;
         } else {
           aRight->Insert(new TCard(c,m));
           lr = LeftHand;
         }
       }
     }
   }
}
//-----------------------------------------------------------------------
void Player::makestatfill(int nCards,int maxmin) {
    int nCounter=0;
    TSide lr=LeftHand;
    if ( maxmin ==1 ) {
       for (int m=1;m<=4;m++) {
         for (int c=FACE_ACE;c>=7;c-- ) {
          if ( !aCards->Exist(c,m) && !aOut->Exist(c,m) ) {
             if (lr==LeftHand) {
               aLeft->Insert(new TCard(c,m));
               lr = RightHand;
             } else {
               aRight->Insert(new TCard(c,m));
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
          if ( !aCards->Exist(c,m) && !aOut->Exist(c,m) ) {
             if (lr==LeftHand) {
               aLeft->Insert(new TCard(c,m));
               lr = RightHand;
             } else {
               aRight->Insert(new TCard(c,m));
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
  if (aCardsOut->At(0)) aCards->Insert(aCardsOut->At(0));
  if (aCardsOut->At(1)) aCards->Insert(aCardsOut->At(1));
  aCardsOut->RemoveAll();
}


///////////////////////////////////////////////////////////////////////////////
// game graphics
///////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
/*
void Player::SetViewPort(void) {
    DeskView->xLen=DeskView->DesktopWidht/2-2*DeskView->xBorder;
    DeskView->yLen=DeskView->DesktopHeight/2-2*DeskView->yBorder;
    Width  = DeskView->xLen;
    switch (nGamer) {
      case 1: {
        Left   = (DeskView->DesktopWidht-DeskView->xLen)/2;
        Top    = DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight;
        break;
      }
      case 2: {
        Left   = DeskView->xBorder;
        Top    = DeskView->yBorder;
        break;
      }
      case 3: {
        Left   = DeskView->DesktopWidht/2+DeskView->xBorder;
        Top    = DeskView->yBorder;
        break;
      }

    }

}
*/


/*
void Player::OnlyMessage (TDeskView *DeskView, int Left, int Top, int Width, int Height) {
  //DeskView->ClearBox(Left, Top, Left+Width, / *Top+* /DeskView->yBorder);
  //Player::Repaint(DeskView, Left, Top, Width, Height);
}
*/


///////////////////////////////////////////////////////////////////////////////
// at least 28 ints (14 int pairs); return # of used ints; the last int is -1
// result: ofs, cardNo, ..., -1
int Player::buildHandXOfs (int *dest, int startX, bool opened) {
  int cnt = 0, oldXX = startX, *oDest = dest;
  TCard *cur = 0, *prev = 0;

  if (nGamer == 3) startX = 0;
  // normal
  for (int i = 0; i < aCards->aCount; i++) {
    TCard *pp = (TCard *)aCards->At(i);
    if (!pp) continue;
    prev = cur;
    cur = pp;
    if (i) {
      if (opened) {
        startX += (prev && prev->CMast != cur->CMast) ? NEW_SUIT_OFFSET : SUIT_OFFSET ;
      } else startX += CLOSED_CARD_OFFSET;
    }
    *dest++ = startX;
    *dest++ = i;
    cnt++;
  }
  *dest++ = -1;
  *dest = -1;

  if (nGamer == 3 && cnt) {
    // righttop
    startX = oldXX-(oDest[(cnt-1)*2]+CARDWIDTH+4);
    for (int f = 0; f < cnt; f++) oDest[f*2] += startX;
  }

  return cnt;
}


void Player::getLeftTop (int playerNo, int *left, int *top) {
  *left = 0; *top = 0;
  //int ofs[28], cnt;
  switch (playerNo) {
    case 1:
      *left = (DeskView->DesktopWidht-(DeskView->DesktopWidht/2-2*DeskView->xBorder))/2;
      *top = DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight;
      break;
    case 2:
      *left = DeskView->xBorder;
      *top = DeskView->yBorder;
      break;
    case 3:
      //*left = DeskView->DesktopWidht/2+DeskView->xBorder;
      *left = DeskView->DesktopWidht-DeskView->xBorder;
      *top = DeskView->yBorder;
/*
      cnt = buildHandXOfs(ofs, *left, true);
      if (cnt) *left = ofs[0]; else *left -= CARDWIDTH+4;
*/
      break;
    default: ;
  }
}

int Player::cardAt (int lx, int ly, bool opened) {
  int ii = -1, ofs[28];
  int left, top;
  getLeftTop(nGamer, &left, &top);
/*
  DeskView->xLen = DeskView->DesktopWidht/2-2*DeskView->xBorder;
  DeskView->yLen = DeskView->DesktopHeight/2-2*DeskView->yBorder;
  Width = DeskView->xLen;
*/
/*
  switch (nGamer) {
    case 1:
      Left = (DeskView->DesktopWidht-(DeskView->DesktopWidht/2-2*DeskView->xBorder))/2;
      Top = DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight;
      break;
    case 2:
      Left = DeskView->xBorder;
      Top = DeskView->yBorder;
      break;
    case 3:
      Left = DeskView->DesktopWidht/2+DeskView->xBorder;
      Top = DeskView->yBorder;
      break;
    default: return -1;
  }
*/
  // end View Port
  buildHandXOfs(ofs, left, opened);
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int curX = ofs[f];
    int x1 = curX, y1 = top;//+DeskView->yBorder;
    int x2 = x1+CARDWIDTH, y2 = y1+CARDHEIGHT;
    if (x1 < lx && lx < x2 && y1 < ly && ly < y2) ii = ofs[f+1];
  } // end for
  return ii;
}


void Player::Repaint (TDeskView *aDeskView, int Left, int Top, int Width, int Height, bool opened, int selNo) {
  Q_UNUSED(Width)
  Q_UNUSED(Height)
  if (!aDeskView) return;
  int ofs[28];

  aCards->mySort();
  buildHandXOfs(ofs, Left, opened);
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = Top;
    TCard *card = (TCard *)aCards->At(ofs[f+1]);
    aDeskView->drawCard(card, x, y, !nInvisibleHand, ofs[f+1]==selNo);
  }
  if (GamesType != undefined) {
    QString msg;
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
    aDeskView->drawText(msg, ofs[0]>=0 ? ofs[0] : Left, Top+CARDHEIGHT+4);
  }
}


void Player::RepaintSimple (bool opened) {
  int left, top;
  getLeftTop(nGamer, &left, &top);
  Repaint(DeskView, left, top, opened, oldii);
}


void Player::HintCard (int lx, int ly) {
  Q_UNUSED(lx)
  Q_UNUSED(ly)
}


///////////////////////////////////////////////////////////////////////////////
// game networking
///////////////////////////////////////////////////////////////////////////////
int Player::GetNikName () {
  return 1;
}


int Player::connecttoserver (const QString &host, unsigned short port) {
  Q_UNUSED(host)
  Q_UNUSED(port)
  return 1;
}


int Player::connecttodesk (int req) {
  Q_UNUSED(req)
  return 1;
}


int Player::join (int req) {
  Q_UNUSED(req)
  return 1;
}


void Player::NetGetCards (TColoda *Coloda) {
  Q_UNUSED(Coloda)
}


int Player::SendnPlayerTakeCards (int who) {
  Q_UNUSED(who)
  return 0;
}
