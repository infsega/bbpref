#include "prfconst.h"

#include <QDebug>

#include <QMainWindow>

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "aialphabeta.h"
#include "aiplayer.h"
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


void PrefDesktop::internalInit () {
  mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard=0;

  nCurrentStart.nValue = nCurrentMove.nValue = (qrand()%3)+1;
  nCurrentStart.nMin = nCurrentMove.nMin = 1;
  nCurrentStart.nMax = nCurrentMove.nMax = 3;

  mPlayers << 0; // 0th player is nobody
  //addPlayer(new AiPlayer(1));
  // I Hate This Game :)
  addPlayer(new HumanPlayer(1, mDeskView));
  if (optNoAlphaBeta) {
    addPlayer(new AiPlayer(2, mDeskView));
    addPlayer(new AiPlayer(3, mDeskView));
  } else {
    addPlayer(new CheatPlayer(2, mDeskView));
    addPlayer(new CheatPlayer(3, mDeskView));
  }
  mShowPool = false;
  mOnDeskClosed = false;
  optMaxPool = 21;
  iMoveX = iMoveY = -1;
}


PrefDesktop::PrefDesktop (DeskView *aDeskView) : QObject(0) {
  mPlayingRound = false;
  internalInit();
  mDeskView = aDeskView;
}


PrefDesktop::~PrefDesktop () {
  foreach (Player *p, mPlayers) if (p) delete p;
  mPlayers.clear();
}


void PrefDesktop::emitRepaint () {
  emit deskChanged();
}


void PrefDesktop::closePool () {
  WrapCounter counter(1, 1, 3);
  int mb = INT_MAX, mm = INT_MAX, i;
  tScores R[4];
  for (i = 1; i <= 3; i++) {
    Player *G = player(i);
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
  for (i = 1; i <= 3;i++) {
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
    player(i)->mScore.setWhists(R[i].leftWh+R[i].rightWh-R[i1].rightWh-R[i2].leftWh);
  }
}


Player *PrefDesktop::addPlayer (Player *plr) {
  mPlayers << plr;
  return plr;
}


// ежели номер не 1 и игра пас или (ловля мизера и игрок не сетевой),
// то пораждаем нового HumanPlayer с номером текущего
// ежели номер 1 и игра пас и не сетевая игра то порождаем Player
Card *PrefDesktop::makeGameMove (Card *lMove, Card *rMove, bool isPassOut) {
  Card *res = 0;
  Player *curPlr = player(nCurrentMove);

  if ((player(1)->myGame() == vist || player(1)->myGame() == g86catch) &&
      curPlr->number() != 1 && (curPlr->myGame() == gtPass || curPlr->myGame() == g86catch)) {
    HumanPlayer *hPlr = new HumanPlayer(nCurrentMove.nValue, mDeskView);
    *hPlr = *curPlr;
    mPlayers[nCurrentMove.nValue] = hPlr;
    res = hPlr->moveSelectCard(lMove, rMove, player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)), isPassOut);
    *curPlr = *hPlr;
    mPlayers[nCurrentMove.nValue] = curPlr;
    delete hPlr;
  } else if (curPlr->number() == 1 && curPlr->myGame() == gtPass) {
    Player *aiPlr;
    if (optNoAlphaBeta) aiPlr = new AiPlayer(nCurrentMove.nValue, mDeskView);
    else aiPlr = new CheatPlayer(nCurrentMove.nValue, mDeskView);
    *aiPlr = *curPlr;
    mPlayers[nCurrentMove.nValue] = aiPlr;
    res = aiPlr->moveSelectCard(lMove, rMove, player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)), isPassOut);
    *curPlr = *aiPlr;
    mPlayers[nCurrentMove.nValue] = curPlr;
    delete aiPlr;
  } else {
    res =
      (player(nCurrentMove))->moveSelectCard(lMove, rMove,
        player(succPlayer(nCurrentMove)), player(predPlayer(nCurrentMove)), isPassOut);
  }
  return res;
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


/*
void PrefDesktop::drawBidWindows (const eGameBid *bids, int curPlr) {
  draw(false);
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
*/


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
  CardList tmp(lst); tmp.mySort();
  for (int f = 0; f < tmp.size(); f++) {
    Card *c = tmp.at(f);
    if (!c) continue;
    strcat(dest, " ");
    cardName(dest, c);
  }
}


