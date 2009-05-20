/***************************************************************************
                          gamer.cpp  -  description
                             -------------------
    begin                : Tue May 23 2000
    copyright            : (C) 2000 by Azarniy I.V.
    email                : azarniy@usa.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include <QPixmap>
#include <QPainter>

//#include "pcommon.h"

#include "prfconst.h"
#include "deskview.h"
#include "gamer.h"
#include "card.h"
#include "cardlist.h"
#include "coloda.h"
#include "ncounter.h"
#include "plscore.h"

//#include "netconst.h"


//-------------------------------------------------------------------
TGamer::TGamer(int _nGamer) {
    nGamer = _nGamer;
    aScore = new TPlScore();
    aCards = new TCardList(12); // мои
    aLeft  = new TCardList(10);  // Противника с лева (предполагаемый или открытые)
    aRight = new TCardList(10); // С права (предполагаемый или открытые)
    aOut   = new TCardList(2);   // Снос (мой или предполагаемый)
    aCardsOut = new TCardList(12); // во взятках мои
    aLeftOut  = new TCardList(12);  // во взятках Противника с лева (предполагаемый или открытые)
    aRightOut = new TCardList(12); // во взятках С права (предполагаемый или открытые)
    Message = new char[MAXMESSAGELEN];
    NikName = new char[MAXNIKNAMELEN];
    NikPasswd = new char[MAXNIKNAMELEN];
    strcpy(Message,"");
    strcpy(NikName,"");
    socket =0;
    set0();
}
//-------------------------------------------------------------------
TGamer::TGamer(int _nGamer,TDeskView *_DeskView) {
    nGamer = _nGamer;
    aScore = new TPlScore();
    aCards = new TCardList(12);
    aLeft  = new TCardList(10);
    aRight = new TCardList(10);
    aOut   = new TCardList(2);
    aCardsOut = new TCardList(12);
    aLeftOut  = new TCardList(12);
    aRightOut = new TCardList(12);
    DeskView = _DeskView;
    Message = new char[MAXMESSAGELEN];
    NikName = new char[MAXNIKNAMELEN];
    NikPasswd = new char[MAXNIKNAMELEN];
    strcpy(Message,"");
    strcpy(NikName,"");
    socket = 0;
    set0();
}
//-------------------------------------------------------------------
TGamer::~TGamer() {
    /*if (socket)
      close(socket);
 */
    delete aCards;
    delete aLeft;
    delete aRight;
    delete aOut;
    delete aCardsOut;
    delete aLeftOut;
    delete aRightOut;
    delete aScore;
    delete Message;
    delete NikName;
    delete NikPasswd;

}
//-----------------------------------------------------------------------
void TGamer::set0(void) {
  flMiser=0;
  aCards->RemoveAll();
  aLeft->RemoveAll();
  aRight->RemoveAll();
  aOut->RemoveAll();
  aCardsOut->RemoveAll();
  aLeftOut->RemoveAll();
  aRightOut->RemoveAll();
  Mast=withoutmast;
  GamesType=undefined;
  Enemy=noside;
  nGetsCard = 0;
  WaitForMouse = 0;
  oldii = -1;
  for ( int i=0;i<=4;i++ )  {
    MastTable[i].vzatok=0;
    MastTable[i].perehvatov=0;
  }
  if (nGamer == 1) {
    nCardClosed = 0;
  } else {
    nCardClosed = 1;
  }
  Pronesti = NULL;

}
//-----------------------------------------------------------------------
void TGamer::AddCard(TCard *aCard) {
  aCards->Insert(aCard);
}
//-----------------------------------------------------------------------
void TGamer::AddCard(int _CName, int _CMast) {
  aCards->Insert(new TCard (_CName,_CMast));
}
//-----------------------------------------------------------------------
TCard *TGamer::Miser1(TGamer *aLeftGamer,TGamer *aRightGamer) {
    TCard *RetVal=NULL;
    if (aCards -> AllCard()==10) {  // first move первый выход в 8..A если она одна
        for (int m=1;m<=4;m++) {
          if (aCards->AllCard(m)==1) {
            RetVal = aCards -> MinCard(m);
            break;
          }
        }
    }
    if (!RetVal) {
        for (int m=4;m>=1;m--) {
          if (RetVal) break;
          for (int c=A;c>=7;c--) {
            if (RetVal) break;
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
                RetVal = my;
              }
            }
          }
        }
    }
    if (!RetVal) RetVal = aCards->MinCard(1);
    if (!RetVal) RetVal = aCards->MinCard(2);
    if (!RetVal) RetVal = aCards->MinCard(3);
    if (!RetVal) RetVal = aCards->MinCard(4);
    return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::Miser2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
    TCard *RetVal=NULL;
      if ( aCards->AllCard( aRightCard->CMast) ) {
          for (int c=A;c>=7;c--) {
            if (RetVal) break;
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
                RetVal = my;
              }
            }
          }
          if (!RetVal) RetVal = aCards->MaxCard(aRightCard->CMast);
        } else {
          TCardList *aMaxCardList=new TCardList(20);
          LoadLists(aRightGamer,aLeftGamer,aMaxCardList);
           RetVal = GetMaxCardWithOutPere();
             if ( !RetVal ) {
               RetVal = GetMaxCardPere();  //масть с перехватами (max)
           }
             if ( !RetVal ) {
             RetVal = GetMinCardWithOutVz(); // лабуду
           }
             if ( !RetVal ) {
              RetVal = aCards->MaxCard();
          }
          delete aMaxCardList;
        }
    return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::Miser3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
