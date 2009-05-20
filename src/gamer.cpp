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


TGamer::TGamer(int _nGamer) {
  nGamer = _nGamer;
  aScore = new TPlScore();
  aCards = new TCardList(12); // ���
  aLeft  = new TCardList(10);  // ���������� � ���� (�������������� ��� ��������)
  aRight = new TCardList(10); // � ����� (�������������� ��� ��������)
  aOut   = new TCardList(2);   // ���� (��� ��� ��������������)
  aCardsOut = new TCardList(12); // �� ������� ���
  aLeftOut  = new TCardList(12);  // �� ������� ���������� � ���� (�������������� ��� ��������)
  aRightOut = new TCardList(12); // �� ������� � ����� (�������������� ��� ��������)
  NikName = QString();
  clear();
}


TGamer::TGamer(int _nGamer,TDeskView *aDeskView) {
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


TGamer::~TGamer() {
  delete aCards;
  delete aLeft;
  delete aRight;
  delete aOut;
  delete aCardsOut;
  delete aLeftOut;
  delete aRightOut;
  delete aScore;
}


void TGamer::clear () {
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


void TGamer::AddCard (TCard *aCard) {
  aCards->Insert(aCard);
}


void TGamer::AddCard (int _CName, int _CMast) {
  aCards->Insert(new TCard (_CName,_CMast));
}


///////////////////////////////////////////////////////////////////////////////
// game mechanics
///////////////////////////////////////////////////////////////////////////////
TCard *TGamer::Miser1(TGamer *aLeftGamer,TGamer *aRightGamer) {
    TCard *cur=NULL;
    if (aCards -> AllCard()==10) {  // first move ������ ����� � 8..��� ���� ��� ����
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
TCard *TGamer::Miser2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
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
               cur = GetMaxCardPere();  //����� � ����������� (max)
           }
             if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
           }
             if ( !cur ) {
              cur = aCards->MaxCard();
          }
          delete aMaxCardList;
        }
    return cur;
}
//-----------------------------------------------------------------------
TCard *TGamer::Miser3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
// copy from MyPass3
  TCard *cur=NULL;
    /*TCard *MaxCard;*/
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
//    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // � ���� ���� �����  � ������� ����� �����
       // ������������ ����������
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
      //  � ���� ��� ����� � ������� ����� �����
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //����� � ����������� (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
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
TCard *TGamer::makemove(TCard *lMove,TCard *rMove,TGamer *aLeftGamer,TGamer *aRightGamer) { //���
    TCard *cur=NULL;
    if (lMove == NULL && rMove == NULL) { // ��� ����� - ������
        if ( GamesType == gtPass || GamesType == vist ) {
              // ���-�� ������ � � ��� �� ������
              cur = MyVist1(aLeftGamer,aRightGamer);
        } else  if (GamesType == g86catch) {
            cur = MiserCatch1(aLeftGamer,aRightGamer);
          } else  if (GamesType == g86) {
          cur = Miser1(aLeftGamer,aRightGamer);
          } else   if (  GamesType == raspass  ) {
            cur = MyPass1(rMove,aLeftGamer,aRightGamer);; // �� ���� ������� ��� �����
        } else {
            // �� ���� ��� ����
            cur = MyGame1(aLeftGamer,aRightGamer);
        }

    }
    if (lMove == NULL && rMove != NULL) { // ��� ����� - ������
        if ( GamesType == gtPass || GamesType == vist ) {
                // ���-�� ������ � � ��� �� ������
                cur = MyVist2(rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86catch) {
                cur = MiserCatch2(rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86) {
              cur = Miser2(rMove,aLeftGamer,aRightGamer);
          } else  if ( GamesType == raspass  ) {
                cur = MyPass2(rMove,aLeftGamer,aRightGamer); // �� ���� ������� ��� �����
        } else {   // �� ���� ��� ����
            cur = MyGame2(rMove,aLeftGamer,aRightGamer);
        }

    }
    if (lMove != NULL && rMove != NULL) { // ��� ����� - 3
        if ( GamesType == gtPass || GamesType == vist ) {
                // ���-�� ������ � � ��� �� ������
           cur = MyVist3(lMove,rMove,aLeftGamer,aRightGamer);
        } else if (GamesType == g86catch) {
            cur = MiserCatch3(lMove,rMove,aLeftGamer,aRightGamer);
          } else  if (GamesType == g86) {
            cur = Miser3(lMove,rMove,aLeftGamer,aRightGamer);
          } else if ( GamesType == raspass ) {
           cur = MyPass3(lMove,rMove,aLeftGamer,aRightGamer);; // �� ���� ������� ��� �����
        } else {
           // �� ���� ��� ����
           cur = MyGame3(lMove,rMove,aLeftGamer,aRightGamer);
        }
    }
    aCards    -> Remove(cur);
    aCardsOut -> Insert(cur);
    return cur;
}
//-----------------------------------------------------------------------
TMastTable TGamer::vzatok(TMast Mast,TCardList *aMaxCardList,int a23) {
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
void TGamer::RecountTables4RasPass(TCardList *aMaxCardList,int a23) { // ������������� ������� ��� �������� ��� ������
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
void TGamer::RecountTables( TCardList *aMaxCardList,int a23 ){ // ������������� �������         TMastTable MastTable[5];
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
// 1. �������� �������
    for (int m=1;m<=4;m++) {
      TCard *EnemyMin = aMaxCardList -> MinCard(m);
      TCard *NaparnikMin = Naparnik -> MinCard(m);
      TCard *MyMin = aCards -> MinCard(m);
      if (cur)
        goto badlabel;
      if (EnemyMin) { //���� �����
        if (MyMin) { // � � ���� ���� �����
          if (NaparnikMin) { // � � ��������� ���� �����
            if ( *NaparnikMin < *EnemyMin && *MyMin < *EnemyMin) {
              cur = MyMin;// �������
              goto badlabel;
            }
            if ( *NaparnikMin > *EnemyMin && *MyMin < *EnemyMin) {
              // ���� �-�� ����� ����� �� ������� �� ����� �������� �
              // �� ������ �������� �� ���
              for (int k=1;k<=4;k++)  {
                if (k==m || aMaxCardList->AllCard(k)==0)
                  continue;
                TCard *EnemyMax = aMaxCardList -> MaxCard(k);
                TCard *NaparnikMax = Naparnik -> MaxCard(k);
                TCard *MyMax = aCards -> MaxCard(k);
                if ( MyMax && NaparnikMax && EnemyMax) {
                  if (Naparnik->AllCard(k) < aCards-> AllCard(k) && Naparnik->AllCard(k) < aMaxCardList->AllCard(k) ) {
                    cur = aCards ->LessThan(EnemyMax);
//                    MyMax; // �� ���� �� ������ �������� NaparnikMax
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
          } else { //� ��������� ��� �����
            if (*MyMin < *EnemyMin) {
              cur = MyMin;// �������
              goto badlabel;
            }
          }
        }
      }
    }


//    2 ��������� ����
    if (!cur) cur = GetMaxCardWithOutPere();
    if (Side == LeftHand) {
      // ��� ������������ ���� �������� �� ������� !!!
    }
// 3    �������
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
TCard *TGamer::MiserCatch2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
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

  if (Side == LeftHand) { //��� ��� ���� , ��������� � �����
      if (aCards->AllCard(aRightCard->CMast)) {
        // � ���� ���� �����  � ������ �����
        TCard *MyMax = aCards->MaxCard(aRightCard->CMast);
        //TCard *MyMin = aCards->MinCard(aRightCard->CMast);
        //TCard *EnMax = aMaxCardList->MaxCard(aRightCard->CMast);
        TCard *EnMin = aMaxCardList->MinCard(aRightCard->CMast);
        if (EnMin) {
          // � ���������� ���� ����� � ������ �����
          if (*aRightCard < *EnMin) {
            // ����� ���������� �������
            cur=aCards->LessThan(EnMin);
            if(!cur) cur = aCards -> MoreThan(EnMin);
          } else {
          cur= aCards -> MoreThan(EnMin);
          }
        } else {
          cur = MyMax; // � ��� ��� ���� �������� ���� ����� ���
        }
      } else {
        // � ���� ��� ����  � ������ �����
        if (Pronesti) {
          //���� �������� �����
          if (aCards->Exist(Pronesti->CName,Pronesti->CMast)) {
            cur = Pronesti;
          } else {
            cur = aCards -> MaxCard(Pronesti->CMast);
            if (!cur)   cur = aCards -> MaxCard();
          }
        } else {
          // �������� �� ������ - ���
          cur = GetMaxCardPere();
        if (!cur) cur = aCards -> MaxCard();
        }
      }
  } else { // ��������� ����� ��� ����
    TCard *MyMax = aCards->MaxCard(aRightCard->CMast);
    TCard *MyMin = aCards->MinCard(aRightCard->CMast);
    if (MyMax) {

      if ( *MyMin < *aRightCard) {
//        ���� ����������� �������
        TCard *NapMin = Naparnik->LessThan(aRightCard);
        if (NapMin) {
          cur = aCards->LessThan(aRightCard);
        } else {
          cur = MyMax; // ���� �������� ��� ?
        }
      } else {
        cur = MyMax; // ������� �� ����� �������
      }

    } else {
      // � � ���� ��� �����

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
TCard *TGamer::MiserCatch3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
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
        // � ���� ���� �����  � ������ �����
//        TCard *MyMax = aCards->MaxCard(aLeftCard->CMast);
//        TCard *MyMin = aCards->MinCard(aLeftCard->CMast);
        if (*aLeftCard < *aRightCard) {
          // �������� ��� �������� �����
          cur = aCards -> MaxCard(aLeftCard->CMast);

        } else {
          //���� ���� �������
          cur = aCards -> LessThan(aLeftCard);
          if (!cur ) cur = aCards -> MaxCard(aLeftCard->CMast);
        }
      } else {
        // � ���� ��� ����  � ������ �����
        if (Pronesti) {
          //���� �������� �����
          if (aCards->Exist(Pronesti->CName,Pronesti->CMast)) {
            cur = Pronesti;
          } else {
            cur = aCards -> MaxCard(Pronesti->CMast);
            if (!cur) cur = aCards -> MaxCard();
          }
        } else {
          // �������� �� ������ - ���
          cur = GetMaxCardPere();
        if (!cur) cur = aCards -> MaxCard();
        }
      }
  } else { // RightHand - my friend
    TCard *MyMax = aCards->MaxCard(aLeftCard->CMast);
    TCard *MyMin = aCards->MinCard(aLeftCard->CMast);
    if (MyMax) {
      if ( *MyMin < *aLeftCard) {
//        ���� ����������� �������
        TCard *NapMin = Naparnik->LessThan(aLeftCard);
        if (NapMin) {
          cur = aCards->LessThan(aLeftCard);
        } else {
          cur = MyMax; // ���� �������� ��� ?
        }
      } else {
        cur = MyMax; // ������� �� ����� �������
      }

    } else {
      // � � ���� ��� �����
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
TCard *TGamer::MyGame3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) { // ��� ���� 3 ����� - ���
    TCard *cur=NULL;
    TCard *MaxCard;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // � ���� ���� �����  � ������� ����� �����
        if ( aLeftCard->CMast == aRightCard->CMast)  {
            // ������������ �������
            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur  ) {
               cur = aCards ->MinCard(aLeftCard->CMast);
            }
        } else { //aLeftCard->CMast == aRightCard->CMast
          if ( aRightCard->CMast !=mast)  {
            // �� ���� �������

            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur ) {
              cur = aCards -> MinCard(aLeftCard->CMast);
            }
          } else {
            // ���� �������, ��� �� ���� ����
            cur = aCards -> MinCard(aLeftCard->CMast);
          }
        }
    } else {
      //  � ���� ��� ����� � ������� ����� �����
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
       if ( ! cur ) {// ��� ���.
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
TCard *TGamer::MyVist3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) { // ��� ���� ��� ��� 3 ����� - ���
    TCard *cur=NULL;
    TCard *MaxCard;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    TGamer *aEnemy,*aFriend;
    int mast;
    // ��� ����� � ��� ��������
    if ( aLeftGamer -> GamesType != gtPass && aLeftGamer -> GamesType != vist ) {
        aEnemy = aLeftGamer;
        aFriend = aRightGamer;
    } else {
        aEnemy = aRightGamer;
        aFriend = aLeftGamer;
    }
    mast = aEnemy->GamesType - (aEnemy->GamesType/10) * 10;
    // �������� ������
//    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
//    RecountTables(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // � ���� ���� �����  � ������� ����� �����
      if ( aEnemy == aLeftGamer ) { // ����� - �������
        LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);

        if ( aLeftCard->CMast == aRightCard->CMast)  {
          if ( *aRightCard > *aLeftCard ) {
            // ���� �� ����
            cur = aCards->MinCard(aRightCard->CMast);
          } else {
            // ������������ �������
            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur  ) {
               cur = aCards ->MinCard(aLeftCard->CMast);
            }
          }
        } else { //aLeftCard->CMast == aRightCard->CMast
          if ( aRightCard->CMast !=mast)  {
            // �� ���� �������
            cur = aCards -> MoreThan(aLeftCard);
            if ( !cur ) {
              cur = aCards -> MinCard(aLeftCard->CMast);
            }
          } else {
            // ���� �������, ��� �� ���� ����
            cur = aCards -> MinCard(aLeftCard->CMast);
          }
        }

      } else {
        //������ ���������
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        if ( aLeftCard->CMast == aRightCard->CMast  )  {
          if ( *aRightCard > *aLeftCard ) {
            // ���������� ������� �������
            cur = aCards->MoreThan(aRightCard);
            if ( !cur ) {
              cur = aCards->MinCard(aRightCard->CMast);
            }
          } else {
            // ���� �� ����
            cur = aCards->MinCard(aRightCard->CMast);
          }
        } else {
          if ( aRightCard->CMast==mast  ) {
            // ������ �����, � ������ ������� �������
            cur = aCards->MinCard(aLeftCard->CMast);
          } else { // �� �� .... ����� ���� ����� � ������ ������ ������ !!!
            cur = aCards->MinCard(aLeftCard->CMast);
          }
        }

      }
    } else {
      //  � ���� ��� ����� � ������� ����� �����
       if (aLeftCard->CMast==aRightCard->CMast) {
           if ( *aLeftCard > *aRightCard ) {   MaxCard = aLeftCard;
           } else { MaxCard = aLeftCard; }
       } else {
           if ( aLeftCard->CMast != mast && aRightCard->CMast != mast) {               MaxCard = aLeftCard;
           } else { MaxCard = aRightCard;
           }
       }
       if ( aEnemy == aLeftGamer ) { // � ���� �����
            LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);

          RecountTables(aMaxCardList,23);

           if (MaxCard == aLeftCard ) { //���������� �������� ��� �����
               cur = aCards -> MoreThan(aLeftCard);
           } else { // ��� ���� ������... ��������� �����������
               cur = aCards -> MinCard(MaxCard->CMast);
           }
       } else { // � ���� - ��������
          LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,23);

           if (MaxCard == aLeftCard ) { // ��� ���� ������... ��������� �����������
               cur = aCards -> MinCard(MaxCard->CMast);
           } else {//���������� �������� ��� �����
               cur = aCards -> MoreThan(aLeftCard);
           }
       }
       if (!cur) {
           cur = aCards -> MinCard(mast);
       }
       if ( ! cur ) {// ��� ���.
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

TCard *TGamer::MyVist2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) { // ��� ���� ��� ��� 2 ����� - ���
    TCard *cur=NULL;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    TGamer *aEnemy,*aFriend;
    int mast;
    // ��� ����� � ��� ��������
    if ( aLeftGamer -> GamesType != gtPass && aLeftGamer -> GamesType != vist ) {
        aEnemy = aLeftGamer;
        aFriend = aRightGamer;
    } else {
        aEnemy = aRightGamer;
        aFriend = aLeftGamer;
    }
    mast = aEnemy->GamesType - (aEnemy->GamesType/10) * 10;
    // �������� ������
    if ( aEnemy == aLeftGamer ) { // � ���� �����
        LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        // �� ���� �� ������� ��� ������� � ����
        if ( aRightCard -> CName >= 10 ) {
            cur = aCards -> MinCard(aRightCard -> CMast);
        } else { // ����� ����� � ������ ������� � ���� ��� � ������ ����� � ���� ������
            cur = aCards -> MaxCard(aRightCard -> CMast);
        }
        if ( !cur ) { // � � ���� ����� � ��� !!!
            cur = aCards -> MaxCard(mast); // ������� ������������
        }
        if ( ! cur ) {
            cur = GetMinCardWithOutVz();
        }
    } else { // � ���� - ��������
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        cur = aCards->MoreThan(aRightCard);
        if (!cur) {
            cur = aCards->MinCard(aRightCard->CMast);
        }
        if (!cur) { // ��� ����� ������� ������
            cur = aCards->MinCard(mast);
        }
        if (!cur) { // ��� ����� � ������
            cur = GetMinCardWithOutVz();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------
TCard *TGamer::MyVist1(TGamer *aLeftGamer,TGamer *aRightGamer) {
// 1 - ������ ������ 2-��������� ����� � ����������� 3-��� ���������� 4-???
    TCard *cur=NULL;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    TGamer *aEnemy,*aFriend;
    int mast;
    // ��� ����� � ��� ��������
    if ( aLeftGamer -> GamesType != gtPass && aLeftGamer -> GamesType != vist ) {
        aEnemy = aLeftGamer;
        aFriend = aRightGamer;
    } else {
        aEnemy = aRightGamer;
        aFriend = aLeftGamer;

    }
    mast = aEnemy->GamesType - (aEnemy->GamesType/10) * 10;
    // �������� ������
//    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
//    RecountTables(aMaxCardList,1);
    if ( aEnemy == aLeftGamer ) { // � ���� �����
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
    } else { // � ���� - ��������
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
TCard *TGamer::MyGame2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
    TCardList *aMaxCardList=new TCardList(20);
    TGamesType tmpGamesType=GamesType;
    TCard *cur=NULL;
    TCard *MaxLeftCard=NULL;
    int mast = aRightCard -> CMast;
    int koz  = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);

    cur = aCards -> MaxCard(mast);
    if ( !cur ) { // ��� �����
        if ( !aLeftGamer -> aCards -> AllCard(mast) ) { // � ������ ��� ����
            MaxLeftCard = aLeftGamer -> aCards ->MaxCard(koz);
            cur = aCards -> MoreThan(MaxLeftCard); //
            if (!cur) {
                cur = aCards -> LessThan(MaxLeftCard);
            }
            if ( !cur ) {
                cur = GetMinCardWithOutVz(); // ������
            }
            if ( !cur ) {
                 cur = aCards->MinCard();
            }
        } else { // ���� ����� � ������
            cur = aCards -> MinCard(koz);
        }
    } else { // � ���� ���� �����
        if ( aLeftGamer -> aCards -> AllCard(mast) ) { //� ������ ����
            MaxLeftCard = aLeftGamer -> aCards ->MaxCard(mast);
            cur = aCards -> MoreThan(MaxLeftCard);
            if ( !cur ) {
                cur = aCards -> LessThan(MaxLeftCard);
            }
        } else { // � ������ ���
            if ( !aLeftGamer -> aCards -> AllCard(koz) ) { // � ������ � ���
                cur = aCards -> MoreThan(aRightCard);
                if ( !cur) {
                    cur = aCards -> MinCard(mast);
                }
            } else { // ���� � ������ ������
                cur = aCards -> MinCard(mast);
            }
        }
    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}
//-----------------------------------------------------------------------
TCard *TGamer::MyGame1(TGamer *aLeftGamer,TGamer *aRightGamer) {
// 1-������ ������ 2-��������� ����� � ����������� 3-��� ���������� 4-???
    TCard *cur=NULL;
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,1);
    if ( aLeft->AllCard(mast) || aRight->AllCard(mast) ) { // � ����������� ���� ������
        cur = aCards->MaxCard(mast);
        if ( !cur ) { // � � ���� �� ��� !!!
             cur = GetMaxCardWithOutPere(); // ��� ����
        }
        if ( !cur ) {
             cur = GetMaxCardPere();  //����� � ����������� (max)
        }
        if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
        }
        if ( !cur ) {
             cur = aCards->MaxCard();
        }
    } else { // � ��������� � ��� � ���� !!!
        if ( !cur ) {
             cur = GetMaxCardPere();  //����� � ����������� (max)
        }
        if ( !cur ) { // � � ���� �� ��� !!!
             cur = GetMaxCardWithOutPere(); // ��� ����
        }
        if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
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
/*    // !!!!!!!!!!!!!!!!!!!!!!!!! ���������� ���������, ��� �� �������
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
TGamesType TGamer::makemove4out(void) {
    TGamesType GamesTypeRetVal;
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

    for (i=1; i<=4; i++) { // �������������� ������ - ����� ������� �����
      if( aCards->AllCard(i) > nMaxMastLen ) {
        nMaxMastLen = aCards->AllCard(i);
        nMaxMast = (TMast) i;
      }
    }

    for (i=1; i<=4; i++) {
      if(aCards->AllCard(i) == nMaxMastLen && nMaxMast != i ) { // ���� ����������� ������� �����
       if ( LocalMastTable[i].sum  > LocalMastTable[nMaxMast].sum  )  {
         nMaxMast = (TMast) i;
        }
      }
    }

    //����� � ������ ��� ���������
    GamesTypeRetVal = ( TGamesType ) (LocalMastTable[0].vzatok*10 + nMaxMast );
    // ��� ��, ��� �� ����� ������ �����������
    return GamesTypeRetVal;

}
//-----------------------------------------------------------------------
TGamesType TGamer::makemove(TGamesType MaxGame,int HaveAVist,int nGamerVist) { // ����� ��������� ������� ������� - ���� ��� ����
  Q_UNUSED(nGamerVist)
  TGamesType Answer;
  TGamesType MyMaxGame = makemove4out();

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
    /* ��� ����� ������ */
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
TGamesType TGamer::makemove(TGamesType lMove,TGamesType rMove) { //��� ��� ��������
/*  if ( GamesType == gtPass )  {
    GamesType=gtPass;
  } else {*/
  if ( GamesType != gtPass )  {
    int i;
    int nMaxMastLen=0;
    TMast nMaxMast=SuitNone;
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
    for (i=1; i<=4; i++) { // �������������� ������ - ����� ������� �����
      if( aCards->AllCard(i) > nMaxMastLen ) {
        nMaxMastLen = aCards->AllCard(i);
        nMaxMast = (TMast) i;
      }
    }
    for (i=1; i<=4; i++) {
      if(aCards->AllCard(i) == nMaxMastLen && nMaxMast != i ) { // ���� ����������� ������� �����
       if ( LocalMastTable[i].sum  > LocalMastTable[nMaxMast].sum  )  {
         nMaxMast = (TMast) i;
        }
      }
    }
    //����� � ������ ��� ���������
    if (MGT <= g75) {
      GamesType = ( TGamesType ) ((LocalMastTable[0].vzatok+1)*10 + nMaxMast );
    } else {
      GamesType = ( TGamesType ) ((LocalMastTable[0].vzatok)*10 + nMaxMast );
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
        GamesType = (TGamesType) NextGame(MGT);
    } else {
      GamesType = gtPass;
    }

/*    if( GamesType >= MGT  ) {
        GamesType = (TGamesType) NextGame(MGT);
    } else {
    }*/
    // ��� ��, ��� �� ����� ������ �����������
    /*if ( GamesType > gtPass  ) {
      if ( rMove < lMove )  {
        if ( lMove <= GamesType ) {
          if ( lMove!=g61 )
              GamesType = lMove; // say here
          else
              GamesType = (TGamesType) NextGame(lMove);
        } else {
          GamesType = gtPass;
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
int TGamer::Check4Miser( void) {
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
TMastTable TGamer::vzatok4game(TMast Mast, int a23) {
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
TMastTable TGamer::vzatok4pass(TMast Mast,TCardList *aMaxCardList) {
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
TMastTable TGamer::Compare2List4Min(TCardList *My,TCardList *Enemy) { // ��� ��� ����������
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
      // ���������� ���  ��� �������
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
      // ������� - ��� ������
      for ( int j =1; j<=My->AllCard();j++ ) {
        MastTable.vzatok++;
      }
      break;
    }
    if ( *MyCardMax > *EnemyCardMax ) {
      // ���������� ��� �������, ��� �������
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
    if (EnemyCardMax) {// �������� �-�� ������ ��m � ����
      MyCardMax = My -> MoreThan(EnemyCardMax);
      if ( ! MyCardMax ) {  // ��� � ���� ������ ��� � ����
        MyCardMax = My -> MinCard();
        if (!MyCardMax ) {  // � ������ ��� ���
          break ;
        } else {  // ������ ��� � ����
          My -> Remove(MyCardMax);
          Enemy -> Remove(EnemyCardMax);
          nIget = 0;
        }
      } else {  // ���� ������ ��� � ����
        My -> Remove(MyCardMax);
        Enemy -> Remove(EnemyCardMax);
        MastTable.vzatok++;
        if (! nIget) MastTable.perehvatov++;
        nIget = 1;
      }
    } else {  // � ���� ��� ���� � ������ ����� ! ������� - ��� ������
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
  TCard *cur=NULL;
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
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // ������� // �����
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList); // !!!!!!!!!
    RecountTables4RasPass(aMaxCardList,1);
     // ���� ����� : 100 % ������ ��ϣ, ���� �������� ��� - ��� � ���������� ������� �����
    cur = GetMaxCardWithOutPere(); // �� ���� ��� ���� ���� ����� ��� ���
    if ( cur ) {
        if ( aLeftGamer->aCards->AllCard(cur->CMast)==0
             && aRightGamer->aCards->AllCard(cur->CMast)==0 ) {

          cur = NULL;
        }
    }
    if ( !cur ) {
         cur = GetMaxCardPere();  //����� � ����������� (max)
    }
    if ( !cur ) {
       cur = GetMinCardWithOutVz(); // ������
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
TCard *TGamer::MyPass2(TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *cur=NULL;
  TGamesType tmpGamesType=GamesType;
  int mast = aRightCard -> CMast;
  TCardList *aMaxCardList=new TCardList(20);
  LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
  RecountTables4RasPass(aMaxCardList,1);
/////////////////////////////////////////////////////////
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // ������� // �����
    cur = aCards -> MaxCard(mast); // !!!!!!!!!!!! ����� ������
    if ( !cur ) { // ��� �����
      // �� ���� !!! ���� ��� � ����� ���  ���� 100 % ������
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //����� � ����������� (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
       }
       if ( !cur ) {
              cur = aCards->MinCard();
       }

    } else { // � ���� ���� �����
        if ( aLeftGamer -> aCards -> AllCard(mast) ) { //� ������ ����
            cur = aCards -> LessThan(aLeftGamer -> aCards ->MaxCard(mast));
            if ( !cur ) {
                cur = aCards -> MaxCard(mast);
            }
        } else { // � ������ ���
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
TCard *TGamer::MyPass3(TCard *aLeftCard,TCard *aRightCard,TGamer *aLeftGamer,TGamer *aRightGamer) {
  TCard *cur=NULL;
    /*TCard *MaxCard;*/
    TGamesType tmpGamesType=GamesType;
    TCardList *aMaxCardList=new TCardList(20);
//    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->AllCard(aLeftCard->CMast) )  {
       // � ���� ���� �����  � ������� ����� �����
       // ������������ ����������
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
      //  � ���� ��� ����� � ������� ����� �����
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //����� � ����������� (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
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
void TGamer::makestatfill(void) {
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
void TGamer::makestatfill(int nCards,int maxmin) {
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


void TGamer::GetBackSnos () {
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
void TGamer::SetViewPort(void) {
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
void TGamer::OnlyMessage (TDeskView *DeskView, int Left, int Top, int Width, int Height) {
  //DeskView->ClearBox(Left, Top, Left+Width, / *Top+* /DeskView->yBorder);
  //TGamer::Repaint(DeskView, Left, Top, Width, Height);
}
*/


///////////////////////////////////////////////////////////////////////////////
// at least 28 ints (14 int pairs); return # of used ints; the last int is -1
// result: ofs, cardNo, ..., -1
int TGamer::buildHandXOfs (int *dest, int startX, bool opened) {
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


void TGamer::getLeftTop (int playerNo, int *left, int *top) {
  *left = 0; *top = 0;
  int ofs[28], cnt;
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

int TGamer::cardAt (int lx, int ly, bool opened) {
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


void TGamer::Repaint (TDeskView *aDeskView, int Left, int Top, int Width, int Height, bool opened, int selNo) {
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


void TGamer::RepaintSimple (bool opened) {
  int left, top;
  getLeftTop(nGamer, &left, &top);
  Repaint(DeskView, left, top, opened, oldii);
}


void TGamer::HintCard (int lx, int ly) {
  Q_UNUSED(lx)
  Q_UNUSED(ly)
}


///////////////////////////////////////////////////////////////////////////////
// game networking
///////////////////////////////////////////////////////////////////////////////
int TGamer::GetNikName () {
  return 1;
}


int TGamer::connecttoserver (const QString &host, unsigned short port) {
  Q_UNUSED(host)
  Q_UNUSED(port)
  return 1;
}


int TGamer::connecttodesk (int req) {
  Q_UNUSED(req)
  return 1;
}


int TGamer::join (int req) {
  Q_UNUSED(req)
  return 1;
}


void TGamer::NetGetCards (TColoda *Coloda) {
  Q_UNUSED(Coloda)
}


int TGamer::SendnPlayerTakeCards (int who) {
  Q_UNUSED(who)
  return 0;
}
