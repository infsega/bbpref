/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

#include "prfconst.h"

#include <QDebug>

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>

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

bool Closed_Whist = false;

/*enum GameStage {
	Bidding,
	Drop,
	Game
}

GameStage gameStage = Bidding;*/

typedef struct {
  int mount, pool, leftWh, rightWh;
} tScores;

static QHash<int, const char *> gameNames;

static const char * sGameName (eGameBid game) {
  if(gameNames.isEmpty()) {
  gameNames[g86catch] = " ";
  gameNames[raspass] = "pass-out";
  gameNames[undefined] = "-----";
  gameNames[whist] = "whist";
  gameNames[halfwhist] = "halfwhist";
  gameNames[gtPass] = "pass";
  gameNames[g61] = "6\1s";
  gameNames[g62] = "6\1c";
  gameNames[g63] = "6\1d";
  gameNames[g64] = "6\1h";
  gameNames[g65] = "6NT";
  gameNames[g71] = "7\1s";
  gameNames[g72] = "7\1c";
  gameNames[g73] = "7\1d";
  gameNames[g74] = "7\1h";
  gameNames[g75] = "7NT";
  gameNames[g81] = "8\1s";
  gameNames[g82] = "8\1c";
  gameNames[g83] = "8\1d";
  gameNames[g84] = "8\1h";
  gameNames[g85] = "8NT";
  gameNames[g86] = "Misere";
  gameNames[g91] = "9\1s";
  gameNames[g92] = "9\1c";
  gameNames[g93] = "9\1d";
  gameNames[g94] = "9\1h";
  gameNames[g95] = "9NT";
  gameNames[g101] = "10\1s";
  gameNames[g102] = "10\1c";
  gameNames[g103] = "10\1d";
  gameNames[g104] = "10\1h";
  gameNames[g105] = "10NT";
  gameNames[zerogame] = "";
  }
  return gameNames[game];
}

static const char * bidMessage(eGameBid game)
{
		if (game == whist) return "whist";
		else if (game == gtPass) return "pass";
		else if (game == g65) return "6 no trump";
		else if (game == g75) return "7 no trump";
		else if (game == g85) return "8 no trump";
		else if (game == g95) return "9 no trump";
		else if (game == g105) return "10 no trump";
		else if (game == g86) return "Misere";
		else if (game == withoutThree) return "without three";
		else return gameNames.value(game);
}