// copy from MyPass3
  TCard *RetVal=NULL;
    /*TCard *MaxCard;*/
    TGamesType tmpGamesType=GamesType;
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
          RetVal = aCards -> LessThan(aRightCard);
        } else {
          RetVal = aCards -> LessThan(aLeftCard);
        }
       } else {
         RetVal = aCards -> LessThan(aLeftCard);
       }
       if ( !RetVal  ) {
           RetVal = aCards ->MaxCard(aLeftCard->CMast);
       }
    } else {
      //  у меня нет масти в которую зашел левый
       RetVal = GetMaxCardWithOutPere();
       if ( !RetVal ) {
               RetVal = GetMaxCardPere();  //масть с перехватами (max)
       }
       if ( !RetVal ) {
             RetVal = GetMinCardWithOutVz(); // лабуду
       }
       if ( !RetVal ) {
              RetVal = aCards->MinCard();
       }

    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
  return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::makemove(TCard *lMove,TCard *rMove,TGamer *aLeftGamer,TGamer *aRightGamer) { //ход
    TCard *RetVal=NULL;
    if (lMove == NULL && rMove == NULL) { // мой заход - первый
        if ( GamesType == pass || GamesType == vist ) {
              // кто-то играет а я как бы вистую
              RetVal = MyVist1(aLeftGamer,aRightGamer);
        } else  if (GamesType == g86catch) {
            RetVal = MiserCatch1(aLeftGamer,aRightGamer);
          } else  if (GamesType == g86) {
          RetVal = Miser1(aLeftGamer,aRightGamer);
          } else   if (  GamesType == raspass  ) {
            RetVal = MyPass1(rMove,aLeftGamer,aRightGamer);; // ну типа распасы или мизер
        } else {
            // ну типа моя игра
            RetVal = MyGame1(aLeftGamer,aRightGamer);
        }

    }
    if (lMove == NULL && rMove != NULL) { // мой заход - второй
        if ( GamesType == pass || GamesType == vist ) {
                // кто-то играет а я как бы вистую
                RetVal = MyVist2(rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86catch) {
                RetVal = MiserCatch2(rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86) {
              RetVal = Miser2(rMove,aLeftGamer,aRightGamer);
          } else  if ( GamesType == raspass  ) {
                RetVal = MyPass2(rMove,aLeftGamer,aRightGamer); // ну типа распасы или мизер
        } else {   // ну типа моя игра
            RetVal = MyGame2(rMove,aLeftGamer,aRightGamer);
        }

    }
    if (lMove != NULL && rMove != NULL) { // мой заход - 3
        if ( GamesType == pass || GamesType == vist ) {
                // кто-то играет а я как бы вистую
           RetVal = MyVist3(lMove,rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86catch) {
            RetVal = MiserCatch3(lMove,rMove,aLeftGamer,aRightGamer);
          } else  if (GamesType == g86) {
            RetVal = Miser3(lMove,rMove,aLeftGamer,aRightGamer);
          } else if ( GamesType == raspass ) {
           RetVal = MyPass3(lMove,rMove,aLeftGamer,aRightGamer);; // ну типа распасы или мизер
        } else {
           // ну типа моя игра
           RetVal = MyGame3(lMove,rMove,aLeftGamer,aRightGamer);
        }
    }
    aCards    -> Remove(RetVal);
    aCardsOut -> Insert(RetVal);
    return RetVal;
}
//-----------------------------------------------------------------------
TMastTable TGamer::vzatok(TMast Mast,TCardList *aMaxCardList,int a23) {
   TMastTable MastTable;
   TCard *MyCard,*tmpCard;
   TCardList *MyCardStack     = new TCardList(MAXMASTLEN);
   TCardList *EnemyCardStack  = new TCardList(MAXMASTLEN);
   for (int c=7;c<=A;c++ ) {
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
   for (int j=7;j<=A;j++ ) {
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
void TGamer::RecountTables4RasPass(TCardList *aMaxCardList,int a23) { // Пересчитывает таблицу дли распасов или мизера
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
void TGamer::RecountTables( TCardList *aMaxCardList,int a23 ){ // Пересчитывает таблицу         TMastTable MastTable[5];
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
TCard *TGamer::MiserCatch1(TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *RetVal=NULL;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *Naparnik;
  TSide Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer -> aCards;
    Side = right;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer -> aCards;
    Side = left;
  }
  RecountTables4RasPass(aMaxCardList,1);
//  if (Side == left) {
// 1. Пытаемся всунуть
    for (int m=1;m<=4;m++) {
      TCard *EnemyMin = aMaxCardList -> MinCard(m);
      TCard *NaparnikMin = Naparnik -> MinCard(m);
      TCard *MyMin = aCards -> MinCard(m);
      if (RetVal)
        goto badlabel;
      if (EnemyMin) { //есть такая
        if (MyMin) { // и у меня есть такая
          if (NaparnikMin) { // и у напарника есть такая
            if ( *NaparnikMin < *EnemyMin && *MyMin < *EnemyMin) {
              RetVal = MyMin;// Всунули
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
                    RetVal = aCards ->LessThan(EnemyMax);
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
              RetVal = MyMin;// Всунули
              goto badlabel;
            }
          }
        }
      }
    }


//    2 отгребаем свое
    if (!RetVal) RetVal = GetMaxCardWithOutPere();
    if (Side == left) {
      // под мизирящегося надо заходить по другому !!!
    }
// 3    пытаемс
    if (!RetVal) RetVal = GetMaxCardWithOutPere();
    if (!RetVal) RetVal = aCards->MinCard();
//  } else { //Side == left
//  }
badlabel:

  aMaxCardList->RemoveAll();
  delete aMaxCardList;
  return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::MiserCatch2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *RetVal=NULL;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *Naparnik;
  TSide Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer -> aCards;
    Side = right;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer -> aCards;
    Side = left;
  }
  RecountTables4RasPass(aMaxCardList,23);

  if (Side == left) { //ход под меня , противник с права
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
            RetVal=aCards->LessThan(EnMin);
            if(!RetVal) RetVal = aCards -> MoreThan(EnMin);
          } else {
          RetVal= aCards -> MoreThan(EnMin);
          }
        } else {
          RetVal = MyMax; // А вот тут надо смотреть кому нужен ход
        }

      } else {




        // у меня нет карт  в данной масти
        if (Pronesti) {
          //надо пронести карту
          if (aCards->Exist(Pronesti->CName,Pronesti->CMast)) {
            RetVal = Pronesti;
          } else {
            RetVal = aCards -> MaxCard(Pronesti->CMast);
            if (!RetVal)   RetVal = aCards -> MaxCard();
          }
        } else {
          // указания на пронос - нет
          RetVal = GetMaxCardPere();
        if (!RetVal) RetVal = aCards -> MaxCard();
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
          RetVal = aCards->LessThan(aRightCard);
        } else {
          RetVal = MyMax; // кому передать код ?
        }
      } else {
        RetVal = MyMax; // грохать ее самой большой
      }

    } else {
      // а у меня нет масти

          RetVal = GetMaxCardPere();
        if (!RetVal) RetVal = aCards -> MaxCard();
    }

  }
  Pronesti = NULL;
  if (!RetVal)
    RetVal = aCards -> MinCard();

  aMaxCardList->RemoveAll();
  delete aMaxCardList;
  return RetVal;

}
//-----------------------------------------------------------------------
TCard *TGamer::MiserCatch3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *RetVal=NULL;
  TCardList *aMaxCardList=new TCardList(20);
  TCardList *Naparnik;
  TSide Side ;
  if (aRightGamer->GamesType == g86) {
    LoadLists(aRightGamer,aRightGamer,aMaxCardList);
    Naparnik = aLeftGamer -> aCards;
    Side = right;
  } else {
    LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
    Naparnik = aRightGamer -> aCards;
    Side = left;
  }
  RecountTables4RasPass(aMaxCardList,23);

  if (Side == left) {
      if (aCards->AllCard(aLeftCard->CMast)) {
        // у меня есть карты  в данной масти
//        TCard *MyMax = aCards->MaxCard(aLeftCard->CMast);
//        TCard *MyMin = aCards->MinCard(aLeftCard->CMast);
        if (*aLeftCard < *aRightCard) {
          // напарник уже придавил карту
          RetVal = aCards -> MaxCard(aLeftCard->CMast);

        } else {
          //есть шанс всунуть
          RetVal = aCards -> LessThan(aLeftCard);
          if (!RetVal ) RetVal = aCards -> MaxCard(aLeftCard->CMast);
        }
      } else {
        // у меня нет карт  в данной масти
        if (Pronesti) {
          //надо пронести карту
          if (aCards->Exist(Pronesti->CName,Pronesti->CMast)) {
            RetVal = Pronesti;
          } else {
            RetVal = aCards -> MaxCard(Pronesti->CMast);
            if (!RetVal) RetVal = aCards -> MaxCard();
          }
        } else {
          // указания на пронос - нет
          RetVal = GetMaxCardPere();
        if (!RetVal) RetVal = aCards -> MaxCard();
        }
      }
  } else { // right - my friend
    TCard *MyMax = aCards->MaxCard(aLeftCard->CMast);
    TCard *MyMin = aCards->MinCard(aLeftCard->CMast);
    if (MyMax) {
      if ( *MyMin < *aLeftCard) {
//        есть возможность всунуть
        TCard *NapMin = Naparnik->LessThan(aLeftCard);
        if (NapMin) {
          RetVal = aCards->LessThan(aLeftCard);
        } else {
          RetVal = MyMax; // кому передать код ?
        }
      } else {
        RetVal = MyMax; // грохать ее самой большой
      }

    } else {
      // а у меня нет масти
          RetVal = GetMaxCardPere();
        if (!RetVal) RetVal = aCards -> MaxCard();
    }

  }
  Pronesti = NULL;

  aMaxCardList->RemoveAll();
  delete aMaxCardList;
  return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::MyGame3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) { // моя игра 3 заход - мой
    TCard *RetVal=NULL;
    TCard *MaxCard;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // у меня есть масть  в которую зашел левый
        if ( aLeftCard->CMast == aRightCard->CMast)  {
            // постараемсмя ударить
            RetVal = aCards -> MoreThan(aLeftCard);
            if ( !RetVal  ) {
               RetVal = aCards ->MinCard(aLeftCard->CMast);
            }
        } else { //aLeftCard->CMast == aRightCard->CMast
          if ( aRightCard->CMast !=mast)  {
            // не бита козырем

            RetVal = aCards -> MoreThan(aLeftCard);
            if ( !RetVal ) {
              RetVal = aCards -> MinCard(aLeftCard->CMast);
            }
          } else {
            // бита козырем, мне не надо бить
            RetVal = aCards -> MinCard(aLeftCard->CMast);
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
       RetVal = aCards -> MoreThan(MaxCard);
       if (!RetVal) {
           RetVal = aCards -> MinCard(mast);
       }
       if ( ! RetVal ) {// нет коз.
           RetVal = GetMinCardWithOutVz();
       }
       if ( ! RetVal ) {
           RetVal = aCards -> MinCard();
       }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return RetVal;

}
//-----------------------------------------------------------------------
TCard *TGamer::MyVist3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) { // мой вист или пас 3 заход - мой
    TCard *RetVal=NULL;
    TCard *MaxCard;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    TGamer *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer -> GamesType != pass && aLeftGamer -> GamesType != vist ) {
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
            RetVal = aCards->MinCard(aRightCard->CMast);
          } else {
            // постараемсмя ударить
            RetVal = aCards -> MoreThan(aLeftCard);
            if ( !RetVal  ) {
               RetVal = aCards ->MinCard(aLeftCard->CMast);
            }
          }
        } else { //aLeftCard->CMast == aRightCard->CMast
          if ( aRightCard->CMast !=mast)  {
            // не бита козырем
            RetVal = aCards -> MoreThan(aLeftCard);
            if ( !RetVal ) {
              RetVal = aCards -> MinCard(aLeftCard->CMast);
            }
          } else {
            // бита козырем, мне не надо бить
            RetVal = aCards -> MinCard(aLeftCard->CMast);
          }
        }

      } else {
        //правый вражииина
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        if ( aLeftCard->CMast == aRightCard->CMast  )  {
          if ( *aRightCard > *aLeftCard ) {
            // попытаться прибить правого
            RetVal = aCards->MoreThan(aRightCard);
            if ( !RetVal ) {
              RetVal = aCards->MinCard(aRightCard->CMast);
            }
          } else {
            // бить не надо
            RetVal = aCards->MinCard(aRightCard->CMast);
          }
        } else {
          if ( aRightCard->CMast==mast  ) {
            // лвевый зашел, а правый пригрел козырем
            RetVal = aCards->MinCard(aLeftCard->CMast);
          } else { // Ух ты .... Левый друг зашел а правый пронес ерунду !!!
            RetVal = aCards->MinCard(aLeftCard->CMast);
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
               RetVal = aCards -> MoreThan(aLeftCard);
           } else { // Уже наша взятка... скидываем минимальную
               RetVal = aCards -> MinCard(MaxCard->CMast);
           }
       } else { // с лева - напарник
          LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,23);

           if (MaxCard == aLeftCard ) { // Уже наша взятка... скидываем минимальную
               RetVal = aCards -> MinCard(MaxCard->CMast);
           } else {//попытаемся хлопнуть его карту
               RetVal = aCards -> MoreThan(aLeftCard);
           }
       }
       if (!RetVal) {
           RetVal = aCards -> MinCard(mast);
       }
       if ( ! RetVal ) {// нет коз.
           RetVal = GetMinCardWithOutVz();
       }
       if ( ! RetVal ) {
           RetVal = aCards -> MinCard();
       }


    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return RetVal;

}
//-----------------------------------------------------------------------

