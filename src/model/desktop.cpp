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
#include <QtCore/QTime>

#include "aialphabeta.h"
#include "aiplayer.h"
#include "baser.h"
#include "deskview.h"
#include "ncounter.h"
#include "player.h"
#include "desktop.h"
#include "scoreboard.h"
#include "human.h"

typedef struct
{
  int mount, pool, leftWh, rightWh;
} tScores;

QString sGameName(eGameBid game)
{
  static QHash<int, QString> gameNames;
  if (gameNames.isEmpty())
  {
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

const QString PrefModel::bidMessage(const eGameBid game)
{
        if (game == whist) return tr("whist");
        else if (game == gtPass) return tr("pass");
        else if (game % 10 == 5) return QString::number(game/10) + ' ' + tr("no trumps");
        else if (game == g86) return tr("Misere");
        else if (game == withoutThree) return tr("without three");
        else return sGameName(game);
}


static int whoseTrick (Card *p1, Card *p2, Card *p3, int trump) {
  Q_ASSERT(p1 != 0);
  Q_ASSERT(p2 != 0);
  Q_ASSERT(p3 != 0);
  // The next assert doesn't work because of dirty hack passing values > 4 here
  //Q_ASSERT((trump >= 0) && (trump <= 4)); // 0 is no trumps
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
static QString cardName (const Card *c)
{
  Q_ASSERT(c != 0);
  static const char *faceN[] = {" 7"," 8"," 9","10"," J"," Q"," K"," A"};
  static const char *suitN[] = {"S","C","D","H"};
  if (!c)
    return QString("...");
  int face = c->face(), suit = c->suit();
  if (face >= 7 && face <= FACE_ACE && suit >= 1 && suit <= 4)
    return QString().sprintf("%s%s", faceN[face - 7], suitN[suit - 1]);
  return QString("???");
}

/// @todo move to CardList class
static QString dumpCardList(const CardList &lst)
{
  QString dest;
  foreach(Card* c, lst.sorted().items())
  {
    if (!c)
      continue;
    dest += " ";
    dest += cardName(c);
  }
  return dest;
}

void PrefModel::initPlayers(bool i_saveScore)
{
  typedef QMap<int, ScoreBoard> ScoreBoards;
  ScoreBoards oldScore;
  foreach(Player* player, mPlayers)
  {
    if (!player)
      continue;
    if (i_saveScore)
      oldScore[player->number()] = player->mScore;
    delete player;
  }
  qDebug() << "clear players";
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
    
  qDebug() << "Set nicks";
  mPlayers[1]->setNick(optHumanName);
  mPlayers[2]->setNick(optPlayerName1);
  mPlayers[3]->setNick(optPlayerName2);

  if (i_saveScore)
  {
    ScoreBoards::iterator it = oldScore.begin();
    for(; it != oldScore.end(); ++it)
      if (mPlayers[it.key()])
        mPlayers[it.key()]->mScore = it.value();
  }
}

PrefModel::PrefModel (DeskView *aDeskView)
    : QObject(0)
    , mPlayingRound(false)
    , mGameRunning(false)
    , optStalingrad(false)
    , opt10Whist(false)
    , optWhistGreedy(true)
    , optMaxPool(10)
    , optQuitAfterMaxRounds(false)
    , optMaxRounds(-1)
    , optAggPass(false)
    , optPassCount(0)
    , optWithoutThree(true)
    , optPlayerName1("Player 1")
    , optPlayerName2("Player 2")
    , optAlphaBeta1(false)
    , optAlphaBeta2(false)
    , mDeskView(aDeskView)
    , m_trump(0)
    , m_closedWhist(false)
    , m_keepLog(true)
{
  optHumanName = getenv("USER");
  nCurrentStart.nValue = nCurrentMove.nValue = (qrand()%3)+1;
  nCurrentStart.nMin = nCurrentMove.nMin = 1;
  nCurrentStart.nMax = nCurrentMove.nMax = 3;
  mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
  mOnDeskClosed = false;
  initPlayers(false);
}


PrefModel::~PrefModel()
{
  foreach (Player *p, mPlayers)
    delete p;
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


void PrefModel::closePool()
{
  if(!mGameRunning)
    return;
  WrapCounter counter(1, 1, 3);
  int i;
  tScores R[4];
  for (i = 1; i <= 3; i++)
  {
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
  for (i = 1; i <= 3; i++)
  {
    counter.nValue = i;
    ++counter;
    R[counter.nValue].rightWh += R[i].mount*10/3;
    ++counter;
    R[counter.nValue].leftWh += R[i].mount*10/3;
  }
  // schitaem visti
  for (i = 1; i <= 3; i++)
  {
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
Card *PrefModel::makeGameMove (Card *lMove, Card *rMove, bool isPassOut)
{
  Card *res = 0;
  Player *curPlr = player(nCurrentMove);
  Player *plr = 0;
  // 1. Current player is not human
  // Human's move if current he whists with open cards or catches misere, and current player
  // passes or catches misere
  if (((player(1)->game() == whist && !m_closedWhist) || player(1)->game() == g86catch) &&
      !curPlr->isHuman() && (curPlr->game() == gtPass || curPlr->game() == g86catch))
  {
    plr = player(1)->create(nCurrentMove.nValue, this);
  }

  // 2. Current player passes
  // if AI whists with open cards, it makes move instead of human or other AI
  else if (curPlr->game() == gtPass && !m_closedWhist)
  {
    if (player(2)->game() == whist)
      plr = player(2)->create(nCurrentMove.nValue, this);
    else if (player(3)->game() == whist)
      plr = player(3)->create(nCurrentMove.nValue, this);
    else
    {      
      qDebug() << player(1)->game() << player(2)->game() << player(3)->game();
      qFatal("Something went wrong!");
    }
  }

  if (plr)
  {
    // Do player logic swap
    *plr = *curPlr;
    mPlayers[nCurrentMove.nValue] = plr;
    qDebug() << plr->type() << " plays for " << nCurrentMove.nValue;
    res = plr->makeMove(lMove, rMove, player(nextPlayer(nCurrentMove)),
      player(previousPlayer(nCurrentMove)), isPassOut);
    *curPlr = *plr;
    mPlayers[nCurrentMove.nValue] = curPlr;
    delete plr;
  }
  else
  {
    // No swaps, current player makes move himself
    res = curPlr->makeMove(lMove, rMove, player(nextPlayer(nCurrentMove)),
      player(previousPlayer(nCurrentMove)), isPassOut);
  }
  return res;
}


int PrefModel::playerWithMaxPool()
{
  int maxBullet = -1, currBullet = -1, res = 0;
  for (int i = 1; i <= 3; i++)
  {
    currBullet = player(i)->mScore.pool();
    if (maxBullet < currBullet && currBullet < optMaxPool )
    {
      maxBullet = currBullet;
      res = i;
    }
  }
  return res;
}


Player *PrefModel::player (int num)
{
  Q_ASSERT(num >= 1 && num <= 3);
  return mPlayers[num];
}


inline Player *PrefModel::player (const WrapCounter &cnt)
{
  Q_ASSERT(cnt.nValue >= 1 && cnt.nValue <= 3);
  return player(cnt.nValue);
}


Card *PrefModel::cardOnDesk(int num) const
{
  Q_ASSERT(num >= 0 && num <= 3);
  return mCardsOnDesk[num];
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

void PrefModel::runGame(bool i_saveScore /* = false */)
{
  qDebug() << "runGame";
  eGameBid playerBids[4];
  initPlayers(i_saveScore);
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
    mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
    mDeck.newDeck();
    mDeck.shuffle();

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
  // end of block to move

    qDebug() << "=========================================";
    qDebug() << "player " << nCurrentStart.nValue << " moves...";
    for (int f = 1; f <= 3; f++)
    {
      Player *plr = player(nCurrentStart);
      ++nCurrentStart;
      qDebug() << "hand " << plr->number() << ":" << dumpCardList(plr->mCards);
    }
    qDebug() << "talon: " << cardName(mDeck.at(30)) << " " << mDeck.at(31);

    emit gameChanged(tr("Bidding"));
    int curBidIdx = 1;
    forever
    {
      Player *currentPlayer = player(plrCounter);
      mPlayerHi = plrCounter.nValue;
      if (playerBids[curBidIdx] != gtPass)
      {
        currentPlayer->setThinking();
        mDeskView->draw(false);
        if (currentPlayer->number() != 1)
            mDeskView->mySleep(2);
        else
            mDeskView->update();
        const eGameBid bid = playerBids[curBidIdx]
                            = currentPlayer->makeBid(playerBids[curBidIdx%3+1], playerBids[(curBidIdx+1)%3+1]);
        qDebug() << "bid:" << bid << bidMessage(bid);
        currentPlayer->setMessage(bidMessage(bid));
        mDeskView->draw();
      }
      ++plrCounter;
      curBidIdx = curBidIdx%3+1;

      if ((playerBids[1] != undefined && playerBids[2] != undefined && playerBids[3] != undefined) &&
          ((playerBids[1] == gtPass?1:0)+(playerBids[2] == gtPass?1:0)+(playerBids[3] == gtPass?1:0) >= 2)) break;
    }
    mPlayerHi = 0;
    mDeskView->draw(false);

    // calculate max game
    for (int i = 1; i <= 3; i++)
    {
      if (playerBids[0] < playerBids[i])
          playerBids[0] = playerBids[i];
    }

    // deliver talon and choose final bid (or pass-out if no player)
    mPlayerActive = 0;
    mPlayingRound = true;
    if (playerBids[0] != gtPass)
    {
      // no pass-out
      optPassCount = 0;
	  // who made maximal bid
      for (int i = 1; i <= 3; i++)
      {
        Player *currentPlayer = player(i);
        if (currentPlayer->game() == playerBids[0])
        {
          qDebug() << "game: " << sGameName(playerBids[0]) << "; player: " << QString::number(i);

          mPlayerHi = i;
          player(i%3+1)->clearMessage();
          player((i+1)%3+1)->clearMessage();

          mPlayerActive = i;
          nPassCounter = 0;
          // show talon
          WrapCounter passOrWhistPlayersCounter(1, 3);
          Player *PassOrVistPlayers;
          int PassOrVist = 0;
          mOnDeskClosed = false;
          mCardsOnDesk[2] = mDeck.at(30);
          mCardsOnDesk[3] = mDeck.at(31);
          
		  // trick with gCurrentGame - shows game on bidboard
          m_currentGame = playerBids[0];
          mDeskView->draw();
          if (currentPlayer->number() != 1)
              emit showHint(tr("Try to remember the cards"));
          mDeskView->longWait(2);
		  emit clearHint();
		  // deal talon
          currentPlayer->takeDeskCard(mDeck.at(30));
          currentPlayer->takeDeskCard(mDeck.at(31));
          QCardList cAni;
          for (int f = 0; f < 4; f++)
          {
            if (mCardsOnDesk[f])
            {
              cAni.append(mCardsOnDesk[f]);
              mCardsOnDesk[f] = 0;
            }
          }
          mDeskView->animateCommunityCardTrick(mPlayerActive, cAni);

          // throw away
          eGameBid maxBid = m_currentGame;
          emitGameChanged(m_currentGame);
          if (currentPlayer->game() != g86)
          {
            // not misere
            nCurrentMove.nValue = i;
            mDeskView->draw();
            if (mPlayerActive == 1)
                emit showHint(tr("Select two cards to drop"));
            else
				mDeskView->mySleep(2);
            playerBids[0] = m_currentGame = currentPlayer->makeDrop();
			emit clearHint();
			emitGameChanged(m_currentGame);
          } else {	// playing misere
            // show all cards
            int tempint = nCurrentMove.nValue;
            int nVisibleState = currentPlayer->invisibleHand();
            currentPlayer->setInvisibleHand(false);
            mDeskView->draw(false);
            if (!currentPlayer->isHuman())
                emit showHint(tr("Try to remember the cards"));
            mDeskView->longWait(2);
            mDeskView->draw(false);

            currentPlayer->setInvisibleHand(nVisibleState);
            nCurrentMove.nValue = currentPlayer->number();

            if (mPlayerActive != 1) 
				mDeskView->mySleep(2);

            playerBids[0] = m_currentGame = currentPlayer->makeDrop();
			emit clearHint();
			nCurrentMove.nValue = tempint;
          }		  	
          passOrWhistPlayersCounter.nValue = i;

          // bid
          player(passOrWhistPlayersCounter)->setMessage(bidMessage(m_currentGame));
          mDeskView->draw();

        if (m_currentGame == withoutThree)
        {
            m_currentGame = maxBid;
            currentPlayer->gotPassPassTricks(gameTricks(currentPlayer->game())-3);
            qDebug() << "clean out!";
            goto LabelRecordOnPaper;
		}
        else
        {
          // pass or whist?
          ++passOrWhistPlayersCounter;
          PassOrVistPlayers = player(passOrWhistPlayersCounter);
          PassOrVistPlayers->setGame(undefined);
          

          // choice of the first player
          int firstWhistPlayer = passOrWhistPlayersCounter.nValue;
          mPlayerHi = firstWhistPlayer;
          if ((m_currentGame != g86)
            && !(!opt10Whist && m_currentGame>=101 && m_currentGame<=105)
            && !(optStalingrad && m_currentGame == g61)) {
            player(passOrWhistPlayersCounter)->setThinking();
			mDeskView->draw(false);
			if (firstWhistPlayer != 1) mDeskView->mySleep(2);
		  }
          PassOrVist = PassOrVistPlayers->makeFinalBid(m_currentGame, nPassCounter);
          if (PassOrVistPlayers->game() == gtPass) {
            nPassCounter++;
            player(passOrWhistPlayersCounter)->setMessage(tr("pass"));
          } else if (PassOrVistPlayers->game() == g86catch)
            player(passOrWhistPlayersCounter)->clearMessage();
		  else
			player(passOrWhistPlayersCounter)->setMessage(tr("whist"));
		  mDeskView->draw(false);


          // choice of the second player
          ++passOrWhistPlayersCounter;
          int secondWhistPlayer = passOrWhistPlayersCounter.nValue;
          mPlayerHi = secondWhistPlayer;
		  mDeskView->draw(false);
		  PassOrVistPlayers = player(passOrWhistPlayersCounter);
          PassOrVistPlayers->setGame(undefined);
          if ((m_currentGame != g86)
            && !(!opt10Whist && m_currentGame>=101 && m_currentGame<=105)
            && !(optStalingrad && m_currentGame == g61)) {
            player(passOrWhistPlayersCounter)->setThinking();
			mDeskView->draw(false);
            if (secondWhistPlayer != 1)
              mDeskView->mySleep(2);
		  }
          PassOrVistPlayers->makeFinalBid(m_currentGame, nPassCounter);
          if (PassOrVistPlayers->game() == gtPass) {
            nPassCounter++;
            player(passOrWhistPlayersCounter)->setMessage(tr("pass"));
          } else if (PassOrVistPlayers->game() == g86catch)
            player(passOrWhistPlayersCounter)->clearMessage();
          else if (PassOrVistPlayers->game() == halfwhist)
			player(passOrWhistPlayersCounter)->setMessage(tr("half of whist"));
		  else
			player(passOrWhistPlayersCounter)->setMessage(tr("whist"));
		  mDeskView->draw(false);

		  // if halfwhist, choice of the first player again
          if (player(secondWhistPlayer)->game() == halfwhist) {
			--passOrWhistPlayersCounter;
			mPlayerHi = firstWhistPlayer;
			PassOrVistPlayers = player(firstWhistPlayer);
            PassOrVistPlayers->setThinking();
			mDeskView->draw(false);			
			if (firstWhistPlayer != 1) mDeskView->mySleep(2);
            PassOrVist = PassOrVistPlayers->makeFinalBid(m_currentGame, 2);	// no more halfwhists!
            if (PassOrVistPlayers->game() == gtPass) {
                player(firstWhistPlayer)->setMessage(tr("pass"));
          	}
		  	else {
				player(firstWhistPlayer)->setMessage(tr("whist"));
                player(secondWhistPlayer)->setGame(gtPass);
			}
			mDeskView->draw(false);
		  }
		  
          mPlayerHi = 0;

          // choice made
          mDeskView->draw(false);
        mDeskView->longWait(1);
        player(1)->clearMessage();
        player(2)->clearMessage();
        player(3)->clearMessage();

        qDebug() << "game: " << sGameName(m_currentGame) << "; player: " << QString::number(mPlayerActive);

      if (player(secondWhistPlayer)->game() == halfwhist) {
        currentPlayer->gotPassPassTricks(gameTricks(currentPlayer->game()));
        int n = gameWhists(currentPlayer->game());
        Q_ASSERT(n > 1);  // Half whist on game >= 8 is illegal
        player(secondWhistPlayer)->gotPassPassTricks(n/2);
        qDebug() << "halfwhist!";
        goto LabelRecordOnPaper;
      }
          if (nPassCounter == 2)
          {
            // two players passed
            currentPlayer->gotPassPassTricks(gameTricks(currentPlayer->game()));
            qDebug() << "clean out!";
            goto LabelRecordOnPaper;
          }
          else
          {
			// On misere or 10 check play with opened cards
            if ((m_currentGame == g86)|| (!opt10Whist && m_currentGame>=101 && m_currentGame<=105)) {
				player(2)->setInvisibleHand(false);
				player(3)->setInvisibleHand(false);
			}

			// If one whist, whister chooses closed or opened cards
			else if ((nPassCounter == 1)) {
				m_closedWhist = false;
				for (int n=1; n<=3; n++)
                    if (player(n)->game() == whist) {
                        player(n)->setThinking();
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
                        if ((player(n)->game() == whist) || (player(n)->game() == gtPass))
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
      qDebug() << "game: pass-out";

      optPassCount++;
      mPlayerActive = 0;
      mPlayerHi = 0;
      playerBids[0] = m_currentGame = raspass;
      player(1)->setGame(raspass);
      player(2)->setGame(raspass);
      player(3)->setGame(raspass);
      mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3] = 0;
      mOnDeskClosed = false;
      mDeskView->draw();
      mDeskView->longWait(1);
    }

    player(1)->clearMessage();
    player(2)->clearMessage();
    player(3)->clearMessage();
    mDeskView->draw(true);

    // game (10 moves)
	mBiddingDone = true;
    nCurrentMove = nCurrentStart;

      //////////////////////////////////////////////////////////////////
      // Warning! Dirty hack!
      //
      // Assuming trump to be playerBids[0]-(playerBids[0]/10)*10 works
      // ONLY because of special choice of numerical constants for no trumps
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
        /// @todo why we need this tmplist?
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
            entry.whist[f-1] = (plr->game() == whist);
            entry.takes[f-1] = plr->tricksTaken();
            entry.score[f-1] = plr->mScore.score();
            entry.mountain[f-1] = plr->mScore.mountain();
            entry.pool[f-1] = plr->mScore.pool();
            entry.leftWhists[f-1] = plr->mScore.leftWhists();
            entry.rightWhists[f-1] = plr->mScore.rightWhists();
//            entry.cardList[f-1] = tmplist[f-1];
            entry.time = elapsedTime / 1000.0;
            for (int i=0; i<plr->mCards.size(); i++)
            {
              const Card *c = plr->mCards.at(i);
              if(c)
                entry.cardList[f-1] << *c;
            }

        }
        m_gameLog.append(entry);
    }

    mPlayingRound = true;
    mDeskView->draw();
    mDeskView->drawPool();
    saveGame( autoSaveSlotName() );
    mPlayingRound = false;
    if (nPassCounter != 2)
    {
      for (int f = 1; f <= 3; f++)
      {
        Player *plr = player(f);
        plr->mCardsOut = plr->mCards;
        plr->mCards = tmplist[f-1];
      }
    }
    emitGameChanged(zerogame);
  } // end of pool
  mDeskView->update();
  emit gameOver();

  mGameRunning = false;
}

void PrefModel::playingRound()
{
  Card *firstCard, *secondCard, *thirdCard;
  m_outCards.clear();
  for (int i = 1; i <= 10; i++)
  {
    Player *tmpg;
    mCardsOnDesk[0] = mCardsOnDesk[1] = mCardsOnDesk[2] = mCardsOnDesk[3]
             = firstCard = secondCard = thirdCard = 0;
    if (m_currentGame == raspass && (i >= 1 && i <= 3)) nCurrentMove = nCurrentStart;

    qDebug() << "------------------------";
    qDebug() << "move #" << i;
    for (int f = 1; f <= 3; f++)
    {
      Player *plr = player(nCurrentMove);
      ++nCurrentMove;
      qDebug() << "hand " << plr->number() << ":" << dumpCardList(plr->mCards);
    }

    mPlayerHi = nCurrentMove.nValue;
    mDeskView->draw(false);
    player(mPlayerHi)->setThinking();
    mDeskView->draw();
    mDeskView->mySleep(0);
    if (m_currentGame == raspass && (i == 1 || i == 2))
    {
      mCardsOnDesk[0] = mDeck.at(29+i);
      mDeskView->draw();
      mDeskView->mySleep(0);
      mCardsOnDesk[nCurrentMove.nValue] = firstCard = makeGameMove(0, mDeck.at(29+i), true);
    }
    else
    {
      mCardsOnDesk[0] = 0;
      mCardsOnDesk[nCurrentMove.nValue] = firstCard = makeGameMove(0, 0, false);
    }
    player(mPlayerHi)->clearMessage();

    ++nCurrentMove;
    mPlayerHi = nCurrentMove.nValue;
    player(mPlayerHi)->setThinking();
    mDeskView->draw();
    mDeskView->mySleep(0);
    mCardsOnDesk[nCurrentMove.nValue] = secondCard = makeGameMove(0, firstCard, false);
    player(mPlayerHi)->clearMessage();

    ++nCurrentMove;
    mPlayerHi = nCurrentMove.nValue;
    player(mPlayerHi)->setThinking();
    mDeskView->draw();
    mDeskView->mySleep(0);
    mCardsOnDesk[nCurrentMove.nValue] = thirdCard = makeGameMove(firstCard, secondCard, false);
    player(mPlayerHi)->clearMessage();

    checkMoves();
    m_outCards << mCardsOnDesk[0] << mCardsOnDesk[1] << mCardsOnDesk[2] << mCardsOnDesk[3];

    ++nCurrentMove;
    mDeskView->draw();
    mDeskView->longWait(1);

    nCurrentMove = nCurrentMove + whoseTrick(firstCard, secondCard, thirdCard, m_trump)-1;

    QCardList cAni;
    for (int f = 0; f < 4; f++)
    {
      cAni.append(mCardsOnDesk[f]);
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
  for (int i=0; i<4; i++)
  {
    if(!mCardsOnDesk[i]) continue;
    if (m_outCards.contains(mCardsOnDesk[i])) {
      qFatal("Player %d is cheating!", i);
      return false;
    }
  }
  return true;
}

void PrefModel::emitGameChanged(eGameBid game)
{
  switch (game) {
      case zerogame:
      case showpool:
      case g86catch:
      case undefined:
      case whist:
      case halfwhist:
      case gtPass:
        emit gameChanged(QString());
        break;
      case raspass:
        emit gameChanged(tr("Pass-out"));
        break;
      default:
        QString game_name = sGameName(game);
        game_name.replace("\1s", QChar((ushort)0x2660));
        game_name.replace("\1c", QChar((ushort)0x2663));
        game_name.replace("\1d", QChar((ushort)0x2666));
        game_name.replace("\1h", QChar((ushort)0x2665));
        emit gameChanged(mPlayers.at(mPlayerActive)->nick() + ' ' + tr("plays") + ' ' + game_name);
        break;
  }
}
