#include "prfconst.h"
#include "plscore.h"


static QString intList2Str (const QIntList &list) {
  QString res;
  for (int f = 1; f < list.size(); f++) {
    //if (!res.isEmpty()) res += '.';
    res += QString::number(list[f]);
    res += '.';
  }
  return res;
}


///////////////////////////////////////////////////////////////////////////////
ScoreBoard::ScoreBoard () {
  mPool << 0;
  mMountain << 0;
  mLeftWhists << 0;
  mRightWhists << 0;
  mWhists = 0;
}


ScoreBoard::~ScoreBoard () {
}


int ScoreBoard::poolAdd (int delta) {
  int score = pool();
  int ns = score+delta;
  if (ns <= optMaxPool) {
    mPool << ns;
    return 0;
  } else {
    if (optMaxPool != score) mPool << optMaxPool;
    return delta-(optMaxPool-score);
  }
}


void ScoreBoard::mountainDown (int delta) {
  int score = mountain()-delta;
  if (score >= 0) {
    mMountain << score;
  } else {
    score = (delta-mountain())*10/2;
    if (mountain()) mMountain << 0;
    mLeftWhists << leftWhists()+score;
    mRightWhists << rightWhists()+score;
  }
}


void ScoreBoard::mountainUp (int delta) {
  int score = mountain();
  mMountain << score+delta;
}


void ScoreBoard::whistsAdd (int index, int myNumber, int delta) {
  QIntList *dList;
  int score;
  dList = ((myNumber%3)+1 == index) ? &mLeftWhists : &mRightWhists;
  int pnCurrent = dList->at(dList->size()-1);
  score = pnCurrent+delta*10;
  dList->append(score);
}


int ScoreBoard::recordScores (
  eGameBid aGamerType,
  eGameBid aMyType,
  int nGamerVz, //разыгрывающего
  int nMyVz, //чиcтых моих
  int nGamer,
  int myNumber,
  int nqVist //кол-во вистующих
  )
{
  int nGamePrice = gamePoolPrice(aGamerType); // цена игры
  int nGameCard = gameTricks(aGamerType); // взяток должно быть при данной игре
  int nVistCard = gameWhists(aGamerType); // минимальное кол-во взяток вистующих на двоих
  int score = 0, pnCurrent = 0;
  QIntList *dList;

  if (aMyType >= g61 && aMyType != g86) {
    // значит я что-то играл!
    if (nGamerVz >= nGameCard) {
      // и сыграл!
      return poolAdd(nGamePrice);
    } else {
      // и наиграл гору
      pnCurrent = mMountain[mMountain.size()-1];
      score = nGamePrice*(nGameCard-nMyVz)+pnCurrent;
      if (score) mMountain << score;
    }
    return 0;
  }

  //-------------------------------------------------------------------------
  if (aMyType == g86) {
    if (nMyVz) {
      mountainUp(nGamePrice*nMyVz);
      return 0;
    }
    return poolAdd(10);
  }

  //-------------------------------------------------------------------------
  if (aMyType == raspass)  {
    pnCurrent = mMountain[mMountain.size()-1];
    score = nGamePrice*(nMyVz)+pnCurrent;
    if (nMyVz && score) {
      mMountain << score;
      return 0;
    }
    if (!nMyVz) return poolAdd(1);
  }

  dList = ((myNumber%3)+1 == nGamer) ? &mLeftWhists : &mRightWhists;
  pnCurrent = dList->at(dList->size()-1);

  //-------------------------------------------------------------------------
  if (aMyType == gtPass && aGamerType != g86) {
    if (!globvist && nGameCard-nGamerVz > 0) {
      // разыгрывающий без лап(ы)
      score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
    }
    if (score) dList->append(score);
  }

  //-------------------------------------------------------------------------
  if (aMyType == vist && aGamerType != g86)  {
    if (nGameCard-nGamerVz > 0) {
      // разыгрывающий без лап(ы)
      if (nqVist == 2) {
        // 2 вистующих
        // запись только за свои взятки + недобор
        score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
      } else {
        // 1 вистующий
        // запись за свои + недобор
        score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
      }
    } else {
      // разыгрывающий взял своё или больше
      if (nqVist == 2) {
        score = nGamePrice*(nMyVz)+pnCurrent;
      } else {
        score = nGamePrice*(10-nGamerVz)+pnCurrent;
      }
      // проверить на наличие недобора со стороны вистующего!
      if (nVistCard > 10-nGamerVz) {
        if (nqVist == 2) {
          // 2 вистующих
          double d = nVistCard;
          d = (d/2-nMyVz)*nGamePrice;
          mountainUp((int)d);
        } else {
          // 1 вистующий
          if (nVistCard-nMyVz > 0) mountainUp(nGamePrice*(nVistCard-(10-nGamerVz)));
        }
      }
    }
    if (score) dList->append(score);
  }
  return 0;
}


QString ScoreBoard::poolStr () const { return intList2Str(mPool); }
QString ScoreBoard::mountainStr () const { return intList2Str(mMountain); }
QString ScoreBoard::leftWhistsStr () const { return intList2Str(mLeftWhists); }
QString ScoreBoard::rightWhistsStr () const { return intList2Str(mRightWhists); }