TCard *TGamer::MyVist2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) { // мой вист или пас 2 заход - мой
    TCard *RetVal=NULL;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    TGamer *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer -> GamesType != pass && aLeftGamer -> GamesType != vist ) {
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
            RetVal = aCards -> MinCard(aRightCard -> CMast);
        } else { // может умный и больше заходов у него нет в данную масть а есть козырь
            RetVal = aCards -> MaxCard(aRightCard -> CMast);
        }
        if ( !RetVal ) { // а у меня масти и нет !!!
            RetVal = aCards -> MaxCard(mast); // прибъем максимальной
        }
        if ( ! RetVal ) {
            RetVal = GetMinCardWithOutVz();
        }
    } else { // с лева - напарник
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        RetVal = aCards->MoreThan(aRightCard);
        if (!RetVal) {
            RetVal = aCards->MinCard(aRightCard->CMast);
        }
        if (!RetVal) { // Нет масти пробуем козырь
            RetVal = aCards->MinCard(mast);
        }
        if (!RetVal) { // Нет масти и козыря
            RetVal = GetMinCardWithOutVz();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return RetVal;

}
//-----------------------------------------------------------------------
TCard *TGamer::MyVist1(TGamer *aLeftGamer,TGamer *aRightGamer) {
// 1 - выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
    TCard *RetVal=NULL;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    TGamer *aEnemy,*aFriend;
    int mast;
    // Кто игрок а кто напарник
    if ( aLeftGamer -> GamesType != pass && aLeftGamer -> GamesType != vist ) {
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
                RetVal = aCards ->MinCard(aLeftGamer ->aCards ->EmptyMast (mast));
              } else {
                if (aLeftGamer ->aCards ->MaxCard(mast)->CName < m->CName) {
                  RetVal = aCards -> MaxCard(mast);
                } else {
                  RetVal = aCards ->MinCard(aLeftGamer ->aCards ->EmptyMast (mast));
                }
              }
            } else {
                  RetVal = aCards ->MinCard(aLeftGamer ->aCards ->EmptyMast (mast));
            }
        if ( !RetVal) {
          RetVal = GetMinCardWithOutVz();
        }

/*        if ( aCards -> Exist(A,RetVal->CMast) || aCards -> Exist(K,RetVal->CMast) ) {
          RetVal = NULL;
            RetVal = aCards ->MinCard(aMaxCardList ->EmptyMast (0));
        }*/


        if ( !RetVal) {
            RetVal = GetMaxCardWithOutPere();
        }
        if ( !RetVal) {
            RetVal = aCards -> MinCard(mast);
        }
        if ( !RetVal) {
            RetVal = GetMaxCardPere();
        }
    } else { // с лева - напарник
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,1);

        RetVal = GetMaxCardWithOutPere();
        if ( !RetVal) {
            RetVal = GetMaxCardPere();
        }
        if ( !RetVal) {
            RetVal = aCards -> MaxCard();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return RetVal;
}

