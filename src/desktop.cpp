#include "prfconst.h"

#include <QMainWindow>
#include "papplication.h"
#include "kpref.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "deskview.h"
#include "ncounter.h"
#include "player.h"
#include "desktop.h"
#include "plscore.h"
#include "human.h"


QString Tclist2pchar (Tclist *list) {
  QString res;
  int nIndex = -1, nItemVal = 0;
  int *pnItemVal = NULL;
  while (1) {
    pnItemVal = (int *)list->NextItem(nIndex++);
    if (!pnItemVal) break;
    nItemVal = *pnItemVal;
    res += QString::number(nItemVal);
    res += ".";
  }
  return res;
}


//-------------------------------------------------------------
//#define ENDOFTORG  if ( (GamesType[1] != undefined &&    GamesType[2] != undefined && GamesType[3] != undefined )&& ((GamesType[1] == gtPass ? 1:0)+(GamesType[2] == gtPass ? 1:0)+(GamesType[3] == gtPass ? 1:0) >= 2 )) break;
#define ENDOFTORG \
  if ((GamesType[1] != undefined && GamesType[2] != undefined && GamesType[3] != undefined) && \
      ((GamesType[1] == gtPass?1:0)+(GamesType[2] == gtPass?1:0)+(GamesType[3] == gtPass?1:0) >= 2)) break;
//-------------------------------------------------------------
typedef struct {
  int m,b,lv, rv;
} TRospis;
//-------------------------------------------------------------
const char *AUTOSAVENAME=".kprefautosave.prf";
//-------------------------------------------------------------
void TDeskTop::InternalConstruct(void) {
  CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard=SecondCard=TherdCard=NULL;
  Coloda = new TColoda(CARDINCOLODA);
  Gamers = new Tclist(4); // 1...3

  nCurrentStart.nValue=nCurrentMove.nValue=2;
  nCurrentStart.nMin=nCurrentMove.nMin=1;
  nCurrentStart.nMax=nCurrentMove.nMax=3;

  InsertGamer(new Player(0));
  //InsertGamer(new Player(1));
  // I Hate This Game :)
  InsertGamer(new HumanPlayer(1, DeskView));
  InsertGamer(new Player(2, DeskView));
  InsertGamer(new Player(3, DeskView));
  Gamers->AtFree(0);
  nflShowPaper = 0;
  nBuletScore = 21;
  flProtocol = 0;
}


TDeskTop::TDeskTop (TDeskView *_DeskView) : QObject(0) {
  InternalConstruct();
  DeskView = _DeskView;
}


TDeskTop::TDeskTop () : QObject(0) {
  InternalConstruct();
}


TDeskTop::~TDeskTop () {
  CloseProtocol();
  delete Coloda;
  delete Gamers;
}


void TDeskTop::emitRepaint () {
  emit deskChanged();
}


int TDeskTop::CloseProtocol () {
  if (flProtocol) {
    t = time(NULL);
    tblock = localtime(&t);
    fprintf(ProtocolFile,"\nClose protocol %s", asctime(tblock));
    fclose(ProtocolFile);
  }
  return 1;
}


int TDeskTop::WriteProtocol (const char *line) {
  if (flProtocol) fprintf(ProtocolFile, "\n %s", line);
  return 1;
}


int TDeskTop::OpenProtocol () {
  const char messageprotocol[1024] = "Cant open or create protocol file!";
  if (!(ProtocolFile = fopen(ProtocolFileName, "a"))) {
    DeskView->MessageBox(messageprotocol, "Warning!");
    flProtocol = 0;
    return 0;
  }
  t = time(NULL);
  tblock = localtime(&t);
  fprintf(ProtocolFile,"\nStart protocol %s", asctime(tblock));
  return 1;
}


