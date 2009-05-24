#include "prfconst.h"

#include <QMainWindow>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "deskview.h"
#include "ncounter.h"
#include "kpref.h"
#include "player.h"
#include "desktop.h"
#include "plscore.h"
#include "human.h"


typedef struct {
  int mount, pool, leftWh, rightWh;
} tScores;


void PrefDesktop::InternalConstruct () {
  CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard=0;
  deck = new Deck;

  qsrand(time(NULL));
  nCurrentStart.nValue=nCurrentMove.nValue=(qrand()%3)+1;
  nCurrentStart.nMin=nCurrentMove.nMin=1;
  nCurrentStart.nMax=nCurrentMove.nMax=3;

  //!InsertGamer(new Player(0));
  mPlayers << 0; // 0th player is nobody
  //InsertGamer(new Player(1));
  // I Hate This Game :)
  InsertGamer(new HumanPlayer(1, DeskView));
  InsertGamer(new Player(2, DeskView));
  InsertGamer(new Player(3, DeskView));
  //mPlayers->AtFree(0);
  nflShowPaper = 0;
  optMaxPool = 21;
  flProtocol = 0;
  iMoveX = iMoveY = -1;
}


PrefDesktop::PrefDesktop (TDeskView *_DeskView) : QObject(0) {
  mPlayingRound = false;
  InternalConstruct();
  DeskView = _DeskView;
}


PrefDesktop::PrefDesktop () : QObject(0) {
  mPlayingRound = false;
  InternalConstruct();
}


PrefDesktop::~PrefDesktop () {
  CloseProtocol();
  delete deck;
  foreach (Player *p, mPlayers) if (p) delete p;
  mPlayers.clear();
}


void PrefDesktop::emitRepaint () {
  emit deskChanged();
}


int PrefDesktop::CloseProtocol () {
  if (flProtocol) {
    t = time(NULL);
    tblock = localtime(&t);
    fprintf(ProtocolFile,"\nClose protocol %s", asctime(tblock));
    fclose(ProtocolFile);
  }
  return 1;
}


int PrefDesktop::WriteProtocol (const char *line) {
  if (flProtocol) fprintf(ProtocolFile, "\n %s", line);
  return 1;
}


int PrefDesktop::OpenProtocol () {
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


void PrefDesktop::CloseBullet () {
  Tncounter counter(1, 1, 3);
  int mb = INT_MAX, mm = INT_MAX, i;
  tScores R[4];
  Player *G;
  for (i = 1; i <= 3; i++) {
    G = player(i);
    R[i].mount = G->aScore->mountain();
    R[i].pool = G->aScore->pool();
    R[i].leftWh = G->aScore->leftWhists();
    R[i].rightWh = G->aScore->rightWhists();
    if (R[i].mount < mm) mm = R[i].mount;
    if (R[i].pool < mb) mb = R[i].pool;
  }
  // Amnistiya gori
  for (i = 1; i <= 3; i++) R[i].mount -= mm;
  // Amnistiya puli
  for (i = 1; i <= 3; i++) R[i].pool -= mb;
  // svou pulu sebe v visti
  for (i=1;i<=3;i++) {
    //R[i].leftWh += R[i].pool*10/2;
    //R[i].rightWh += R[i].pool*10/2;
    R[i].leftWh += R[i].pool*10/3;
    R[i].rightWh += R[i].pool*10/3;
  }
  // goru - drugim na sebya
  for (i = 1; i <= 3; i++) {
/*
    counter.nValue = i;
    ++counter;
    R[counter.nValue].rightWh += R[i].mount*10/2;
    ++counter;
    R[counter.nValue].leftWh += R[i].mount*10/2;
*/
    counter.nValue = i;
    ++counter;
    R[counter.nValue].rightWh += R[i].mount*10/3;
    ++counter;
    R[counter.nValue].leftWh += R[i].mount*10/3;
  }
  // schitaem visti
  for (i = 1; i <= 3; i++) {
    int i1, i2;
    counter.nValue = i;
    ++counter;
    i1 = counter.nValue;
    ++counter;
    i2 = counter.nValue;
    player(i)->aScore->setWhists(R[i].leftWh + R[i].rightWh - R[i1].rightWh - R[i2].leftWh);
  }
}


Player *PrefDesktop::InsertGamer (Player *Gamer) {
  mPlayers << Gamer;
  return Gamer;
}


void PrefDesktop::GamerAssign (Player *newgamer,Player *oldgamer) {
  newgamer->aCards->shallowCopy(oldgamer->aCards);
  newgamer->aLeft->shallowCopy(oldgamer->aLeft);
  newgamer->aRight->shallowCopy(oldgamer->aRight);
  newgamer->aOut->shallowCopy(oldgamer->aOut);
  newgamer->aCardsOut->shallowCopy(oldgamer->aCardsOut);
  newgamer->aLeftOut->shallowCopy(oldgamer->aLeftOut);
  newgamer->aRightOut->shallowCopy(oldgamer->aRightOut);
  newgamer->nGetsCard = oldgamer->nGetsCard;
  newgamer->Mast = oldgamer->Mast;
  newgamer->GamesType = oldgamer->GamesType;
  newgamer->Enemy = oldgamer->Enemy;
  newgamer->nInvisibleHand = oldgamer->nInvisibleHand;
  newgamer->DeskView = oldgamer->DeskView;
  newgamer->Pronesti = oldgamer->Pronesti;
}


Card *PrefDesktop::PipeMakemove (Card *lMove, Card *rMove) {
  return (player(nCurrentMove.nValue))->
    makemove(lMove, rMove, player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)));
}