void PrefDesktop::animateDeskToPlayer (int plrNo, bool doAnim) {
  static const int steps = 10;
  Card *cAni[4];
  int left, top;

  if (!mDeskView) return;
  Player *plr = player(plrNo);
  if (!plr) return;
  plr->getLeftTop(&left, &top);
  if (plrNo == 3) left -= CARDWIDTH-4;

  for (int f = 0; f < 4; f++) {
    cAni[f] = mCardsOnDesk[f];
    mCardsOnDesk[f] = 0;
  }

  for (int f = doAnim?0:steps; f <= steps; f++) {
    draw(false);
    for (int c = 0; c <= 3; c++) {
      if (!cAni[c]) continue;
      int x, y;
      inGameCardLeftTop(c, &x, &y);
      x = x+((int)((double)(left-x)/steps*f));
      y = y+((int)((double)(top-y)/steps*f));
      mDeskView->drawCard(cAni[c], x, y, 1, 0);
    }
    mDeskView->aniSleep(20);
  }
  draw();
}


int PrefDesktop::playerWithMaxPool () {
  int MaxBullet= -1,CurrBullet=-1,res=0;
  for (int i =1 ;i<=3;i++) {
     CurrBullet = player(i)->mScore.pool();
      if (MaxBullet < CurrBullet && CurrBullet < optMaxPool ) {
        MaxBullet = CurrBullet;
        res = i;
      }
  }
  return res;
}


int PrefDesktop::whoseTrick (Card *p1, Card *p2, Card *p3, int trump) {
  Card *maxC = p1;
  int res = 1;
  if ((maxC->suit() == p2->suit() && maxC->face() < p2->face()) || (maxC->suit() != trump && p2->suit() == trump)) {
    maxC = p2;
    res = 2;
  }
  if ((maxC->suit() == p3->suit() && maxC->face() < p3->face()) || (maxC->suit() != trump && p3->suit() == trump)) {
    maxC = p3;
    res = 3;
  }
  return res;
}


Player *PrefDesktop::player (int num) {
  if (num < 1 || num > 3) return 0;
  return mPlayers[num];
}


Player *PrefDesktop::player (const WrapCounter &cnt) {
  return player(cnt.nValue);
}


bool PrefDesktop::loadGame (const QString &name)  {
  QFile fl(name);
  if (!fl.open(QIODevice::ReadOnly)) return false;
  QByteArray ba(fl.readAll());
  fl.close();
  int pos = 0;
  return unserialize(ba, &pos);
}


bool PrefDesktop::saveGame (const QString &name)  {
  QFile fl(name);
  if (!fl.open(QIODevice::WriteOnly)) return false;
  QByteArray ba;
  serialize(ba);
  fl.write(ba);
  fl.close();
  return true;
}


// draw ingame card (the card that is in game, not in hand)
void PrefDesktop::inGameCardLeftTop (int mCardNo, int *left, int *top) {
  int w = mDeskView->DesktopWidth/2, h = mDeskView->DesktopHeight/2;
  int x = w, y = h;
  switch (mCardNo) {
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
  *left = x; *top = y;
}


void PrefDesktop::drawInGameCard (int mCardNo, Card *card) {
  if (!card) return;
  int x, y;
  inGameCardLeftTop(mCardNo, &x, &y);
  mDeskView->drawCard(card, x, y, !mOnDeskClosed, 0);
}


void PrefDesktop::drawPool () {
  QString sb, sm, slw, srw, tw;
  mDeskView->ShowBlankPaper(optMaxPool);
  for (int i = 1;i<=3;i++) {
    Player *plr = player(i);
    sb = plr->mScore.poolStr();
    sm = plr->mScore.mountainStr();
    slw = plr->mScore.leftWhistsStr();
    srw = plr->mScore.rightWhistsStr();
    tw = plr->mScore.whistsStr();
    mDeskView->showPlayerScore(i, sb, sm, slw, srw, tw);
  }
}


void PrefDesktop::draw (bool emitSignal) {
  if (!mDeskView) return;
  mDeskView->ClearScreen();
  // repaint players
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    if (plr) {
      plr->mDeskView = mDeskView;
      plr->draw();
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
      mDeskView->drawBidsBmp(mPlayerActive, plr1->tricksTaken(), plr2->tricksTaken(), plr3->tricksTaken(), gCurrentGame);
    }
  }
  if (iMoveX >= 0) mDeskView->drawIMove(iMoveX, iMoveY);
  // сообщения
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    if (plr) {
      QString msg(plr->message());
      if (!msg.isEmpty()) {
        int plrmx, plrmy;
        getPMsgXY(f, &plrmx, &plrmy);
        mDeskView->drawMessageWindow(plrmx, plrmy, msg, f!=mPlayerHi);
      }
    }
  }
  // repaint scoreboard
  if (mShowPool) drawPool();
  if (emitSignal) emitRepaint();
}