//-----------------------------------------------------------------------
TCard *TGamer::MyGame2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
    TCardList *aMaxCardList=new TCardList(20);
    TGamesType tmpGamesType=GamesType;
    TCard *RetVal=NULL;
    TCard *MaxLeftCard=NULL;
    int mast = aRightCard -> CMast;
    int koz  = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);

    RetVal = aCards -> MaxCard(mast);
    if ( !RetVal ) { // Нет Масти
        if ( !aLeftGamer -> aCards -> AllCard(mast) ) { // У левого тож нету
            MaxLeftCard = aLeftGamer -> aCards ->MaxCard(koz);
            RetVal = aCards -> MoreThan(MaxLeftCard); //
            if (!RetVal) {
                RetVal = aCards -> LessThan(MaxLeftCard);
            }
            if ( !RetVal ) {
                RetVal = GetMinCardWithOutVz(); // лабуду
            }
            if ( !RetVal ) {
                 RetVal = aCards->MinCard();
            }
        } else { // Есть масть у левого
            RetVal = aCards -> MinCard(koz);
        }
    } else { // У меня есть масть
        if ( aLeftGamer -> aCards -> AllCard(mast) ) { //У левого есть
            MaxLeftCard = aLeftGamer -> aCards ->MaxCard(mast);
            RetVal = aCards -> MoreThan(MaxLeftCard);
            if ( !RetVal ) {
                RetVal = aCards -> LessThan(MaxLeftCard);
            }
        } else { // У левого нет
            if ( !aLeftGamer -> aCards -> AllCard(koz) ) { // И козыре й нет
                RetVal = aCards -> MoreThan(aRightCard);
                if ( !RetVal) {
                    RetVal = aCards -> MinCard(mast);
                }
            } else { // есть у левого козыря
                RetVal = aCards -> MinCard(mast);
            }
        }
    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::MyGame1(TGamer *aLeftGamer,TGamer *aRightGamer) {
// 1-выбить козыря 2-разиграть масти с перехватами 3-без перехватов 4-???
    TCard *RetVal=NULL;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    int mast = GamesType - (GamesType/10) * 10;
    // набираем списки
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,1);
    if ( aLeft->AllCard(mast) || aRight->AllCard(mast) ) { // У противников есть козыря
        RetVal = aCards->MaxCard(mast);
        if ( !RetVal ) { // а у меня их нет !!!
             RetVal = GetMaxCardWithOutPere(); // без пере
        }
        if ( !RetVal ) {
             RetVal = GetMaxCardPere();  //масть с перехватами (max)
        }
        if ( !RetVal ) {
             RetVal = GetMinCardWithOutVz(); // лабуду
        }
        if ( !RetVal ) {
             RetVal = aCards->MaxCard();
        }
    } else { // а козырьков у них и нету !!!
        if ( !RetVal ) {
             RetVal = GetMaxCardPere();  //масть с перехватами (max)
        }
        if ( !RetVal ) { // а у меня их нет !!!
             RetVal = GetMaxCardWithOutPere(); // без пере
        }
        if ( !RetVal ) {
             RetVal = GetMinCardWithOutVz(); // лабуду
        }
        if ( !RetVal ) {
            RetVal = aCards->MaxCard(mast);
        }
        if ( !RetVal ) {
             RetVal = aCards->MaxCard();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::GetMaxCardPere(void) {
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
TCard *TGamer::GetMaxCardWithOutPere(void) {
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
TCard *TGamer::GetMinCardWithOutVz(void) {
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
void TGamer::LoadLists(TGamer *aLeftGamer,TGamer *aRightGamer,TCardList *aMaxCardList) {
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
      for (int c=A;c>=7;c-- ) {
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
TGamesType TGamer::makemove4out(void) {
    TGamesType GamesTypeRetVal;
    int i;
    int nMaxMastLen=0;
    TMast nMaxMast=withoutmast;
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
    GamesTypeRetVal = ( TGamesType ) (LocalMastTable[0].vzatok*10 + nMaxMast );
    // это то, что мы можем играть максимально
    return GamesTypeRetVal;

}
//-----------------------------------------------------------------------
TGamesType TGamer::makemove(TGamesType MaxGame,int HaveAVist,int nGamerVist) { // после получения игроком прикупа - пасс или вист
  Q_UNUSED(nGamerVist)
  TGamesType Answer;
  TGamesType MyMaxGame = makemove4out();

  int vz = MyMaxGame / 10;
  if ( HaveAVist != vist && vz >= nGetMinCard4Vist(MaxGame) ) {
     Answer = vist;
  } else {
     Answer = pass;
  }
  if ( HaveAVist==pass && vz < nGetMinCard4Vist(MaxGame) ) {
     Answer = pass;
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
TGamesType TGamer::makeout4miser(void) {
  TCard *FirstCardOut=NULL,*SecondCardOut=NULL;
  TCard *tmpFirstCardOut,*tmpSecondCardOut;
  TCard *RealFirstCardOut,*RealSecondCardOut;
  TGamesType  Hight = g105,  tmpHight = g105;
  for ( int i=0;i<12;i++ ) {
    tmpFirstCardOut = (TCard*) aCards -> At(i);
    aCards -> AtRemove(i);
    //for ( int j=0;j<12;j++ )  {
    for (int j=i+1; j<12; j++) { // patch from Rasskazov K. (kostik450@mail.ru)
      if ( j!=i ) {
        tmpSecondCardOut = (TCard*) aCards -> At(j);
        aCards -> AtRemove(j);
        TGamer *tmpGamer = new TGamer(99);
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
TGamesType TGamer::makeout4game(void) {
  TCard *FirstCardOut=NULL,*SecondCardOut=NULL;
  TCard *tmpFirstCardOut,*tmpSecondCardOut;
  TCard *RealFirstCardOut,*RealSecondCardOut;
  TGamesType  Hight = zerogame,  tmpHight = zerogame;
  for ( int i=0;i<12;i++ ) {
    tmpFirstCardOut = (TCard*) aCards -> At(i);
    aCards -> AtRemove(i);
    for ( int j=0;j<12;j++ )  {
      if ( j!=i ) {
        tmpSecondCardOut = (TCard*) aCards -> At(j);
        aCards -> AtRemove(j);
        TGamer *tmpGamer = new TGamer(99);
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
      tmpHight = (TGamesType) (tmpvz*10+tmpmast);
    }
    GamesType = tmpHight;
  } else {
    GamesType = Hight;
  }
  return GamesType;
}
//-----------------------------------------------------------------------
TGamesType TGamer::makemove(TGamesType lMove,TGamesType rMove) { //ход при торговле
/*  if ( GamesType == pass )  {
    GamesType=pass;
  } else {*/
  if ( GamesType != pass )  {
    int i;
    int nMaxMastLen=0;
    TMast nMaxMast=withoutmast;
    TMastTable LocalMastTable[5];
    TGamesType MGT = qMax(lMove,rMove);
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
    //масть и звятки уже посчитали
    if (MGT <= g75) {
      GamesType = ( TGamesType ) ((LocalMastTable[0].vzatok+1)*10 + nMaxMast );
    } else {
      GamesType = ( TGamesType ) ((LocalMastTable[0].vzatok)*10 + nMaxMast );
    }
    if ( (rMove==pass || rMove==undefined) && ( lMove==pass || lMove==undefined ) ) {
      if ( LocalMastTable[0].vzatok >= 4 ) {
        GamesType = g61;
      } else {
        if ( LocalMastTable[0].vzatok == 0 )  {
           // check for miser !!!
          if (Check4Miser()) {
            GamesType = g86;
          } else {
            GamesType = pass;
          }
        } else {
          GamesType = pass;
        }
      }
    } else if(GamesType >= MGT) {
        GamesType = (TGamesType) NextGame(MGT);
    } else {
      GamesType = pass;
    }

/*    if( GamesType >= MGT  ) {
        GamesType = (TGamesType) NextGame(MGT);
    } else {
    }*/
    // это то, что мы можем играть максимально
    /*if ( GamesType > pass  ) {
      if ( rMove < lMove )  {
        if ( lMove <= GamesType ) {
          if ( lMove!=g61 )
              GamesType = lMove; // say here
          else
              GamesType = (TGamesType) NextGame(lMove);
        } else {
          GamesType = pass;
        }
      } else { // rMove > lMove
        if ( GamesType > rMove ) {
//          asm int 3;
            if (rMove!=undefined) {
              GamesType = (TGamesType) NextGame(rMove);
             } else {
              GamesType = g61;
             }
        } else {
          GamesType = pass;
        }

      }
    } else {     //  GamesType > pass
      if ( (rMove==pass || rMove==undefined) && ( lMove==pass || lMove==undefined ) ) {
        if ( LocalMastTable[0].vzatok >= 4 ) {
          GamesType = g61;
        } else {
          if ( LocalMastTable[0].vzatok == 0 )  {
            // check for miser !!!
            if (Check4Miser()) {
              GamesType = g86;
            } else {
              GamesType = pass;
            }
          } else {
            GamesType = pass;
          }
        }
      } else {
        GamesType = pass;
      }
    }*/
  }
  return GamesType;
}
//-----------------------------------------------------------------------
int TGamer::Check4Miser( void) {
  int RetVal = 0;
  for (int Mast = pica;Mast<=cherva;Mast++) {
    if ( aCards -> Exist(7,Mast) && ( aCards -> Exist(9,Mast) || aCards -> Exist(8,Mast) ) && ( aCards -> Exist(J,Mast) || aCards -> Exist(10,Mast) )    ){
        RetVal++;
    }
  }
  if (RetVal==4)  return 1;
  return 0;
}
//-----------------------------------------------------------------------
TMastTable TGamer::vzatok4game(TMast Mast, int a23) {
   TMastTable MastTable;
   TCard *MyCard,*tmpCard;
   TCardList *MyCardStack     = new TCardList(MAXMASTLEN);
   TCardList *EnemyCardStack  = new TCardList(MAXMASTLEN);
   for (int c=7;c<=A;c++ ) {
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
   for (int j=7;j<=A;j++ ) {
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
TMastTable TGamer::vzatok4pass(TMast Mast,TCardList *aMaxCardList) {
   TMastTable MastTable;
   TCard *MyCard,*tmpCard;
   TCardList *MyCardStack     = new TCardList(MAXMASTLEN);
   TCardList *EnemyCardStack  = new TCardList(MAXMASTLEN);
   for (int c=7;c<=A;c++ ) {
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
   for (int j=7;j<=A;j++ ) {
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
TMastTable TGamer::Compare2List4Min(TCardList *My,TCardList *Enemy) { // Для мин результата
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
TMastTable TGamer::Compare2List4Max(TCardList *My,TCardList *Enemy) {
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
TMastTable TGamer::Compare2List4Max23(TCardList *My,TCardList *Enemy) {
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
TCard *TGamer::MyPass1(TCard *rMove,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *RetVal=NULL;
  TGamesType tmpGamesType=GamesType;
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
     // тема такая : 100 % отбери сво└, если такового нет - мах в минимально длинной масти
    RetVal = GetMaxCardWithOutPere(); // за искл тех случ коды масти уже нет
    if ( RetVal ) {
        if ( aLeftGamer->aCards->AllCard(RetVal->CMast)==0
             && aRightGamer->aCards->AllCard(RetVal->CMast)==0 ) {

          RetVal = NULL;
        }
    }
    if ( !RetVal ) {
         RetVal = GetMaxCardPere();  //масть с перехватами (max)
    }
    if ( !RetVal ) {
       RetVal = GetMinCardWithOutVz(); // лабуду
    }
    if ( !RetVal ) {
        RetVal = aCards->MinCard();
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
  return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::MyPass2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *RetVal=NULL;
  TGamesType tmpGamesType=GamesType;
  int mast = aRightCard -> CMast;
  TCardList *aMaxCardList=new TCardList(20);
  LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
  RecountTables4RasPass(aMaxCardList,1);
/////////////////////////////////////////////////////////
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // распасы // мизер
    RetVal = aCards -> MaxCard(mast); // !!!!!!!!!!!! здеся обышка
    if ( !RetVal ) { // Нет Масти
      // во шара !!! Льем мах в масти где  есть 100 % взятки
       RetVal = GetMaxCardWithOutPere();
       if ( !RetVal ) {
               RetVal = GetMaxCardPere();  //масть с перехватами (max)
       }
       if ( !RetVal ) {
             RetVal = GetMinCardWithOutVz(); // лабуду
       }
       if ( !RetVal ) {
              RetVal = aCards->MinCard();
       }

    } else { // У меня есть масть
        if ( aLeftGamer -> aCards -> AllCard(mast) ) { //У левого есть
            RetVal = aCards -> LessThan(aLeftGamer -> aCards ->MaxCard(mast));
            if ( !RetVal ) {
                RetVal = aCards -> MaxCard(mast);
            }
        } else { // У левого нет
           RetVal = aCards -> LessThan(aRightCard);
           if ( !RetVal) {
               RetVal = aCards -> MaxCard(mast);
           }
        }
    }
  }
/////////////////////////////////////////////////////////
  delete aMaxCardList;
  GamesType=tmpGamesType;
  return RetVal;
}
//-----------------------------------------------------------------------
TCard *TGamer::MyPass3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *RetVal=NULL;
    /*TCard *MaxCard;*/
    TGamesType tmpGamesType=GamesType;
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
          RetVal = aCards -> LessThan(aRightCard);
        } else {
          RetVal = aCards -> LessThan(aLeftCard);
        }
       } else {
         RetVal = aCards -> LessThan(aLeftCard);
       }
       if ( !RetVal  ) {
           RetVal = aCards ->MaxCard(aLeftCard->CMast);
       }
    } else {
      //  у меня нет масти в которую зашел левый
       RetVal = GetMaxCardWithOutPere();
       if ( !RetVal ) {
               RetVal = GetMaxCardPere();  //масть с перехватами (max)
       }
       if ( !RetVal ) {
             RetVal = GetMinCardWithOutVz(); // лабуду
       }
       if ( !RetVal ) {
              RetVal = aCards->MinCard();
       }

    }
    GamesType=tmpGamesType;
    delete aMaxCardList;

  return RetVal;
}
//-----------------------------------------------------------------------
void TGamer::makestatfill(void) {
   TSide lr=left;
   for (int m=1;m<=4;m++) {
     for (int c=A;c>=7;c-- ) {
      if ( !aCards->Exist(c,m) ) {
         if (lr==left) {
           aLeft->Insert(new TCard(c,m));
           lr = right;
         } else {
           aRight->Insert(new TCard(c,m));
           lr = left;
         }
       }
     }
   }
}
//-----------------------------------------------------------------------
void TGamer::makestatfill(int nCards,int maxmin) {
    int nCounter=0;
    TSide lr=left;
    if ( maxmin ==1 ) {
       for (int m=1;m<=4;m++) {
         for (int c=A;c>=7;c-- ) {
          if ( !aCards->Exist(c,m) && !aOut->Exist(c,m) ) {
             if (lr==left) {
               aLeft->Insert(new TCard(c,m));
               lr = right;
             } else {
               aRight->Insert(new TCard(c,m));
               lr = left;
               nCounter++;
               if ( nCounter >= nCards ) return;
             }
           }
         }
       }
    } else {
       for (int m=1;m<=4;m++) {
         for (int c=7;c<=A;c++ ) {
          if ( !aCards->Exist(c,m) && !aOut->Exist(c,m) ) {
             if (lr==left) {
               aLeft->Insert(new TCard(c,m));
               lr = right;
             } else {
               aRight->Insert(new TCard(c,m));
               lr = left;
               nCounter++;
               if ( nCounter >= nCards ) return;
             }
           }
         }
       }
    }
}
//-------------------------------------------------------------------------
/*void TGamer::SetViewPort(void) {
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

}*/
//-------------------------------------------------------------------------
void TGamer::OnlyMessage (TDeskView *DeskView,int Left,int Top,int Width,int Height) {
//  DeskView -> ClearBox( Left,Top,Left+Width,/*Top+*/DeskView->yBorder);
  TGamer::Repaint(DeskView,Left,Top,Width,Height);

}
//-------------------------------------------------------------------------
void TGamer::Repaint (TDeskView *_DeskView,int Left,int Top,int Width,int Height) {
    char *buff = new char [512];
    char *buff2 = new char [512];
    TCard *my;
    int nDx=0 ,j=0;
      QPixmap *Bitmap1 = new QPixmap(Width,Height);
      QPainter p(_DeskView ->PaintDevice);
      TDeskView *DeskView = new TDeskView();
      (*DeskView) = (*_DeskView);
      DeskView -> PaintDevice = Bitmap1;
    if (aCards->aCount)
        nDx = (DeskView->xLen - DeskView->xBorder*2) / (aCards -> aCount+1);
    aCards->mySort();
    DeskView -> ClearBox( 0,0,Width,Height);
    if (GamesType != undefined ) {
        buff[0]=0;
        if ( nGetsCard ) {
            itoa(nGetsCard,buff,10);
        }
        sprintf(buff2," %s %s ",sGameName(GamesType),NikName);
        strcat (buff,buff2);
        strcat (buff,Message);
        //strcat(buff,Message)
        DeskView->SysSayMessage(buff,0,0);
    }

//    p.begin();
    for (int i = 0;i<aCards->aCount ;i++) {
       my = (TCard *)aCards->At(i);
       if ( my ) {
          if ( i+1 == aCards->aCount ) {
//            DeskView->SysDrawCard(my,Left+j*nDx,Top+DeskView->yBorder,!nCardClosed,0);
            DeskView->SysDrawCard(my,j*nDx,DeskView->yBorder,!nCardClosed,0);
          } else {
//            DeskView->SysDrawCardPartialy(my,Left+j*nDx,Top+DeskView->yBorder,!nCardClosed,0,Left+(j+1)*nDx);
            DeskView->SysDrawCardPartialy(my,j*nDx,DeskView->yBorder,!nCardClosed,0,(j+1)*nDx);
          }

       } else {
          char tmpbuff[512];
          sprintf(tmpbuff, "%i",i);
          DeskView -> SysSayMessage(tmpbuff,Left+j*nDx,Top+DeskView->yBorder);
       }
       j++;
    }
    p.drawPixmap( QPoint(Left,Top) ,*Bitmap1);
    p.end();
    delete Bitmap1;
    delete DeskView;
    delete buff;
    delete buff2;
}
//--------------------------------------------------------------
void TGamer::HintCard(int ,int ) {
}
//--------------------------------------------------------------
void TGamer::GetBackSnos(void) {

  // Vernut snos
  if (aCardsOut->At(0))
    aCards->Insert(aCardsOut->At(0));
  if (aCardsOut->At(1))
    aCards->Insert(aCardsOut->At(1));
  aCardsOut -> RemoveAll();
}
//--------------------------------------------------------------
//        NETWORK PART
//--------------------------------------------------------------
int TGamer::GetNikName(void) {
/*  char buff[MAXDATASIZE];
  memset(buff,0,MAXDATASIZE);
//  printf("\n wait for nikname");
  read_from_socket(socket,buff,MAXDATASIZE,0);
  sscanf(buff,"%s",NikName);
*/
  return 1;
}
//--------------------------------------------------------------
int TGamer::connecttoserver(char *host,unsigned short port) {
  Q_UNUSED(host)
  Q_UNUSED(port)
/*  socket = connect_to_socket(host,port);
  return socket;
*/
  return 1;

}
//--------------------------------------------------------------
int TGamer::connecttodesk(int req) {
  Q_UNUSED(req)
  return 1;
}
//--------------------------------------------------------------
int TGamer::join(int req) {
  Q_UNUSED(req)
  return 1;
}
//--------------------------------------------------------------
void TGamer::NetGetCards(TColoda *Coloda) {
  Q_UNUSED(Coloda)
/*
  int i=0,nVal;
  TCard *Card;
  char buff[MAXDATASIZE];
  char *pbuff=buff;
  memset(buff,0,MAXDATASIZE);
  read_from_socket(socket,buff,MAXDATASIZE,0);
//  printf("\n REad from socket %s",buff);
  while ( sscanf(pbuff,"%i",&nVal) !=EOF  ) {
//        if (nVal==1000) {
//          aCards -> Insert( new TCard(10,10) );
//          aCards -> Insert(Coloda -> Exist(7,1) );
//        } else {
          aCards -> Insert(Coloda -> Exist(nVal) );
//        }
      pbuff = strchr(pbuff,' ')+1;
      if (*pbuff=='\n')
        break;
  }
  */

}
//--------------------------------------------------------------
int TGamer::SendnPlayerTakeCards(int who) {
  Q_UNUSED(who)
  return 0;
}
//--------------------------------------------------------------