void TDeskTop::CloseBullet () {
  Tncounter counter(1, 1, 3);
  int mb = INT_MAX, mm = INT_MAX, i;
  TRospis R[4];
  Player *G;
  for (i = 1; i <= 3; i++) {
    G = GetGamerByNum(i);
    R[i].m = G->aScore->nGetMount();
    R[i].b = G->aScore->nGetBull();
    R[i].lv = G->aScore->nGetLeftVists();
    R[i].rv = G->aScore->nGetRightVists();
    if (R[i].m < mm) mm = R[i].m;
    if (R[i].b < mb) mb = R[i].b;
  }
  // Amnistiya gori
  for (i = 1; i <= 3; i++) R[i].m -= mm;
  // Amnistiya puli
  for (i = 1; i <= 3; i++) R[i].b -= mb;
  // svou pulu sebe v visti
  for (i=1;i<=3;i++) {
    //R[i].lv += R[i].b*10/2;
    //R[i].rv += R[i].b*10/2;
    R[i].lv += R[i].b*10/3;
    R[i].rv += R[i].b*10/3;
  }
  // goru - drugim na sebya
  for (i = 1; i <= 3; i++) {
/*
    counter.nValue = i;
    ++counter;
    R[counter.nValue].rv += R[i].m*10/2;
    ++counter;
    R[counter.nValue].lv += R[i].m*10/2;
*/
    counter.nValue = i;
    ++counter;
    R[counter.nValue].rv += R[i].m*10/3;
    ++counter;
    R[counter.nValue].lv += R[i].m*10/3;
  }
  // schitaem visti
  for (i = 1; i <= 3; i++) {
    int i1, i2;
    counter.nValue = i;
    ++counter;
    i1 = counter.nValue;
    ++counter;
    i2 = counter.nValue;
    GetGamerByNum(i)->aScore->Vists = R[i].lv + R[i].rv - R[i1].rv - R[i2].lv;
  }
}


Player *TDeskTop::InsertGamer (Player *Gamer) {
  Gamers->Insert(Gamer);
  return Gamer;
}


void TDeskTop::GamerAssign (Player *newgamer,Player *oldgamer) {
  newgamer->aCards->Assign(oldgamer->aCards);
  newgamer->aLeft->Assign(oldgamer->aLeft);
  newgamer->aRight->Assign(oldgamer->aRight);
  newgamer->aOut->Assign(oldgamer->aOut);
  newgamer->aCardsOut->Assign(oldgamer->aCardsOut);
  newgamer->aLeftOut->Assign(oldgamer->aLeftOut);
  newgamer->aRightOut->Assign(oldgamer->aRightOut);
  newgamer->nGetsCard = oldgamer->nGetsCard;
  newgamer->Mast = oldgamer->Mast;
  newgamer->GamesType = oldgamer->GamesType;
  newgamer->Enemy = oldgamer->Enemy;
  newgamer->nInvisibleHand = oldgamer->nInvisibleHand;
  newgamer->DeskView = oldgamer->DeskView;
  newgamer->Pronesti = oldgamer->Pronesti;
}


TCard *TDeskTop::PipeMakemove (TCard *lMove, TCard *rMove) {
  return (GetGamerByNum(nCurrentMove.nValue))->
    makemove(lMove, rMove, GetGamerByNum(NextGamer(nCurrentMove)), GetGamerByNum(PrevGamer(nCurrentMove)));
}


// ежели  номер не 1 и игра пас или ( ловля мизера и игрок не сетевой),
// то пораждаем нового HumanPlayer с номером текущего
// ежели номер 1 и игра пас и не сетевая игра то порождаем Player
TCard *TDeskTop::ControlingMakemove (TCard *lMove, TCard *rMove) {
  TCard *RetVal = NULL;
  Player *Now = GetGamerByNum(nCurrentMove.nValue);

  if ((GetGamerByNum(1)->GamesType == vist || GetGamerByNum(1)->GamesType == g86catch) &&
      Now->nGamer != 1 && (Now->GamesType == gtPass || Now->GamesType == g86catch)) {
    HumanPlayer *NewHuman = new HumanPlayer(nCurrentMove.nValue, DeskView);
    GamerAssign(NewHuman, Now);
    Gamers->AtPut(nCurrentMove.nValue, NewHuman);
    RetVal = NewHuman->makemove(lMove, rMove, GetGamerByNum(NextGamer(nCurrentMove)), GetGamerByNum(PrevGamer(nCurrentMove)));
    GamerAssign(Now, NewHuman);
    Gamers->AtPut(nCurrentMove.nValue, Now);
    NewHuman->clear();
    delete NewHuman;
  } else if (Now->nGamer == 1 && Now->GamesType == gtPass) {
    Player *NewHuman = new Player(nCurrentMove.nValue, DeskView);
    GamerAssign(NewHuman, Now);
    Gamers->AtPut(nCurrentMove.nValue, NewHuman);
    RetVal = NewHuman->makemove(lMove, rMove, GetGamerByNum(NextGamer(nCurrentMove)), GetGamerByNum(PrevGamer(nCurrentMove)));
    GamerAssign(Now, NewHuman);
    Gamers->AtPut(nCurrentMove.nValue, Now);
    NewHuman->clear();
    delete NewHuman;
  } else {
    RetVal = PipeMakemove(lMove, rMove);
  }
  return RetVal;
}