// ежели номер не 1 и игра пас или (ловля мизера и игрок не сетевой),
// то пораждаем нового HumanPlayer с номером текущего
// ежели номер 1 и игра пас и не сетевая игра то порождаем Player
Card *PrefDesktop::ControlingMakemove (Card *lMove, Card *rMove) {
  Card *RetVal = 0;
  Player *Now = player(nCurrentMove.nValue);

  if ((player(1)->GamesType == vist || player(1)->GamesType == g86catch) &&
      Now->mPlayerNo != 1 && (Now->GamesType == gtPass || Now->GamesType == g86catch)) {
    HumanPlayer *NewHuman = new HumanPlayer(nCurrentMove.nValue, DeskView);
    GamerAssign(NewHuman, Now);
    mPlayers[nCurrentMove.nValue] = NewHuman;
    RetVal = NewHuman->makemove(lMove, rMove, player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)));
    GamerAssign(Now, NewHuman);
    mPlayers[nCurrentMove.nValue] = Now;
    NewHuman->clear();
    delete NewHuman;
  } else if (Now->mPlayerNo == 1 && Now->GamesType == gtPass) {
    Player *NewHuman = new Player(nCurrentMove.nValue, DeskView);
    GamerAssign(NewHuman, Now);
    mPlayers[nCurrentMove.nValue] = NewHuman;
    RetVal = NewHuman->makemove(lMove, rMove, player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)));
    GamerAssign(Now, NewHuman);
    mPlayers[nCurrentMove.nValue] = Now;
    NewHuman->clear();
    delete NewHuman;
  } else {
    RetVal = PipeMakemove(lMove, rMove);
  }
  return RetVal;
}


void PrefDesktop::getPMsgXY (int plr, int *x, int *y) {
  switch (plr) {
    case 1:
      *x = -666;
      *y = -(DeskView->yBorder+CARDHEIGHT+50);
      break;
    case 2:
      *x = 30;
      *y = 30;
      break;
    case 3:
      *x = -30;
      *y = 30;
      break;
    default:
      *x = -666;
      *y = -666;
      break;
  }
}


void PrefDesktop::drawBidWindows (const eGameBid *bids, int curPlr) {
  Repaint(false);
  for (int f = 1; f <= 3; f++) {
    int x, y;
    getPMsgXY(f, &x, &y);
    if (f != curPlr) {
      if (bids[f] != undefined) {
        DeskView->drawMessageWindow(x, y, sGameName(bids[f]), true);
      }
    } else DeskView->drawMessageWindow(x, y, QString("thinking..."));
  }
  //emitRepaint();
  //if (curPlr != 3) DeskView->mySleep(2); else emitRepaint();
}


