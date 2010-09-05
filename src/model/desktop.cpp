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

#include <limits.h>

#include "prfconst.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTime>

#include "aialphabeta.h"
#include "aiplayer.h"
#include "baser.h"
#include "debug.h"
#include "deskview.h"
#include "ncounter.h"
#include "player.h"
#include "desktop.h"
#include "scoreboard.h"
#include "human.h"

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

const char * sGameName (eGameBid game)
{
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


void PrefModel::initPlayers() {
  mPlayers.clear();
  mPlayers << 0; // 0th player is nobody

  mPlayers << new HumanPlayer(1, this);
  if (!optAlphaBeta1)
    mPlayers << new AiPlayer(2, this);
  else 
    mPlayers << new AlphaBetaPlayer(2, this);
  if (!optAlphaBeta2)
    mPlayers << new AiPlayer(3, this);
  else 
    mPlayers << new AlphaBetaPlayer(3, this);
    
  mPlayers[1]->setNick(optHumanName);  
  mPlayers[2]->setNick(optPlayerName1);
  mPlayers[3]->setNick(optPlayerName2);
}


PrefModel::PrefModel (DeskView *aDeskView) : QObject(0), mPlayingRound(false),
                            mGameRunning(false), mDeskView(aDeskView), m_trump(0),
                            optStalingrad(false), opt10Whist(false), optWhistGreedy(true),
                            optMaxPool(10), optQuitAfterMaxRounds(false), optMaxRounds(-1),
                            optAggPass(false), optPassCount(0), optWithoutThree(true),
 optPlayerName1("Player 1"),
 optAlphaBeta1(false),
 optPlayerName2("Player 2"),
 optAlphaBeta2(false),
 m_closedWhist(false),
 m_keepLog(true)
{
  #if defined Q_WS_X11 || defined Q_WS_QWS || defined Q_WS_MAC
	QString optHumanName = getenv("USER");
#elif defined Q_WS_WIN
  QString optHumanName = ""; // get user name from WinAPI
#else
	QString optHumanName = "";
#endif
  nCurrentStart.nValue = nCurrentMove.nValue = (qrand()%3)+1;
  nCurrentStart.nMin = nCurrentMove.nMin = 1;
  nCurrentStart.nMax = nCurrentMove.nMax = 3;
  mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard=0;
  mOnDeskClosed = false;
  initPlayers();
}


PrefModel::~PrefModel () {
  foreach (Player *p, mPlayers) delete p;
  mPlayers.clear();
}

int PrefModel::gameWhists (eGameBid gType) const
{
  if (gType >= g71 && gType <= 75) return 2;
  if (gType >= g81 && gType <= 85) return 1;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 1;
  if (gType >= g101 && gType <= 105) {
	  if (opt10Whist)
	  	return 1;
	  else
	  	return 0;
  }
  return 4;
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
  }
  // svou pulu sebe v visti
  for (i = 1; i <= 3;i++) {
    R[i].leftWh += R[i].pool*10/3;
    R[i].rightWh += R[i].pool*10/3;
  }
  // goru - drugim na sebya
  for (i = 1; i <= 3; i++) {
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
    player(i)->mScore.setScore(R[i].leftWh+R[i].rightWh-R[i1].rightWh-R[i2].leftWh);
  }
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
  if (((player(1)->myGame() == whist && !m_closedWhist) || player(1)->myGame() == g86catch) &&
      !curPlr->isHuman() && (curPlr->myGame() == gtPass || curPlr->myGame() == g86catch)) {
    plr = player(1)->create(nCurrentMove.nValue, this);
  }

  // 2. Current player passes
  // if AI whists with open cards, it makes move instead of human or other AI
  else if (curPlr->myGame() == gtPass && !m_closedWhist) {
    if (player(2)->myGame() == whist) {
      plr = player(2)->create(nCurrentMove.nValue, this);
    }
    else if (player(3)->myGame() == whist) {
      plr = player(3)->create(nCurrentMove.nValue, this);
    }
  #ifndef QT_NO_DEBUG
    else
    {      
      qDebug() << player(1)->myGame() << player(2)->myGame() << player(3)->myGame();
      qFatal("Something went wrong!");
    }
  #endif
  }

  if (plr) {
    // Do player logic swap
    *plr = *curPlr;
    mPlayers[nCurrentMove.nValue] = plr;
    qDebug() << plr->type() << " plays for " << nCurrentMove.nValue;
    res = plr->makeMove(lMove, rMove, player(nextPlayer(nCurrentMove)),
      player(previousPlayer(nCurrentMove)), isPassOut);
    *curPlr = *plr;
    mPlayers[nCurrentMove.nValue] = curPlr;
    delete plr;
  } else {
    // No swaps, current player makes move himself
    res = curPlr->makeMove(lMove, rMove, player(nextPlayer(nCurrentMove)),
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


Player *PrefModel::player (int num) {
  Q_ASSERT(num >= 1 && num <= 3);
  return mPlayers[num];
}


inline Player *PrefModel::player (const WrapCounter &cnt) {
  Q_ASSERT(cnt.nValue >= 1 && cnt.nValue <= 3);
  return player(cnt.nValue);
}


Card *PrefModel::cardOnDesk(int num) const
{
  Q_ASSERT(num >= 0 && num <= 3);
  return mCardsOnDesk[num];
}


bool PrefModel::loadGame (const QString & name)  {
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


void PrefModel::showMoveImpossible(const bool canRetry)
{
  emit showHint(tr("This move is impossible"));
  if (!canRetry)
    qFatal("Somebody is plaing unfair!");
    ///@todo redirect message to view
}

void PrefModel::showMoveHint()
{
  if (mBiddingDone)
    emitShowHint(tr("Your move"));
  else
    emitShowHint(tr("Select two cards to drop"));
}

bool PrefModel::saveGame (const QString & name)  {
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
  initPlayers();
  // Randomize
  const QTime t = QTime::currentTime();
  qsrand((double)t.minute()*t.msec()/(t.second()+1)*UINT_MAX/3600);

  mGameRunning = true;
  emit clearHint();
  // while !end of pool
  int roundNo = 0;
  while (!(player(1)->mScore.pool() >= optMaxPool &&
           player(2)->mScore.pool() >= optMaxPool &&
           player(3)->mScore.pool() >= optMaxPool)) {
    // Number of current round
    roundNo++;
    if (optQuitAfterMaxRounds && roundNo > optMaxRounds)
      break;

    WrapCounter plrCounter(1, 1, 3);
    mPlayingRound = false;
    mBiddingDone = false;
    int nPassCounter = 0; // number of passes
    int elapsedTime = 0;
    QTime pt;
    playerBids[3] = playerBids[2] = playerBids[1] = playerBids[0] = undefined;
    mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard = 0;
    mDeck.newDeck();
    mDeck.shuffle();

#ifndef QT_NO_DEBUG
  {
    QFile dbgD("dbg.deck");
    QFile dbgDT("dbg.deck.txt");
    if (dbgDT.open(QIODevice::ReadOnly)) {
      mDeck.clear();
      for (int f = 0; f < 32; f++) {
        QByteArray ba(dbgDT.readLine());
        Card *c = cardFromName(ba.constData());
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
      Q_ASSERT(result);
      nCurrentStart.nValue = 1;
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
#endif

    player(1)->clear();
    player(2)->clear();
    player(3)->clear();
    mPlayerHi = 0;
    m_currentGame = undefined;

  // Starter of bidding
  plrCounter = nCurrentStart;
  for (int i=1; i<=3; i++)
    player(i)->setCurrentStart(i == nCurrentStart.nValue);

  CardList tmpDeck; int cc = nextPlayer(nCurrentStart), tPos = 0, tNo = 0;
  mOnDeskClosed = true;

  /// @todo Probably this block should go to DeskView
    mDeskView->draw();
    {
	  if (mDeskView->optDealAnim)
	  	player(1)->setInvisibleHand(true);
      for (int f = 0; f < 15; f++) {
        if (f == 4) {
          // talon
          tNo = tPos;
          mCardsOnDesk[2] = mDeck.at(tPos++);
          if (mDeskView->optDealAnim) { mDeskView->draw(); mDeskView->aniSleep(40); }
          mCardsOnDesk[3] = mDeck.at(tPos++);
          if (mDeskView->optDealAnim) { mDeskView->draw(); mDeskView->aniSleep(40); }
        }
        Player *plr = player(cc); cc = (cc%3)+1;
        plr->dealCard(mDeck.at(tPos)); tmpDeck << mDeck.at(tPos++);
        //if (optDealAnim) { draw(); mDeskView->aniSleep(40); }
        plr->dealCard(mDeck.at(tPos)); tmpDeck << mDeck.at(tPos++);
        if (mDeskView->optDealAnim) { mDeskView->draw(); mDeskView->aniSleep(80); }
        if (mDeskView->optDealAnim) { 
          if (f%3 == 2) mDeskView->aniSleep(200);
        }
      }
	  mDeskView->aniSleep(200);
	  if (mDeskView->optDealAnim)
	  	player(1)->setInvisibleHand(false);
      tmpDeck << mDeck.at(tNo++);
      tmpDeck << mDeck.at(tNo);
      if (tmpDeck.count() != 32) abort();
      mDeck = tmpDeck;
      mDeskView->draw();
    }
    /*if (!mDeskView->optDealAnim) {
      mDeskView->draw();
//      mDeskView->update();
      mDeskView->mySleep(0);
    }*/
  // end of block to move

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

    int curBidIdx = 1;
    forever {
      //      
      Player *currentPlayer = player(plrCounter);
      mPlayerHi = plrCounter.nValue;
      if (playerBids[curBidIdx] != gtPass) {
        currentPlayer->setMessage(tr("thinking..."));
        //drawBidWindows(bids4win, p);
        mDeskView->draw(false);
        if (currentPlayer->number() != 1)
            mDeskView->mySleep(2);
        else
            mDeskView->update();
        const eGameBid bid = playerBids[curBidIdx]
                            = currentPlayer->makeBid(playerBids[curBidIdx%3+1], playerBids[(curBidIdx+1)%3+1]);
        qDebug() << "bid:" << sGameName(bid) << bid;
        QString message;
		if (bid == whist) message = tr("whist");
		else if (bid == gtPass) message = tr("pass");
		else if (bid == g65) message = tr("6 no trump");
		else if (bid == g75) message = tr("7 no trump");
		else if (bid == g85) message = tr("8 no trump");
		else if (bid == g95) message = tr("9 no trump");
		else if (bid == g105) message = tr("10 no trump");
		else if (bid == g86) message = tr("Misere");
		else message = sGameName(bid);
		currentPlayer->setMessage(message);
        mDeskView->draw();
      }// else plr->setMessage("PASS");
      ++plrCounter;
      curBidIdx = curBidIdx%3+1;

      if ((playerBids[1] != undefined && playerBids[2] != undefined && playerBids[3] != undefined) &&
          ((playerBids[1] == gtPass?1:0)+(playerBids[2] == gtPass?1:0)+(playerBids[3] == gtPass?1:0) >= 2)) break;
    }
    mPlayerHi = 0;
    mDeskView->draw(false); //mDeskView->mySleep(0);

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
        Player *currentPlayer = player(i);
        if (currentPlayer->myGame() == playerBids[0]) {
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
          WrapCounter passOrWhistPlayersCounter(1, 3);
          Player *PassOrVistPlayers;
          int PassOrVist = 0; //, nPassOrVist = 0;
          mOnDeskClosed = false;
          mCardsOnDesk[2] = mDeck.at(30);
          mCardsOnDesk[3] = mDeck.at(31);
          
		  // trick with gCurrentGame - shows game on bidboard
          m_currentGame = playerBids[0];
          mDeskView->draw();
          //drawBidWindows(bids4win, 0);
          if (currentPlayer->number() != 1)
              emit showHint(tr("Try to remember the cards"));
      mDeskView->longWait(2);
		  emit clearHint();
		  // deal talon
          currentPlayer->dealCard(mDeck.at(30));
          currentPlayer->dealCard(mDeck.at(31));
          Card *cAni[4];
          for (int f = 0; f < 4; f++) {
            cAni[f] = mCardsOnDesk[f];
            mCardsOnDesk[f] = 0;
          }
          mDeskView->animateTrick(mPlayerActive, cAni); // will clear mCardsOnDesk[]

          // throw away
          eGameBid maxBid = m_currentGame;
		  if (currentPlayer->myGame() != g86) {		//  not misere
            // not misere
            nCurrentMove.nValue = i;
            mDeskView->draw();
            if (mPlayerActive == 1)
                emit showHint(tr("Select two cards to drop"));
            else
				mDeskView->mySleep(2);
            playerBids[0] = m_currentGame = currentPlayer->dropForGame();
			emit clearHint();
          } else {	// playing misere
            // show all cards
            int tempint = nCurrentMove.nValue;
            int nVisibleState = currentPlayer->invisibleHand();
            currentPlayer->setInvisibleHand(false);
            mDeskView->draw(false);
            if (!currentPlayer->isHuman())
                emit showHint(tr("Try to remember the cards"));
            /*else 
				tmpg->setMessage(tr("Misere"));*/
            mDeskView->longWait(2);
            mDeskView->draw(false);

            currentPlayer->setInvisibleHand(nVisibleState);
            nCurrentMove.nValue = currentPlayer->number();

            if (mPlayerActive != 1) 
				mDeskView->mySleep(2);

            playerBids[0] = m_currentGame = currentPlayer->dropForMisere();
			emit clearHint();
			nCurrentMove.nValue = tempint;
          }		  	
          passOrWhistPlayersCounter.nValue = i;

          // bid
          player(passOrWhistPlayersCounter)->setMessage(tr(bidMessage(m_currentGame)));
          mDeskView->draw();

        if (m_currentGame == withoutThree) {
            m_currentGame = maxBid;
			currentPlayer->gotPassPassTricks(gameTricks(currentPlayer->myGame())-3);
            dlogf("clean out!\n");
            goto LabelRecordOnPaper;
		}
		else {
          // pass or whist?
          ++passOrWhistPlayersCounter;
          PassOrVistPlayers = player(passOrWhistPlayersCounter);
          PassOrVistPlayers->setMyGame(undefined);
          

          // choice of the first player
          int firstWhistPlayer = passOrWhistPlayersCounter.nValue;
          mPlayerHi = firstWhistPlayer;
          if ((m_currentGame != g86) && !(!opt10Whist && m_currentGame>=101 && m_currentGame<=105)) {
			player(passOrWhistPlayersCounter)->setMessage(tr("thinking..."));
			mDeskView->draw(false);
			if (firstWhistPlayer != 1) mDeskView->mySleep(2);
		  }
          PassOrVist = PassOrVistPlayers->makeFinalBid(m_currentGame, whist, nPassCounter);
          if (PassOrVistPlayers->myGame() == gtPass) {
            nPassCounter++;
            player(passOrWhistPlayersCounter)->setMessage(tr("pass"));
          } else if (PassOrVistPlayers->myGame() == g86catch)
            player(passOrWhistPlayersCounter)->setMessage("");
		  else
			player(passOrWhistPlayersCounter)->setMessage(tr("whist"));
		  mDeskView->draw(false);


          // choice of the second player
          ++passOrWhistPlayersCounter;
          int secondWhistPlayer = passOrWhistPlayersCounter.nValue;
          mPlayerHi = secondWhistPlayer;
		  mDeskView->draw(false);
		  PassOrVistPlayers = player(passOrWhistPlayersCounter);
          PassOrVistPlayers->setMyGame(undefined);
          if ((m_currentGame != g86) && !(!opt10Whist && m_currentGame>=101 && m_currentGame<=105)) {
			player(passOrWhistPlayersCounter)->setMessage(tr("thinking..."));
			mDeskView->draw(false);
			if (secondWhistPlayer != 1) mDeskView->mySleep(2);
		  }
          PassOrVistPlayers->makeFinalBid(m_currentGame, PassOrVist, nPassCounter);
          if (PassOrVistPlayers->myGame() == gtPass) {
            nPassCounter++;
            player(passOrWhistPlayersCounter)->setMessage(tr("pass"));
          } else if (PassOrVistPlayers->myGame() == g86catch)
            player(passOrWhistPlayersCounter)->setMessage("");
		  else if (PassOrVistPlayers->myGame() == halfwhist) 
			player(passOrWhistPlayersCounter)->setMessage(tr("half of whist"));
		  else
			player(passOrWhistPlayersCounter)->setMessage(tr("whist"));
		  mDeskView->draw(false);

		  // if halfwhist, choice of the first player again
		  if (player(secondWhistPlayer)->myGame() == halfwhist) {
			--passOrWhistPlayersCounter;
			mPlayerHi = firstWhistPlayer;
			PassOrVistPlayers = player(firstWhistPlayer);
			PassOrVistPlayers->setMessage(tr("thinking..."));
			mDeskView->draw(false);			
			if (firstWhistPlayer != 1) mDeskView->mySleep(2);
            PassOrVist = PassOrVistPlayers->makeFinalBid(m_currentGame, gtPass, 0);	// no more halfwhists!
          	if (PassOrVistPlayers->myGame() == gtPass) {
                player(firstWhistPlayer)->setMessage(tr("pass"));
          	}
		  	else {
				player(firstWhistPlayer)->setMessage(tr("whist"));
				player(secondWhistPlayer)->setMyGame(gtPass);
			}
			mDeskView->draw(false);
		  }
		  
          mPlayerHi = 0;

          // choice made
          mDeskView->draw(false);
        mDeskView->longWait(1);
		  player(1)->setMessage("");
		  player(2)->setMessage("");
          player(3)->setMessage("");

          gnS = sGameName(m_currentGame);
          gnS.prepend("game: ");
          gnS += "; player: ";
          gnS += QString::number(mPlayerActive);
          dlogS(gnS);
          //!DUMP OTHERS!

      if (player(secondWhistPlayer)->myGame() == halfwhist) {
        currentPlayer->gotPassPassTricks(gameTricks(currentPlayer->myGame()));
        int n = gameWhists(currentPlayer->myGame());
        Q_ASSERT(n > 1);  // Half whist on game >= 8 is illegal
        player(secondWhistPlayer)->gotPassPassTricks(n/2);
        dlogf("halfwhist!\n");
        goto LabelRecordOnPaper;
      }
          if (nPassCounter == 2) {
            // two players passed
            currentPlayer->gotPassPassTricks(gameTricks(currentPlayer->myGame()));
            dlogf("clean out!\n");
            goto LabelRecordOnPaper;
          } else {

			// On misere or 10 check play with opened cards
            if ((m_currentGame == g86)|| (!opt10Whist && m_currentGame>=101 && m_currentGame<=105)) {
				player(2)->setInvisibleHand(false);
				player(3)->setInvisibleHand(false);
			}

			// If one whist, whister chooses closed or opened cards
			else if ((nPassCounter == 1)) {
				m_closedWhist = false;
				for (int n=1; n<=3; n++)
					if (player(n)->myGame() == whist) {
    					player(n)->setMessage(tr("thinking..."));
						mDeskView->draw(false);
						if (n != 1)
							mDeskView->mySleep(1);
						m_closedWhist = player(n)->chooseClosedWhist();
						if (m_closedWhist)
							player(n)->setMessage(tr("close"));
						else
							player(n)->setMessage(tr("open"));
						mDeskView->draw(false);
					}

				// if closed whist chosen, no hand become opened
				// otherwise, whist and pass players open cards
				if (!m_closedWhist) {
					for (int n=2; n<=3; n++)
						if ((player(n)->myGame() == whist) || (player(n)->myGame() == gtPass))
							player(n)->setInvisibleHand(false);
				}
				mDeskView->draw(false);
				mDeskView->mySleep(1);
			
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
      playerBids[0] = m_currentGame = raspass;
      player(1)->setMyGame(raspass);
      player(2)->setMyGame(raspass);
      player(3)->setMyGame(raspass);
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
      mOnDeskClosed = false;
      mDeskView->draw();
      mDeskView->longWait(1);
    }

    player(1)->setMessage("");
    player(2)->setMessage("");
    player(3)->setMessage("");
    mDeskView->draw(true);

    // game (10 moves)
	mBiddingDone = true;
    nCurrentMove = nCurrentStart;

      //////////////////////////////////////////////////////////////////
      // Warning! Dirty hack!
      //
      // Assuming trump to be playerBids[0]-(playerBids[0]/10)*10 works
      // ONLY because of special choice of numerical constants for no trump
      // games!
      // See prfconst.h for more details      

      m_trump = playerBids[0]-(playerBids[0]/10)*10;
      qDebug() << "Trump = " << m_trump;

    pt.restart();
    playingRound();
    elapsedTime = pt.elapsed();

LabelRecordOnPaper:

    mPlayingRound = false;
    ++nCurrentStart;

    ScoreBoard::calculateScore(this, nPassCounter);
    closePool();

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

    if(m_keepLog) {
        GameLogEntry entry;
        entry.game = m_currentGame;
        entry.player = mPlayerActive;
        for (int f=1; f<=3; f++) {
            const Player *plr = player(f);
            entry.score[f-1] = plr->mScore.score();
            entry.mountain[f-1] = plr->mScore.mountain();
            entry.pool[f-1] = plr->mScore.pool();
            entry.leftWhists[f-1] = plr->mScore.leftWhists();
            entry.rightWhists[f-1] = plr->mScore.rightWhists();
//            entry.cardList[f-1] = tmplist[f-1];
            entry.time = elapsedTime / 1000.0;
            for (int i=0; i<tmplist[f-1].size(); i++) {
              const Card *c = tmplist[f-1].at(i);
              if(c)
                entry.cardList[f-1] << *c;
            }

        }
        m_gameLog.append(entry);
    }

    mPlayingRound = true;
    mDeskView->draw();
    mDeskView->drawPool();
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

void PrefModel::playingRound()
{
  char xxBuf[1024];
  m_outCards.clear();
    for (int i = 1; i <= 10; i++) {
      Player *tmpg;
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = mFirstCard = mSecondCard = mThirdCard = 0;
      if (m_currentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;

      dlogf("------------------------\nmove #%i", i);
      for (int f = 1; f <= 3; f++) {
        Player *plr = player(nCurrentMove); ++nCurrentMove;
        xxBuf[0] = 0;
        dumpCardList(xxBuf, plr->mCards);
        dlogf("hand %i:%s", plr->number(), xxBuf);
      }

      mPlayerHi = nCurrentMove.nValue;
      mDeskView->draw(false);
	  player(mPlayerHi)->setMessage(tr("thinking..."));
	  mDeskView->draw();
      mDeskView->mySleep(0);
      if (m_currentGame == raspass && (i == 1 || i == 2)) {
        mCardsOnDesk[0] = mDeck.at(29+i);
        mDeskView->draw();
        mDeskView->mySleep(0);
        mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = makeGameMove(0, mDeck.at(29+i), true);
      } else {
        mCardsOnDesk[0] = 0;
        mCardsOnDesk[nCurrentMove.nValue] = mFirstCard = makeGameMove(0, 0, false);
      }
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (1st) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      mPlayerHi = nCurrentMove.nValue;
      player(mPlayerHi)->setMessage(tr("thinking..."));
      mDeskView->draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = mSecondCard = makeGameMove(0, mFirstCard, false);
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (2nd) player %i:%s", nCurrentMove.nValue, xxBuf);

      ++nCurrentMove;
      mPlayerHi = nCurrentMove.nValue;
      player(mPlayerHi)->setMessage(tr("thinking..."));
      mDeskView->draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = mThirdCard = makeGameMove(mFirstCard, mSecondCard, false);
      player(mPlayerHi)->setMessage("");

      xxBuf[0] = 0;
      cardName(xxBuf, mCardsOnDesk[nCurrentMove.nValue]);
      dlogf(" (3rd) player %i:%s", nCurrentMove.nValue, xxBuf);

      checkMoves();
      m_outCards << mCardsOnDesk[0] << mCardsOnDesk[1] << mCardsOnDesk[2] << mCardsOnDesk[3];

      ++nCurrentMove;
      mDeskView->draw();
      mDeskView->longWait(1);

      nCurrentMove = nCurrentMove
        + whoseTrick(mFirstCard, mSecondCard, mThirdCard, m_trump)-1;

      Card *cAni[4];
      for (int f = 0; f < 4; f++) {
        cAni[f] = mCardsOnDesk[f];
        mCardsOnDesk[f] = 0;
      }
      mDeskView->animateTrick(nCurrentMove.nValue, cAni); // will clear mCardsOnDesk[]
      tmpg = player(nCurrentMove);
      tmpg->gotTrick();
      mDeskView->draw(false);
    }
}

int PrefModel::trumpSuit () const
{
  return m_currentGame-(m_currentGame/10)*10;
}

bool PrefModel::checkMoves ()
{
  // Protect from ace in a sleeve
  for (int i=0; i<4; i++) {
    if(!mCardsOnDesk[i]) continue;
    if (m_outCards.contains(mCardsOnDesk[i])) {
      qFatal("Player %d is cheating!", i);
      return false;
    }
  }
  return true;
}