void TDeskTop::RunGame () {
  tGameBid GamesType[4];
  //char *filename;
  //TCard *FirstCard, *SecondCard, *TherdCard;
  int npasscounter;
  //DeskView->ClearScreen();
  if (flProtocol) OpenProtocol();
  // while !конец пули
  while (!(GetGamerByNum(1)->aScore->nGetBull() >= nBuletScore &&
           GetGamerByNum(2)->aScore->nGetBull() >= nBuletScore &&
           GetGamerByNum(3)->aScore->nGetBull() >= nBuletScore)) {
    Tncounter GamersCounter(1, 1, 3);
    int nPl;
    int nGamernumber = 0 ; // номер заказавшего игру
    int nPassCounter = 0; // кол-во спасовавших
    GamesType[3] = GamesType[2] = GamesType[1] = GamesType[0] = undefined;
    CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard = 0;
    delete Coloda;
    Coloda = new TColoda(CARDINCOLODA);
    Coloda->Mix();
    GetGamerByNum(1)->clear();
    GetGamerByNum(2)->clear();
    GetGamerByNum(3)->clear();
    CurrentGame = undefined;
    nflShowPaper = 0;
    // сдали карты
    for (int i = 0; i < CARDINCOLODA-2; i++) {
      Player *tmpGamer = GetGamerByNum(GamersCounter.nValue);
      tmpGamer->AddCard((TCard *)Coloda->At(i));
      if (!((TCard *)Coloda->At(i))) DeskView->MessageBox("TCard = NULL", "Error!!!");
      ++GamersCounter;
    }
    Repaint();
    emitRepaint();
    //DeskView->mySleep(1);
    GamersCounter = nCurrentStart;

    sprintf(ProtocolBuff, "Current start:%i", nCurrentStart.nValue);
    WriteProtocol(ProtocolBuff);

    npasscounter = 0;
    while (npasscounter < 2) {
      // пока как минимум 2 е не спасовали
      if (GamesType[1] != gtPass) GamesType[1] = GetGamerByNum(GamersCounter.nValue)->makemove(GamesType[2], GamesType[3]);
      sprintf(ProtocolBuff, "Gamer 1:%i say: %s", GamersCounter.nValue, sGameName(GamesType[1]));
      WriteProtocol(ProtocolBuff);
      Repaint();
      emitRepaint();

      ++GamersCounter;
      ENDOFTORG

      if (GamesType[2] != gtPass) GamesType[2] = GetGamerByNum(GamersCounter.nValue)->makemove(GamesType[3], GamesType[1]);
      sprintf(ProtocolBuff, "Gamer 2:%i say: %s", GamersCounter.nValue, sGameName(GamesType[1]));
      WriteProtocol(ProtocolBuff);
      Repaint();
      emitRepaint();

      ++GamersCounter;
      ENDOFTORG

      if (GamesType[3] != gtPass) GamesType[3] = GetGamerByNum(GamersCounter.nValue)->makemove(GamesType[1], GamesType[2]);
      sprintf(ProtocolBuff, "Gamer 3:%i say: %s", GamersCounter.nValue, sGameName(GamesType[1]));
      WriteProtocol(ProtocolBuff);
      Repaint();
      emitRepaint();

      ++GamersCounter;
      ENDOFTORG
    }

    // выч. максим игру
    for (int i = 1; i <= 3; i++) {
      if (GamesType[0] < GamesType[i]) GamesType[0] = GamesType[i];
    }

    if (GamesType[0] !=gtPass) {
      // не расспасы
      // узнаем кто назначил максимальную игру
      for (int i = 1; i <= 3; i++) {
        Player *tmpg = GetGamerByNum(i);
        if (tmpg->GamesType == GamesType[0]) {
          nPassCounter = 0;
          Tncounter tmpGamersCounter(1, 3);
          Player *PassOrVistGamers;
          int PassOrVist = 0, nPassOrVist = 0;
          CardOnDesk[2] = (TCard *)Coloda->At(30);
          CardOnDesk[3] = (TCard *)Coloda->At(31);
          sprintf(ProtocolBuff,"Get cards %i %i for %i game", Card2Int(CardOnDesk[2]), Card2Int(CardOnDesk[3]), GamesType[0]);
          WriteProtocol(ProtocolBuff);
          Repaint();
          emitRepaint();
          DeskView->mySleep(4);
          // запихиваем ему прикуп
          tmpg->AddCard((TCard *)Coloda->At(30));
          tmpg->AddCard((TCard *)Coloda->At(31));
          CardOnDesk[2] = CardOnDesk[3] = NULL;
          nGamernumber = tmpg->nGamer;
          //DeskView->ClearScreen();
          Repaint();
          emitRepaint();
          DeskView->mySleep(2);
          if (tmpg->GamesType != g86) { // не мизер
            nCurrentMove.nValue = i;
            GamesType[0] = CurrentGame = tmpg->makeout4game();
          } else {
            // показать все карты
            int tempint = nCurrentMove.nValue;
            int nVisibleState = tmpg->nInvisibleHand;
            //tmpg->nCardsVisible = 0;
            tmpg->nInvisibleHand = 0;
            Repaint();
            emitRepaint();
            if (tmpg->nGamer !=1) DeskView->MessageBox("Try to remember the cards", "Message");
            // ждать до события
            //DeskView->mySleep(0);
            tmpg->nInvisibleHand = nVisibleState;
            nCurrentMove.nValue = tmpg->nGamer;
            GamesType[0] = CurrentGame = tmpg->makeout4miser();
            nCurrentMove.nValue = tempint;
          }
          tmpGamersCounter.nValue = i;
          DeskView->ClearScreen();
          Repaint();
          emitRepaint();
          // пас или вист
          ++tmpGamersCounter;
          PassOrVistGamers=GetGamerByNum(tmpGamersCounter.nValue);
          PassOrVistGamers->GamesType = undefined;
          Repaint();
          emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          DeskView->mySleep(2);
          PassOrVist = PassOrVistGamers->makemove(CurrentGame,gtPass,0);
          nPassOrVist = tmpGamersCounter.nValue;
          if (PassOrVistGamers->GamesType == gtPass ) nPassCounter++;
          Repaint();
          emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          DeskView->mySleep(1);
          ++tmpGamersCounter;
          PassOrVistGamers=GetGamerByNum(tmpGamersCounter.nValue);
          PassOrVistGamers->GamesType = undefined;
          //Repaint(tmpGamersCounter.nValue);
          Repaint();
          emitRepaint();
          DeskView->mySleep(1);
          PassOrVistGamers->makemove(CurrentGame, PassOrVist, nPassOrVist);
          if (PassOrVistGamers->GamesType == gtPass) nPassCounter++;
          Repaint();
          emitRepaint();
          DeskView->mySleep(1);
          if (nPassCounter==2) {
            // двое спасовали :)
            tmpg->nGetsCard = nGetGameCard(tmpg->GamesType);
            sprintf(ProtocolBuff,"Two gamers said 'pass'");
            WriteProtocol(ProtocolBuff);
            goto LabelRecordOnPaper;
          } else {
            // Открытые или закрытые карты
            for (int ntmp = 2 ; ntmp <= 3 ; ntmp++) {
              Player *Gamer4Open;
              Gamer4Open = GetGamerByNum(ntmp);
              if (nPassCounter || CurrentGame == g86) {
                // если не 2 виста
                if (Gamer4Open->GamesType == gtPass || Gamer4Open->GamesType == vist || Gamer4Open->GamesType == g86catch)
                  Gamer4Open->nInvisibleHand = 0;
              }
            }
            Repaint();
            emitRepaint();
          }
          break;
        }
      } // узнаем кто назначил максимальную игру
    }  else {
      // раскручиваем распас
      GamesType[0] = CurrentGame = raspass;
      GetGamerByNum(1)->GamesType = raspass;
      GetGamerByNum(2)->GamesType = raspass;
      GetGamerByNum(3)->GamesType = raspass;
      Repaint();
      emitRepaint();
    }
    nCurrentMove = nCurrentStart;
    for (int i = 1; i <= 10; i++) {
      Player *tmpg;
      CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard = NULL;
      if (CurrentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;
      if (CurrentGame == raspass && (i == 1 || i == 2)) {
        TCard *tmp4show;
        TCard *ptmp4rpass;
        tmp4show = (TCard *)Coloda->At(29+i);
        ptmp4rpass = new TCard(1, tmp4show->CMast);
        CardOnDesk[0] = tmp4show;
        ShowCard(0, CardOnDesk[0]);
        DeskView->mySleep(2);
        CardOnDesk[nCurrentMove.nValue] = FirstCard = ControlingMakemove((TCard*)NULL, ptmp4rpass);
        delete ptmp4rpass;
      } else {
        CardOnDesk[nCurrentMove.nValue] = FirstCard = ControlingMakemove((TCard*)NULL, (TCard*)NULL);
      }
      sprintf(ProtocolBuff, "First card:%i", Card2Int(FirstCard));
      WriteProtocol(ProtocolBuff);
      Repaint();
      emitRepaint();
      //!!!ShowCard(nCurrentMove.nValue, FirstCard);
      ++nCurrentMove;
      CardOnDesk[nCurrentMove.nValue] = SecondCard = ControlingMakemove((TCard*)NULL, FirstCard);
      sprintf(ProtocolBuff, "Second card:%i", Card2Int(SecondCard));
      WriteProtocol(ProtocolBuff);
      Repaint(nCurrentMove.nValue);
      emitRepaint();
      //!!!ShowCard(nCurrentMove.nValue,SecondCard);
      ++nCurrentMove;
      CardOnDesk[nCurrentMove.nValue] = TherdCard = ControlingMakemove(FirstCard, SecondCard);
      sprintf(ProtocolBuff,"Therd card :%i",Card2Int(TherdCard));
      WriteProtocol(ProtocolBuff);
      Repaint(nCurrentMove.nValue);
      emitRepaint();
      //!!!ShowCard(nCurrentMove.nValue,TherdCard);
      CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = NULL;
      ++nCurrentMove;
      DeskView->mySleep(3);
      nPl = nPlayerTakeCards(FirstCard, SecondCard, TherdCard, GamesType[0]-(GamesType[0]/10)*10)-1;
      nCurrentMove = nCurrentMove+nPl;
      tmpg = (Player *)Gamers->At(nCurrentMove.nValue);
      tmpg->nGetsCard++;
      CardOnDesk[0] = NULL;
      //DeskView->ClearScreen();
      Repaint();
      emitRepaint();
    }
    //DeskView->mySleep(2);
LabelRecordOnPaper:
    Repaint();
    emitRepaint();
    ++nCurrentStart;
    // записи по сдаче
    for (int i = 1; i<= 3;i++ ) {
      Player * tmpg = GetGamerByNum(i);
      Player * Gamer = GetGamerByNum(nGamernumber);
      int RetValAddRec = tmpg->aScore->AddRecords (CurrentGame,
                tmpg->GamesType ,
                Gamer !=NULL ? Gamer->nGetsCard : 0,
                tmpg->nGetsCard,
                Gamer !=NULL ? nGamernumber : 0,
                i,
        2-nPassCounter);

        if ( RetValAddRec ) {
            int index = GetGamerWithMaxBullet();
            if (index) {
              tmpg->aScore->AddVist(index,i,RetValAddRec) ; // этот на него висты
              RetValAddRec =   GetGamerByNum(index)->aScore->AddBullet(RetValAddRec); // этому в пулю
              if (RetValAddRec) {
                index = GetGamerWithMaxBullet();
                if (index) {
                  tmpg->aScore->AddVist(index,i,RetValAddRec) ; // этот на него висты
                  RetValAddRec =  GetGamerByNum(index)->aScore->AddBullet(RetValAddRec);
                  if (RetValAddRec) {
                    tmpg->aScore->MountanDown(RetValAddRec);
                  }
                } else {
                  tmpg->aScore->MountanDown(RetValAddRec);
                }
              }
            } else {
              tmpg->aScore->MountanDown(RetValAddRec);
            }
        }
    }
    CloseBullet();
    // если сетевая игра -  передаем на сервер результаты круга  и кто след. заходит

    // после игры - перевернуть карты и показать их
      for (int k = 1; k<=3;k++) {
        TCardList *tmplist;
        Player * Gamer = GetGamerByNum(k);
        tmplist = Gamer->aCards;
        Gamer->aCards = Gamer->aCardsOut;
        Gamer->nInvisibleHand = 0;
        Repaint(k);
        emitRepaint();
        Gamer->aCardsOut = Gamer->aCards;
        Gamer->aCards = tmplist;
      }
      DeskView->mySleep(2);
    // показать пулю
/*
      filename = new char[FILENAME_MAX];
      strcpy(filename,getenv("HOME"));
      strcat(filename,"/");
      strcat(filename,AUTOSAVENAME);
      SaveGame(filename);
      delete filename;
*/
    nflShowPaper = 1;
    ShowPaper();
    DeskView->mySleep(4);
    nflShowPaper = 0;
    DeskView->ClearScreen();
  } // конец пули
  ShowPaper();
  DeskView->mySleep(0);
}

//-------------------------------------------------------------
int TDeskTop::GetGamerWithMaxBullet(void) {
    int MaxBullet= -1,CurrBullet=-1,RetVal=0;
    for (int i =1 ;i<=3;i++) {
       CurrBullet = GetGamerByNum(i)->aScore->nGetBull();
        if (MaxBullet < CurrBullet && CurrBullet < nBuletScore ) {
          MaxBullet = CurrBullet;
          RetVal = i;
        }
    }
    return RetVal;
}
//-------------------------------------------------------------
/*
void TDeskTop::OnlyMessage (int  nGamer) {
  Player *tmpg = GetGamerByNum(nGamer);
  if (tmpg) {
    tmpg->DeskView = DeskView;
    switch (nGamer) {
      case 1: 
        tmpg->OnlyMessage(DeskView, (DeskView->DesktopWidht-DeskView->xLen)/2, DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight, DeskView->xLen, DeskView->yLen);
        break;
      case 2:
        tmpg->OnlyMessage(DeskView, DeskView->xBorder, DeskView->yBorder, DeskView->xLen, DeskView->yLen);
        break;
      case 3:
        tmpg->OnlyMessage(DeskView, DeskView->DesktopWidht/2+DeskView->xBorder, DeskView->yBorder, DeskView->xLen, DeskView->yLen);
        break;
      default: ;
    }
  }
  emitRepaint();
}
*/


void TDeskTop::Repaint (int nGamer) {
  Player *tmpg = GetGamerByNum(nGamer);
  if (!tmpg) return;
  tmpg->DeskView = DeskView;
  tmpg->RepaintSimple(!(tmpg->nInvisibleHand));
/*
  Player *tmpg = GetGamerByNum(nGamer);
  if (!tmpg) return;
  tmpg->DeskView = DeskView;
  switch (nGamer) {
    case 1:
      tmpg->Repaint(DeskView, (DeskView->DesktopWidht-DeskView->xLen)/2,
        DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight, DeskView->xLen,DeskView->yLen);
      break;
    case 2:
      tmpg->Repaint(DeskView, DeskView->xBorder, DeskView->yBorder, DeskView->xLen, DeskView->yLen);
      break;
    case 3:
      tmpg->Repaint(DeskView, DeskView->DesktopWidht/2+DeskView->xBorder,
        DeskView->yBorder, DeskView->xLen,DeskView->yLen);
    default: ;
  }
*/
/*
  Repaint(1);
  Repaint(2);
  Repaint(3);
*/
}


void TDeskTop::RepaintCardOnDesk () {
  for (int f = 0; f <= 3; f++) {
    if (CardOnDesk[f]) ShowCard(f, CardOnDesk[f]);
  }
}


void TDeskTop::Repaint (int left, int top, int right, int bottom) {
  Q_UNUSED(left)
  Q_UNUSED(right)
  Q_UNUSED(top)
  Q_UNUSED(bottom)
/*
  DeskView->xLen=DeskView->DesktopWidht/2-2*DeskView->xBorder;
  DeskView->yLen=DeskView->DesktopHeight/2-2*DeskView->yBorder;

  if (  ( ((DeskView->DesktopWidht-DeskView->xLen)/2)>=left && ((DeskView->DesktopWidht-DeskView->xLen)/2)<=right ) ||( (DeskView->xLen)>=left && (DeskView->xLen)<=right )   ) {
    Repaint(1);
  }
  if (  ((DeskView->xBorder) >=left && (DeskView->xBorder) <=right) ||((DeskView->xLen) >=left && (DeskView->xLen) <=right) ) {
    Repaint(2);
  }
  if (  ( (DeskView->DesktopWidht/2+DeskView->xBorder)>=left  && (DeskView->DesktopWidht/2+DeskView->xBorder)<=right )  ||(  (DeskView->xLen)>=left  && (DeskView->xLen)<=right     ) ) {
    Repaint(3);
  }
*/
  Repaint(1);
  Repaint(2);
  Repaint(3);
}


void TDeskTop::Repaint () {
  DeskView->ClearScreen();
/*
  DeskView->xLen = DeskView->DesktopWidht/2-2*DeskView->xBorder;
  DeskView->yLen = DeskView->DesktopHeight/2-2*DeskView->yBorder;
*/
  Repaint(1);
  Repaint(2);
  Repaint(3);
  RepaintCardOnDesk();
  if (nflShowPaper) ShowPaper();
}


int TDeskTop::nPlayerTakeCards (TCard *p1, TCard *p2, TCard *p3, int koz) {
  TCard *Max = p1;
  int nRelVal = 1;
  if ((Max->CMast == p2->CMast && Max->CName < p2->CName) || (Max->CMast != koz && p2->CMast == koz)) {
    Max = p2;
    nRelVal = 2;
  }
  if ((Max->CMast == p3->CMast && Max->CName < p3->CName) || (Max->CMast != koz && p3->CMast == koz)) {
    Max = p3;
    nRelVal = 3;
  }
  return nRelVal;
}


// draw ingame card (the card that is in game, not in hand)
void TDeskTop::ShowCard (int nGamer, TCard *card) {
  int w = DeskView->DesktopWidht/2, h = DeskView->DesktopHeight/2;
  int x = w, y = h;
  switch (nGamer) {
    case 0:
      x -= CARDWIDTH*2+8;
      y -= CARDHEIGHT/2;
      break;
    case 1:
      x -= CARDWIDTH/2;
      break;
    case 2:
      x -= CARDWIDTH;
      y -= CARDHEIGHT;
      break;
    case 3:
      y -= CARDHEIGHT;
      break;
    default: return;
  }
  DeskView->drawCard(card, x, y, 1, 0);
}


Player *TDeskTop::GetGamerByNum (int num) {
  return (Player *)Gamers->At(num);
}


/*
void TDeskTop::replace (char *str, char csource, char cdestination) {
  while (*str) {
    if (*str == csource) *str = cdestination;
    str++;
  }
}
*/


/*
void TDeskTop::insertscore (Tclist *list, char *pbuff) {
  int nVal;
  while ( sscanf(pbuff,"%i",&nVal) !=EOF  ) {
    if (nVal) list->Insert(new int (nVal));
    pbuff = strchr(pbuff,' ')+1;
  }
}
*/


int TDeskTop::LoadGame (const QString &name)  {
  Q_UNUSED(name)
  return 0;
/*
  if (name.isEmpty()) return 0;
  QByteArray ba(name.toUtf8());
    FILE *in;
    if ((in = fopen(ba.constData(), "r")) == NULL)    return 0;

    char *buff = new char[1024];
    for (int i = 1; i<=3; i++) {
        Player *Gamer = GetGamerByNum(i);
        Gamer->aScore->Bullet->FreeAll();
        Gamer->aScore->Mountan->FreeAll();
        Gamer->aScore->LeftVists->FreeAll();
        Gamer->aScore->RightVists->FreeAll();
        fgets(buff,1024-1,in);
        insertscore(Gamer->aScore->Bullet,buff);
        fgets(buff,1024-1,in);
        insertscore(Gamer->aScore->Mountan,buff);
        fgets(buff,1024-1,in);
        insertscore(Gamer->aScore->LeftVists,buff);
        fgets(buff,1024-1,in);
        insertscore(Gamer->aScore->RightVists,buff);
    }
    fgets(buff,1024-1,in);
    sscanf(buff,"%i",&nCurrentStart.nValue);
    fgets(buff,1024-1,in);
    sscanf(buff,"%i",&nBuletScore);
    fgets(buff,1024-1,in);
    sscanf(buff,"%i",&g61stalingrad);
    fgets(buff,1024-1,in);
    sscanf(buff,"%i",&g10vist);
    fgets(buff,1024-1,in);
    sscanf(buff,"%i",&globvist);
    fclose(in);
    delete buff;
    return 1;
*/
}


int TDeskTop::SaveGame (const QString &name)  {
  Q_UNUSED(name)
  return 0;
/*
  if (name.isEmpty()) return 0;
  QByteArray ba(name.toUtf8());
    FILE *out;
    if ((out = fopen(ba.constData(), "w")) == NULL)    return 0;
    char *sb = new char[1024];
    char *sm = new char[1024];
    char *slv = new char[1024];
    char *srv = new char[1024];
    for (int i = 1;i<=3;i++) {
      Player *Gamer = GetGamerByNum(i);
      sb = Tclist2pchar ( Gamer->aScore->Bullet,sb);
      replace(sb,'.',' ');
      sm = Tclist2pchar (Gamer->aScore->Mountan,sm);
      replace(sm,'.',' ');
      slv = Tclist2pchar(Gamer->aScore->LeftVists,slv);
      replace(slv,'.',' ');
      srv = Tclist2pchar(Gamer->aScore->RightVists,srv);
      replace(srv,'.',' ');
      fprintf(out,"%s\n",sb);
      fprintf(out,"%s\n",sm);
      fprintf(out,"%s\n",slv);
      fprintf(out,"%s\n",srv);
    }
    fprintf(out,"%i\n",nCurrentStart.nValue);
    fprintf(out,"%i\n",nBuletScore);
    fprintf(out,"%i\n",g61stalingrad);
    fprintf(out,"%i\n",g10vist);
    fprintf(out,"%i\n",globvist);
    fclose(out);
    delete sb;
    delete sm;
    delete slv;
    delete srv;
    return 1;
*/
}


void TDeskTop::ShowPaper () {
  QString sb, sm, slv, srv, tv;
  DeskView->ShowBlankPaper(nBuletScore);
  for (int i = 1;i<=3;i++) {
    Player *Gamer = GetGamerByNum(i);
    sb = Tclist2pchar(Gamer->aScore->Bullet);
    sm = Tclist2pchar(Gamer->aScore->Mountan);
    slv = Tclist2pchar(Gamer->aScore->LeftVists);
    srv = Tclist2pchar(Gamer->aScore->RightVists);
    tv = QString::number(Gamer->aScore->Vists);
    DeskView->showPlayerScore(i, sb, sm, slv, srv, tv);
  }
}