void PrefDesktop::serialize (QByteArray &ba) {
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    plr->mScore.serialize(ba);
  }
  serializeInt(ba, nCurrentStart.nValue);
  serializeInt(ba, optMaxPool);
  serializeInt(ba, optStalingrad);
  serializeInt(ba, opt10Whist);
  serializeInt(ba, optWhistGreedy);
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
  optStalingrad = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  opt10Whist = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  optWhistGreedy = t;
  return true;
}


void PrefDesktop::runGame () {
  eGameBid playerBids[4];
  //char *filename;
  //Card *mFirstCard, *mSecondCard, *mThirdCard;
  int npasscounter;

  //mDeskView->ClearScreen();
  // while !конец пули
  int roundNo = 0;
  while (!(player(1)->mScore.pool() >= optMaxPool &&
           player(2)->mScore.pool() >= optMaxPool &&
           player(3)->mScore.pool() >= optMaxPool)) {
    WrapCounter plrCounter(1, 1, 3);
    mPlayingRound = false;
    int nPl;
    int nPassCounter = 0; // кол-во спасовавших
    playerBids[3] = playerBids[2] = playerBids[1] = playerBids[0] = undefined;
    mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard = 0;
    mDeck.newDeck();
    mDeck.shuffle();

  {
    QFile dbgD("dbg.deck");
    QFile dbgDT("dbg.deck.txt");
    if (dbgDT.open(QIODevice::ReadOnly)) {
      mDeck.clear();
      for (int f = 0; f < 32; f++) {
        QByteArray ba(dbgDT.readLine());
        Card *c = cardFromName(ba.constData());
        if (!c) abort();
        qDebug() << c->toString();
        mDeck << c;
        nCurrentStart.nValue = 1;
      }
    } else if (dbgD.open(QIODevice::ReadOnly)) {
      QByteArray ba(dbgD.readAll());
      dbgD.close();
      int pos = 0;
      if (!mDeck.unserialize(ba, &pos)) abort();
      nCurrentStart.nValue = 1;
    //} else if (allowDebugLog) {
    } else if (allowDebugLog) {
      QString fns(QString::number(roundNo));
      //roundNo++
      while (fns.length() < 2) fns.prepend('0');
      QFile fl(fns);
      if (fl.open(QIODevice::WriteOnly)) {
        QByteArray ba;
        mDeck.serialize(ba);
        fl.write(ba);
        fl.close();
      }
    }
  }

    player(1)->clear();
    player(2)->clear();
    player(3)->clear();
    mPlayerHi = 0;
    gCurrentGame = undefined;
    mShowPool = false;
    // сдаём карты
    /*
    for (int i = 0; i < CARDINCOLODA-2; i++) {
      Player *tmpGamer = player(plrCounter);
      if (!(mDeck.at(i))) mDeskView->MessageBox("Card = 0", "Error!!!");
      tmpGamer->dealCard(mDeck.at(i));
      ++plrCounter;
    }
    */
    // значок "я хожу"
    plrCounter = nCurrentStart;
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
    draw();
    {
      CardList tmpDeck; int cc = succPlayer(nCurrentStart), tPos = 0, tNo = 0;
      mOnDeskClosed = true;
      for (int f = 0; f < 15; f++) {
        if (f == 4) {
          // talon
          tNo = tPos;
          mCardsOnDesk[2] = mDeck.at(tPos++);
          if (optDealAnim) { draw(); mDeskView->aniSleep(40); }
          mCardsOnDesk[3] = mDeck.at(tPos++);
          if (optDealAnim) { draw(); mDeskView->aniSleep(40); }
        }
        Player *plr = player(cc); cc = (cc%3)+1;
        plr->dealCard(mDeck.at(tPos)); tmpDeck << mDeck.at(tPos++);
        if (optDealAnim) { draw(); mDeskView->aniSleep(40); }
        plr->dealCard(mDeck.at(tPos)); tmpDeck << mDeck.at(tPos++);
        if (optDealAnim) { draw(); mDeskView->aniSleep(80); }
        if (optDealAnim) { 
          if (f%3 == 2) mDeskView->aniSleep(200);
        }
      }
      tmpDeck << mDeck.at(tNo++);
      tmpDeck << mDeck.at(tNo);
      if (tmpDeck.count() != 32) abort();
      mDeck = tmpDeck;
/*
      mOnDeskClosed = false;
      mCardsOnDesk[1] = mCardsOnDesk[2] = 0;
*/
      draw(false);
    }
    if (!optDealAnim) {
      draw();
      emitRepaint();
      mDeskView->mySleep(0);
    }

    dlogf("=========================================");
    dlogf("player %i moves...", nCurrentStart.nValue);
    char xxBuf[1024];
    for (int f = 1; f <= 3; f++) {
      Player *plr = player(nCurrentStart); ++nCurrentStart;
      xxBuf[0] = 0;
      dumpCardList(xxBuf, plr->mCards);
      dlogf("hand %i:%s", plr->number(), xxBuf);
    }
    xxBuf[0] = 0;
    cardName(xxBuf, mDeck.at(30));
    cardName(xxBuf, mDeck.at(31));
    dlogf("talon: %s", xxBuf);

    //draw();

    // пошла торговля
    npasscounter = 0;
    //bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
    int curBidIdx = 1;
    while (npasscounter < 2) {
      // пока как минимум двое не спасовали
      Player *plr = player(plrCounter);
      mPlayerHi = plrCounter.nValue;
      if (playerBids[curBidIdx] != gtPass) {
        plr->setMessage("thinking...");
        //drawBidWindows(bids4win, p);
        draw(false);
        if (plr->number() != 1) mDeskView->mySleep(2); else emitRepaint();
        eGameBid bb;
        bb = playerBids[curBidIdx] = plr->moveBidding(playerBids[curBidIdx%3+1], playerBids[(curBidIdx+1)%3+1]);
        qDebug() << "bid:" << sGameName(bb) << bb;
        plr->setMessage(sGameName(bb));
        //bids4win[plrCounter] = playerBids[curBidIdx+1];
      }// else plr->setMessage("PASS");
      ++plrCounter;
      curBidIdx = curBidIdx%3+1;

      if ((playerBids[1] != undefined && playerBids[2] != undefined && playerBids[3] != undefined) &&
          ((playerBids[1] == gtPass?1:0)+(playerBids[2] == gtPass?1:0)+(playerBids[3] == gtPass?1:0) >= 2)) break;
    }
    mPlayerHi = 0;
    draw(false); //mDeskView->mySleep(0);

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
        if (tmpg->myGame() == playerBids[0]) {
          QString gnS(sGameName(playerBids[0]));
          gnS.prepend("game: ");
          gnS += "; player: ";
          gnS += QString::number(i);
          dlogS(gnS);

          mPlayerHi = i;
          player(i%3+1)->setMessage("");
          player((i+1)%3+1)->setMessage("");

          mPlayerActive = i;
          nPassCounter = 0;
          // показываем прикуп
          WrapCounter tmpGamersCounter(1, 3);
          Player *PassOrVistGamers;
          int PassOrVist = 0, nPassOrVist = 0;
          mOnDeskClosed = false;
          mCardsOnDesk[2] = mDeck.at(30);
          mCardsOnDesk[3] = mDeck.at(31);
          // извращение с gCurrentGame -- для того, чтобы показало игру на bidboard
          gCurrentGame = playerBids[0];
          draw();
          //drawBidWindows(bids4win, 0);
          mDeskView->mySleep(-1);
          // запихиваем ему прикуп
          tmpg->dealCard(mDeck.at(30));
          tmpg->dealCard(mDeck.at(31));
          animateDeskToPlayer(mPlayerActive, optTakeAnim); // will clear mCardsOnDesk[]

          //bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
          // снос
          if (tmpg->myGame() != g86) {
            // не мизер
            nCurrentMove.nValue = i;
            draw(false);
            /*if (mPlayerActive == 1)*/ tmpg->setMessage("Select cards to drop");
            if (mPlayerActive != 1) mDeskView->mySleep(2);
            playerBids[0] = gCurrentGame = tmpg->dropForGame();
          } else {
            // показать все карты
            int tempint = nCurrentMove.nValue;
            int nVisibleState = tmpg->invisibleHand();
            tmpg->setInvisibleHand(false);
            draw();
            if (tmpg->number() != 1) tmpg->setMessage("Try to remember the cards");
            else tmpg->setMessage("Misere");
            // ждать до события
            mDeskView->mySleep(-1);
            draw(false);
            tmpg->setMessage("Select cards to drop");
/*
            if (mPlayerActive == 1) {
              int x, y;
              getPMsgXY(mPlayerActive, &x, &y);
              mDeskView->drawMessageWindow(x, y, "Select cards to drop");
            }
*/
            tmpg->setInvisibleHand(nVisibleState);
            nCurrentMove.nValue = tmpg->number();

            if (mPlayerActive != 1) mDeskView->mySleep(2);

            playerBids[0] = gCurrentGame = tmpg->dropForMisere();
            nCurrentMove.nValue = tempint;
          }
          tmpGamersCounter.nValue = i;

          // ставка
          player(tmpGamersCounter)->setMessage(sGameName(gCurrentGame));

          // попёрли выбирать: пас или вист?
          ++tmpGamersCounter;
          player(tmpGamersCounter)->setMessage("thinking...");
          PassOrVistGamers = player(tmpGamersCounter);
          PassOrVistGamers->setMyGame(undefined);
          draw(false);

          // выбирает первый
          int firstPW = tmpGamersCounter.nValue;
          mPlayerHi = firstPW;
          if (firstPW != 1) mDeskView->mySleep(2);
          PassOrVist = PassOrVistGamers->moveFinalBid(gCurrentGame, gtPass, 0);
          nPassOrVist = tmpGamersCounter.nValue;
          if (PassOrVistGamers->myGame() == gtPass) {
            nPassCounter++;
            player(tmpGamersCounter)->setMessage("PASS");
          } else player(tmpGamersCounter)->setMessage("WHIST");
          //bids4win[1] = PassOrVistGamers->myGame();

          // выбирает второй
          ++tmpGamersCounter;
          player(tmpGamersCounter)->setMessage("thinking...");
          int nextPW = tmpGamersCounter.nValue;
          mPlayerHi = nextPW;
          draw(false);
          PassOrVistGamers = player(tmpGamersCounter);
          PassOrVistGamers->setMyGame(undefined);
          PassOrVistGamers->moveFinalBid(gCurrentGame, PassOrVist, nPassOrVist);
          if (PassOrVistGamers->myGame() == gtPass) {
            nPassCounter++;
            player(tmpGamersCounter)->setMessage("PASS");
          } else player(tmpGamersCounter)->setMessage("WHIST");
          //bids4win[2] = PassOrVistGamers->myGame();
          mPlayerHi = 0;

          // всё, отвыбирались
          draw(false);
          mDeskView->mySleep(-1);

          gnS = sGameName(gCurrentGame);
          gnS.prepend("game: ");
          gnS += "; player: ";
          gnS += QString::number(mPlayerActive);
          dlogS(gnS);
          //!DUMP OTHERS!

          if (nPassCounter == 2) {
            // двое спасовали :)
            tmpg->gotPassPassTricks(gameTricks(tmpg->myGame()));
            dlogf("clean out!\n");
            goto LabelRecordOnPaper;
          } else {
            // Открытые или закрытые карты
            for (int ntmp = 2 ; ntmp <= 3 ; ntmp++) {
              Player *Gamer4Open;
              Gamer4Open = player(ntmp);
              if (nPassCounter || gCurrentGame == g86) {
                // если не 2 виста
                if (Gamer4Open->myGame() == gtPass || Gamer4Open->myGame() == vist || Gamer4Open->myGame() == g86catch)
                  Gamer4Open->setInvisibleHand(false);
              }
            }
          }
          break;
        }
      } // узнаем кто назначил максимальную игру
    }  else {
      // раскручиваем распас
      dlogf("game: pass-out");

      mPlayerActive = 0;
      mPlayerHi = 0;
      playerBids[0] = gCurrentGame = raspass;
      player(1)->setMyGame(raspass);
      player(2)->setMyGame(raspass);
      player(3)->setMyGame(raspass);
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
      mOnDeskClosed = false;
      draw();
      mDeskView->mySleep(-1);
    }

    player(1)->setMessage("");
    player(2)->setMessage("");
    player(3)->setMessage("");
    draw(false);

    // партия (10 ходов)
    nCurrentMove = nCurrentStart;
    for (int i = 1; i <= 10; i++) {
      Player *tmpg;
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard = 0;
      if (gCurrentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;

      dlogf("------------------------\nmove #%i", i);
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(nCurrentMove); ++nCurrentMove;
        xxBuf[0] = 0;
        dumpCardList(xxBuf, plr->mCards);
        dlogf("hand %i:%s", plr->number(), xxBuf);
      }

      mPlayerHi = nCurrentMove.nValue;
      player(mPlayerHi)->setMessage("thinking...");
      draw();
      mDeskView->mySleep(0);
      if (gCurrentGame == raspass && (i == 1 || i == 2)) {
        Card *tmp4show;
        Card *ptmp4rpass;
        tmp4show = mDeck.at(29+i);
        ptmp4rpass = newCard(6, tmp4show->suit());
        mCardsOnDesk[0] = tmp4show;
        draw();
        mDeskView->mySleep(0);
        //if (nCurrentMove.nValue != 1) mDeskView->mySleep(-1);
        mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = makeGameMove(0, ptmp4rpass, true);
        //mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = makeGameMove(0, 0);
      } else {
        mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = makeGameMove(0, 0, false);
      }
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (1st) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      mPlayerHi = nCurrentMove.nValue;
      player(mPlayerHi)->setMessage("thinking...");
      draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = mSecondCard = makeGameMove(0, mFirstCard, false);
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (2nd) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      mPlayerHi = nCurrentMove.nValue;
      player(mPlayerHi)->setMessage("thinking...");
      draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = mThirdCard = makeGameMove(mFirstCard, mSecondCard, false);
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (3rd) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      draw();
      mDeskView->mySleep(-1);

      nPl = whoseTrick(mFirstCard, mSecondCard, mThirdCard, playerBids[0]-(playerBids[0]/10)*10)-1;
      nCurrentMove = nCurrentMove+nPl;
      animateDeskToPlayer(nCurrentMove.nValue, optTakeAnim); // will clear mCardsOnDesk[]
      tmpg = player(nCurrentMove);
      tmpg->gotTrick();
      draw(false);
    }