static int whoseTrick (Card *p1, Card *p2, Card *p3, int trump) {
  Q_ASSERT(p1 != 0);
  Q_ASSERT(p2 != 0);
  Q_ASSERT(p3 != 0);
  // The next assert doesn't work because of dirty hack passing values > 4 here
  //Q_ASSERT((trump >= 0) && (trump <= 4)); // 0 is no trump
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

/// @todo move to Card class
static void cardName (char *dest, const Card *c) {
  Q_ASSERT(dest != 0);
  Q_ASSERT(c != 0);
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

/// @todo move to CardList class
static void dumpCardList (char *dest, const CardList &lst) {
  CardList tmp(lst); tmp.mySort();
  for (int f = 0; f < tmp.size(); f++) {
    Card *c = tmp.at(f);
    if (!c) continue;
    strcat(dest, " ");
    cardName(dest, c);
  }
}


void PrefModel::internalInit () {
  mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard=0;

  nCurrentStart.nValue = nCurrentMove.nValue = (qrand()%3)+1;
  nCurrentStart.nMin = nCurrentMove.nMin = 1;
  nCurrentStart.nMax = nCurrentMove.nMax = 3;

  mPlayers << 0; // 0th player is nobody
  //addPlayer(new AiPlayer(1));
  // I Hate This Game :)
  addPlayer(new HumanPlayer(1, mDeskView));
  if (!optAlphaBeta1)
    addPlayer(new AiPlayer(2, mDeskView));
  else 
    addPlayer(new CheatPlayer(2, mDeskView));
  if (!optAlphaBeta2)
    addPlayer(new AiPlayer(3, mDeskView));
  else 
    addPlayer(new CheatPlayer(3, mDeskView));

  mOnDeskClosed = false;
}


PrefModel::PrefModel (DeskView *aDeskView) : QObject(0) {
  mPlayingRound = false;
  mGameRunning = false;  
  mDeskView = aDeskView;
  internalInit();
}


PrefModel::~PrefModel () {
  foreach (Player *p, mPlayers) delete p;
  mPlayers.clear();
}


void PrefModel::closePool () {
  if(!mGameRunning)
    return;
  WrapCounter counter(1, 1, 3);
  int i;
  tScores R[4];
  for (i = 1; i <= 3; i++) {
    Player *G = player(i);
    R[i].mount = G->mScore.mountain();
    R[i].pool = G->mScore.pool();
    R[i].leftWh = G->mScore.leftWhists();
    R[i].rightWh = G->mScore.rightWhists();
    //if (R[i].mount < mm) mm = R[i].mount;
    //if (R[i].pool < mb) mb = R[i].pool;
  }
  // Amnistiya gori
  //for (i = 1; i <= 3; i++) R[i].mount -= mm;
  // Amnistiya puli
  //for (i = 1; i <= 3; i++) R[i].pool -= mb;
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


Player *PrefModel::addPlayer (Player *plr) {
  mPlayers << plr;
  return plr;
}


//    1     (     ),
//    HumanPlayer   
// if number is 1(pass) and offline game, create Player
Card *PrefModel::makeGameMove (Card *lMove, Card *rMove, bool isPassOut) {
  Card *res = 0;
  Player *curPlr = player(nCurrentMove);
  Player *plr = 0;
  // 1. Current player is not human
  // Human's move if current he whists whith open cards or catches misere, and current player
  // passes or catches misere
  if (((player(1)->myGame() == whist && !Closed_Whist) || player(1)->myGame() == g86catch) &&
      !curPlr->isHuman() && (curPlr->myGame() == gtPass || curPlr->myGame() == g86catch)) {
    plr = player(1)->create(nCurrentMove.nValue, mDeskView);
  }

  // 2. Current player passes
  // if AI whists with open cards, it makes move instead of human or other AI
  else if (curPlr->myGame() == gtPass && !Closed_Whist) {
    if (player(2)->myGame() == whist) {
      plr = player(2)->create(nCurrentMove.nValue, mDeskView);
    }
    else if (player(3)->myGame() == whist) {
      plr = player(3)->create(nCurrentMove.nValue, mDeskView);
    }
    else
    {
      QMessageBox::about(0,"Error","Something went wrong!");
      qDebug() << player(1)->myGame() << player(2)->myGame() << player(3)->myGame();
      Q_ASSERT(1 == 0);
    }
  }

  if (plr) {
    // Do player logic swap
    *plr = *curPlr;
    mPlayers[nCurrentMove.nValue] = plr;
    qDebug() << plr->type() << " plays for " << nCurrentMove.nValue;
    res = plr->moveSelectCard(lMove, rMove, player(nextPlayer(nCurrentMove)),
      player(previousPlayer(nCurrentMove)), isPassOut);
    *curPlr = *plr;
    mPlayers[nCurrentMove.nValue] = curPlr;
    delete plr;
  } else {
    // No swaps, current player makes move himself
    res = curPlr->moveSelectCard(lMove, rMove, player(nextPlayer(nCurrentMove)),
      player(previousPlayer(nCurrentMove)), isPassOut);
  }
  return res;
}


int PrefModel::playerWithMaxPool () {
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


inline Player *PrefModel::player (int num) {
  //if (num < 1 || num > 3) return 0;
  Q_ASSERT(num >= 1 || num <= 3);
  return mPlayers[num];
}


inline Player *PrefModel::player (const WrapCounter &cnt) {
  Q_ASSERT(cnt.nValue >= 1 || cnt.nValue <= 3);
  return player(cnt.nValue);
}


bool PrefModel::loadGame (const QString &name)  {
  QFile fl(name);
  if (!fl.open(QIODevice::ReadOnly)) {
      printf("Load failed\n");
	  return false;
  }
  QByteArray ba(fl.readAll());
  fl.close();
  int pos = 0;
  return unserialize(ba, &pos);
}


bool PrefModel::saveGame (const QString &name)  {
  QFile fl(name);
  if (!fl.open(QIODevice::WriteOnly)) {
      printf("Save failed\n");
	  return false;
  }
  QByteArray ba;
  serialize(ba);
  fl.write(ba);
  fl.close();
  return true;
}

void PrefModel::draw (bool emitSignal) {
  //if (!mDeskView) return;
  Q_ASSERT(mDeskView != 0);
  mDeskView->ClearScreen();

  if (!mGameRunning)
  	return;

  // repaint players
  for (int f = 1; f <= 3; f++) 
      mPlayers.at(f)->draw();

  // repaint in-game cards
  for (int f = 0; f <= 3; f++)
    if (mCardsOnDesk[f]) mDeskView->drawInGameCard(f, mCardsOnDesk[f], mOnDeskClosed);

  // draw bidboard
  if (mPlayingRound)
    mDeskView->drawBidBoard();

  mDeskView->drawIMove();
  // 
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
      QString msg(plr->message());
      if (!msg.isEmpty()) {
        mDeskView->drawPlayerMessage(f, msg, f!=mPlayerHi);
      }
  }
  if (emitSignal) mDeskView->update();
}


void PrefModel::serialize (QByteArray &ba) {
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


bool PrefModel::unserialize (QByteArray &ba, int *pos) {
  for (int f = 1; f <= 3; f++) {
    Player *plr = player(f);
    if (!plr->mScore.unserialize(ba, pos)) return false;
  }
  int t;
  if (!unserializeInt(ba, pos, &t)) return false;
  nCurrentStart.nValue = t;
  if (!unserializeInt(ba, pos, &t)) return false;
  optMaxPool = static_cast<bool>(t);
  if (!unserializeInt(ba, pos, &t)) return false;
  optStalingrad = static_cast<bool>(t);
  if (!unserializeInt(ba, pos, &t)) return false;
  opt10Whist = static_cast<bool>(t);
  if (!unserializeInt(ba, pos, &t)) return false;
  optWhistGreedy = static_cast<bool>(t);
  return true;
}


void PrefModel::runGame () {
  eGameBid playerBids[4];
  //char *filename;
  //Card *mFirstCard, *mSecondCard, *mThirdCard;
  int npasscounter;

  mGameRunning = true;
  //mDeskView->ClearScreen();
  kpref->clearHint();
  // while !end of pool
  int roundNo = 0;
  while (!(player(1)->mScore.pool() >= optMaxPool &&
           player(2)->mScore.pool() >= optMaxPool &&
           player(3)->mScore.pool() >= optMaxPool)) {
    WrapCounter plrCounter(1, 1, 3);
    mPlayingRound = false;
	mBiddingDone = false;
    int nPl;
    int nPassCounter = 0; // number of passes
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
        //if (!c) abort();
        Q_ASSERT(c != 0);
        qDebug() << c->toString();
        mDeck << c;
        nCurrentStart.nValue = 1;
      }
    } else if (dbgD.open(QIODevice::ReadOnly)) {
      QByteArray ba(dbgD.readAll());
      dbgD.close();
      int pos = 0;
      const bool result = mDeck.unserialize(ba, &pos);
      //if (!mDeck.unserialize(ba, &pos)) abort();
      Q_ASSERT(result);
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
    // deal cards
    /*
    for (int i = 0; i < CARDINCOLODA-2; i++) {
      Player *tmpGamer = player(plrCounter);
      if (!(mDeck.at(i))) mDeskView->MessageBox("Card = 0", "Error!!!");
      tmpGamer->dealCard(mDeck.at(i));
      ++plrCounter;
    }
    */

  // Starter of bidding
  plrCounter = nCurrentStart;
  for (int i=1; i<=3; i++)
    player(i)->setCurrentStart(i == nCurrentStart.nValue);

    draw();
    {
      CardList tmpDeck; int cc = nextPlayer(nCurrentStart), tPos = 0, tNo = 0;
      mOnDeskClosed = true;
	  if (optDealAnim)
	  	player(1)->setInvisibleHand(true);
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
        //if (optDealAnim) { draw(); mDeskView->aniSleep(40); }
        plr->dealCard(mDeck.at(tPos)); tmpDeck << mDeck.at(tPos++);
        if (optDealAnim) { draw(); mDeskView->aniSleep(80); }
        if (optDealAnim) { 
          if (f%3 == 2) mDeskView->aniSleep(200);
        }
      }
	  mDeskView->aniSleep(200);
	  if (optDealAnim)
	  	player(1)->setInvisibleHand(false);
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
      mDeskView->update();
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

    //  
    npasscounter = 0;
    //bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
    int curBidIdx = 1;
    while (npasscounter < 2) {
      //      
      Player *plr = player(plrCounter);
      mPlayerHi = plrCounter.nValue;
      if (playerBids[curBidIdx] != gtPass) {
        plr->setMessage(tr("thinking..."));
        //drawBidWindows(bids4win, p);
        draw(false);
        if (plr->number() != 1) mDeskView->mySleep(2); else mDeskView->update();
        eGameBid bb;
        bb = playerBids[curBidIdx] = plr->moveBidding(playerBids[curBidIdx%3+1], playerBids[(curBidIdx+1)%3+1]);
        qDebug() << "bid:" << sGameName(bb) << bb;
		QString message;
		if (bb == whist) message = tr("whist");
		else if (bb == gtPass) message = tr("pass");
		else if (bb == g65) message = tr("6 no trump");
		else if (bb == g75) message = tr("7 no trump");
		else if (bb == g85) message = tr("8 no trump");
		else if (bb == g95) message = tr("9 no trump");
		else if (bb == g105) message = tr("10 no trump");
		else if (bb == g86) message = tr("Misere");
		else message = sGameName(bb);  
        plr->setMessage(message);
        //bids4win[plrCounter] = playerBids[curBidIdx+1];
      }// else plr->setMessage("PASS");
      ++plrCounter;
      curBidIdx = curBidIdx%3+1;

      if ((playerBids[1] != undefined && playerBids[2] != undefined && playerBids[3] != undefined) &&
          ((playerBids[1] == gtPass?1:0)+(playerBids[2] == gtPass?1:0)+(playerBids[3] == gtPass?1:0) >= 2)) break;
    }
    mPlayerHi = 0;
    draw(false); //mDeskView->mySleep(0);

    // calculate max game
    for (int i = 1; i <= 3; i++) {
      if (playerBids[0] < playerBids[i]) playerBids[0] = playerBids[i];
    }

    // deliver talon and choose final bid (or pass-out if no player)
    mPlayerActive = 0;
    mPlayingRound = true;
    if (playerBids[0] != gtPass) {
      // no pass-out
      optPassCount = 0;
	  // who made maximal bid
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
          // show talon
          WrapCounter tmpPlayersCounter(1, 3);
          Player *PassOrVistPlayers;
          int PassOrVist = 0, nPassOrVist = 0;
          mOnDeskClosed = false;
          mCardsOnDesk[2] = mDeck.at(30);
          mCardsOnDesk[3] = mDeck.at(31);
          
		  // trick with gCurrentGame - shows game on bidboard
          gCurrentGame = playerBids[0];
          draw();
          //drawBidWindows(bids4win, 0);
		  if (tmpg->number() != 1)
				kpref->showHint(tr("Try to remember the cards"));
		  if (optPrefClub)
		  	mDeskView->mySleep(-1);
		  else
		  	mDeskView->mySleep(4);
		  kpref->clearHint();
          // deal talon
          tmpg->dealCard(mDeck.at(30));
          tmpg->dealCard(mDeck.at(31));
          mDeskView->animateDeskToPlayer(mPlayerActive, mCardsOnDesk, optTakeAnim); // will clear mCardsOnDesk[]
          draw();

          //bids4win[0] = bids4win[1] = bids4win[2] = bids4win[3] = undefined;
          // throw away
		  eGameBid maxBid = gCurrentGame;
          if (tmpg->myGame() != g86) {		//  not misere
            // not misere
            nCurrentMove.nValue = i;
            draw(false);
			if (mPlayerActive == 1)
				kpref->showHint(tr("Select two cards to drop"));
            else
				mDeskView->mySleep(2);
            playerBids[0] = gCurrentGame = tmpg->dropForGame();
			kpref->clearHint();
          } else {	// playing misere
            // show all cards
            int tempint = nCurrentMove.nValue;
            int nVisibleState = tmpg->invisibleHand();
            tmpg->setInvisibleHand(false);
            draw(false);
            if (tmpg->number() != 1)
				kpref->showHint(tr("Try to remember the cards"));
            /*else 
				tmpg->setMessage(tr("Misere"));*/
            // wait for event
			if (optPrefClub)
            	mDeskView->mySleep(-1);
			else
		  		mDeskView->mySleep(4);
            draw(false);

            tmpg->setInvisibleHand(nVisibleState);
            nCurrentMove.nValue = tmpg->number();

            if (mPlayerActive != 1) 
				mDeskView->mySleep(2);

            playerBids[0] = gCurrentGame = tmpg->dropForMisere();
			kpref->clearHint();
            nCurrentMove.nValue = tempint;
          }		  	
          tmpPlayersCounter.nValue = i;

          // bid
          player(tmpPlayersCounter)->setMessage(tr(bidMessage(gCurrentGame)));

		if (gCurrentGame == withoutThree) {
			gCurrentGame = maxBid;
			tmpg->gotPassPassTricks(gameTricks(tmpg->myGame())-3);
			dlogf("clean out!\n");
            goto LabelRecordOnPaper;
		}
		else {
          // pass or whist?
          ++tmpPlayersCounter;
          PassOrVistPlayers = player(tmpPlayersCounter);
          PassOrVistPlayers->setMyGame(undefined);
          

          // choice of the first player
          int firstPW = tmpPlayersCounter.nValue;
          mPlayerHi = firstPW;
		  if ((gCurrentGame != g86) && !(!opt10Whist && gCurrentGame>=101 && gCurrentGame<=105)) {
			player(tmpPlayersCounter)->setMessage(tr("thinking..."));
			draw(false);
			if (firstPW != 1) mDeskView->mySleep(2);
		  }
          //PassOrVist = PassOrVistPlayers->moveFinalBid(gCurrentGame, gtPass, 0);
		  PassOrVist = PassOrVistPlayers->moveFinalBid(gCurrentGame, whist, nPassCounter);
          nPassOrVist = tmpPlayersCounter.nValue;
          if (PassOrVistPlayers->myGame() == gtPass) {
            nPassCounter++;
            player(tmpPlayersCounter)->setMessage(tr("pass"));
          } else if (PassOrVistPlayers->myGame() == g86catch)
		    player(tmpPlayersCounter)->setMessage("");
		  else
		    player(tmpPlayersCounter)->setMessage(tr("whist"));
		  draw(false);
          //bids4win[1] = PassOrVistPlayers->myGame();

          // choice of the second player
          ++tmpPlayersCounter;
          int nextPW = tmpPlayersCounter.nValue;
          mPlayerHi = nextPW;
		  draw(false);
          PassOrVistPlayers = player(tmpPlayersCounter);
          PassOrVistPlayers->setMyGame(undefined);
		  if ((gCurrentGame != g86) && !(!opt10Whist && gCurrentGame>=101 && gCurrentGame<=105)) {
			player(tmpPlayersCounter)->setMessage(tr("thinking..."));
			draw(false);
		  	if (nextPW != 1) mDeskView->mySleep(2);
		  }
          //PassOrVistPlayers->moveFinalBid(gCurrentGame, PassOrVist, nPassOrVist);
		  //qDebug() << nPassCounter;
		  PassOrVistPlayers->moveFinalBid(gCurrentGame, PassOrVist, nPassCounter);
          if (PassOrVistPlayers->myGame() == gtPass) {
            nPassCounter++;
            player(tmpPlayersCounter)->setMessage(tr("pass"));
          } else if (PassOrVistPlayers->myGame() == g86catch)
		    player(tmpPlayersCounter)->setMessage("");
		  else if (PassOrVistPlayers->myGame() == halfwhist) 
		    player(tmpPlayersCounter)->setMessage(tr("half of whist"));
		  else
		    player(tmpPlayersCounter)->setMessage(tr("whist"));
		  draw(false);
          //bids4win[2] = PassOrVistPlayers->myGame();

		  // if halfwhist, choice of the first player again
		  if (player(nextPW)->myGame() == halfwhist) {
			--tmpPlayersCounter;
		  	mPlayerHi = firstPW;
			PassOrVistPlayers = player(firstPW);
			PassOrVistPlayers->setMessage(tr("thinking..."));
			draw(false);			
          	if (firstPW != 1) mDeskView->mySleep(2);
		  	PassOrVist = PassOrVistPlayers->moveFinalBid(gCurrentGame, gtPass, 0);	// no more halfwhists!
          	//nPassOrVist = tmpPlayersCounter.nValue;
          	if (PassOrVistPlayers->myGame() == gtPass) {
            	//nPassCounter++;
            	player(firstPW)->setMessage(tr("pass"));
          	}
		  	else {
		    	player(firstPW)->setMessage(tr("whist"));
				//wasHalfWhist = false;
				player(nextPW)->setMyGame(gtPass);
			}
			draw(false);
		  }
		  
          mPlayerHi = 0;

          // choice made
          draw(false);
		  if (optPrefClub)
          	mDeskView->mySleep(-1);
		  else
		  	mDeskView->mySleep(2);
		  player(1)->setMessage("");
		  player(2)->setMessage("");
          player(3)->setMessage("");

          gnS = sGameName(gCurrentGame);
          gnS.prepend("game: ");
          gnS += "; player: ";
          gnS += QString::number(mPlayerActive);
          dlogS(gnS);
          //!DUMP OTHERS!

      if (player(nextPW)->myGame() == halfwhist) {
        tmpg->gotPassPassTricks(gameTricks(tmpg->myGame()));
        int n = gameWhists(tmpg->myGame());
        Q_ASSERT(n > 1);  // Half whist on game >= 8 is illegal
        player(nextPW)->gotPassPassTricks(n/2);
        dlogf("halfwhist!\n");
        goto LabelRecordOnPaper;
      }
          if (nPassCounter == 2) {
            // two players passed
            tmpg->gotPassPassTricks(gameTricks(tmpg->myGame()));
            dlogf("clean out!\n");
            goto LabelRecordOnPaper;
          } else {
			  
            // Opened or closed cards
            /*for (int ntmp = 2 ; ntmp <= 3 ; ntmp++) {
              Player *Gamer4Open;
              Gamer4Open = player(ntmp);
              if (nPassCounter || gCurrentGame == g86) {
                // if not 2 whists
                if (Gamer4Open->myGame() == gtPass || Gamer4Open->myGame() == whist || Gamer4Open->myGame() == g86catch)
                  Gamer4Open->setInvisibleHand(false);
            }
				  
            }*/

			// On misere or 10 check play with opened cards
			if ((gCurrentGame == g86)|| (!opt10Whist && gCurrentGame>=101 && gCurrentGame<=105)) {
				player(2)->setInvisibleHand(false);
				player(3)->setInvisibleHand(false);
			}

			// If one whist, whister chooses closed or opened cards
			else if ((nPassCounter == 1)) {
				//qDebug() << "passcount=" << nPassCounter << endl;
				Closed_Whist = false;
				for (int n=1; n<=3; n++)
					if (player(n)->myGame() == whist) {
    					player(n)->setMessage(tr("thinking..."));
						draw(false);
						if (n != 1)
							mDeskView->mySleep(1);
						Closed_Whist = player(n)->chooseClosedWhist();
						if (Closed_Whist)
							player(n)->setMessage(tr("close"));
						else
							player(n)->setMessage(tr("open"));
						draw(false);
			          	mDeskView->mySleep(1);
					}

				// if closed whist chosen, no hand become opened
				// otherwise, whist and pass players open cards
				if (!Closed_Whist) {
					for (int n=2; n<=3; n++)
						if ((player(n)->myGame() == whist) || (player(n)->myGame() == gtPass))
							player(n)->setInvisibleHand(false);
				}
				/*if (player(2)->myGame() == gtPass)
					invis = player(3)->chooseClosedWhist();
				else
					invis = player(2)->chooseClosedWhist();
					player(2)->setInvisibleHand(invis);
					player(3)->setInvisibleHand(invis);*/
				player(1)->setMessage("");
    			player(2)->setMessage("");
    			player(3)->setMessage("");
			
			}
          }
          break;
        }
	   }
      } // who made maximal bid
    }  else {
      // pass out
      dlogf("game: pass-out");

      optPassCount++;
      mPlayerActive = 0;
      mPlayerHi = 0;
      playerBids[0] = gCurrentGame = raspass;
      player(1)->setMyGame(raspass);
      player(2)->setMyGame(raspass);
      player(3)->setMyGame(raspass);
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
      mOnDeskClosed = false;
      draw();
	  if (optPrefClub)
	  	mDeskView->mySleep(-1);
	  else
		mDeskView->mySleep(2);
    }

    player(1)->setMessage("");
    player(2)->setMessage("");
    player(3)->setMessage("");
    draw(false);

    // game (10 moves)
	mBiddingDone = true;
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
      draw(false);
	  player(mPlayerHi)->setMessage(tr("thinking..."));
	  draw();
      mDeskView->mySleep(0);
      if (gCurrentGame == raspass && (i == 1 || i == 2)) {
        Card *tmp4show, *ptmp4rpass;
        tmp4show = mDeck.at(29+i);
        ptmp4rpass = tmp4show;//newCard(6, tmp4show->suit());
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
      player(mPlayerHi)->setMessage(tr("thinking..."));
      draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = mSecondCard = makeGameMove(0, mFirstCard, false);
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (2nd) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      mPlayerHi = nCurrentMove.nValue;
      player(mPlayerHi)->setMessage(tr("thinking..."));
      draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = mThirdCard = makeGameMove(mFirstCard, mSecondCard, false);
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (3rd) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      draw();
	  if (optPrefClub)
      	mDeskView->mySleep(-1);
	  else
	  	mDeskView->mySleep(1);

      //////////////////////////////////////////////////////////////////
      // Warning! Dirty hack!
      //
      // Assuming trump to be playerBids[0]-(playerBids[0]/10)*10 works
      // ONLY because of special choice of numerical constants for no trump
      // games!
      // See prfconst.h for more details
      nPl = whoseTrick(mFirstCard, mSecondCard, mThirdCard, playerBids[0]-(playerBids[0]/10)*10)-1;
      
      nCurrentMove = nCurrentMove+nPl;
      mDeskView->animateDeskToPlayer(nCurrentMove.nValue, mCardsOnDesk, optTakeAnim); // will clear mCardsOnDesk[]
      draw();
      tmpg = player(nCurrentMove);
      tmpg->gotTrick();
      draw(false);
    }
LabelRecordOnPaper:
				
    mPlayingRound = false;
    ++nCurrentStart;
    //   
    for (int i = 1; i <= 3;i++ ) {
      Player *tmpg = player(i);
      Player *plr = player(mPlayerActive);
      int RetValAddRec = tmpg->mScore.recordScores(gCurrentGame, tmpg->myGame(),
        plr ? plr->tricksTaken() : 0, tmpg->tricksTaken(), plr ? mPlayerActive : 0,
        i, 2-nPassCounter);
      if (RetValAddRec) {
        int index = playerWithMaxPool();
        if (index) {
          tmpg->mScore.whistsAdd(index, i, RetValAddRec); //   
          RetValAddRec = player(index)->mScore.poolAdd(RetValAddRec); //   
          if (RetValAddRec) {
            index = playerWithMaxPool();
            if (index) {
              tmpg->mScore.whistsAdd(index, i, RetValAddRec); //   
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
	
	//amnesty for pass out (don't matter for score)
	if (gCurrentGame == raspass) {
		int mm=10;
		int m=0;
		for (int i=1; i<=3; i++) {			
			m = player(i)->tricksTaken();
			if (m<mm) mm = m;
		}
		if (mm !=0) 
			for (int i=1; i<=3; i++) 				
				player(i)->mScore.mountainAmnesty(mm);
	}
	closePool();
    //    --        . 

    //   --     
    CardList tmplist[3];
    if (nPassCounter != 2) {
      //  
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(f);
        tmplist[f-1] = plr->mCards;
        plr->mCards = plr->mCardsOut;
        plr->setInvisibleHand(false);
      }
    }
    mPlayingRound = true;
    draw();
    mDeskView->drawPool();
/*	if (optPrefClub)
    	mDeskView->mySleep(-1);
	else
		mDeskView->mySleep(4);*/
    mPlayingRound = false;
    if (nPassCounter != 2) {
      //  
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(f);
        plr->mCardsOut = plr->mCards;
        plr->mCards = tmplist[f-1];
      }
    }
  } // end of pool
  mDeskView->update();
  emit gameOver();

  mGameRunning = false;
}