void PrefDesktop::RunGame () {
  eGameBid GamesType[4], bids4win[4];
  //char *filename;
  //Card *FirstCard, *SecondCard, *TherdCard;
  int npasscounter;

  //DeskView->ClearScreen();
  if (flProtocol) OpenProtocol();
  // while !конец пули
  qsrand(time(NULL));
  while (!(player(1)->aScore->pool() >= optMaxPool &&
           player(2)->aScore->pool() >= optMaxPool &&
           player(3)->aScore->pool() >= optMaxPool)) {
    Tncounter GamersCounter(1, 1, 3);
    mPlayingRound = false;
    int nPl;
    int nGamernumber = 0; // номер заказавшего игру
    int nPassCounter = 0; // кол-во спасовавших
    GamesType[3] = GamesType[2] = GamesType[1] = GamesType[0] = undefined;
    CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard = 0;
    delete deck;
    deck = new Deck;
    deck->shuffle();
    player(1)->clear();
    player(2)->clear();
    player(3)->clear();
    CurrentGame = undefined;
    nflShowPaper = 0;
    // сдаём карты
    for (int i = 0; i < CARDINCOLODA-2; i++) {
      Player *tmpGamer = player(GamersCounter.nValue);
      if (!(deck->at(i))) DeskView->MessageBox("Card = 0", "Error!!!");
      tmpGamer->AddCard(deck->at(i));
      ++GamersCounter;
    }
/*
    for (int f = 0; f < 2; f++) {
      Player *plr = player(f);
      plr->sortCards();
    }
*/
    //emitRepaint();
    //DeskView->mySleep(0);
    GamersCounter = nCurrentStart;
    switch (nCurrentStart.nValue) {
      case 1:
        iMoveX = DeskView->DesktopWidth/2-15;
        iMoveY = DeskView->DesktopHeight-DeskView->yBorder-CARDHEIGHT-40;
        break;
      case 2:
        iMoveX = DeskView->xBorder+20;
        iMoveY = DeskView->yBorder+CARDHEIGHT+40;
        break;
      case 3:
        iMoveX = DeskView->DesktopWidth-DeskView->xBorder-20;
        iMoveY = DeskView->yBorder+CARDHEIGHT+40;
        break;
      default: iMoveX = iMoveY = -1; break;
    }
    Repaint();

    sprintf(ProtocolBuff, "Current start:%i", nCurrentStart.nValue);
    WriteProtocol(ProtocolBuff);

    // пошла торговля
    npasscounter = 0;
    bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
    int curBidIdx = 0;
    //drawBidWindows(bids4win, GamersCounter.nValue);
    //if (GamersCounter.nValue != 1) DeskView->mySleep(2); else emitRepaint();
    while (npasscounter < 2) {
      // пока как минимум двое не спасовали
      int p = GamersCounter.nValue;
      drawBidWindows(bids4win, p);
      if (p != 1) DeskView->mySleep(2); else emitRepaint();

      if (GamesType[curBidIdx+1] != gtPass) {
        GamesType[curBidIdx+1] = player(p)->makemove(GamesType[((curBidIdx+1)%3)+1], GamesType[((curBidIdx+2)%3)+1]);
      }
      bids4win[p] = GamesType[curBidIdx+1];
      ++GamersCounter;
      curBidIdx = (curBidIdx+1)%3;

      if ((GamesType[1] != undefined && GamesType[2] != undefined && GamesType[3] != undefined) &&
          ((GamesType[1] == gtPass?1:0)+(GamesType[2] == gtPass?1:0)+(GamesType[3] == gtPass?1:0) >= 2)) break;
    }

    // выч. максим игру
    for (int i = 1; i <= 3; i++) {
      if (GamesType[0] < GamesType[i]) GamesType[0] = GamesType[i];
    }

    // поехали совать прикуп и выбирать финальную ставку (ну, или ничего, если распасы)
    mPlayerActive = 0;
    mPlayingRound = true;
    if (GamesType[0] != gtPass) {
      // не расспасы
      // узнаем кто назначил максимальную игру
      for (int i = 1; i <= 3; i++) {
        Player *tmpg = player(i);
        //tmpg->sortCards();
        if (tmpg->GamesType == GamesType[0]) {
          mPlayerActive = i;
          nPassCounter = 0;
          // показываем прикуп
          Tncounter tmpGamersCounter(1, 3);
          Player *PassOrVistGamers;
          int PassOrVist = 0, nPassOrVist = 0;
          CardOnDesk[2] = deck->at(30);
          CardOnDesk[3] = deck->at(31);
          //!.!sprintf(ProtocolBuff, "Get cards %i %i for %i game", Card2Int(CardOnDesk[2]), Card2Int(CardOnDesk[3]), GamesType[0]);
          //!.!WriteProtocol(ProtocolBuff);
          // извращение с CurrentGame -- для того, чтобы показало игру на bidboard
          //!eGameBid oc = CurrentGame;
          CurrentGame = GamesType[0];
          drawBidWindows(bids4win, 0);
          DeskView->mySleep(-1);
          //!CurrentGame = oc; // вернём CurrentGame на место -- на всякий случай
          // запихиваем ему прикуп
          tmpg->AddCard(deck->at(30));
          tmpg->AddCard(deck->at(31));
          CardOnDesk[2] = CardOnDesk[3] = 0;
          nGamernumber = tmpg->mPlayerNo;
          //DeskView->ClearScreen();
          //tmpg->sortCards();
          Repaint();
          //emitRepaint();
          //DeskView->mySleep(2);

          bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
          // снос
          if (tmpg->GamesType != g86) { // не мизер
            nCurrentMove.nValue = i;
            Repaint(false);
            if (mPlayerActive == 1) {
              int x, y;
              getPMsgXY(mPlayerActive, &x, &y);
              DeskView->drawMessageWindow(x, y, "Select cards to drop");
            }
            GamesType[0] = CurrentGame = tmpg->makeout4game();
          } else {
            // показать все карты
            int tempint = nCurrentMove.nValue;
            int nVisibleState = tmpg->nInvisibleHand;
            //tmpg->nCardsVisible = 0;
            tmpg->nInvisibleHand = 0;
            Repaint();
            //emitRepaint();
            //if (tmpg->mPlayerNo != 1) DeskView->MessageBox("Try to remember the cards", "Message");
            if (tmpg->mPlayerNo != 1) {
              //DeskView->MessageBox("Try to remember the cards", "Message");
              int x, y;
              getPMsgXY(1, &x, &y);
              DeskView->drawMessageWindow(x, y, "Try to remember the cards");
            }
            // ждать до события
            DeskView->mySleep(-1);
            Repaint(false);
            if (mPlayerActive == 1) {
              int x, y;
              getPMsgXY(mPlayerActive, &x, &y);
              DeskView->drawMessageWindow(x, y, "Select cards to drop");
            }
            tmpg->nInvisibleHand = nVisibleState;
            nCurrentMove.nValue = tmpg->mPlayerNo;
            GamesType[0] = CurrentGame = tmpg->makeout4miser();
            nCurrentMove.nValue = tempint;
          }
          tmpGamersCounter.nValue = i;
          //DeskView->ClearScreen();
          //Repaint();
          //emitRepaint();

          // попёрли выбирать: пас или вист?
          ++tmpGamersCounter;
          PassOrVistGamers = player(tmpGamersCounter.nValue);
          PassOrVistGamers->GamesType = undefined;
          //PassOrVistGamers->sortCards();
          Repaint();
          //emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          //DeskView->mySleep(2);

          // выбирает первый
          int firstPW = tmpGamersCounter.nValue;
          { // рисуем ставку
            int x, y;
            getPMsgXY(mPlayerActive, &x, &y);
            DeskView->drawMessageWindow(x, y, sGameName(CurrentGame), true);
          }
          { // рисуем думалку
            int x, y;
            getPMsgXY(firstPW, &x, &y);
            DeskView->drawMessageWindow(x, y, "thinking...");
            if (firstPW != 1) DeskView->mySleep(2);
          }
          PassOrVist = PassOrVistGamers->makemove(CurrentGame, gtPass, 0);
          nPassOrVist = tmpGamersCounter.nValue;
          if (PassOrVistGamers->GamesType == gtPass) nPassCounter++;
          bids4win[1] = PassOrVistGamers->GamesType;
          //emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          //DeskView->mySleep(1);

          // выбирает второй
          Repaint(false);
          ++tmpGamersCounter;
          int nextPW = tmpGamersCounter.nValue;
          { // рисуем ставку
            int x, y;
            getPMsgXY(mPlayerActive, &x, &y);
            DeskView->drawMessageWindow(x, y, sGameName(CurrentGame), true);
          }
          { // рисуем выбор
            int x, y;
            getPMsgXY(firstPW, &x, &y);
            DeskView->drawMessageWindow(x, y, sGameName(bids4win[1]), true);
          }
          { // рисуем думалку
            int x, y;
            getPMsgXY(nextPW, &x, &y);
            DeskView->drawMessageWindow(x, y, "thinking...");
            if (nextPW != 1) DeskView->mySleep(2);
          }
          PassOrVistGamers = player(tmpGamersCounter.nValue);
          PassOrVistGamers->GamesType = undefined;
          //Repaint(tmpGamersCounter.nValue);
          //Repaint();
          //emitRepaint();
          //DeskView->mySleep(1);
          PassOrVistGamers->makemove(CurrentGame, PassOrVist, nPassOrVist);
          if (PassOrVistGamers->GamesType == gtPass) nPassCounter++;
          bids4win[2] = PassOrVistGamers->GamesType;
          Repaint();
          //emitRepaint();
          //DeskView->mySleep(1);

          // всё, рисуем финалы
          { // рисуем ставку
            int x, y;
            getPMsgXY(mPlayerActive, &x, &y);
            DeskView->drawMessageWindow(x, y, sGameName(CurrentGame), true);
          }
          { // рисуем выбор
            int x, y;
            getPMsgXY(firstPW, &x, &y);
            DeskView->drawMessageWindow(x, y, sGameName(bids4win[1]), true);
          }
          { // рисуем выбор
            int x, y;
            getPMsgXY(nextPW, &x, &y);
            DeskView->drawMessageWindow(x, y, sGameName(bids4win[2]), true);
          }
          /*if (nextPW != 1) */DeskView->mySleep(-1);

          if (nPassCounter == 2) {
            // двое спасовали :)
            tmpg->nGetsCard = gameTricks(tmpg->GamesType);
            sprintf(ProtocolBuff, "Two gamers said 'pass'");
            WriteProtocol(ProtocolBuff);
            goto LabelRecordOnPaper;
          } else {
            // Открытые или закрытые карты
            for (int ntmp = 2 ; ntmp <= 3 ; ntmp++) {
              Player *Gamer4Open;
              Gamer4Open = player(ntmp);
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
      player(1)->GamesType = raspass;
      player(2)->GamesType = raspass;
      player(3)->GamesType = raspass;
      //Repaint();
      //emitRepaint();
    }

    // партия (10 ходов)
    nCurrentMove = nCurrentStart;
    //mPlayingRound = true;
    for (int i = 1; i <= 10; i++) {
      Player *tmpg;
      CardOnDesk[0] = CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = FirstCard = SecondCard = TherdCard = 0;
      if (CurrentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;
      if (CurrentGame == raspass && (i == 1 || i == 2)) {
        Card *tmp4show;
        Card *ptmp4rpass;
        tmp4show = deck->at(29+i);
        ptmp4rpass = newCard(1, tmp4show->suit());
        CardOnDesk[0] = tmp4show;
        //drawInGameCard(0, CardOnDesk[0]);
        Repaint();
        DeskView->mySleep(-1);
        CardOnDesk[nCurrentMove.nValue] = FirstCard = ControlingMakemove(0, ptmp4rpass);
        //!.!delete ptmp4rpass;
      } else {
        CardOnDesk[nCurrentMove.nValue] = FirstCard = ControlingMakemove(0, 0);
      }
      //!.!sprintf(ProtocolBuff, "First card:%i", Card2Int(FirstCard));
      //!.!WriteProtocol(ProtocolBuff);
      Repaint();
      //emitRepaint();
      //!!!drawInGameCard(nCurrentMove.nValue, FirstCard);
      ++nCurrentMove;
      CardOnDesk[nCurrentMove.nValue] = SecondCard = ControlingMakemove(0, FirstCard);
      //!.!sprintf(ProtocolBuff, "Second card:%i", Card2Int(SecondCard));
      //!.!WriteProtocol(ProtocolBuff);
      //Repaint(nCurrentMove.nValue);
      Repaint();
      //!!!drawInGameCard(nCurrentMove.nValue,SecondCard);
      ++nCurrentMove;
      CardOnDesk[nCurrentMove.nValue] = TherdCard = ControlingMakemove(FirstCard, SecondCard);
      //!.!sprintf(ProtocolBuff,"Therd card :%i",Card2Int(TherdCard));
      //!.!WriteProtocol(ProtocolBuff);
      //Repaint(nCurrentMove.nValue);
      Repaint();
      //!!!drawInGameCard(nCurrentMove.nValue,TherdCard);
      CardOnDesk[1] = CardOnDesk[2] = CardOnDesk[3] = 0;
      ++nCurrentMove;
      DeskView->mySleep(-1);
      nPl = nPlayerTakeCards(FirstCard, SecondCard, TherdCard, GamesType[0]-(GamesType[0]/10)*10)-1;
      nCurrentMove = nCurrentMove+nPl;
      tmpg = mPlayers[nCurrentMove.nValue];
      tmpg->nGetsCard++;
      CardOnDesk[0] = 0;
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
      Player *tmpg = player(i);
      Player *Gamer = player(nGamernumber);
      int RetValAddRec = tmpg->aScore->recordScores(CurrentGame,
                tmpg->GamesType,
                Gamer !=0 ? Gamer->nGetsCard : 0,
                tmpg->nGetsCard,
                Gamer !=0 ? nGamernumber : 0,
                i,
        2-nPassCounter);

      if (RetValAddRec) {
        int index = playerWithMaxPool();
        if (index) {
          tmpg->aScore->whistsAdd(index,i,RetValAddRec); // на этого висты
          RetValAddRec = player(index)->aScore->poolAdd(RetValAddRec); // этому в пулю
          if (RetValAddRec) {
            index = playerWithMaxPool();
            if (index) {
              tmpg->aScore->whistsAdd(index, i, RetValAddRec); // на этого висты
              RetValAddRec = player(index)->aScore->poolAdd(RetValAddRec);
              if (RetValAddRec) tmpg->aScore->mountainDown(RetValAddRec);
            } else {
              tmpg->aScore->mountainDown(RetValAddRec);
            }
          }
        } else {
          tmpg->aScore->mountainDown(RetValAddRec);
        }
      }
    }
    CloseBullet();
    // если сетевая игра -  передаем на сервер результаты круга  и кто след. заходит

    // после игры - перевернуть карты и показать их
    CardList *tmplist[3];
    for (int f = 1; f <= 3; f++) {
      Player *plr = player(f);
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
        Player *plr = player(f);
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


int PrefDesktop::playerWithMaxPool () {
  int MaxBullet= -1,CurrBullet=-1,RetVal=0;
  for (int i =1 ;i<=3;i++) {
     CurrBullet = player(i)->aScore->pool();
      if (MaxBullet < CurrBullet && CurrBullet < optMaxPool ) {
        MaxBullet = CurrBullet;
        RetVal = i;
      }
  }
  return RetVal;
}


int PrefDesktop::nPlayerTakeCards (Card *p1, Card *p2, Card *p3, int koz) {
  Card *Max = p1;
  int nRelVal = 1;
  if ((Max->suit() == p2->suit() && Max->face() < p2->face()) || (Max->suit() != koz && p2->suit() == koz)) {
    Max = p2;
    nRelVal = 2;
  }
  if ((Max->suit() == p3->suit() && Max->face() < p3->face()) || (Max->suit() != koz && p3->suit() == koz)) {
    Max = p3;
    nRelVal = 3;
  }
  return nRelVal;
}


Player *PrefDesktop::player (int num) {
  if (num < 1 || num > 3) return 0;
  return mPlayers[num];
}


int PrefDesktop::LoadGame (const QString &name)  {
  Q_UNUSED(name)
  return 0;
}


int PrefDesktop::SaveGame (const QString &name)  {
  Q_UNUSED(name)
  return 0;
}


// draw ingame card (the card that is in game, not in hand)
void PrefDesktop::drawInGameCard (int mPlayerNo, Card *card) {
  if (!card) return;
  int w = DeskView->DesktopWidth/2, h = DeskView->DesktopHeight/2;
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


void PrefDesktop::ShowPaper () {
  QString sb, sm, slw, srw, tw;
  DeskView->ShowBlankPaper(optMaxPool);
  for (int i = 1;i<=3;i++) {
    Player *Gamer = player(i);
    sb = Gamer->aScore->poolStr();
    sm = Gamer->aScore->mountainStr();
    slw = Gamer->aScore->leftWhistsStr();
    srw = Gamer->aScore->rightWhistsStr();
    tw = Gamer->aScore->whistsStr();
    DeskView->showPlayerScore(i, sb, sm, slw, srw, tw);
  }
}


void PrefDesktop::Repaint (bool emitSignal) {
  if (!DeskView) return;
  DeskView->ClearScreen();
  // repaint players
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
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
    Player *plr1 = player(1);
    Player *plr2 = player(2);
    Player *plr3 = player(3);
    if (plr1 && plr2 && plr3) {
      DeskView->drawBidsBmp(mPlayerActive, plr1->nGetsCard, plr2->nGetsCard, plr3->nGetsCard, CurrentGame);
    }
  }
  if (iMoveX >= 0) DeskView->drawIMove(iMoveX, iMoveY);
  // repaint scoreboard
  if (nflShowPaper) ShowPaper();
  if (emitSignal) emitRepaint();
}
