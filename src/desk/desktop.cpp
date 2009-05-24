#include "prfconst.h"

#include <QMainWindow>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "baser.h"
#include "debug.h"
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
  mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard=0;
  deck = new Deck;

  qsrand(time(NULL));
  nCurrentStart.nValue=nCurrentMove.nValue=1;//(qrand()%3)+1;
  nCurrentStart.nMin=nCurrentMove.nMin=1;
  nCurrentStart.nMax=nCurrentMove.nMax=3;

  //!InsertGamer(new Player(0));
  mPlayers << 0; // 0th player is nobody
  //InsertGamer(new Player(1));
  // I Hate This Game :)
  InsertGamer(new HumanPlayer(1, mDeskView));
  InsertGamer(new Player(2, mDeskView));
  InsertGamer(new Player(3, mDeskView));
  //mPlayers->AtFree(0);
  nflShowPaper = 0;
  optMaxPool = 21;
  iMoveX = iMoveY = -1;
}


PrefDesktop::PrefDesktop (TDeskView *_DeskView) : QObject(0) {
  mPlayingRound = false;
  InternalConstruct();
  mDeskView = _DeskView;
}


PrefDesktop::PrefDesktop () : QObject(0) {
  mPlayingRound = false;
  InternalConstruct();
}


PrefDesktop::~PrefDesktop () {
  delete deck;
  foreach (Player *p, mPlayers) if (p) delete p;
  mPlayers.clear();
}


void PrefDesktop::emitRepaint () {
  emit deskChanged();
}


void PrefDesktop::CloseBullet () {
  WrapCounter counter(1, 1, 3);
  int mb = INT_MAX, mm = INT_MAX, i;
  tScores R[4];
  Player *G;
  for (i = 1; i <= 3; i++) {
    G = player(i);
    R[i].mount = G->mScore.mountain();
    R[i].pool = G->mScore.pool();
    R[i].leftWh = G->mScore.leftWhists();
    R[i].rightWh = G->mScore.rightWhists();
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
    player(i)->mScore.setWhists(R[i].leftWh + R[i].rightWh - R[i1].rightWh - R[i2].leftWh);
  }
}


Player *PrefDesktop::InsertGamer (Player *Gamer) {
  mPlayers << Gamer;
  return Gamer;
}


void PrefDesktop::GamerAssign (Player *newgamer,Player *oldgamer) {
  newgamer->mCards.shallowCopy(oldgamer->mCards);
  newgamer->mLeft.shallowCopy(oldgamer->mLeft);
  newgamer->mRight.shallowCopy(oldgamer->mRight);
  newgamer->mOut.shallowCopy(oldgamer->mOut);
  newgamer->mCardsOut.shallowCopy(oldgamer->mCardsOut);
  //newgamer->mLeftOut.shallowCopy(oldgamer->mLeftOut);
  //newgamer->mRightOut.shallowCopy(oldgamer->mRightOut);
  newgamer->mTricksTaken = oldgamer->mTricksTaken;
  //newgamer->Mast = oldgamer->Mast;
  newgamer->mMyGame = oldgamer->mMyGame;
  //newgamer->Enemy = oldgamer->Enemy;
  newgamer->nInvisibleHand = oldgamer->nInvisibleHand;
  newgamer->mDeskView = oldgamer->mDeskView;
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

  if ((player(1)->mMyGame == vist || player(1)->mMyGame == g86catch) &&
      Now->mPlayerNo != 1 && (Now->mMyGame == gtPass || Now->mMyGame == g86catch)) {
    HumanPlayer *NewHuman = new HumanPlayer(nCurrentMove.nValue, mDeskView);
    GamerAssign(NewHuman, Now);
    mPlayers[nCurrentMove.nValue] = NewHuman;
    RetVal = NewHuman->makemove(lMove, rMove, player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)));
    GamerAssign(Now, NewHuman);
    mPlayers[nCurrentMove.nValue] = Now;
    NewHuman->clear();
    delete NewHuman;
  } else if (Now->mPlayerNo == 1 && Now->mMyGame == gtPass) {
    Player *NewHuman = new Player(nCurrentMove.nValue, mDeskView);
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
      *y = -(mDeskView->yBorder+CARDHEIGHT+50);
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
        mDeskView->drawMessageWindow(x, y, sGameName(bids[f]), true);
      }
    } else mDeskView->drawMessageWindow(x, y, QString("thinking..."));
  }
  //emitRepaint();
  //if (curPlr != 3) mDeskView->mySleep(2); else emitRepaint();
}


