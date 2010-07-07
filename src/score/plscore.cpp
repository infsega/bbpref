/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
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

#include "plscore.h"

#include "baser.h"
#include "prfconst.h"


static QString intList2Str (const QIntList &list, int maxItems) {
  QString res;
  int st = 1, len = list.size();
  if (maxItems == 0) return QString();
  if (maxItems > 0 && len > maxItems) st += (maxItems-len);
  for (int f = st; f < list.size(); f++) {
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
  if (delta == 0)
  	return;
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

void ScoreBoard::mountainAmnesty (int delta) {
	if (delta == 0)
		return;
	mMountain.last() -= delta;
	if ((mMountain.last() == 0) || (mMountain.at(mMountain.size()-2) == mMountain.last()))
		mMountain.removeLast();
}


void ScoreBoard::mountainUp (int delta) {
  if (delta == 0)
  	return;
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
    if (!optWhistGreedy && nGameCard-nGamerVz > 0) {
      // разыгрывающий без лап(ы)
      score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
    }
    if (score) dList->append(score);
  }

  if (aMyType == halfwhist && aGamerType != g86) {
	  score = nGamePrice*nMyVz+pnCurrent;
	  if (score) dList->append(score);
  }

  //-------------------------------------------------------------------------
  if (aMyType == whist && aGamerType != g86)  {
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


QString ScoreBoard::poolStr (int maxItems) const { return intList2Str(mPool, maxItems); }
QString ScoreBoard::mountainStr (int maxItems) const { return intList2Str(mMountain, maxItems); }
QString ScoreBoard::leftWhistsStr (int maxItems) const { return intList2Str(mLeftWhists, maxItems); }
QString ScoreBoard::rightWhistsStr (int maxItems) const { return intList2Str(mRightWhists, maxItems); }


static void serializeIntList (QByteArray &ba, const QIntList &lst) {
  serializeInt(ba, lst.size());
  for (int f = 0; f < lst.size(); f++) serializeInt(ba, lst[f]);
}


static bool unserializeIntList (QByteArray &ba, int *pos, QIntList &lst) {
  int t;
  lst.clear();
  if (!unserializeInt(ba, pos, &t)) return false;
  while (t-- > 0) {
    int c;
    if (!unserializeInt(ba, pos, &c)) return false;
    lst << c;
  }
  return true;
}


void ScoreBoard::serialize (QByteArray &ba) {
  serializeIntList(ba, mPool);
  serializeIntList(ba, mMountain);
  serializeIntList(ba, mLeftWhists);
  serializeIntList(ba, mRightWhists);
  serializeInt(ba, mWhists);
}


bool ScoreBoard::unserialize (QByteArray &ba, int *pos) {
  if (!unserializeIntList(ba, pos, mPool)) return false;
  if (!unserializeIntList(ba, pos, mMountain)) return false;
  if (!unserializeIntList(ba, pos, mLeftWhists)) return false;
  if (!unserializeIntList(ba, pos, mRightWhists)) return false;
  if (!unserializeInt(ba, pos, &mWhists)) return false;
  return true;
}
