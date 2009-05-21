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
  aCards = new CardList; // ���
  aLeft = new CardList;  // ���������� ����� (�������������� ��� ��������)
  aRight = new CardList; // ������ (�������������� ��� ��������)
  aOut = new CardList;   // ���� (��� ��� ��������������)
  aCardsOut = new CardList; // �� ������� ���
  aLeftOut = new CardList;  // �� ������� ���������� ����� (�������������� ��� ��������)
  aRightOut = new CardList; // �� ������� ������ (�������������� ��� ��������)
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
  for (int i = 0; i <= 4; i++)  {
    MastTable[i].vzatok = 0;
    MastTable[i].perehvatov = 0;
  }
  nInvisibleHand = (mPlayerNo != 1);
  Pronesti = NULL;
}


void Player::AddCard (Card *aCard) {
  aCards->append(aCard);
}


void Player::AddCard (int _CName, int _CMast) {
  aCards->append(new Card (_CName,_CMast));
}


///////////////////////////////////////////////////////////////////////////////
// game mechanics
///////////////////////////////////////////////////////////////////////////////
Card *Player::Miser1(Player *aLeftGamer,Player *aRightGamer) {
    Card *cur=NULL;
    if (aCards->count()==10) {  // first move ������ ����� � 8..��� ���� ��� ����
        for (int m=1;m<=4;m++) {
          if (aCards->cardsInSuit(m)==1) {
            cur = aCards->minInSuit(m);
            break;
          }
        }
    }
    if (!cur) {
        for (int m=4;m>=1;m--) {
          if (cur) break;
          for (int c=FACE_ACE;c>=7;c--) {
            if (cur) break;
            Card *my;
            //,*leftmax,*leftmin,*rightmax,*rightmin;
            my = aCards->exists(c,m);
            if (my) {
              int matrixindex=0;
              matrixindex  += aLeftGamer->aCards->lesserInSuit(my) ? 1:0;     matrixindex <<= 1;
              matrixindex  += aLeftGamer->aCards->greaterInSuit(my) ? 1:0;   matrixindex <<= 1;
              matrixindex  += aRightGamer->aCards->lesserInSuit(my) ? 1:0;  matrixindex <<= 1;
              matrixindex  += aRightGamer->aCards->greaterInSuit(my) ? 1:0;
              if (matrixindex == 1 || matrixindex == 9 || matrixindex==13||(matrixindex >=4 && matrixindex <=7)) {
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
//-----------------------------------------------------------------------
Card *Player::Miser2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
    Card *cur=NULL;
      if ( aCards->cardsInSuit( aRightCard->suit()) ) {
          for (int c=FACE_ACE;c>=7;c--) {
            if (cur) break;
            Card *my;
//,*leftmax,*leftmin,*rightmax,*rightmin;
            my = aCards->exists(c,aRightCard->suit());
            if (my) {
              int matrixindex=0;
              matrixindex  += aLeftGamer->aCards->lesserInSuit(my) ? 1:0;   matrixindex <<= 1;
              matrixindex  += aLeftGamer->aCards->greaterInSuit(my) ? 1:0;   matrixindex <<= 1;
              matrixindex  += (*aRightCard < *my) ? 1:0;  matrixindex <<= 1;
              matrixindex  += (*aRightCard > *my) ? 1:0;
              if (matrixindex == 1 || matrixindex == 9 || matrixindex==13||(matrixindex >=4 && matrixindex <=7)) {
                cur = my;
              }
            }
          }
          if (!cur) cur = aCards->maxInSuit(aRightCard->suit());
        } else {
          CardList *aMaxCardList=new CardList;
          LoadLists(aRightGamer,aLeftGamer,aMaxCardList);
           cur = GetMaxCardWithOutPere();
             if ( !cur ) {
               cur = GetMaxCardPere();  //����� � ����������� (max)
           }
             if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
           }
             if ( !cur ) {
              cur = aCards->maxCard();
          }
          delete aMaxCardList;
        }
    return cur;
}
//-----------------------------------------------------------------------
Card *Player::Miser3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
// copy from MyPass3
  Card *cur=NULL;
    /*Card *maxInSuit;*/
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
//    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // � ���� ���� �����  � ������� ����� �����
       // ������������ ����������
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
      //  � ���� ��� ����� � ������� ����� �����
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //����� � ����������� (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
       }
       if ( !cur ) {
              cur = aCards->minCard();
       }

    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::makemove(Card *lMove,Card *rMove,Player *aLeftGamer,Player *aRightGamer) { //���
    Card *cur=NULL;
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
    aCards->remove(cur);
    aCardsOut->append(cur);
    return cur;
}
//-----------------------------------------------------------------------
tSuitProbs Player::vzatok(eSuit Mast,CardList *aMaxCardList,int a23) {
   tSuitProbs MastTable;
   Card *MyCard,*tmpCard;
   CardList *MyCardStack     = new CardList;
   CardList *EnemyCardStack  = new CardList;
   for (int c=7;c<=FACE_ACE;c++ ) {
        MyCard = aCards->exists(c, Mast);
        if ( MyCard ) {
          MyCardStack->append(MyCard);
        }
        MyCard = aMaxCardList->exists(c, Mast);
        if ( MyCard ) {
          EnemyCardStack->append(MyCard);
        }
   }
   if (a23 != 23) {
       MastTable = Compare2List4Max(MyCardStack,EnemyCardStack);
   } else {
       MastTable = Compare2List4Max23(MyCardStack,EnemyCardStack);
   }
   MastTable.len = aCards->cardsInSuit(Mast);
   MastTable.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->exists(j,Mast);
    if (  tmpCard ) {
      MastTable.sum +=  tmpCard->face();
    }
   }
   MyCardStack->clear();
   EnemyCardStack->clear();
   delete MyCardStack;
   delete EnemyCardStack;
   return MastTable;
}
//-----------------------------------------------------------------------
void Player::RecountTables4RasPass(CardList *aMaxCardList,int a23) { // ������������� ������� ��� �������� ��� ������
  Q_UNUSED(a23)
    int i;
    MastTable[0].vzatok=0;
    MastTable[0].perehvatov=0;
    MastTable[0].sum=0;
    for (i=1; i<=4; i++ )  {
      MastTable[i] = vzatok4pass( (eSuit) i,aMaxCardList );
      MastTable[0].vzatok += MastTable[i].vzatok ;
      MastTable[0].perehvatov += MastTable[i].perehvatov ;
      MastTable[0].sum += MastTable[i].sum;
    }

}
//-----------------------------------------------------------------------
void Player::RecountTables( CardList *aMaxCardList,int a23 ){ // ������������� �������         tSuitProbs MastTable[5];
    int i;
    MastTable[0].vzatok=0;
    MastTable[0].perehvatov=0;
    MastTable[0].sum=0;
    for (i=1; i<=4; i++ )  {
      MastTable[i] = vzatok ( (eSuit) i,aMaxCardList, a23 );
      MastTable[0].vzatok += MastTable[i].vzatok ;
      MastTable[0].perehvatov += MastTable[i].perehvatov ;
      MastTable[0].sum += MastTable[i].sum;
    }
}
//-----------------------------------------------------------------------
Card *Player::MiserCatch1(Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=NULL;
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
// 1. �������� �������
    for (int m=1;m<=4;m++) {
      Card *EnemyMin = aMaxCardList->minInSuit(m);
      Card *NaparnikMin = Naparnik->minInSuit(m);
      Card *MyMin = aCards->minInSuit(m);
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
                if (k==m || aMaxCardList->cardsInSuit(k)==0)
                  continue;
                Card *EnemyMax = aMaxCardList->maxInSuit(k);
                Card *NaparnikMax = Naparnik->maxInSuit(k);
                Card *MyMax = aCards->maxInSuit(k);
                if ( MyMax && NaparnikMax && EnemyMax) {
                  if (Naparnik->cardsInSuit(k) < aCards->cardsInSuit(k) && Naparnik->cardsInSuit(k) < aMaxCardList->cardsInSuit(k) ) {
                    cur = aCards->lesserInSuit(EnemyMax);
//                    MyMax; // �� ���� �� ������ �������� NaparnikMax
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
    if (!cur) cur = aCards->minCard();
//  } else { //Side == LeftHand
//  }
badlabel:

  aMaxCardList->clear();
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::MiserCatch2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=NULL;
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

  if (Side == LeftHand) { //��� ��� ���� , ��������� � �����
      if (aCards->cardsInSuit(aRightCard->suit())) {
        // � ���� ���� �����  � ������ �����
        Card *MyMax = aCards->maxInSuit(aRightCard->suit());
        //Card *MyMin = aCards->minInSuit(aRightCard->suit());
        //Card *EnMax = aMaxCardList->maxInSuit(aRightCard->suit());
        Card *EnMin = aMaxCardList->minInSuit(aRightCard->suit());
        if (EnMin) {
          // � ���������� ���� ����� � ������ �����
          if (*aRightCard < *EnMin) {
            // ����� ���������� �������
            cur=aCards->lesserInSuit(EnMin);
            if(!cur) cur = aCards->greaterInSuit(EnMin);
          } else {
          cur= aCards->greaterInSuit(EnMin);
          }
        } else {
          cur = MyMax; // � ��� ��� ���� �������� ���� ����� ���
        }
      } else {
        // � ���� ��� ����  � ������ �����
        if (Pronesti) {
          //���� �������� �����
          if (aCards->exists(Pronesti->face(),Pronesti->suit())) {
            cur = Pronesti;
          } else {
            cur = aCards->maxInSuit(Pronesti->suit());
            if (!cur)   cur = aCards->maxCard();
          }
        } else {
          // �������� �� ������ - ���
          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxCard();
        }
      }
  } else { // ��������� ����� ��� ����
    Card *MyMax = aCards->maxInSuit(aRightCard->suit());
    Card *MyMin = aCards->minInSuit(aRightCard->suit());
    if (MyMax) {

      if ( *MyMin < *aRightCard) {
//        ���� ����������� �������
        Card *NapMin = Naparnik->lesserInSuit(aRightCard);
        if (NapMin) {
          cur = aCards->lesserInSuit(aRightCard);
        } else {
          cur = MyMax; // ���� �������� ��� ?
        }
      } else {
        cur = MyMax; // ������� �� ����� �������
      }

    } else {
      // � � ���� ��� �����

          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxCard();
    }

  }
  Pronesti = NULL;
  if (!cur)
    cur = aCards->minCard();

  aMaxCardList->clear();
  delete aMaxCardList;
  return cur;

}
//-----------------------------------------------------------------------
Card *Player::MiserCatch3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=NULL;
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
        // � ���� ���� �����  � ������ �����
//        Card *MyMax = aCards->maxInSuit(aLeftCard->suit());
//        Card *MyMin = aCards->minInSuit(aLeftCard->suit());
        if (*aLeftCard < *aRightCard) {
          // �������� ��� �������� �����
          cur = aCards->maxInSuit(aLeftCard->suit());

        } else {
          //���� ���� �������
          cur = aCards->lesserInSuit(aLeftCard);
          if (!cur ) cur = aCards->maxInSuit(aLeftCard->suit());
        }
      } else {
        // � ���� ��� ����  � ������ �����
        if (Pronesti) {
          //���� �������� �����
          if (aCards->exists(Pronesti->face(),Pronesti->suit())) {
            cur = Pronesti;
          } else {
            cur = aCards->maxInSuit(Pronesti->suit());
            if (!cur) cur = aCards->maxCard();
          }
        } else {
          // �������� �� ������ - ���
          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxCard();
        }
      }
  } else { // RightHand - my friend
    Card *MyMax = aCards->maxInSuit(aLeftCard->suit());
    Card *MyMin = aCards->minInSuit(aLeftCard->suit());
    if (MyMax) {
      if ( *MyMin < *aLeftCard) {
//        ���� ����������� �������
        Card *NapMin = Naparnik->lesserInSuit(aLeftCard);
        if (NapMin) {
          cur = aCards->lesserInSuit(aLeftCard);
        } else {
          cur = MyMax; // ���� �������� ��� ?
        }
      } else {
        cur = MyMax; // ������� �� ����� �������
      }

    } else {
      // � � ���� ��� �����
          cur = GetMaxCardPere();
        if (!cur) cur = aCards->maxCard();
    }

  }
  Pronesti = NULL;

  aMaxCardList->clear();
  delete aMaxCardList;
  return cur;
}
//-----------------------------------------------------------------------
Card *Player::MyGame3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // ��� ���� 3 ����� - ���
    Card *cur=NULL;
    Card *maxInSuit;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // � ���� ���� �����  � ������� ����� �����
        if ( aLeftCard->suit() == aRightCard->suit())  {
            // ������������ �������
            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur  ) {
               cur = aCards->minInSuit(aLeftCard->suit());
            }
        } else { //aLeftCard->suit() == aRightCard->suit()
          if ( aRightCard->suit() !=mast)  {
            // �� ���� �������

            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur ) {
              cur = aCards->minInSuit(aLeftCard->suit());
            }
          } else {
            // ���� �������, ��� �� ���� ����
            cur = aCards->minInSuit(aLeftCard->suit());
          }
        }
    } else {
      //  � ���� ��� ����� � ������� ����� �����
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
       if ( ! cur ) {// ��� ���.
           cur = GetMinCardWithOutVz();
       }
       if ( ! cur ) {
           cur = aCards->minCard();
       }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------
Card *Player::MyVist3(Card *aLeftCard,Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // ��� ���� ��� ��� 3 ����� - ���
    Card *cur=NULL;
    Card *maxInSuit;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    Player *aEnemy,*aFriend;
    int mast;
    // ��� ����� � ��� ��������
    if ( aLeftGamer->GamesType != gtPass && aLeftGamer->GamesType != vist ) {
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
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // � ���� ���� �����  � ������� ����� �����
      if ( aEnemy == aLeftGamer ) { // ����� - �������
        LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);

        if ( aLeftCard->suit() == aRightCard->suit())  {
          if ( *aRightCard > *aLeftCard ) {
            // ���� �� ����
            cur = aCards->minInSuit(aRightCard->suit());
          } else {
            // ������������ �������
            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur  ) {
               cur = aCards->minInSuit(aLeftCard->suit());
            }
          }
        } else { //aLeftCard->suit() == aRightCard->suit()
          if ( aRightCard->suit() !=mast)  {
            // �� ���� �������
            cur = aCards->greaterInSuit(aLeftCard);
            if ( !cur ) {
              cur = aCards->minInSuit(aLeftCard->suit());
            }
          } else {
            // ���� �������, ��� �� ���� ����
            cur = aCards->minInSuit(aLeftCard->suit());
          }
        }

      } else {
        //������ ���������
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        if ( aLeftCard->suit() == aRightCard->suit()  )  {
          if ( *aRightCard > *aLeftCard ) {
            // ���������� ������� �������
            cur = aCards->greaterInSuit(aRightCard);
            if ( !cur ) {
              cur = aCards->minInSuit(aRightCard->suit());
            }
          } else {
            // ���� �� ����
            cur = aCards->minInSuit(aRightCard->suit());
          }
        } else {
          if ( aRightCard->suit()==mast  ) {
            // ������ �����, � ������ ������� �������
            cur = aCards->minInSuit(aLeftCard->suit());
          } else { // �� �� .... ����� ���� ����� � ������ ������ ������ !!!
            cur = aCards->minInSuit(aLeftCard->suit());
          }
        }

      }
    } else {
      //  � ���� ��� ����� � ������� ����� �����
       if (aLeftCard->suit()==aRightCard->suit()) {
           if ( *aLeftCard > *aRightCard ) {   maxInSuit = aLeftCard;
           } else { maxInSuit = aLeftCard; }
       } else {
           if ( aLeftCard->suit() != mast && aRightCard->suit() != mast) {               maxInSuit = aLeftCard;
           } else { maxInSuit = aRightCard;
           }
       }
       if ( aEnemy == aLeftGamer ) { // � ���� �����
            LoadLists(aLeftGamer,aLeftGamer,aMaxCardList);

          RecountTables(aMaxCardList,23);

           if (maxInSuit == aLeftCard ) { //���������� �������� ��� �����
               cur = aCards->greaterInSuit(aLeftCard);
           } else { // ��� ���� ������... ��������� �����������
               cur = aCards->minInSuit(maxInSuit->suit());
           }
       } else { // � ���� - ��������
          LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,23);

           if (maxInSuit == aLeftCard ) { // ��� ���� ������... ��������� �����������
               cur = aCards->minInSuit(maxInSuit->suit());
           } else {//���������� �������� ��� �����
               cur = aCards->greaterInSuit(aLeftCard);
           }
       }
       if (!cur) {
           cur = aCards->minInSuit(mast);
       }
       if ( ! cur ) {// ��� ���.
           cur = GetMinCardWithOutVz();
       }
       if ( ! cur ) {
           cur = aCards->minCard();
       }


    }

    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;

}
//-----------------------------------------------------------------------