LabelRecordOnPaper:
    mPlayingRound = false;
    ++nCurrentStart;
    // записи по сдаче
    for (int i = 1; i <= 3;i++ ) {
      Player *tmpg = player(i);
      Player *plr = player(mPlayerActive);
      int RetValAddRec = tmpg->mScore.recordScores(gCurrentGame, tmpg->myGame(),
        plr ? plr->tricksTaken() : 0, tmpg->tricksTaken(), plr ? mPlayerActive : 0,
        i, 2-nPassCounter);
      if (RetValAddRec) {
        int index = playerWithMaxPool();
        if (index) {
          tmpg->mScore.whistsAdd(index, i, RetValAddRec); // на этого висты
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
    closePool();
    // если сетевая игра -- передаем на сервер результаты круга и кто след. заходит

    // после игры -- перевернуть карты и показать их
    CardList tmplist[3];
    if (nPassCounter != 2) {
      // была партия
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(f);
        tmplist[f-1] = plr->mCards;
        plr->mCards = plr->mCardsOut;
        plr->setInvisibleHand(false);
      }
    }
    mShowPool = true;
    mPlayingRound = true;
    draw();
    mDeskView->mySleep(-1);
    mPlayingRound = false;
    if (nPassCounter != 2) {
      // была партия
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(f);
        plr->mCardsOut = plr->mCards;
        plr->mCards = tmplist[f-1];
      }
    }
  } // конец пули
  mShowPool = true;
  drawPool();
  emitRepaint();
  emit gameOver();
}