static void cardName (char *dest, const Card *c) {
  static const char *faceN[] = {" 7"," 8"," 9","10"," J"," Q"," K"," A"};
  static const char *suitN[] = {"S","C","D","H"};
  if (!c) { strcat(dest, "..."); return; }
  int face = c->face(), suit = c->suit();
  if (face >= 7 && face <= FACE_ACE && suit >= 1 && suit <= 4) {
    strcat(dest, faceN[face-7]);
    strcat(dest, suitN[suit-1]);
    return;
  }
  strcat(dest, "???");
}


static void dumpCardList (char *dest, const CardList &lst) {
  CardList tmp; tmp.shallowCopy(lst); tmp.mySort();
  for (int f = 0; f < tmp.size(); f++) {
    Card *c = tmp.at(f);
    if (!c) continue;
    strcat(dest, " ");
    cardName(dest, c);
  }
}


void PrefDesktop::RunGame () {
  eGameBid playerBids[4], bids4win[4];
  //char *filename;
  //Card *mFirstCard, *mSecondCard, *mThirdCard;
  int npasscounter;

  //mDeskView->ClearScreen();
  // while !конец пули
  qsrand(time(NULL));
  int roundNo = 0;
  while (!(player(1)->mScore.pool() >= optMaxPool &&
           player(2)->mScore.pool() >= optMaxPool &&
           player(3)->mScore.pool() >= optMaxPool)) {
    WrapCounter GamersCounter(1, 1, 3);
    mPlayingRound = false;
    int nPl;
    int nGamernumber = 0; // номер заказавшего игру
    int nPassCounter = 0; // кол-во спасовавших
    playerBids[3] = playerBids[2] = playerBids[1] = playerBids[0] = undefined;
    mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard = 0;
    delete deck;
    deck = new Deck;
    deck->shuffle();

    QFile dbgD("dbg.deck");
    if (dbgD.open(QIODevice::ReadOnly)) {
      QByteArray ba(dbgD.readAll());
      dbgD.close();
      int pos = 0;
      if (!deck->unserialize(ba, &pos)) abort();
    } else {
      QString fns(QString::number(roundNo++));
      while (fns.length() < 2) fns.prepend('0');
      QFile fl(fns);
      if (fl.open(QIODevice::WriteOnly)) {
        QByteArray ba;
        deck->serialize(ba);
        fl.write(ba);
        fl.close();
      }
    }

    player(1)->clear();
    player(2)->clear();
    player(3)->clear();
    CurrentGame = undefined;
    nflShowPaper = 0;
    // сдаём карты
    for (int i = 0; i < CARDINCOLODA-2; i++) {
      Player *tmpGamer = player(GamersCounter.nValue);
      if (!(deck->at(i))) mDeskView->MessageBox("Card = 0", "Error!!!");
      tmpGamer->AddCard(deck->at(i));
      ++GamersCounter;
    }
/*
    for (int f = 0; f < 2; f++) {
      Player *plr = player(f);
      plr->sortCards();
    }
*/
    dlogf("=========================================");
    dlogf("player %i moves...", nCurrentStart.nValue);
    char xxBuf[1024];
    for (int f = 1; f <= 3; f++) {
      Player *plr = player(nCurrentStart.nValue); ++nCurrentStart;
      xxBuf[0] = 0;
      dumpCardList(xxBuf, plr->mCards);
      dlogf("hand %i:%s", plr->mPlayerNo, xxBuf);
    }
    xxBuf[0] = 0;
    cardName(xxBuf, deck->at(30));
    cardName(xxBuf, deck->at(31));
    dlogf("talion: %s", xxBuf);
    //emitRepaint();
    //mDeskView->mySleep(0);
    GamersCounter = nCurrentStart;
    switch (nCurrentStart.nValue) {
      case 1:
        iMoveX = mDeskView->DesktopWidth/2-15;
        iMoveY = mDeskView->DesktopHeight-mDeskView->yBorder-CARDHEIGHT-40;
        break;
      case 2:
        iMoveX = mDeskView->xBorder+20;
        iMoveY = mDeskView->yBorder+CARDHEIGHT+40;
        break;
      case 3:
        iMoveX = mDeskView->DesktopWidth-mDeskView->xBorder-20;
        iMoveY = mDeskView->yBorder+CARDHEIGHT+40;
        break;
      default: iMoveX = iMoveY = -1; break;
    }
    Repaint();

    // пошла торговля
    npasscounter = 0;
    bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
    int curBidIdx = 0;
    //drawBidWindows(bids4win, GamersCounter.nValue);
    //if (GamersCounter.nValue != 1) mDeskView->mySleep(2); else emitRepaint();
    while (npasscounter < 2) {
      // пока как минимум двое не спасовали
      int p = GamersCounter.nValue;
      drawBidWindows(bids4win, p);
      if (p != 1) mDeskView->mySleep(2); else emitRepaint();

      if (playerBids[curBidIdx+1] != gtPass) {
        playerBids[curBidIdx+1] = player(p)->makemove(playerBids[((curBidIdx+1)%3)+1], playerBids[((curBidIdx+2)%3)+1]);
      }
      bids4win[p] = playerBids[curBidIdx+1];
      ++GamersCounter;
      curBidIdx = (curBidIdx+1)%3;

      if ((playerBids[1] != undefined && playerBids[2] != undefined && playerBids[3] != undefined) &&
          ((playerBids[1] == gtPass?1:0)+(playerBids[2] == gtPass?1:0)+(playerBids[3] == gtPass?1:0) >= 2)) break;
    }

    // выч. максим игру
    for (int i = 1; i <= 3; i++) {
      if (playerBids[0] < playerBids[i]) playerBids[0] = playerBids[i];
    }

    // поехали совать прикуп и выбирать финальную ставку (ну, или ничего, если распасы)
    mPlayerActive = 0;
    mPlayingRound = true;
    if (playerBids[0] != gtPass) {
      // не распасы
      // узнаем, кто назначил максимальную игру
      for (int i = 1; i <= 3; i++) {
        Player *tmpg = player(i);
        //tmpg->sortCards();
        if (tmpg->mMyGame == playerBids[0]) {
          QString gnS(sGameName(playerBids[0]));
          gnS.prepend("game: ");
          gnS += "; player: ";
          gnS += QString::number(i);
          dlogS(gnS);

          mPlayerActive = i;
          nPassCounter = 0;
          // показываем прикуп
          WrapCounter tmpGamersCounter(1, 3);
          Player *PassOrVistGamers;
          int PassOrVist = 0, nPassOrVist = 0;
          mCardsOnDesk[2] = deck->at(30);
          mCardsOnDesk[3] = deck->at(31);
          // извращение с CurrentGame -- для того, чтобы показало игру на bidboard
          //!eGameBid oc = CurrentGame;
          CurrentGame = playerBids[0];
          drawBidWindows(bids4win, 0);
          mDeskView->mySleep(-1);
          //!CurrentGame = oc; // вернём CurrentGame на место -- на всякий случай
          // запихиваем ему прикуп
          tmpg->AddCard(deck->at(30));
          tmpg->AddCard(deck->at(31));
          mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
          nGamernumber = tmpg->mPlayerNo;
          //mDeskView->ClearScreen();
          //tmpg->sortCards();
          Repaint();
          //emitRepaint();
          //mDeskView->mySleep(2);

          bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
          // снос
          if (tmpg->mMyGame != g86) { // не мизер
            nCurrentMove.nValue = i;
            Repaint(false);
            if (mPlayerActive == 1) {
              int x, y;
              getPMsgXY(mPlayerActive, &x, &y);
              mDeskView->drawMessageWindow(x, y, "Select cards to drop");
            }
            playerBids[0] = CurrentGame = tmpg->makeout4game();
          } else {
            // показать все карты
            int tempint = nCurrentMove.nValue;
            int nVisibleState = tmpg->nInvisibleHand;
            //tmpg->nCardsVisible = 0;
            tmpg->nInvisibleHand = 0;
            Repaint();
            //emitRepaint();
            //if (tmpg->mPlayerNo != 1) mDeskView->MessageBox("Try to remember the cards", "Message");
            if (tmpg->mPlayerNo != 1) {
              //mDeskView->MessageBox("Try to remember the cards", "Message");
              int x, y;
              getPMsgXY(1, &x, &y);
              mDeskView->drawMessageWindow(x, y, "Try to remember the cards");
            }
            // ждать до события
            mDeskView->mySleep(-1);
            Repaint(false);
            if (mPlayerActive == 1) {
              int x, y;
              getPMsgXY(mPlayerActive, &x, &y);
              mDeskView->drawMessageWindow(x, y, "Select cards to drop");
            }
            tmpg->nInvisibleHand = nVisibleState;
            nCurrentMove.nValue = tmpg->mPlayerNo;
            playerBids[0] = CurrentGame = tmpg->makeout4miser();
            nCurrentMove.nValue = tempint;
          }
          tmpGamersCounter.nValue = i;
          //mDeskView->ClearScreen();
          //Repaint();
          //emitRepaint();

          // попёрли выбирать: пас или вист?
          ++tmpGamersCounter;
          PassOrVistGamers = player(tmpGamersCounter.nValue);
          PassOrVistGamers->mMyGame = undefined;
          //PassOrVistGamers->sortCards();
          Repaint();
          //emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          //mDeskView->mySleep(2);

          // выбирает первый
          int firstPW = tmpGamersCounter.nValue;
          { // рисуем ставку
            int x, y;
            getPMsgXY(mPlayerActive, &x, &y);
            mDeskView->drawMessageWindow(x, y, sGameName(CurrentGame), true);
          }
          { // рисуем думалку
            int x, y;
            getPMsgXY(firstPW, &x, &y);
            mDeskView->drawMessageWindow(x, y, "thinking...");
            if (firstPW != 1) mDeskView->mySleep(2);
          }
          PassOrVist = PassOrVistGamers->makemove(CurrentGame, gtPass, 0);
          nPassOrVist = tmpGamersCounter.nValue;
          if (PassOrVistGamers->mMyGame == gtPass) nPassCounter++;
          bids4win[1] = PassOrVistGamers->mMyGame;
          //emitRepaint();
          //OnlyMessage(tmpGamersCounter.nValue);
          //mDeskView->mySleep(1);

          // выбирает второй
          Repaint(false);
          ++tmpGamersCounter;
          int nextPW = tmpGamersCounter.nValue;
          { // рисуем ставку
            int x, y;
            getPMsgXY(mPlayerActive, &x, &y);
            mDeskView->drawMessageWindow(x, y, sGameName(CurrentGame), true);
          }
          { // рисуем выбор
            int x, y;
            getPMsgXY(firstPW, &x, &y);
            mDeskView->drawMessageWindow(x, y, sGameName(bids4win[1]), true);
          }
          { // рисуем думалку
            int x, y;
            getPMsgXY(nextPW, &x, &y);
            mDeskView->drawMessageWindow(x, y, "thinking...");
            if (nextPW != 1) mDeskView->mySleep(2);
          }
          PassOrVistGamers = player(tmpGamersCounter.nValue);
          PassOrVistGamers->mMyGame = undefined;
          //Repaint(tmpGamersCounter.nValue);
          //Repaint();
          //emitRepaint();
          //mDeskView->mySleep(1);
          PassOrVistGamers->makemove(CurrentGame, PassOrVist, nPassOrVist);
          if (PassOrVistGamers->mMyGame == gtPass) nPassCounter++;
          bids4win[2] = PassOrVistGamers->mMyGame;
          Repaint();
          //emitRepaint();
          //mDeskView->mySleep(1);

          // всё, рисуем финалы
          { // рисуем ставку
            int x, y;
            getPMsgXY(mPlayerActive, &x, &y);
            mDeskView->drawMessageWindow(x, y, sGameName(CurrentGame), true);
          }
          { // рисуем выбор
            int x, y;
            getPMsgXY(firstPW, &x, &y);
            mDeskView->drawMessageWindow(x, y, sGameName(bids4win[1]), true);
          }
          { // рисуем выбор
            int x, y;
            getPMsgXY(nextPW, &x, &y);
            mDeskView->drawMessageWindow(x, y, sGameName(bids4win[2]), true);
          }
          /*if (nextPW != 1) */mDeskView->mySleep(-1);

          gnS = sGameName(CurrentGame);
          gnS.prepend("game: ");
          gnS += "; player: ";
          gnS += QString::number(nGamernumber);
          dlogS(gnS);
          //!DUMP OTHERS!

          if (nPassCounter == 2) {
            // двое спасовали :)
            tmpg->mTricksTaken = gameTricks(tmpg->mMyGame);
            dlogf("clean out!\n");
            goto LabelRecordOnPaper;
          } else {
            // Открытые или закрытые карты
            for (int ntmp = 2 ; ntmp <= 3 ; ntmp++) {
              Player *Gamer4Open;
              Gamer4Open = player(ntmp);
              if (nPassCounter || CurrentGame == g86) {
                // если не 2 виста
                if (Gamer4Open->mMyGame == gtPass || Gamer4Open->mMyGame == vist || Gamer4Open->mMyGame == g86catch)
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
      dlogf("game: pass-out");
      mPlayerActive = 0;
      playerBids[0] = CurrentGame = raspass;
      player(1)->mMyGame = raspass;
      player(2)->mMyGame = raspass;
      player(3)->mMyGame = raspass;
      //Repaint();
      //emitRepaint();
    }

    // партия (10 ходов)
    nCurrentMove = nCurrentStart;
    //mPlayingRound = true;
    for (int i = 1; i <= 10; i++) {
      Player *tmpg;
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard = 0;
      if (CurrentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;

      dlogf("------------------------\nmove #%i", i);
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(nCurrentMove.nValue); ++nCurrentMove;
        xxBuf[0] = 0;
        dumpCardList(xxBuf, plr->mCards);
        dlogf("hand %i:%s", plr->mPlayerNo, xxBuf);
      }

      if (CurrentGame == raspass && (i == 1 || i == 2)) {
        Card *tmp4show;
        Card *ptmp4rpass;
        tmp4show = deck->at(29+i);
        ptmp4rpass = newCard(1, tmp4show->suit());
        mCardsOnDesk[0] = tmp4show;
        //drawInGameCard(0, mCardsOnDesk[0]);
        Repaint();
        mDeskView->mySleep(-1);
        mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = ControlingMakemove(0, ptmp4rpass);
        //!.!delete ptmp4rpass;
      } else {
        mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = ControlingMakemove(0, 0);
      }
      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (1st) player %i:%s", nCurrentMove.nValue, xxBuf);
      //!.!sprintf(ProtocolBuff, "First card:%i", Card2Int(mFirstCard));
      //!.!WriteProtocol(ProtocolBuff);
      Repaint();
      //emitRepaint();
      //!!!drawInGameCard(nCurrentMove.nValue, mFirstCard);
      ++nCurrentMove;
      mCardsOnDesk[nCurrentMove.nValue] = mSecondCard = ControlingMakemove(0, mFirstCard);
      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (2nd) player %i:%s", nCurrentMove.nValue, xxBuf);
      //!.!sprintf(ProtocolBuff, "Second card:%i", Card2Int(mSecondCard));
      //!.!WriteProtocol(ProtocolBuff);
      //Repaint(nCurrentMove.nValue);
      Repaint();
      //!!!drawInGameCard(nCurrentMove.nValue,mSecondCard);
      ++nCurrentMove;
      mCardsOnDesk[nCurrentMove.nValue] = mThirdCard = ControlingMakemove(mFirstCard, mSecondCard);
      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (3rd) player %i:%s", nCurrentMove.nValue, xxBuf);
      //!.!sprintf(ProtocolBuff,"Therd card :%i",Card2Int(mThirdCard));
      //!.!WriteProtocol(ProtocolBuff);
      //Repaint(nCurrentMove.nValue);
      Repaint();
      //!!!drawInGameCard(nCurrentMove.nValue,mThirdCard);
      mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
      ++nCurrentMove;
      mDeskView->mySleep(-1);
      nPl = nPlayerTakeCards(mFirstCard, mSecondCard, mThirdCard, playerBids[0]-(playerBids[0]/10)*10)-1;
      nCurrentMove = nCurrentMove+nPl;
      tmpg = mPlayers[nCurrentMove.nValue];
      tmpg->mTricksTaken++;
      mCardsOnDesk[0] = 0;
      //mDeskView->ClearScreen();
      Repaint();
      //emitRepaint();
    }
    //mDeskView->mySleep(2);
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
      int RetValAddRec = tmpg->mScore.recordScores(CurrentGame,
                tmpg->mMyGame,
                Gamer !=0 ? Gamer->mTricksTaken : 0,
                tmpg->mTricksTaken,
                Gamer !=0 ? nGamernumber : 0,
                i,
        2-nPassCounter);

      if (RetValAddRec) {
        int index = playerWithMaxPool();
        if (index) {
          tmpg->mScore.whistsAdd(index,i,RetValAddRec); // на этого висты
          RetValAddRec = player(index)->mScore.poolAdd(RetValAddRec); // этому в пулю
          if (RetValAddRec) {
            index = playerWithMaxPool();
            if (index) {
              tmpg->mScore.whistsAdd(index, i, RetValAddRec); // на этого висты
              RetValAddRec = player(index)->mScore.poolAdd(RetValAddRec);
              if (RetValAddRec) tmpg->mScore.mountainDown(RetValAddRec);
            } else {
              tmpg->mScore.mountainDown(RetValAddRec);
            }
          }
        } else {
          tmpg->mScore.mountainDown(RetValAddRec);
        }
      }
    }
    CloseBullet();
    // если сетевая игра -  передаем на сервер результаты круга  и кто след. заходит

    // после игры - перевернуть карты и показать их
    CardList tmplist[3];
    if (nPassCounter != 2) {
      // была партия
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(f);
        tmplist[f-1].shallowCopy(&plr->mCards);
        plr->mCards.shallowCopy(&plr->mCardsOut);
        plr->nInvisibleHand = false;
      }
    }
    nflShowPaper = 1;
    mPlayingRound = true;
    Repaint();
    mDeskView->mySleep(-1);
    mPlayingRound = false;
    if (nPassCounter != 2) {
      // была партия
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(f);
        plr->mCardsOut.shallowCopy(&plr->mCards);
        plr->mCards.shallowCopy(&tmplist[f-1]);
      }
    }
  } // конец пули
  nflShowPaper = 1;
  ShowPaper();
  mDeskView->mySleep(0);
}


int PrefDesktop::playerWithMaxPool () {
  int MaxBullet= -1,CurrBullet=-1,RetVal=0;
  for (int i =1 ;i<=3;i++) {
     CurrBullet = player(i)->mScore.pool();
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


bool PrefDesktop::LoadGame (const QString &name)  {
  QFile fl(name);
  if (!fl.open(QIODevice::ReadOnly)) return false;
  QByteArray ba(fl.readAll());
  fl.close();
  int pos = 0;
  return unserialize(ba, &pos);
}


bool PrefDesktop::SaveGame (const QString &name)  {
  QFile fl(name);
  if (!fl.open(QIODevice::WriteOnly)) return false;
  QByteArray ba;
  serialize(ba);
  fl.write(ba);
  fl.close();
  return true;
}


// draw ingame card (the card that is in game, not in hand)
void PrefDesktop::drawInGameCard (int mPlayerNo, Card *card) {
  if (!card) return;
  int w = mDeskView->DesktopWidth/2, h = mDeskView->DesktopHeight/2;
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
  mDeskView->drawCard(card, x, y, 1, 0);
}


void PrefDesktop::ShowPaper () {
  QString sb, sm, slw, srw, tw;
  mDeskView->ShowBlankPaper(optMaxPool);
  for (int i = 1;i<=3;i++) {
    Player *Gamer = player(i);
    sb = Gamer->mScore.poolStr();
    sm = Gamer->mScore.mountainStr();
    slw = Gamer->mScore.leftWhistsStr();
    srw = Gamer->mScore.rightWhistsStr();
    tw = Gamer->mScore.whistsStr();
    mDeskView->showPlayerScore(i, sb, sm, slw, srw, tw);
  }
}


void PrefDesktop::Repaint (bool emitSignal) {
  if (!mDeskView) return;
  mDeskView->ClearScreen();
  // repaint players
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    if (plr) {
      plr->mDeskView = mDeskView;
      plr->Repaint();
    }
  }
  // repaint in-game cards
  for (int f = 0; f <= 3; f++) {
    if (mCardsOnDesk[f]) drawInGameCard(f, mCardsOnDesk[f]);
  }
  // draw bidboard
  if (mPlayingRound) {
    Player *plr1 = player(1);
    Player *plr2 = player(2);
    Player *plr3 = player(3);
    if (plr1 && plr2 && plr3) {
      mDeskView->drawBidsBmp(mPlayerActive, plr1->mTricksTaken, plr2->mTricksTaken, plr3->mTricksTaken, CurrentGame);
    }
  }
  if (iMoveX >= 0) mDeskView->drawIMove(iMoveX, iMoveY);
  // repaint scoreboard
  if (nflShowPaper) ShowPaper();
  if (emitSignal) emitRepaint();
}


void PrefDesktop::serialize (QByteArray &ba) {
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    plr->mScore.serialize(ba);
  }
  serializeInt(ba, nCurrentStart.nValue);
  serializeInt(ba, optMaxPool);
  serializeInt(ba, g61stalingrad);
  serializeInt(ba, g10vist);
  serializeInt(ba, globvist);
}


bool PrefDesktop::unserialize (QByteArray &ba, int *pos) {
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    if (!plr->mScore.unserialize(ba, pos)) return false;
  }
  int t;
  if (!unserializeInt(ba, pos, &t)) return false;
  nCurrentStart.nValue = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  optMaxPool = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  g61stalingrad = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  g10vist = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  globvist = t;
  return true;
}