Card *Player::MyVist2(Card *aRightCard,Player *aLeftGamer,Player *aRightGamer) { // ��� ���� ��� ��� 2 ����� - ���
    Card *cur=NULL;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    Player *aEnemy,*aFriend;
    int mast;
    // ��� ����� � ��� ��������
    if ( aLeftGamer->GamesType != gtPass && aLeftGamer->GamesType != vist ) {
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
        if ( aRightCard->face() >= 10 ) {
            cur = aCards->minInSuit(aRightCard->suit());
        } else { // ����� ����� � ������ ������� � ���� ��� � ������ ����� � ���� ������
            cur = aCards->maxInSuit(aRightCard->suit());
        }
        if ( !cur ) { // � � ���� ����� � ��� !!!
            cur = aCards->maxInSuit(mast); // ������� ������������
        }
        if ( ! cur ) {
            cur = GetMinCardWithOutVz();
        }
    } else { // � ���� - ��������
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
        RecountTables(aMaxCardList,23);
        cur = aCards->greaterInSuit(aRightCard);
        if (!cur) {
            cur = aCards->minInSuit(aRightCard->suit());
        }
        if (!cur) { // ��� ����� ������� ������
            cur = aCards->minInSuit(mast);
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
Card *Player::MyVist1(Player *aLeftGamer,Player *aRightGamer) {
// 1 - ������ ������ 2-��������� ����� � ����������� 3-��� ���������� 4-???
    Card *cur=NULL;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    Player *aEnemy,*aFriend;
    int mast;
    // ��� ����� � ��� ��������
    if ( aLeftGamer->GamesType != gtPass && aLeftGamer->GamesType != vist ) {
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
          cur = NULL;
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
    } else { // � ���� - ��������
        LoadLists(aRightGamer,aRightGamer,aMaxCardList);
          RecountTables(aMaxCardList,1);

        cur = GetMaxCardWithOutPere();
        if ( !cur) {
            cur = GetMaxCardPere();
        }
        if ( !cur) {
            cur = aCards->maxCard();
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
    Card *cur=NULL;
    Card *MaxLeftCard=NULL;
    int mast = aRightCard->suit();
    int koz  = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,23);

    cur = aCards->maxInSuit(mast);
    if ( !cur ) { // ��� �����
        if ( !aLeftGamer->aCards->cardsInSuit(mast) ) { // � ������ ��� ����
            MaxLeftCard = aLeftGamer->aCards->maxInSuit(koz);
            cur = aCards->greaterInSuit(MaxLeftCard); //
            if (!cur) {
                cur = aCards->lesserInSuit(MaxLeftCard);
            }
            if ( !cur ) {
                cur = GetMinCardWithOutVz(); // ������
            }
            if ( !cur ) {
                 cur = aCards->minCard();
            }
        } else { // ���� ����� � ������
            cur = aCards->minInSuit(koz);
        }
    } else { // � ���� ���� �����
        if ( aLeftGamer->aCards->cardsInSuit(mast) ) { //� ������ ����
            MaxLeftCard = aLeftGamer->aCards->maxInSuit(mast);
            cur = aCards->greaterInSuit(MaxLeftCard);
            if ( !cur ) {
                cur = aCards->lesserInSuit(MaxLeftCard);
            }
        } else { // � ������ ���
            if ( !aLeftGamer->aCards->cardsInSuit(koz) ) { // � ������ � ���
                cur = aCards->greaterInSuit(aRightCard);
                if ( !cur) {
                    cur = aCards->minInSuit(mast);
                }
            } else { // ���� � ������ ������
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
// 1-������ ������ 2-��������� ����� � ����������� 3-��� ���������� 4-???
    Card *cur=NULL;
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables(aMaxCardList,1);
    if ( aLeft->cardsInSuit(mast) || aRight->cardsInSuit(mast) ) { // � ����������� ���� ������
        cur = aCards->maxInSuit(mast);
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
             cur = aCards->maxCard();
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
            cur = aCards->maxInSuit(mast);
        }
        if ( !cur ) {
             cur = aCards->maxCard();
        }
    }
    GamesType=tmpGamesType;
    delete aMaxCardList;
    return cur;
}
//-----------------------------------------------------------------------
Card *Player::GetMaxCardPere(void) {
    int index=0,vz=0,pere=0;

    for (int i=1;i<=4;i++) {
        if ( MastTable[i].vzatok > vz || (MastTable[i].vzatok == vz && MastTable[i].perehvatov > pere) ) {
            index = i;
            vz = MastTable[i].vzatok;
            pere = MastTable[i].perehvatov;
        }
    }

    if (index) return aCards->maxInSuit(index);
    return NULL;
}
//-----------------------------------------------------------------------
Card *Player::GetMaxCardWithOutPere(void) {
    int index=0,vz=0,pere=0;
    for (int i=1;i<=4;i++) {
        if ( MastTable[i].vzatok > vz || (MastTable[i].vzatok == vz && MastTable[i].perehvatov < pere) ) {
            index = i;
            vz = MastTable[i].vzatok;
            pere = MastTable[i].perehvatov;
        }
    }
    if (index) return aCards->maxInSuit(index);
    return NULL;
}
//-----------------------------------------------------------------------
Card *Player::GetMinCardWithOutVz(void) {
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
        return aCards->minInSuit(index);
      else
        return aCards->minCard();
//    return NULL;
}
//-----------------------------------------------------------------------
void Player::LoadLists(Player *aLeftGamer,Player *aRightGamer,CardList *aMaxCardList) {
/*    int nLeftVisible = aLeftGamer->nCardsVisible,nRightVisible = aRightGamer->nCardsVisible;
    int nCards = aCards->count();       */
    aLeft->clear();
    aRight->clear();
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
        aLeft->shallowCopy(aLeftGamer->aCards);
        aRight->shallowCopy(aRightGamer->aCards);
/*    }*/
// Get Max List only len
/*    for (int m=1;m<=4;m++) {
      for (int c=FACE_ACE;c>=7;c-- ) {
       if ( aLeft->exists(c,m) || aRight->exists(c,m) ) {
           aMaxCardList->append(new Card(c,m));
       }
      }
    }*/
    for (int m=1;m<=4;m++) {
          Card *LeftMax,*RightMax,*Max=NULL;
          while ( aLeft->cardsInSuit(m) || aRight->cardsInSuit(m)) {
            LeftMax = aLeft->maxInSuit(m);
            RightMax=aRight->maxInSuit(m);
            if (LeftMax == NULL && RightMax== NULL) {
              Max=NULL;
            } else if (LeftMax == NULL && RightMax!= NULL) {
              Max = RightMax;
              aRight->remove(Max);
            } else if (LeftMax != NULL && RightMax== NULL) {
              Max = LeftMax ;
              aLeft->remove(Max);
            } else if  ( *LeftMax > *RightMax ) {
              Max = LeftMax;
              aLeft->remove(LeftMax);
              aRight->remove(RightMax);
            } else {
              Max = RightMax;
              aLeft->remove(LeftMax);
              aRight->remove(RightMax);
            }
          }
          if (Max !=NULL) {
            aMaxCardList->append(new Card(Max->face(),Max->suit()));
          }
    }
    aLeft->clear();
    aRight->clear();
    aLeft->shallowCopy(aLeftGamer->aCards);
    aRight->shallowCopy(aRightGamer->aCards);
}
//-----------------------------------------------------------------------
eGameBid Player::makemove4out(void) {
    eGameBid GamesTypeRetVal;
    int i;
    int nMaxMastLen=0;
    eSuit nMaxMast=SuitNone;
    tSuitProbs LocalMastTable[5];
    LocalMastTable[0].vzatok=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum=0;


    for (i=1; i<=4; i++ )  {
      LocalMastTable[i] = vzatok4game( (eSuit) i,1  );
      LocalMastTable[0].vzatok += LocalMastTable[i].vzatok ;
      LocalMastTable[0].perehvatov +=LocalMastTable[i].perehvatov ;
      LocalMastTable[0].sum += LocalMastTable[i].sum;
    }

    for (i=1; i<=4; i++) { // �������������� ������ - ����� ������� �����
      if( aCards->cardsInSuit(i) > nMaxMastLen ) {
        nMaxMastLen = aCards->cardsInSuit(i);
        nMaxMast = (eSuit) i;
      }
    }

    for (i=1; i<=4; i++) {
      if(aCards->cardsInSuit(i) == nMaxMastLen && nMaxMast != i ) { // ���� ����������� ������� �����
       if ( LocalMastTable[i].sum  > LocalMastTable[nMaxMast].sum  )  {
         nMaxMast = (eSuit) i;
        }
      }
    }

    //����� � ������ ��� ���������
    GamesTypeRetVal = ( eGameBid ) (LocalMastTable[0].vzatok*10 + nMaxMast );
    // ��� ��, ��� �� ����� ������ �����������
    return GamesTypeRetVal;

}
//-----------------------------------------------------------------------
eGameBid Player::makemove(eGameBid MaxGame,int HaveAVist,int nGamerVist) { // ����� ��������� ������� ������� - ���� ��� ����
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
  Card *FirstCardOut=NULL,*SecondCardOut=NULL;
  Card *tmpFirstCardOut,*tmpSecondCardOut;
  Card *RealFirstCardOut,*RealSecondCardOut;
  eGameBid  Hight = g105,  tmpHight = g105;
  for ( int i=0;i<12;i++ ) {
    tmpFirstCardOut = (Card*) aCards->at(i);
    aCards->removeAt(i);
    //for ( int j=0;j<12;j++ )  {
    for (int j=i+1; j<12; j++) { // patch from Rasskazov K. (kostik450@mail.ru)
      if ( j!=i ) {
        tmpSecondCardOut = (Card*) aCards->at(j);
        aCards->removeAt(j);
        Player *tmpGamer = new Player(99);
        tmpGamer->aCards->shallowCopy(aCards);
        tmpGamer->aCards->mySort();
        tmpHight = tmpGamer->makemove4out();
        if ( (tmpHight < Hight )
         || (tmpHight == Hight  && tmpGamer->MastTable[0].perehvatov < MastTable[0].perehvatov) ) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer->aCards->clear();
        delete tmpGamer;
        aCards->putAt(j,tmpSecondCardOut);
      }
    }
    aCards->putAt(i,tmpFirstCardOut);
  }

  RealFirstCardOut     = aCards->maxInSuit(FirstCardOut->suit());
  aCards->remove(RealFirstCardOut);
  aOut->append(RealFirstCardOut);
  if (SecondCardOut->suit() != FirstCardOut->suit()) {
    RealSecondCardOut = aCards->maxInSuit(SecondCardOut->suit());
   } else {
    RealSecondCardOut = aCards->greaterInSuit(FirstCardOut);
    if ( !RealSecondCardOut )
      RealSecondCardOut = aCards->maxCard(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
   }
  aCards->remove(RealSecondCardOut);
  aOut->append(RealSecondCardOut);
  aCards->mySort();
  return g86;
}


eGameBid Player::makeout4game () {
  Card *FirstCardOut = NULL, *SecondCardOut = NULL;
  Card *tmpFirstCardOut, *tmpSecondCardOut;
  Card *RealFirstCardOut, *RealSecondCardOut;
  eGameBid Hight = zerogame, tmpHight = zerogame;

  for (int i = 0; i < 12; i++) {
    tmpFirstCardOut = aCards->at(i);
    aCards->removeAt(i);
    for (int j = 0; j < 12; j++)  {
      if (j != i) {
        tmpSecondCardOut = aCards->at(j);
        aCards->removeAt(j);
        Player *tmpGamer = new Player(99);
        tmpGamer->aCards->shallowCopy(aCards);
        tmpGamer->aCards->mySort();
        tmpHight = tmpGamer->makemove4out();
        if ((tmpHight > Hight) ||
            (tmpHight == Hight && tmpGamer->MastTable[0].perehvatov < MastTable[0].perehvatov)) {
          Hight = tmpHight;
          FirstCardOut = tmpFirstCardOut;
          SecondCardOut = tmpSecondCardOut;
        }
        tmpGamer->aCards->clear();
        delete tmpGamer;
        aCards->putAt(j, tmpSecondCardOut);
      }
    }
    aCards->putAt(i, tmpFirstCardOut);
  }

  RealFirstCardOut = aCards->minInSuit(FirstCardOut->suit());
  aCards->remove(RealFirstCardOut);
  aOut->append(RealFirstCardOut);
  if (SecondCardOut->suit() != FirstCardOut->suit()) {
    RealSecondCardOut = aCards->minInSuit(SecondCardOut->suit());
  } else {
    RealSecondCardOut = aCards->greaterInSuit(FirstCardOut);
    if (!RealSecondCardOut)
      RealSecondCardOut = aCards->minCard(); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 Bad algoritm
  }
  aCards->remove(RealSecondCardOut);
  aOut->append(RealSecondCardOut);
  aCards->mySort();
  //Hight = makemove(undefined,undefined);
  Hight = makemove4out();
  if (Hight < GamesType )  {
    /* ��� ����� ������ */
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


//��� ��� ��������
eGameBid Player::makemove (eGameBid lMove, eGameBid rMove) {
/*
  if ( GamesType == gtPass )  {
    GamesType=gtPass;
  } else {
*/
  if (GamesType != gtPass)  {
    int i;
    int nMaxMastLen = 0;
    eSuit nMaxMast = SuitNone;
    tSuitProbs LocalMastTable[5];
    eGameBid MGT = qMax(lMove,rMove);
    LocalMastTable[0].vzatok=0;
    LocalMastTable[0].perehvatov=0;
    LocalMastTable[0].sum = 0;
    for (i = 1; i <= 4; i++)  {
      LocalMastTable[i] = vzatok4game((eSuit)i, 1);
      LocalMastTable[0].vzatok += LocalMastTable[i].vzatok;
      LocalMastTable[0].perehvatov +=LocalMastTable[i].perehvatov;
      LocalMastTable[0].sum += LocalMastTable[i].sum;
    }
    // �������������� ������ - ����� ������� �����
    for (i = 1; i <= 4; i++) {
      if (aCards->cardsInSuit(i) > nMaxMastLen) {
        nMaxMastLen = aCards->cardsInSuit(i);
        nMaxMast = (eSuit)i;
      }
    }
    for (i = 1; i <= 4; i++) {
      if (aCards->cardsInSuit(i) == nMaxMastLen && nMaxMast != i) {
        // ���� ����������� ������� �����
        if (LocalMastTable[i].sum > LocalMastTable[nMaxMast].sum) nMaxMast = (eSuit)i;
      }
    }
    // ����� � ������ ��� ���������
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
    // ��� ��, ��� �� ����� ������ �����������
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
   tSuitProbs MastTable;
   Card *MyCard,*tmpCard;
   CardList *MyCardStack     = new CardList;
   CardList *EnemyCardStack  = new CardList;
   for (int c=7;c<=FACE_ACE;c++ ) {
    MyCard = aCards->exists(c, Mast);
    if ( MyCard )
      MyCardStack->append(MyCard);
    else
      EnemyCardStack->append(new Card(c, Mast));
   }
   if ( MyCardStack->count() >=4 && MyCardStack->count() <=5 )    {
      EnemyCardStack->free(EnemyCardStack->minCard());
   }
   if ( a23 == 23 ) {
       MastTable = Compare2List4Max23(MyCardStack,EnemyCardStack);
   } else {
       MastTable = Compare2List4Max(MyCardStack,EnemyCardStack);
   }
   MastTable.len = aCards->cardsInSuit(Mast);
   MastTable.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->exists(j,Mast);
    if (  tmpCard ) {
      MastTable.sum +=  tmpCard->face();
    }
   }
   MyCardStack->clear();
   delete MyCardStack;
   delete EnemyCardStack;
   return MastTable;
}
//-----------------------------------------------------------------------
tSuitProbs Player::vzatok4pass(eSuit Mast,CardList *aMaxCardList) {
   tSuitProbs MastTable;
   Card *MyCard,*tmpCard;
   CardList *MyCardStack     = new CardList;
   CardList *EnemyCardStack  = new CardList;
   for (int c=7;c<=FACE_ACE;c++ ) {
        MyCard = aCards->exists(c, Mast);
        if ( MyCard ) {
          MyCardStack->append(MyCard);
        }
        MyCard = aMaxCardList->exists(c, Mast);
        if ( MyCard ) {
          EnemyCardStack->append(MyCard);
        }
   }
   MastTable = Compare2List4Min(MyCardStack,EnemyCardStack);
   MastTable.len = aCards->cardsInSuit(Mast);
   MastTable.sum = 0;
   for (int j=7;j<=FACE_ACE;j++ ) {
    tmpCard = aCards->exists(j,Mast);
    if (  tmpCard ) {
      MastTable.sum +=  tmpCard->face();
    }
   }
   MyCardStack->clear();
   EnemyCardStack->clear();
   delete MyCardStack;
   delete EnemyCardStack;
   return MastTable;

}

//-----------------------------------------------------------------------
tSuitProbs Player::Compare2List4Min(CardList *My,CardList *Enemy) { // ��� ��� ����������
  tSuitProbs MastTable;
  MastTable.vzatok=0;
  MastTable.perehvatov=0;
  int nMaxLen;
  Card *MyCardMin,*EnemyCardMax,*EnemyCardMin;
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  Enemy->free(Enemy->minCard());
  My->count()>Enemy->count() ? nMaxLen = My->count() : nMaxLen = Enemy->count();
  for ( int i=1; i<=nMaxLen; i++ )  {
    MyCardMin = My->minCard();
    EnemyCardMax = Enemy->maxCard();
    if ( MyCardMin ) {
      EnemyCardMin = Enemy->lesserInSuit(MyCardMin);
      if (!EnemyCardMin) {
          EnemyCardMin = Enemy->minCard();
      }

    } else {
//      EnemyCardMin = Enemy->minCard();
      break;
    }
    if ( !EnemyCardMax && !EnemyCardMin)  {
      break;
    }
    if ( *MyCardMin > *EnemyCardMin ) {
      // ���������� ���  ��� �������
      My->remove(MyCardMin);
//      Enemy->free(EnemyCardMin);
      Enemy->remove(EnemyCardMin);

      MastTable.vzatok++;
    } else {
      My->remove(MyCardMin);
//      Enemy->free(EnemyCardMax);
      Enemy->remove(EnemyCardMax);
    }
  }
  return MastTable;
}
//-----------------------------------------------------------------------
tSuitProbs Player::Compare2List4Max(CardList *My,CardList *Enemy) {
  tSuitProbs MastTable;
  MastTable.vzatok=0;

  MastTable.perehvatov=0;
  int nMaxLen,nIget=1;
  Card *MyCardMax=0,*EnemyCardMax=0,*EnemyCardMin=0;
  My->count()>Enemy->count() ? nMaxLen = My->count() : nMaxLen = Enemy->count();
  for ( int i=1; i<=nMaxLen; i++ )  {
    MyCardMax = My->maxCard();
    if ( MyCardMax ) {
      EnemyCardMax = Enemy->greaterInSuit(MyCardMax);
      if (!EnemyCardMax) {
        EnemyCardMax = Enemy->minCard();
      }
    } else {
      EnemyCardMax = Enemy->maxCard();
    }
    EnemyCardMin = Enemy->minCard();
    if ( !MyCardMax )  {
      break;
    }
    if ( !EnemyCardMax && !EnemyCardMin)  {
      // ������� - ��� ������
      for ( int j =1; j<=My->count();j++ ) {
        MastTable.vzatok++;
      }
      break;
    }
    if ( *MyCardMax > *EnemyCardMax ) {
      // ���������� ��� �������, ��� �������
      My->remove(MyCardMax);
//      Enemy->free(EnemyCardMin);
  Enemy->remove(EnemyCardMin);
      MastTable.vzatok++;
      if ( !nIget )  {
        MastTable.perehvatov++;
      }

      nIget = 1;
    } else {
      My->remove(MyCardMax);
//      Enemy->free(EnemyCardMax);
      Enemy->remove(EnemyCardMax);
      nIget = 0;
    }
  }
  return MastTable;
}
//-----------------------------------------------------------------------
tSuitProbs Player::Compare2List4Max23(CardList *My,CardList *Enemy) {
  tSuitProbs MastTable;
  MastTable.vzatok=0;
  MastTable.perehvatov=0;
  int nMaxLen,nIget=1;
  Card *MyCardMax,*EnemyCardMax;
  My->count()>Enemy->count() ? nMaxLen = My->count() : nMaxLen = Enemy->count();
  for ( int i=1; i<=nMaxLen; i++ )  {
    EnemyCardMax = Enemy->maxCard();
    if (EnemyCardMax) {// �������� �-�� ������ ��m � ����
      MyCardMax = My->greaterInSuit(EnemyCardMax);
      if ( ! MyCardMax ) {  // ��� � ���� ������ ��� � ����
        MyCardMax = My->minCard();
        if (!MyCardMax ) {  // � ������ ��� ���
          break ;
        } else {  // ������ ��� � ����
          My->remove(MyCardMax);
          Enemy->remove(EnemyCardMax);
          nIget = 0;
        }
      } else {  // ���� ������ ��� � ����
        My->remove(MyCardMax);
        Enemy->remove(EnemyCardMax);
        MastTable.vzatok++;
        if (! nIget) MastTable.perehvatov++;
        nIget = 1;
      }
    } else {  // � ���� ��� ���� � ������ ����� ! ������� - ��� ������
      if ( ! Enemy->count() ) {
        for ( int j =1; j<=My->count();j++ ) MastTable.vzatok++;
      }
//25.07.2000 for ( int j =1; j<=My->count();j++ ) MastTable.vzatok++;
      break;
    }
  }
  return MastTable;
}
//-----------------------------------------------------------------------
Card *Player::MyPass1(Card *rMove,Player *aLeftGamer,Player *aRightGamer) {
  Card *cur=NULL;
  eGameBid tmpGamesType=GamesType;
  CardList *aMaxCardList=new CardList;
  CardList *aTmpList=new CardList;
  CardList *aStackStore=NULL;

  if ( rMove != NULL )  {
    aTmpList=new CardList;
    aTmpList->copySuit(aCards, (eSuit)rMove->suit());
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
        if ( aLeftGamer->aCards->cardsInSuit(cur->suit())==0
             && aRightGamer->aCards->cardsInSuit(cur->suit())==0 ) {

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
        cur = aCards->minCard();
    }
  }
  if ( rMove != NULL )  {
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
  Card *cur=NULL;
  eGameBid tmpGamesType=GamesType;
  int mast = aRightCard->suit();
  CardList *aMaxCardList=new CardList;
  LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
  RecountTables4RasPass(aMaxCardList,1);
/////////////////////////////////////////////////////////
  if ( tmpGamesType == raspass || tmpGamesType == g86 )  { // ������� // �����
    cur = aCards->maxInSuit(mast); // !!!!!!!!!!!! ����� ������
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
              cur = aCards->minCard();
       }

    } else { // � ���� ���� �����
        if ( aLeftGamer->aCards->cardsInSuit(mast) ) { //� ������ ����
            cur = aCards->lesserInSuit(aLeftGamer->aCards->maxInSuit(mast));
            if ( !cur ) {
                cur = aCards->maxInSuit(mast);
            }
        } else { // � ������ ���
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
  Card *cur=NULL;
    /*Card *maxInSuit;*/
    eGameBid tmpGamesType=GamesType;
    CardList *aMaxCardList=new CardList;
//    int mast = GamesType - (GamesType/10) * 10;
    // �������� ������
    LoadLists(aLeftGamer,aRightGamer,aMaxCardList);
    RecountTables4RasPass(aMaxCardList,23);
    if ( aCards->cardsInSuit(aLeftCard->suit()) )  {
       // � ���� ���� �����  � ������� ����� �����
       // ������������ ����������
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
      //  � ���� ��� ����� � ������� ����� �����
       cur = GetMaxCardWithOutPere();
       if ( !cur ) {
               cur = GetMaxCardPere();  //����� � ����������� (max)
       }
       if ( !cur ) {
             cur = GetMinCardWithOutVz(); // ������
       }
       if ( !cur ) {
              cur = aCards->minCard();
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
           aLeft->append(new Card(c,m));
           lr = RightHand;
         } else {
           aRight->append(new Card(c,m));
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
               aLeft->append(new Card(c,m));
               lr = RightHand;
             } else {
               aRight->append(new Card(c,m));
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
               aLeft->append(new Card(c,m));
               lr = RightHand;
             } else {
               aRight->append(new Card(c,m));
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
  if (aCardsOut->at(0)) aCards->append(aCardsOut->at(0));
  if (aCardsOut->at(1)) aCards->append(aCardsOut->at(1));
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

  if (mPlayerNo == 3) startX = 0;
  // normal
  startX -= opened ? SUIT_OFFSET : CLOSED_CARD_OFFSET ;
  for (int i = 0; i < aCards->size(); i++) {
    Card *pp = (Card *)aCards->at(i);
    if (!pp) continue;
    prev = cur;
    cur = pp;
    if (opened) {
      startX += (prev && prev->suit() != cur->suit()) ? NEW_SUIT_OFFSET : SUIT_OFFSET ;
    } else startX += CLOSED_CARD_OFFSET;
    *dest++ = startX;
    *dest++ = i;
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

  aCards->mySort();

  int cnt = buildHandXOfs(ofs, Left, !nInvisibleHand);
  if (cnt) Left = ofs[0];
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = Top;
    Card *card = (Card *)aCards->at(ofs[f+1]);
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
  aCards->mySort();
  int cnt = buildHandXOfs(ofs, left, !nInvisibleHand);
  if (!cnt) return;
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = top;
    DeskView->ClearBox(x, y, CARDWIDTH, CARDHEIGHT);
  }
}
