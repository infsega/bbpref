#include "prfconst.h"

#include <QMainWindow>
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
  Coloda = new Deck(CARDINCOLODA);
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
  mPlayingRound = false;
  InternalConstruct();
  DeskView = _DeskView;
}


TDeskTop::TDeskTop () : QObject(0) {
  mPlayingRound = false;
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


Card *TDeskTop::PipeMakemove (Card *lMove, Card *rMove) {
  return (GetGamerByNum(nCurrentMove.nValue))->
    makemove(lMove, rMove, GetGamerByNum(NextGamer(nCurrentMove)), GetGamerByNum(PrevGamer(nCurrentMove)));
}


// ежели номер не 1 и игра пас или (ловля мизера и игрок не сетевой),
// то пораждаем нового HumanPlayer с номером текущего
// ежели номер 1 и игра пас и не сетевая игра то порождаем Player
Card *TDeskTop::ControlingMakemove (Card *lMove, Card *rMove) {
  Card *RetVal = NULL;
  Player *Now = GetGamerByNum(nCurrentMove.nValue);

  if ((GetGamerByNum(1)->GamesType == vist || GetGamerByNum(1)->GamesType == g86catch) &&
      Now->mPlayerNo != 1 && (Now->GamesType == gtPass || Now->GamesType == g86catch)) {
    HumanPlayer *NewHuman = new HumanPlayer(nCurrentMove.nValue, DeskView);
    GamerAssign(NewHuman, Now);
    Gamers->AtPut(nCurrentMove.nValue, NewHuman);
    RetVal = NewHuman->makemove(lMove, rMove, GetGamerByNum(NextGamer(nCurrentMove)), GetGamerByNum(PrevGamer(nCurrentMove)));
    GamerAssign(Now, NewHuman);
    Gamers->AtPut(nCurrentMove.nValue, Now);
    NewHuman->clear();
    delete NewHuman;
  } else if (Now->mPlayerNo == 1 && Now->GamesType == gtPass) {
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
  //Card *FirstCard, *SecondCard, *TherdCard;
  int npasscounter;
  //DeskView->ClearScreen();
  if (flProtocol) OpenProtocol();
  // while !конец пули
  while (!(GetGamerByNum(1)->aScore->nGetBull() >= nBuletScore &&
           GetGamerByNum(2)->aScore->nGetBull() >= nBuletScore &&
           GetGamerByNum(3)->aScore->nGetBull() >= nBuletScore)) {
    mPlayingRound = false;
    Tncounter GamersCounter(1, 1, 3);
    int nPl;
    int nGamernumber = 0; // номер заказавшего игру
    int nPassCounter = 0; // кол-во спасовавших
    GamesType[3] = GamesType[2] = GamesType[1] = GamesType[0] = undefined;
    CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard = 0;
    delete Coloda;
    Coloda = new Deck(CARDINCOLODA);
    Coloda->shuffle();
    GetGamerByNum(1)->clear();
    GetGamerByNum(2)->clear();
    GetGamerByNum(3)->clear();
    CurrentGame = undefined;
    nflShowPaper = 0;
    // сдали карты
    for (int i = 0; i < CARDINCOLODA-2; i++) {
      Player *tmpGamer = GetGamerByNum(GamersCounter.nValue);
      if (!(Coloda->At(i))) DeskView->MessageBox("Card = NULL", "Error!!!");
      tmpGamer->AddCard((Card *)Coloda->At(i));
      ++GamersCounter;
    }
    Repaint();
    //emitRepaint();
    DeskView->mySleep(0);
    GamersCounter = nCurrentStart;

    sprintf(ProtocolBuff, "Current start:%i", nCurrentStart.nValue);
    WriteProtocol(ProtocolBuff);

    // пошла торговля
    npasscounter = 0;
    while (npasscounter < 2) {
      // пока как минимум двое не спасовали
      if (GamesType[1] != gtPass) GamesType[1] = GetGamerByNum(GamersCounter.nValue)->makemove(GamesType[2], GamesType[3]);
      sprintf(ProtocolBuff, "Gamer 1:%i say: %s", GamersCounter.nValue, sGameName(GamesType[1]));
      WriteProtocol(ProtocolBuff);
      Repaint();
      //emitRepaint();

      ++GamersCounter;
      ENDOFTORG

      if (GamesType[2] != gtPass) GamesType[2] = GetGamerByNum(GamersCounter.nValue)->makemove(GamesType[3], GamesType[1]);
      sprintf(ProtocolBuff, "Gamer 2:%i say: %s", GamersCounter.nValue, sGameName(GamesType[1]));
      WriteProtocol(ProtocolBuff);
      Repaint();
      //emitRepaint();

      ++GamersCounter;
      ENDOFTORG

      if (GamesType[3] != gtPass) GamesType[3] = GetGamerByNum(GamersCounter.nValue)->makemove(GamesType[1], GamesType[2]);
      sprintf(ProtocolBuff, "Gamer 3:%i say: %s", GamersCounter.nValue, sGameName(GamesType[1]));
      WriteProtocol(ProtocolBuff);
      Repaint();
      //emitRepaint();

      ++GamersCounter;
      ENDOFTORG
    }

    // выч. максим игру
    for (int i = 1; i <= 3; i++) {
      if (GamesType[0] < GamesType[i]) GamesType[0] = GamesType[i];
    }

    mPlayerActive = 0;
    mPlayingRound = true;
    if (GamesType[0] != gtPass) {
      // не расспасы
      // узнаем кто назначил максимальную игру
      for (int i = 1; i <= 3; i++) {
        Player *tmpg = GetGamerByNum(i);
        if (tmpg->GamesType == GamesType[0]) {
          mPlayerActive = i;
          nPassCounter = 0;
          // показываем прикуп
          Tncounter tmpGamersCounter(1, 3);
          Player *PassOrVistGamers;
          int PassOrVist = 0, nPassOrVist = 0;
          CardOnDesk[2] = (Card *)Coloda->At(30);
          CardOnDesk[3] = (Card *)Coloda->At(31);
          sprintf(ProtocolBuff, "Get cards %i %i for %i game", Card2Int(CardOnDesk[2]), Card2Int(CardOnDesk[3]), GamesType[0]);
          WriteProtocol(ProtocolBuff);
          // извращение с CurrentGame -- для того, чтобы показало игру на bidboard
          //!tGameBid oc = CurrentGame;
          CurrentGame = GamesType[0];
          Repaint();
          //emitRepaint();
          DeskView->mySleep(-1);
          //!CurrentGame = oc; // вернём CurrentGame на место -- на всякий случай
          // запихиваем ему прикуп
          tmpg->AddCard((Card *)Coloda->At(30));
          tmpg->AddCard((Card *)Coloda->At(31));
          CardOnDesk[2] = CardOnDesk[3] = NULL;
          nGamernumber = tmpg->mPlayerNo;
          //DeskView->ClearScreen();
          Repaint();
          //emitRepaint();
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
            //emitRepaint();
            if (tmpg->mPlayerNo != 1) DeskView->MessageBox("Try to remember the cards", "Message");
            // ждать до события
            DeskView->mySleep(-1);
            tmpg->nInvisibleHand = nVisibleState;
            nCurrentMove.nValue = tmpg->mPlayerNo;
            GamesType[0] = CurrentGame = tmpg->makeout4miser();
            nCurrentMove.nValue = tempint;
          }
          tmpGamersCounter.nValue = i;
          //DeskView->ClearScreen();
          //Repaint();
          //emitRepaint();
          // пас или вист
          ++tmpGamersCounter;
          PassOrVistGamers = GetGamerByNum(tmpGamersCounter.nValue);
          PassOrVistGamers->GamesType = undefined;
          Repaint();
          //emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          DeskView->mySleep(2);
          PassOrVist = PassOrVistGamers->makemove(CurrentGame, gtPass, 0);
          nPassOrVist = tmpGamersCounter.nValue;
          if (PassOrVistGamers->GamesType == gtPass) nPassCounter++;
          Repaint();
          //emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          DeskView->mySleep(1);
          ++tmpGamersCounter;
          PassOrVistGamers = GetGamerByNum(tmpGamersCounter.nValue);
          PassOrVistGamers->GamesType = undefined;
          //Repaint(tmpGamersCounter.nValue);
          Repaint();
          //emitRepaint();
          DeskView->mySleep(1);
          PassOrVistGamers->makemove(CurrentGame, PassOrVist, nPassOrVist);
          if (PassOrVistGamers->GamesType == gtPass) nPassCounter++;
          Repaint();
          //emitRepaint();
          DeskView->mySleep(1);
          if (nPassCounter == 2) {
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
            //emitRepaint();
          }
          break;
        }
      } // узнаем кто назначил максимальную игру
    }  else {
      // раскручиваем распас
      mPlayerActive = 0;
      GamesType[0] = CurrentGame = raspass;
      GetGamerByNum(1)->GamesType = raspass;
      GetGamerByNum(2)->GamesType = raspass;
      GetGamerByNum(3)->GamesType = raspass;
      //Repaint();
      //emitRepaint();
    }
    // партия (10 ходов)
    nCurrentMove = nCurrentStart;
    //mPlayingRound = true;
    for (int i = 1; i <= 10; i++) {
      Player *tmpg;
      CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard = NULL;
      if (CurrentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;
      if (CurrentGame == raspass && (i == 1 || i == 2)) {
        Card *tmp4show;
        Card *ptmp4rpass;
        tmp4show = (Card *)Coloda->At(29+i);
        ptmp4rpass = new Card(1, tmp4show->CMast);
        CardOnDesk[0] = tmp4show;
        //drawInGameCard(0, CardOnDesk[0]);
        Repaint();
        DeskView->mySleep(-1);
        CardOnDesk[nCurrentMove.nValue] = FirstCard = ControlingMakemove(0, ptmp4rpass);
        delete ptmp4rpass;
      } else {
        CardOnDesk[nCurrentMove.nValue] = FirstCard = ControlingMakemove(0, 0);
      }
      sprintf(ProtocolBuff, "First card:%i", Card2Int(FirstCard));
      WriteProtocol(ProtocolBuff);
      Repaint();
      //emitRepaint();
      //!!!drawInGameCard(nCurrentMove.nValue, FirstCard);
      ++nCurrentMove;
      CardOnDesk[nCurrentMove.nValue] = SecondCard = ControlingMakemove(0, FirstCard);
      sprintf(ProtocolBuff, "Second card:%i", Card2Int(SecondCard));
      WriteProtocol(ProtocolBuff);
      //Repaint(nCurrentMove.nValue);
      Repaint();
      //!!!drawInGameCard(nCurrentMove.nValue,SecondCard);
      ++nCurrentMove;
      CardOnDesk[nCurrentMove.nValue] = TherdCard = ControlingMakemove(FirstCard, SecondCard);
      sprintf(ProtocolBuff,"Therd card :%i",Card2Int(TherdCard));
      WriteProtocol(ProtocolBuff);
      //Repaint(nCurrentMove.nValue);
      Repaint();
      //!!!drawInGameCard(nCurrentMove.nValue,TherdCard);
      CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = NULL;
      ++nCurrentMove;
      DeskView->mySleep(-1);
      nPl = nPlayerTakeCards(FirstCard, SecondCard, TherdCard, GamesType[0]-(GamesType[0]/10)*10)-1;
      nCurrentMove = nCurrentMove+nPl;
      tmpg = (Player *)Gamers->At(nCurrentMove.nValue);
      tmpg->nGetsCard++;
      CardOnDesk[0] = NULL;
      //DeskView->ClearScreen();
      Repaint();
      //emitRepaint();
    }
    //DeskView->mySleep(2);
LabelRecordOnPaper:
    mPlayingRound = false;
    // партия закончена
    //Repaint();
    //emitRepaint();
    ++nCurrentStart;
    // записи по сдаче
    for (int i = 1; i <= 3;i++ ) {
      Player *tmpg = GetGamerByNum(i);
      Player *Gamer = GetGamerByNum(nGamernumber);
      int RetValAddRec = tmpg->aScore->AddRecords(CurrentGame,
                tmpg->GamesType,
                Gamer !=NULL ? Gamer->nGetsCard : 0,
                tmpg->nGetsCard,
                Gamer !=NULL ? nGamernumber : 0,
                i,
        2-nPassCounter);

      if (RetValAddRec) {
        int index = GetGamerWithMaxBullet();
        if (index) {
          tmpg->aScore->AddVist(index,i,RetValAddRec); // на этого висты
          RetValAddRec = GetGamerByNum(index)->aScore->AddBullet(RetValAddRec); // этому в пулю
          if (RetValAddRec) {
            index = GetGamerWithMaxBullet();
            if (index) {
              tmpg->aScore->AddVist(index, i, RetValAddRec); // на этого висты
              RetValAddRec = GetGamerByNum(index)->aScore->AddBullet(RetValAddRec);
              if (RetValAddRec) tmpg->aScore->MountanDown(RetValAddRec);
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
    CardList *tmplist[3];
    for (int f = 1; f <= 3; f++) {
      Player *plr = GetGamerByNum(f);
      tmplist[f] = 0;
      if (!plr) continue;
      if (nPassCounter != 2) {
        // была партия
        tmplist[f] = plr->aCards;
        plr->aCards = plr->aCardsOut;
      }
      plr->nInvisibleHand = false;
    }
    nflShowPaper = 1;
    mPlayingRound = true;
    Repaint();
    DeskView->mySleep(-1);
    mPlayingRound = false;
    if (nPassCounter != 2) {
      // была партия
      for (int f = 1; f <= 3; f++) {
        Player *plr = GetGamerByNum(f);
        if (!plr) continue;
        plr->aCardsOut = plr->aCards;
        plr->aCards = tmplist[f];
      }
    }
  } // конец пули
  nflShowPaper = 1;
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


int TDeskTop::nPlayerTakeCards (Card *p1, Card *p2, Card *p3, int koz) {
  Card *Max = p1;
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


// draw ingame card (the card that is in game, not in hand)
void TDeskTop::drawInGameCard (int mPlayerNo, Card *card) {
  if (!card) return;
  int w = DeskView->DesktopWidht/2, h = DeskView->DesktopHeight/2;
  int x = w, y = h;
  switch (mPlayerNo) {
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


void TDeskTop::Repaint () {
  if (!DeskView) return;
  DeskView->ClearScreen();
  // repaint players
  for (int f = 1; f <= 3; f++) {
    Player *plr = GetGamerByNum(f);
    if (plr) {
      plr->DeskView = DeskView;
      plr->Repaint();
    }
  }
  // repaint in-game cards
  for (int f = 0; f <= 3; f++) {
    if (CardOnDesk[f]) drawInGameCard(f, CardOnDesk[f]);
  }
  // draw bidboard
  if (mPlayingRound) {
    Player *plr1 = GetGamerByNum(1);
    Player *plr2 = GetGamerByNum(2);
    Player *plr3 = GetGamerByNum(3);
    if (plr1 && plr2 && plr3) {
      DeskView->drawBidsBmp(mPlayerActive, plr1->nGetsCard, plr2->nGetsCard, plr3->nGetsCard, CurrentGame);
      //DeskView->drawBidsBmp(int plrAct, int p0t, int p1t, int p2t, tGameBid game) {
    }
  }
  // repaint scoreboard
  if (nflShowPaper) ShowPaper();
  emitRepaint();
}
